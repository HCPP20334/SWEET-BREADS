// logger.hpp by HCPP20334 (to PahomEngineGL 1.0.4)
// Easy to Log system lib Writtein to C++20
// support VT100 and 16 bit color mode
// use:
// #include "logger.hpp"
//
// int main(){
// logger console;
// no fmt
// console.send("Hello World",0); // WARN
// console.send("Hello World",1); // INFO
// console.send("Hello World",2); // ERR
// console.send("Hello World",1); // DEBUG
// fmt stdout
// std::string data = "Hello World";
// console.ft->send(0,"{}",data); // WARN
// console.ft->send(1,"{}",data); // INFO
// console.ft->send(2,"{}",data); // ERR
// console.ft->send(3,"{}",data); // DEBUG
// return 0;
// }

#pragma once
#pragma warning(disable: 4996)
#include <Windows.h>
#include <string>
#include <stdint.h>
#include "tui.hpp"
#include <chrono>
struct settings {
    // setting colors RGBA (VT100)
    ColorV3 WARN_COLOR = { 200,250,0 };
    ColorV3 INFO_COLOR = { 133,133,133 };
    ColorV3 ERR_COLOR = { 250,33,133 };
    ColorV3 DEBUG_COLOR = { 250,250,250 };
    ColorV3 BACK_COLOR = { 12,12,19 };
    // default colors WinAPI32 (!VT)
    int32_t WARN_COLORI32 = 0xE;
    int32_t INFO_COLORI32 = 0x8;
    int32_t ERR_COLORI32 = 0xC;
    int32_t DEBUG_COLORI32 = 0xF;
    // flags to loggers
    bool bLogEnabled = true;
    bool bColoredConsole = true;
    // project name to log
    std::string_view NAME_PROJECT = "[PahomEngine]";
    HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
};
struct fval {
    float x, y, z, w;
    fval() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    fval(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};
settings* s = new settings();
struct Logger {
    
    enum type {
        WARN = 0,
        INFO = 1,
        ERR = 2,
        DEBUG = 3,
    };


    // warning! send func not mutex supported!
    void send(std::string text, int iLogTypeFlags = 1) {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&t);

        int32_t i32LogColorText = 0;
        s->bLogEnabled = true;
        std::vector<std::string_view> typeStr = {
            "WARN",
            "INFO",
            "ERR",
            "DEBUG"
        };
        std::string_view typeCurrent;
        typeCurrent = (iLogTypeFlags == type::WARN ? typeStr[0] : typeStr[iLogTypeFlags]);
        typeCurrent = (iLogTypeFlags == type::INFO ? typeStr[1] : typeStr[iLogTypeFlags]);
        typeCurrent = (iLogTypeFlags == type::ERR ? typeStr[2] : typeStr[iLogTypeFlags]);
        typeCurrent = (iLogTypeFlags == type::DEBUG ? typeStr[3] : typeStr[iLogTypeFlags]);
        fval Time = {
           static_cast<float>(local_tm->tm_hour),      // 0–23
           static_cast<float>(local_tm->tm_min),       // 0–59
           static_cast<float>(local_tm->tm_sec),       // 0–59
           static_cast<float>(now.time_since_epoch().count() % 10000000 / 10000.0f) // миллисекунды (примерно)
        };
        if (s->bLogEnabled) {
            static ColorV3 colorBacki32 = { 12,12,19 };
            static ColorV3 colorTexti32 = { 0,0,0 };
            switch (iLogTypeFlags) {
            case type::WARN:
                colorTexti32 = s->WARN_COLOR;
                colorBacki32 = s->BACK_COLOR;
                i32LogColorText = s->WARN_COLORI32;
                break;
            case type::INFO:
                colorBacki32 = s->BACK_COLOR;
                colorTexti32 = s->INFO_COLOR;
                i32LogColorText = s->INFO_COLORI32;
                break;
            case type::ERR:
                colorBacki32 = s->BACK_COLOR;
                colorTexti32 = s->ERR_COLOR;
                i32LogColorText = s->ERR_COLORI32;
                break;
            case type::DEBUG:
                colorBacki32 = s->BACK_COLOR;
                colorTexti32 = s->DEBUG_COLOR;
                i32LogColorText = s->DEBUG_COLORI32;
                break;

            }
            if (s->bColoredConsole)
            {
                tui->pout(std::format(" {}::({}:{}:{}) {} {}\n ", typeCurrent, Time.x, Time.y, Time.z, s->NAME_PROJECT, text), colorTexti32, colorBacki32, false, false);
            }
            else {
                SetConsoleTextAttribute(s->hConsoleHandle, i32LogColorText);
                std::cout << std::format(" {}::({}:{}:{}) ", typeCurrent, Time.x, Time.y, Time.z) << s->NAME_PROJECT << text << std::endl;
            }
        }
    }
    template <class... Tm>
    void send(int iLogTypeFlags, const std::format_string<Tm...> _Fmt, Tm&&... _Args) {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm* local_tm = std::localtime(&t);

        int32_t i32LogColorText = 0;
        s->bLogEnabled = true;
        std::vector<std::string_view> typeStr = {
            "WARN",
            "INFO",
            "ERR",
            "DEBUG"
        };
        std::string_view typeCurrent;
        typeCurrent = (iLogTypeFlags == type::WARN ? typeStr[0] : typeStr[iLogTypeFlags]);
        typeCurrent = (iLogTypeFlags == type::INFO ? typeStr[1] : typeStr[iLogTypeFlags]);
        typeCurrent = (iLogTypeFlags == type::ERR ? typeStr[2] : typeStr[iLogTypeFlags]);
        typeCurrent = (iLogTypeFlags == type::DEBUG ? typeStr[3] : typeStr[iLogTypeFlags]);
        fval Time = {
           static_cast<float>(local_tm->tm_hour),      // 0–23
           static_cast<float>(local_tm->tm_min),       // 0–59
           static_cast<float>(local_tm->tm_sec),       // 0–59
           static_cast<float>(now.time_since_epoch().count() % 10000000 / 10000.0f) // миллисекунды (примерно)
        };
        if (s->bLogEnabled) {
            static ColorV3 colorBacki32 = { 12,12,19 };
            static ColorV3 colorTexti32 = { 0,0,0 };
            switch (iLogTypeFlags) {
            case type::WARN:
                colorTexti32 = s->WARN_COLOR;
                colorBacki32 = s->BACK_COLOR;
                i32LogColorText = s->WARN_COLORI32;
                break;
            case type::INFO:
                colorBacki32 = s->BACK_COLOR;
                colorTexti32 = s->INFO_COLOR;
                i32LogColorText = s->INFO_COLORI32;
                break;
            case type::ERR:
                colorBacki32 = s->BACK_COLOR;
                colorTexti32 = s->ERR_COLOR;
                i32LogColorText = s->ERR_COLORI32;
                break;
            case type::DEBUG:
                colorBacki32 = s->BACK_COLOR;
                colorTexti32 = s->DEBUG_COLOR;
                i32LogColorText = s->DEBUG_COLORI32;
                break;

            }
            //SetConsoleTextAttribute(hConsoleHandle, i32LogColorText);
           // std::cout << std::format(" {}::({}:{}:{}) ", typeCurrent, Time.x, Time.y, Time.z) << " [PahomEngine] " << text << std::endl;
            if (s->bColoredConsole)
            {
                tui->pout(std::format(" {}::({}:{}:{}) {} {}\n ", typeCurrent, Time.x, Time.y, Time.z, s->NAME_PROJECT, std::vformat(_Fmt.get(), std::make_format_args(_Args...))), colorTexti32, colorBacki32, false, false);
            }
            else {
                SetConsoleTextAttribute(s->hConsoleHandle, i32LogColorText);
                std::cout << std::format(" {}::({}:{}:{}) ", typeCurrent, Time.x, Time.y, Time.z) << " [PahomEngine] " << std::vformat(_Fmt.get(), std::make_format_args(_Args...)) << std::endl;
            }
            // SetConsoleTextAttribute(hConsoleHandle, 15);
        }
    }
  
    
};