#include "menu.hpp"
#include "../core/debug_log.hpp"

#include "globals.hpp"
#include "hooks.hpp"
#include "interfaces.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include <stdexcept>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace menu {
    namespace detail {
        // ============================================
        // INTERNAL: Implementation Details
        // ============================================

        static WNDPROC original_wndproc = nullptr;


        LRESULT __stdcall hook_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
            if (msg == WM_KEYDOWN && wparam == VK_INSERT) {
                logger::DebugLog("[MENU] VK_INSERT pressed, toggling menu");
                globals::SetMenuOpened(!globals::IsMenuOpened());

                // Die Maus wird für das Menü befreit oder wieder an CS2 übergeben
                hooks::original_set_relative_mouse_mode(
                    interfaces::input_system,
                    globals::IsMenuOpened() ? false : globals::IsRelativeMouseMode());
            }

            // WICHTIG: ImGui muss IMMER mitlesen dürfen, um "Taste losgelassen" Events nicht zu
            // verpassen

            logger::DebugLog("[MENU] WndProc msg=0x%X wparam=0x%llX lparam=0x%llX", msg, (unsigned long long)wparam, (unsigned long long)lparam);
            ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

            // Wenn das Menü offen ist, fangen wir die Eingaben ab, damit der Spieler im Hintergrund
            // nicht schießt/läuft
            if (globals::IsMenuOpened()) {
                // Ein paar wichtige Nachrichten müssen trotzdem durch, damit das Spiel nicht
                // einfriert
                if (msg == WM_KEYUP || msg == WM_SYSKEYUP) {
                    return CallWindowProcA(original_wndproc, hwnd, msg, wparam, lparam);
                }
                return true;
            }

            // Wenn das Menü ZU ist, geht jeder Input 1:1 an CS2 -> Du kannst laufen und zielen!
            return CallWindowProcA(original_wndproc, hwnd, msg, wparam, lparam);
        }

        void render_impl() {
            if (!globals::IsMenuOpened()) {
                return;
            }

            bool menu_opened = globals::IsMenuOpened();

            // Setzt die Startgröße des Fensters (nur beim ersten Öffnen)
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

            // Erstellt das Hauptfenster
            if (ImGui::Begin("CS2 Internal Cheat - made with ImGui", &menu_opened)) {
                globals::SetMenuOpened(menu_opened);

                // Startet die Tab-Leiste (Reiter-Menü)
                if (ImGui::BeginTabBar("CheatTabs")) {
                    // --- REITER 1: AIMBOT ---
                    if (ImGui::BeginTabItem("Aimbot")) {
                        ImGui::Text("Hier werden wir bald den Aimbot einbauen!");
                        ImGui::Separator();  // Zieht eine schöne optische Trennlinie

                        // 'static' sorgt dafür, dass ImGui sich den Zustand der Checkbox merkt
                        static bool aimbot_enabled = false;
                        ImGui::Checkbox("Aimbot aktivieren (Dummy)", &aimbot_enabled);
                        ImGui::EndTabItem();
                    }

                    // --- REITER 2: VISUALS (ESP & CHAMS) ---
                    if (ImGui::BeginTabItem("Visuals")) {
                        bool esp_enabled = globals::IsESPEnabled();
                        if (ImGui::Checkbox("ESP", &esp_enabled)) {
                            globals::SetESPEnabled(esp_enabled);
                        }
                        ImGui::Separator();

                        // Die separaten Toggles für ESP
                        bool esp_box = globals::IsESPBox();
                        if (ImGui::Checkbox("Draw Box", &esp_box)) {
                            globals::SetESPBox(esp_box);
                        }

                        bool esp_skeleton = globals::IsESPSkeleton();
                        if (ImGui::Checkbox("Draw Skeleton", &esp_skeleton)) {
                            globals::SetESPSkeleton(esp_skeleton);
                        }

                        bool esp_name = globals::IsESPName();
                        if (ImGui::Checkbox("Draw Names", &esp_name)) {
                            globals::SetESPName(esp_name);
                        }

                        ImGui::Separator();

                        // NEU: Toggle für Chams
                        bool chams_enabled = globals::IsChamsEnabled();
                        if (ImGui::Checkbox("Enable Chams (Magenta)", &chams_enabled)) {
                            globals::SetChamsEnabled(chams_enabled);
                        }

                        ImGui::EndTabItem();
                    }

                    // --- REITER 3: MISC ---
                    if (ImGui::BeginTabItem("Misc")) {
                        ImGui::Text("Hier landen sonstige Funktionen.");
                        ImGui::Separator();

                        static bool bhop_enabled = false;
                        ImGui::Checkbox("Bunnyhop aktivieren (Dummy)", &bhop_enabled);
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();  // Beendet die Tab-Leiste
                }
            }
            ImGui::End();  // Beendet das ImGui Fenster
        }

    }  // namespace detail

    // ============================================
    // PUBLIC API
    // ============================================

    void create() {
        if (!interfaces::hwnd) {
            throw std::runtime_error("HWND not initialized");
        }

        detail::original_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(
            interfaces::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(detail::hook_wndproc)));

        // Optional: Style/IO-Setup kann hier bleiben, falls gewünscht
        // ImGui::StyleColorsDark();
    }

    void destroy() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (detail::original_wndproc) {
            SetWindowLongPtrA(interfaces::hwnd, GWLP_WNDPROC,
                              reinterpret_cast<LONG_PTR>(detail::original_wndproc));
            detail::original_wndproc = nullptr;
        }
    }

    void render() {
        detail::render_impl();
    }

}  // namespace menu