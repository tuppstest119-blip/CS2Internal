#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include "offsets.hpp"
#include "util/module_memory_access.hpp"

struct Vector3 {
    float x, y, z;
};

class C_CSPlayerPawn {
public:
    uintptr_t address;

    C_CSPlayerPawn(uintptr_t addr = 0) : address(addr) {
    }

    bool IsValid() const {
        if (address <= 0x100000 || address >= 0x7FFFFFFEFFFF) {
            return false;
        }
        int hp = GetHealth();
        return (hp > 0 && hp <= 100);
    }

    int GetHealth() const {
        int health = 0;
        mem_access::safe_read_entity<int>(address + offsets::pawn::m_iHealth, health);
        return health;
    }

    int GetTeam() const {
        int team = 0;
        mem_access::safe_read_entity<int>(address + offsets::controller::m_iTeamNum, team);
        return team;
    }

    Vector3 GetOrigin() const {
        Vector3 origin{};
        uintptr_t sceneNode = 0;

        if (!mem_access::safe_read_entity<uintptr_t>(address + offsets::pawn::m_pGameSceneNode,
                                                     sceneNode))
            return origin;

        mem_access::safe_read_entity<Vector3>(sceneNode + offsets::scenenode::m_vecAbsOrigin,
                                              origin);
        return origin;
    }
};
