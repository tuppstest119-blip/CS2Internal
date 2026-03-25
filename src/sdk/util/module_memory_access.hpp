#pragma once
#include <cstdint>
#include <cstring>

namespace mem_access {
    namespace detail {
        // ============================================
        // MEMORY ACCESS VARIANTS
        // Multiple Implementierungen für unterschiedliche Module
        // ============================================

        // Variant 1: Standard Try/Except
        template <typename T>
        inline bool read_v1(uintptr_t addr, T& out) {
            if (addr < 0x100000 || addr > 0x7FFFFFFEFFFF)
                return false;
            __try {
                out = *reinterpret_cast<T*>(addr);
                return true;
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                return false;
            }
        }

        // Variant 2: Memcpy-basiert (für ESP)
        template <typename T>
        inline bool read_v2(uintptr_t addr, T& out) {
            if (addr < 0x100000 || addr > 0x7FFFFFFEFFFF)
                return false;
            __try {
                std::memcpy(&out, reinterpret_cast<const void*>(addr), sizeof(T));
                return true;
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                return false;
            }
        }

        // Variant 3: Volatile (für Chams)
        template <typename T>
        inline bool read_v3(uintptr_t addr, T& out) {
            if (addr < 0x100000 || addr > 0x7FFFFFFEFFFF)
                return false;
            __try {
                volatile const T* ptr = reinterpret_cast<volatile const T*>(addr);
                out = *ptr;
                return true;
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                return false;
            }
        }

        // Variant 4: Entity-spezifisch
        template <typename T>
        inline bool read_v4(uintptr_t addr, T& out) {
            if (addr < 0x10000 || addr > 0x7FFFFFFFFFFF)
                return false;
            __try {
                out = *reinterpret_cast<T*>(addr);
                return true;
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                return false;
            }
        }
    }  // namespace detail

    // ============================================
    // PUBLIC: Module-spezifische Wrappers
    // ============================================

    template <typename T>
    inline bool safe_read_esp(uintptr_t addr, T& out) {
        return detail::read_v2(addr, out);
    }

    template <typename T>
    inline bool safe_read_chams(uintptr_t addr, T& out) {
        return detail::read_v3(addr, out);
    }

    template <typename T>
    inline bool safe_read_entity(uintptr_t addr, T& out) {
        return detail::read_v4(addr, out);
    }

    template <typename T>
    inline bool safe_read(uintptr_t addr, T& out) {
        return detail::read_v1(addr, out);
    }
}  // namespace mem_access
