#pragma once
#include <cstdint>  // für uintptr_t

// ============================================
// INTERNAL: Private State
// ============================================
namespace globals::detail {
    inline bool menu_opened = true;
    inline bool relative_mouse_mode = false;
    inline bool esp_enabled = false;
    inline bool esp_box = true;
    inline bool esp_skeleton = true;
    inline bool esp_name = true;
    inline bool esp_weapon = false;
    inline bool chams_enabled = false;
    inline uintptr_t entity_list = 0;
    inline int local_team = 0;
}  // namespace globals::detail

// ============================================
// PUBLIC API: Safe Accessors
// ============================================
namespace globals {

    // Menu
    inline bool IsMenuOpened() {
        return detail::menu_opened;
    }
    inline void SetMenuOpened(bool value) {
        detail::menu_opened = value;
    }

    // Mouse Mode
    inline bool IsRelativeMouseMode() {
        return detail::relative_mouse_mode;
    }
    inline void SetRelativeMouseMode(bool value) {
        detail::relative_mouse_mode = value;
    }

    // ESP
    inline bool IsESPEnabled() {
        return detail::esp_enabled;
    }
    inline void SetESPEnabled(bool value) {
        detail::esp_enabled = value;
    }

    inline bool IsESPBox() {
        return detail::esp_box;
    }
    inline void SetESPBox(bool value) {
        detail::esp_box = value;
    }

    inline bool IsESPSkeleton() {
        return detail::esp_skeleton;
    }
    inline void SetESPSkeleton(bool value) {
        detail::esp_skeleton = value;
    }

    inline bool IsESPName() {
        return detail::esp_name;
    }
    inline void SetESPName(bool value) {
        detail::esp_name = value;
    }

    inline bool IsESPWeapon() {
        return detail::esp_weapon;
    }
    inline void SetESPWeapon(bool value) {
        detail::esp_weapon = value;
    }

    // Chams
    inline bool IsChamsEnabled() {
        return detail::chams_enabled;
    }
    inline void SetChamsEnabled(bool value) {
        detail::chams_enabled = value;
    }

    // Entity & Team
    inline uintptr_t GetEntityList() {
        return detail::entity_list;
    }
    inline void SetEntityList(uintptr_t value) {
        detail::entity_list = value;
    }

    inline int GetLocalTeam() {
        return detail::local_team;
    }
    inline void SetLocalTeam(int value) {
        detail::local_team = value;
    }

}  // namespace globals