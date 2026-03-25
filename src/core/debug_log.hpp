#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <cstdarg>

namespace logger {
    inline void DebugLog(const char* fmt, ...) {
        constexpr const char* log_path = "D:\\SteamLibrary\\steamapps\\common\\Counter-Strike Global Offensive\\game\\bin\\win64\\cs2_cheat_debug.txt";
        char buffer[1024];

        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        // Zeitstempel erzeugen
        auto now = std::time(nullptr);
        struct tm tm_info = {};
        localtime_s(&tm_info, &now);

        std::ofstream log_file(log_path, std::ios::app);
        if (log_file.is_open()) {
            log_file << "[" << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S") << "] " << buffer << "\n";
            log_file.close();
        }
    }
}
