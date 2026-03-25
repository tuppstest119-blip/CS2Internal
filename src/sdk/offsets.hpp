#pragma once

#include <cstddef>
#include <cstdint>

namespace cs2_dumper {

    // ============================================
    // CLIENT.DLL Offsets
    // ============================================
    namespace offsets {
        namespace client_dll {

            // --- globale Offsets ---
            constexpr uintptr_t dwCSGOInput = 0x2319FC0;         // Input-System
            constexpr uintptr_t dwGameEntitySystem = 0x24AF268;  // CGameEntitySystem*
            constexpr uintptr_t dwGameEntitySystem_highestEntityIndex = 0x20A0;
            constexpr uintptr_t dwLocalPlayerPawn = 0x2069B50;
            constexpr uintptr_t dwEntityList = 0x24AF268;  // Beispiel: statischer Fallback
            constexpr uintptr_t dwViewMatrix = 0x230FF20;

            // --- Pawn (C_CSPlayerPawn) ---
            constexpr uintptr_t m_iHealth = 0x354;
            constexpr uintptr_t m_iTeamNum = 0x3F3;
            constexpr uintptr_t m_vOldOrigin = 0x1588;
            constexpr uintptr_t m_CBodyComponent = 0x38;
            constexpr uintptr_t m_pGameSceneNode = 0x338;
            constexpr uintptr_t m_vecViewOffset = 0xD58;
            constexpr uintptr_t m_iShotsFired = 0x270C;
            constexpr uintptr_t m_aimPunchAngle = 0x16CC;

            // --- Controller (C_CSPlayerController) ---
            constexpr uintptr_t m_iszPlayerName = 0x6F8;
            constexpr uintptr_t m_hPlayerPawn = 0x90C;
            constexpr uintptr_t m_bPawnIsAlive = 0x914;

            // --- Body Component / Skeleton ---
            constexpr uintptr_t m_modelState = 0x160;

            // --- Observer ---
            constexpr std::ptrdiff_t m_pObserverServices = 0x13F0;
            constexpr std::ptrdiff_t m_hObserverTarget = 0x4C;
            constexpr std::ptrdiff_t m_iObserverMode = 0x48;

        }  // namespace client_dll

        // ============================================
        // Pawn Offsets
        // ============================================
        namespace pawn {
            constexpr uintptr_t m_iHealth = 0x354;
            constexpr uintptr_t m_iTeamNum = 0x3F3;
            constexpr uintptr_t m_pHealthServices = 0xB28;
            constexpr uintptr_t m_pBodyComponent = 0x38;
            constexpr uintptr_t m_hController = 0x1478;
            constexpr uintptr_t m_pGameSceneNode = 0x338;
        }  // namespace pawn

        // ============================================
        // Controller Offsets
        // ============================================
        namespace controller {
            constexpr uintptr_t m_iszPlayerName = 0x6F8;
            constexpr uintptr_t m_hPlayerPawn = 0x90C;
            constexpr uintptr_t m_bPawnIsAlive = 0x914;
        }  // namespace controller

        // ============================================
        // Body / SceneNode Offsets
        // ============================================
        namespace body {
            constexpr uintptr_t m_pSceneNode = 0x28;
        }

        namespace scenenode {
            constexpr uintptr_t m_vecAbsOrigin = 0xD0;
        }

        namespace skeleton {
            constexpr uintptr_t m_modelState = 0x160;
            constexpr uintptr_t m_vecBones = 0x80;
            constexpr uintptr_t bone_size = 0x20;
            constexpr uintptr_t m_nBoneCount = 0x78;
        }  // namespace skeleton

        // ============================================
        // Internal Dynamic Offsets
        // ============================================
        namespace detail {
            namespace dynamic_offsets {
                inline uintptr_t view_matrix = 0;
                inline uintptr_t entity_list = 0;
                inline uintptr_t local_controller = 0;
                inline uintptr_t calculate_world_space_bones = 0;
                inline uintptr_t cgc_client_system_get_instance = 0;
                inline uintptr_t cgc_client_find_so_cache = 0;
                inline uintptr_t cecon_item_create_instance = 0;
                inline uintptr_t cecon_item_set_dynamic_attribute = 0;
                inline uintptr_t ccs_inventory_manager_get_instance = 0;
                inline uintptr_t create_base_type_cache = 0;
                inline uintptr_t draw_aggregate_scene_object_array = 0;
            }  // namespace dynamic_offsets
        }  // namespace detail

        // ============================================
        // Getter / Setter API
        // ============================================
        inline uintptr_t GetViewMatrix() {
            return detail::dynamic_offsets::view_matrix;
        }
        inline void SetViewMatrix(uintptr_t addr) {
            detail::dynamic_offsets::view_matrix = addr;
        }

        inline uintptr_t GetEntityList() {
            if (detail::dynamic_offsets::entity_list)
                return detail::dynamic_offsets::entity_list;
            return client_dll::dwEntityList;  // fallback
        }
        inline void SetEntityList(uintptr_t addr) {
            detail::dynamic_offsets::entity_list = addr;
        }

        inline uintptr_t GetLocalController() {
            return detail::dynamic_offsets::local_controller;
        }
        inline void SetLocalController(uintptr_t addr) {
            detail::dynamic_offsets::local_controller = addr;
        }

        inline uintptr_t GetCalculateWorldSpaceBones() {
            return detail::dynamic_offsets::calculate_world_space_bones;
        }
        inline void SetCalculateWorldSpaceBones(uintptr_t addr) {
            detail::dynamic_offsets::calculate_world_space_bones = addr;
        }

        inline uintptr_t GetDrawAggregateSceneObjectArray() {
            return detail::dynamic_offsets::draw_aggregate_scene_object_array;
        }
        inline void SetDrawAggregateSceneObjectArray(uintptr_t addr) {
            detail::dynamic_offsets::draw_aggregate_scene_object_array = addr;
        }

        // ============================================
        // Signatures (Pattern Scan)
        // ============================================
        namespace signatures {
            inline const char* ViewMatrix = "48 63 C1 48 8D 0D ? ? ? ? 48 C1 E0 06 48 03 C1 C3";
            inline const char* EntityList =
                "B8 FF FF FF 7F 89 45 B3 48 8B 05 ? ? ? ? 48 89 BC 24 F0 00 00 00";
            inline const char* LocalController = "48 8D 0D ? ? ? ? 33 F6 4C 89 34 F9";
            inline const char* CalculateWorldSpaceBones =
                "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 "
                "48 8D 6C 24 ? 48 8B 81";
            inline const char* CGCClientSystem_GetInstance =
                "48 8B 05 ? ? ? ? 48 85 C0 75 ? 53 48 83 EC 20 B9 48 01 00 00 E8";
            inline const char* CGCClient_FindSOCache = "48 8B F0 48 85 C0 74 ? 4C 8B";
            inline const char* CEconItem_CreateInstance =
                "48 83 EC 28 B9 48 00 00 00 E8 ? ? ? ? 48 85";
            inline const char* CEconItem_SetDynamicAttributeValue =
                "48 89 6C 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B FA C7 44 24";
            inline const char* CCSInventoryManager_GetInstance = "48 8D 05 ? ? ? ? C3";
            inline const char* CreateBaseTypeCache = "40 53 48 83 EC ? 4C 8B 49 ? 44 8B D2";
            inline const char* DrawAggregateSceneObjectArray =
                "48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? "
                "? 48 81 EC ? ? ? ? 0F 29 70";
        }  // namespace signatures

        // ============================================
        // Inventory Offsets
        // ============================================
        namespace inventory {
            constexpr uintptr_t m_pCGCClient = 0xB8;
            constexpr uintptr_t m_pSOCache = 0x60;
            constexpr uintptr_t m_pOwner = 0x10;
        }  // namespace inventory

        // ============================================
        // Scene / Rendering
        // ============================================
        namespace scene {
            constexpr uintptr_t m_pSceneSystem = 0xA0;
            constexpr uintptr_t m_vecDrawList = 0x40;
        }  // namespace scene

        // ============================================
        // VTable Indexes
        // ============================================
        namespace vtable {
            constexpr int Init_CGCClientSystem = 33;
            constexpr int EquipItemInLoadout_CCSInventoryManager = 66;
            constexpr int GetLocalInventory_CCSInventoryManager = 69;
        }  // namespace vtable

    }  // namespace offsets
}  // namespace cs2_dumper