#pragma once
#include <cstdint>
#include <string>
#include "offsets.hpp"

class C_CSPlayerController {
public:
    uintptr_t address;

    C_CSPlayerController(uintptr_t addr = 0) : address(addr) {
    }

    bool IsValid() const {
        return address != 0;
    }

    uint32_t GetPawnHandle() const {
        return *reinterpret_cast<uint32_t*>(address + offsets::controller::m_hPlayerPawn);
    }

    int GetTeam() const {
        return *reinterpret_cast<uint8_t*>(address + offsets::controller::m_iTeamNum);
    }

    const char* GetPlayerName() const {
        return *reinterpret_cast<const char**>(address + offsets::controller::m_iszPlayerName);
    }
};