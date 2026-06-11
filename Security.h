#ifndef SECURITY_H
#define SECURITY_H

#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cstring>
#include <termios.h>
#include <unistd.h>

namespace Security {
    #define SHA2_SHFR(x, n)    (x >> n)
    #define SHA2_ROTR(x, n)    ((x >> n) | (x << ((sizeof(x) << 3) - n)))
    #define SHA2_ROTL(x, n)    ((x << n) | (x >> ((sizeof(x) << 3) - n)))
    #define SHA256_CH(x, y, z)   ((x & y) ^ (~x & z))
    #define SHA256_MAJ(x, y, z)  ((x & y) ^ (x & z) ^ (y & z))
    #define SHA256_F1(x)       (SHA2_ROTR(x, 2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
    #define SHA256_F2(x)       (SHA2_ROTR(x, 6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
    #define SHA256_F3(x)       (SHA2_ROTR(x, 7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x, 3))
    #define SHA256_F4(x)       (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))

    class SHA256 {
    protected:
        typedef unsigned char uint8;
        typedef unsigned int uint32;
        typedef unsigned long long uint64;

        const static uint32 sha256_k[];
        static const unsigned int SHA224_256_BLOCK_SIZE = (512/8);

    public:
        void init() {
            m_h[0] = 0x6a09e667; m_h[1] = 0xbb67ae85; m_h[2] = 0x3c6ef372; m_h[3] = 0xa54ff53a;
            m_h[4] = 0x510e527f; m_h[5] = 0x9b05688c; m_h[6] = 0x1f83d9ab; m_h[7] = 0x5be0cd19;
            m_tot_len = 0;
            m_len = 0;
        }

        void update(const unsigned char *message, unsigned int len) {
            unsigned int block_nb;
            unsigned int new_len, rem_len, tmp_len;
            const unsigned char *shifted_message;
            tmp_len = SHA224_256_BLOCK_SIZE - m_len;
            rem_len = len < tmp_len ? len : tmp_len;
            std::memcpy(&m_block[m_len], message, rem_len);
            if (m_len + len < SHA224_256_BLOCK_SIZE) {
                m_len += len;
                return;
            }
            new_len = len - rem_len;
            block_nb = new_len / SHA224_256_BLOCK_SIZE;
            shifted_message = message + rem_len;
            transform(m_block, 1);
            transform(shifted_message, block_nb);
            rem_len = new_len % SHA224_256_BLOCK_SIZE;
            std::memcpy(m_block, shifted_message + block_nb * SHA224_256_BLOCK_SIZE, rem_len);
            m_len = rem_len;
            m_tot_len += (block_nb + 1) * SHA224_256_BLOCK_SIZE;
        }

        void final(unsigned char *digest) {
            unsigned int block_nb;
            unsigned int pm_len;
            unsigned int len_b;
            int i;
            block_nb = (1 + ((SHA224_256_BLOCK_SIZE - 9) < m_len));
            len_b = (m_tot_len + m_len) << 3;
            pm_len = block_nb * SHA224_256_BLOCK_SIZE;
            std::memset(m_block + m_len, 0, pm_len - m_len);
            m_block[m_len] = 0x80;
            m_block[pm_len - 4] = (unsigned char)((len_b >> 24) & 0xFF);
            m_block[pm_len - 3] = (unsigned char)((len_b >> 16) & 0xFF);
            m_block[pm_len - 2] = (unsigned char)((len_b >> 8) & 0xFF);
            m_block[pm_len - 1] = (unsigned char)((len_b) & 0xFF);
            transform(m_block, block_nb);
            for (i = 0 ; i < 8; i++) {
                digest[i*4]     = (unsigned char)((m_h[i] >> 24) & 0xFF);
                digest[i*4 + 1] = (unsigned char)((m_h[i] >> 16) & 0xFF);
                digest[i*4 + 2] = (unsigned char)((m_h[i] >> 8) & 0xFF);
                digest[i*4 + 3] = (unsigned char)((m_h[i]) & 0xFF);
            }
        }

        static const unsigned int DIGEST_SIZE = ( 256 / 8);

    protected:
        void transform(const unsigned char *message, unsigned int block_nb) {
            uint32 w[64];
            uint32 wv[8];
            uint32 t1, t2;
            const unsigned char *sub_block;
            int i, j;
            for (i = 0; i < (int) block_nb; i++) {
                sub_block = message + (i * SHA224_256_BLOCK_SIZE);
                for (j = 0; j < 16; j++) {
                    w[j] =  ((uint32)sub_block[j*4] << 24)
                           | ((uint32)sub_block[j*4 + 1] << 16)
                           | ((uint32)sub_block[j*4 + 2] << 8)
                           | ((uint32)sub_block[j*4 + 3]);
                }
                for (j = 16; j < 64; j++) {
                    w[j] =  SHA256_F4(w[j - 2]) + w[j - 7] + SHA256_F3(w[j - 15]) + w[j - 16];
                }
                for (j = 0; j < 8; j++) {
                    wv[j] = m_h[j];
                }
                for (j = 0; j < 64; j++) {
                    t1 = wv[7] + SHA256_F2(wv[4]) + SHA256_CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
                    t2 = SHA256_F1(wv[0]) + SHA256_MAJ(wv[0], wv[1], wv[2]);
                    wv[7] = wv[6];
                    wv[6] = wv[5];
                    wv[5] = wv[4];
                    wv[4] = wv[3] + t1;
                    wv[3] = wv[2];
                    wv[2] = wv[1];
                    wv[1] = wv[0];
                    wv[0] = t1 + t2;
                }
                for (j = 0; j < 8; j++) {
                    m_h[j] += wv[j];
                }
            }
        }

    private:
        uint32 m_h[8];
        uint32 m_tot_len;
        uint32 m_len;
        unsigned char m_block[2*SHA224_256_BLOCK_SIZE];
    };

    inline const SHA256::uint32 SHA256::sha256_k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664f, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    inline std::string hashPassword(const std::string& password) {
        SHA256 ctx;
        ctx.init();
        ctx.update((const unsigned char*)password.c_str(), password.length());
        unsigned char digest[SHA256::DIGEST_SIZE];
        ctx.final(digest);
        
        std::stringstream ss;
        for (int i = 0; i < 32; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        }
        return ss.str();
    }

    inline std::string getMaskedInput() {
        std::string input;
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        char ch;
        while (true) {
            ch = getchar();
            if (ch == '\n' || ch == '\r') {
                break;
            } else if (ch == 127 || ch == 8) {
                if (!input.empty()) {
                    input.pop_back();
                    std::cout << "\b \b" << std::flush;
                }
            } else {
                input.push_back(ch);
                std::cout << '*' << std::flush;
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        std::cout << "\n";
        return input;
    }

    inline bool isValidEmail(const std::string& email) {
        if (email.empty()) return false;
        size_t at = email.find('@');
        if (at == std::string::npos || at == 0) return false;
        size_t dot = email.find('.', at);
        return dot != std::string::npos && dot > at + 1 && dot < email.length() - 1;
    }

    inline bool isValidPhone(const std::string& phone) {
        if (phone.empty() || phone.length() < 7) return false;
        for (char c : phone) {
            if (!std::isdigit(c) && c != '+' && c != '-' && c != ' ') return false;
        }
        return true;
    }
}

#endif
