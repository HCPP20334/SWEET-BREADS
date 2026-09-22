#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <stdint.h>
#include <cstring>
std::string xor32(std::string &sText,std::string sSecretKey){
    if (sSecretKey.empty()) return sText;
    std::string result;
    for (size_t chunk = 0; chunk < sText.size(); ++chunk) {
        char c = sText[chunk] ^ sSecretKey[chunk % sSecretKey.size()];
        result.push_back(c);
    }
    return result;
}
std::string addc(int s, const std::string& i) {
    std::string od;
    int d = 0;
    for (char c : i) {
        od += c;
        if (++d == s) {
            od += "$$";
            d = 0;
        }
    }
    return od;
}
std::string addce(const std::string& s, int group_size = 5) {
    std::string r;
    r.reserve(s.size());
    size_t i = 0;
    while (i < s.size()) {
        size_t take = std::min<size_t>((size_t)group_size, s.size() - i);
        r.append(s, i, take);
        i += take + 2; 
    }
    return r;
}
static const char* BASE64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64_encode(const std::string& input) {
    std::string out;
    out.reserve(((input.size() + 2) / 3) * 4);
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}
std::string rvbstr(std::string _key) {
    std::reverse(_key.begin(), _key.end());
    return _key;
}
std::string base64_decode(const std::string& input) {
    std::string out;
    out.reserve(input.size() * 3 / 4);
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (c == '=') break;
        const char* p = strchr(BASE64_CHARS, c);
        if (!p)  continue;
        val = (val << 6) + (p - BASE64_CHARS);
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}
void  deaalocate(std::string& s) {
    if (!s.empty()) {
        std::fill(s.begin(), s.end(), '\0'); 
    }
    std::string().swap(s);
    s.clear();
    s.shrink_to_fit();
}
// m0 text buffer , m1 = secret key
// encode: text -> reverse -> add to 5 byte marker $$ -> xor32 obfs (key)
// decode: xor32 obfs (key) <-  add to 5 byte marker $$ <- reverse  
std::string obxrac32b64(bool isDecode,std::string m0,std::string m1) {
    std::string a = "";
    std::string b = "";
    std::string c = "";
     if (isDecode) {
	a = base64_decode((m0)); // base64 decode m0 text
    deaalocate(m0);
    m0.clear(); // m0 cleanup 
	b = xor32(a, m1); // xor32 obfs 
    deaalocate(a);
    a.clear(); // a cleanup 
	b = addce(b); // cleanup to any 5 byte char $$
	b = rvbstr(b); // reverse
     } else {
	a = rvbstr(m0); // reverse string encoded text
    deaalocate(m0);
    m0.clear(); // m0 cleaup 
	a = addc(5, a); // $$
	b = xor32(a, m1); // xor32 obfs
    a.clear(); // cleanup a
	b = base64_encode(b); // base64 encode text
     }
     return b;
}
