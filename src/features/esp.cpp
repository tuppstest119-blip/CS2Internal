#include "esp.hpp"
#include "../core/globals.hpp"
#include "../sdk/offsets.hpp"

#include <imgui/imgui.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

using namespace cs2_dumper::offsets::client_dll;

namespace esp {
    static uintptr_t client_base = 0;

    // Dynamische Offsets (werden vom Auto-Scanner gefunden)
    static int dyn_chunk_offset = -1;
    static int dyn_identity_size = -1;
    static bool scan_attempted = false;

    struct Vector3 {
        float x, y, z;
    };
    struct ViewMatrix {
        float matrix[4][4];
    };

    struct BoneConnection {
        int bone1;
        int bone2;
    };
    const std::vector<BoneConnection> boneConnections = {
        {6, 5},   {5, 4},   {4, 2},  {2, 0},   {5, 8},   {8, 9},  {9, 10},  {5, 13},
        {13, 14}, {14, 15}, {0, 22}, {22, 23}, {23, 24}, {0, 25}, {25, 26}, {26, 27}};

    template <typename T>
    bool SafeRead(uintptr_t address, T& out) {
        if (address < 0x100000 || address > 0x7FFFFFFEFFFF)
            return false;
        if (IsBadReadPtr(reinterpret_cast<const void*>(address), sizeof(T)))
            return false;
        out = *reinterpret_cast<T*>(address);
        return true;
    }

    bool WorldToScreen(const Vector3& pos, ImVec2& screen, const ViewMatrix& matrix,
                       ImVec2 windowSize) {
        float w = matrix.matrix[3][0] * pos.x + matrix.matrix[3][1] * pos.y +
                  matrix.matrix[3][2] * pos.z + matrix.matrix[3][3];
        if (w < 0.01f)
            return false;

        float x = matrix.matrix[0][0] * pos.x + matrix.matrix[0][1] * pos.y +
                  matrix.matrix[0][2] * pos.z + matrix.matrix[0][3];
        float y = matrix.matrix[1][0] * pos.x + matrix.matrix[1][1] * pos.y +
                  matrix.matrix[1][2] * pos.z + matrix.matrix[1][3];

        x /= w;
        y /= w;
        screen.x = (windowSize.x / 2.0f) * (x + 1.0f);
        screen.y = (windowSize.y / 2.0f) * (1.0f - y);
        return true;
    }

    Vector3 GetBonePos(uintptr_t pawn, int bone_index) {
        uintptr_t scene_node;
        if (!SafeRead(pawn + m_pGameSceneNode, scene_node) || !scene_node)
            return {0, 0, 0};
        uintptr_t bone_array = 0;
        if (!SafeRead(scene_node + 0x210, bone_array))
            SafeRead(scene_node + 0x1F0, bone_array);
        if (!bone_array)
            return {0, 0, 0};
        Vector3 bone_pos;
        if (!SafeRead(bone_array + bone_index * 32, bone_pos))
            return {0, 0, 0};
        return bone_pos;
    }

    void render() {
        if (!globals::IsESPEnabled())  // Nutzt jetzt den korrekten Getter
            return;

        if (!client_base) {
            client_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll"));
            if (!client_base)
                return;
        }

        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        ImVec2 screen_size = ImGui::GetIO().DisplaySize;

        ViewMatrix view_matrix;
        uintptr_t entity_list;

        uintptr_t local_pawn = 0;
        uint8_t local_team = 0;
        if (SafeRead(client_base + dwLocalPlayerPawn, local_pawn) && local_pawn) {
            SafeRead(local_pawn + m_iTeamNum, local_team);
        }

        if (!SafeRead(client_base + dwViewMatrix, view_matrix) ||
            !SafeRead(client_base + dwEntityList, entity_list) || !entity_list)
            return;

        // --- DER AUTO-SCANNER ---
        // Sucht dynamisch nach der echten Identity Size und dem Chunk Offset!
        if (!scan_attempted && local_pawn) {
            for (int chunk_idx = 0; chunk_idx < 4; ++chunk_idx) {   // Suche in den ersten 4 Chunks
                for (int c_off = 0x0; c_off <= 0x20; c_off += 8) {  // Teste Offsets 0x0 bis 0x20
                    uintptr_t chunk;
                    if (!SafeRead(entity_list + c_off + (chunk_idx * 8), chunk) || !chunk)
                        continue;

                    for (int i = 0; i < 512; ++i) {  // Teste alle Slots im Chunk
                        for (int size = 0x70; size <= 0x90;
                             size += 8) {  // Teste ChatGPTs Größen (0x78, 0x80, 0x88, 0x90)
                            uintptr_t ptr;
                            // Liegt an dieser berechneten Stelle unser lokaler Spieler?
                            if (SafeRead(chunk + i * size, ptr) && ptr == local_pawn) {
                                dyn_chunk_offset = c_off;
                                dyn_identity_size = size;
                                break;
                            }
                        }
                        if (dyn_chunk_offset != -1)
                            break;
                    }
                    if (dyn_chunk_offset != -1)
                        break;
                }
                if (dyn_chunk_offset != -1)
                    break;
            }

            // Fallback, falls der lokale Spieler (noch) nicht im Speicher lag
            if (dyn_chunk_offset == -1) {
                dyn_chunk_offset = 0x10;
                dyn_identity_size = 0x78;
            }
            scan_attempted = true;
        }

        int actual_players_drawn = 0;

        // WIR NUTZEN JETZT DIE GEFUNDENEN WERTE
        int chunk_offset = dyn_chunk_offset;
        int identity_size = dyn_identity_size;

        for (int i = 1; i < 16384; ++i) {
            uintptr_t list_entry;
            // Gefundener Chunk-Offset wird hier angewendet
            if (!SafeRead(entity_list + (8 * (i >> 9) + chunk_offset), list_entry) || !list_entry)
                continue;

            // Gefundene Identity-Size wird hier angewendet
            uintptr_t identity_address = list_entry + identity_size * (i & 0x1FF);

            uint32_t handle;
            if (!SafeRead(identity_address + 0x10, handle) || (handle & 0x7FFF) != i)
                continue;

            uintptr_t pawn;
            if (!SafeRead(identity_address, pawn) || !pawn)
                continue;
            if (pawn == local_pawn)
                continue;

            int health = 0;
            if (!SafeRead(pawn + m_iHealth, health) || health <= 0 || health > 200)
                continue;

            uint8_t team = 0;
            if (!SafeRead(pawn + m_iTeamNum, team) || team < 2)
                continue;

            Vector3 feet_pos;
            if (!SafeRead(pawn + m_vOldOrigin, feet_pos) ||
                (feet_pos.x == 0.0f && feet_pos.y == 0.0f))
                continue;

            Vector3 head_pos = {feet_pos.x, feet_pos.y, feet_pos.z + 72.0f};
            Vector3 real_head_pos = GetBonePos(pawn, 6);

            if (real_head_pos.x != 0.0f) {
                head_pos = {real_head_pos.x, real_head_pos.y, real_head_pos.z + 8.0f};
            }

            ImVec2 feet_screen, head_screen;
            if (WorldToScreen(feet_pos, feet_screen, view_matrix, screen_size) &&
                WorldToScreen(head_pos, head_screen, view_matrix, screen_size)) {
                actual_players_drawn++;

                float h = feet_screen.y - head_screen.y;
                float w = h / 2.0f;
                float x = head_screen.x - w / 2;
                float y = head_screen.y;

                ImU32 team_color = (team == local_team && local_team != 0)
                                       ? IM_COL32(0, 255, 0, 255)
                                       : IM_COL32(255, 0, 0, 255);

                draw_list->AddRect({x - 1, y - 1}, {x + w + 1, y + h + 1}, IM_COL32(0, 0, 0, 150),
                                   0.0f, 0, 3.0f);
                draw_list->AddRect({x, y}, {x + w, y + h}, team_color, 0.0f, 0, 1.5f);

                float hpFrac = std::clamp(health / 100.0f, 0.0f, 1.0f);
                float hpH = h * hpFrac;
                ImU32 hpColor = IM_COL32(static_cast<int>((1.0f - hpFrac) * 255),
                                         static_cast<int>(hpFrac * 255), 0, 255);
                draw_list->AddRectFilled({x - 7, y - 1}, {x - 3, y + h + 1},
                                         IM_COL32(0, 0, 0, 150));
                draw_list->AddRectFilled({x - 6, y + h - hpH}, {x - 4, y + h}, hpColor);

                if (real_head_pos.x != 0.0f) {
                    for (const auto& conn : boneConnections) {
                        Vector3 b1 = GetBonePos(pawn, conn.bone1);
                        Vector3 b2 = GetBonePos(pawn, conn.bone2);
                        ImVec2 s1, s2;
                        if (WorldToScreen(b1, s1, view_matrix, screen_size) &&
                            WorldToScreen(b2, s2, view_matrix, screen_size)) {
                            draw_list->AddLine(s1, s2, IM_COL32(255, 255, 255, 200), 1.5f);
                        }
                    }
                }
            }
        }

        // Zeigt dir oben links genau an, was ChatGPT und wir gesucht haben!
        std::string stats = "Real Players Drawn: " + std::to_string(actual_players_drawn);
        if (dyn_chunk_offset != -1) {
            char debug_info[128];
            snprintf(debug_info, sizeof(debug_info), " | Auto-Offset: 0x%X | Size: 0x%X",
                     dyn_chunk_offset, dyn_identity_size);
            stats += debug_info;
        }
        draw_list->AddText({15, 15}, IM_COL32(0, 255, 0, 255), stats.c_str());
    }
}  // namespace esp