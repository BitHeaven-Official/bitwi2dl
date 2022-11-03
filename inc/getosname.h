#include <string>

std::string getOsName() {
    #ifdef _WIN64
        return "windows 64-bit";
    #elif _WIN32
        return "windows 32-bit";
    #elif __APPLE__ || __MACH__
        return "mac osx";
    #elif __linux__
        return "linux";
    #elif __FreeBSD__
        return "freebsd";
    #elif __unix || __unix__
        return "unix";
    #else
        return "other";
    #endif
}
