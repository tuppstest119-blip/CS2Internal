#pragma once
#include <cstdint>
#include "offsets.hpp"

namespace entity_system {

    inline uintptr_t GetEntityList(uintptr_t client_base) {
        return *reinterpret_cast<uintptr_t*>(client_base +
                                             cs2_dumper::offsets::client_dll::dwEntityList);
    }

    inline uintptr_t ResolveHandle(uintptr_t entity_list, uint32_t handle) {
        if (!handle)
            return 0;

        // Index extrahieren
        int index = handle & 0x7FFF;
        if (index <= 0 || index > 0x7FFF)
            return 0;

        // Chunk berechnen
        uintptr_t list_entry =
            *reinterpret_cast<uintptr_t*>(entity_list + 0x10 + (0x8 * (index >> 9)));

        if (!list_entry)
            return 0;

        // Entity holen
        uintptr_t entity = *reinterpret_cast<uintptr_t*>(list_entry + 0x78 * (index & 0x1FF));

        return entity;
    }

}  // namespace entity_system