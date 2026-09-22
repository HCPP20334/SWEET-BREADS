#pragma once
#include <stdint.h>
#include <vector>
#include <cstring>
#include "logger.hpp"

// pusher - Texture separator OpenGL
struct Pusher {
    Logger Ln;
    std::vector<uint8_t> u8bufferM;
    bool bUseMemCpy = false;
    void useMemcpy() {
        bUseMemCpy = !bUseMemCpy;
    }
    bool isUsedMemcpy() const {
        return bUseMemCpy;
    }
    void set_buffer(const uint8_t* u8Buffer, size_t byteSize) {
        uint8_t* out = 0;
        Ln.send(1, "Pusher::set_buffer() -> push memory buffer [addr={:p} block={}] to [addr={:p}]", (void*)u8Buffer, byteSize, (void*)&u8bufferM);
        size_t szo = u8bufferM.size();
        u8bufferM.resize(szo + byteSize);
        if(!bUseMemCpy)
        {
           
            for (uint64_t _s = 0; _s < byteSize; _s++) {
                u8bufferM[_s + szo] = u8Buffer[_s];
            }
        }
        else {
            std::memcpy(u8bufferM.data() + szo, u8Buffer, byteSize);
        }

    }
    void swap(std::vector<std::uint32_t>& u32buffer) {
        size_t szo = u8bufferM.size();
        Ln.send("Pusher::swap() call", 1);

        Ln.send(1, "Pusher::swap() buffer allocate {} bytes", szo);
        if (!u8bufferM.empty()) {
            size_t byteSize = u8bufferM.size();
            u32buffer.resize(byteSize / sizeof(uint32_t));

            if (byteSize > 0) {
                std::memcpy(u32buffer.data(), u8bufferM.data(), byteSize);
            }
            u8bufferM.clear();
        }
        else {
            Ln.send("Pusher::swap() buffer (u8) empty", 3);
        }
    }
    std::vector<uint8_t> get() const {
        return u8bufferM;
    }
    uint8_t* Pushed() {
        return (u8bufferM.data());
    }
};
Pusher pt;