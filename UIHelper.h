#ifndef UI_HELPER_H
#define UI_HELPER_H

#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

namespace UI {
    inline std::string formatCurrency(double amount) {
        std::stringstream ss;
        ss << "$" << std::fixed << std::setprecision(2) << amount;
        return ss.str();
    }

    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string ITALIC = "\033[3m";
    const std::string UNDERLINE = "\033[4m";
    const std::string HIDE_CURSOR = "\033[?25l";
    const std::string SHOW_CURSOR = "\033[?25h";

    struct Color {
        int r, g, b;
    };

    const Color NEON_CYAN = {0, 243, 255};
    const Color NEON_PINK = {255, 0, 127};
    const Color NEON_GREEN = {57, 255, 20};
    const Color NEON_PURPLE = {189, 0, 255};
    const Color NEON_YELLOW = {255, 230, 0};
    const Color NEON_ORANGE = {255, 110, 0};
    const Color WHITE = {255, 255, 255};
    const Color GRAY = {128, 128, 128};

    inline std::string rgbColor(Color c) {
        return "\033[38;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
    }

    inline std::string rgbBgColor(Color c) {
        return "\033[48;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
    }

    inline Color interpolate(Color start, Color end, float factor) {
        factor = std::max(0.0f, std::min(1.0f, factor));
        return {
            static_cast<int>(start.r + (end.r - start.r) * factor),
            static_cast<int>(start.g + (end.g - start.g) * factor),
            static_cast<int>(start.b + (end.b - start.b) * factor)
        };
    }

    inline void printColored(const std::string& text, Color c, bool newLine = true) {
        std::cout << rgbColor(c) << text << RESET;
        if (newLine) std::cout << "\n";
    }

    inline void printGradient(const std::string& text, Color start, Color end, bool newLine = true) {
        size_t len = text.length();
        if (len == 0) return;
        for (size_t i = 0; i < len; ++i) {
            float factor = static_cast<float>(i) / (len > 1 ? (len - 1) : 1);
            Color c = interpolate(start, end, factor);
            std::cout << rgbColor(c) << text[i];
        }
        std::cout << RESET;
        if (newLine) std::cout << "\n";
    }

    inline void clearScreen() {
        std::cout << "\033[2J\033[H";
    }

    inline void typeText(const std::string& text, Color start, Color end, int delayMs = 12) {
        size_t len = text.length();
        for (size_t i = 0; i < len; ++i) {
            float factor = static_cast<float>(i) / (len > 1 ? (len - 1) : 1);
            Color c = interpolate(start, end, factor);
            std::cout << rgbColor(c) << text[i] << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
        std::cout << RESET << "\n";
    }

    inline void typeText(const std::string& text, Color c, int delayMs = 12) {
        size_t len = text.length();
        std::cout << rgbColor(c);
        for (size_t i = 0; i < len; ++i) {
            std::cout << text[i] << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
        std::cout << RESET << "\n";
    }

    inline void drawBorder(int width, Color c) {
        std::cout << rgbColor(c);
        for (int i = 0; i < width; ++i) std::cout << "═";
        std::cout << RESET << "\n";
    }

    inline void drawBoxHeader(const std::string& title, Color borderC, Color textC, int width = 60) {
        std::cout << rgbColor(borderC) << "╔";
        for (int i = 0; i < width - 2; ++i) std::cout << "═";
        std::cout << "╗\n║" << RESET;

        int spaces = width - 2 - static_cast<int>(title.length());
        int leftSpaces = spaces / 2;
        int rightSpaces = spaces - leftSpaces;

        for (int i = 0; i < leftSpaces; ++i) std::cout << " ";
        std::cout << rgbColor(textC) << BOLD << title << RESET;
        for (int i = 0; i < rightSpaces; ++i) std::cout << " ";

        std::cout << rgbColor(borderC) << "║\n╠";
        for (int i = 0; i < width - 2; ++i) std::cout << "═";
        std::cout << "╣\n" << RESET;
    }

    inline void drawBoxFooter(Color borderC, int width = 60) {
        std::cout << rgbColor(borderC) << "╚";
        for (int i = 0; i < width - 2; ++i) std::cout << "═";
        std::cout << "╝\n" << RESET;
    }

    inline void drawCardLine(const std::string& key, const std::string& val, Color borderC, Color keyC, Color valC, int width = 60) {
        std::cout << rgbColor(borderC) << "║ " << RESET;
        std::cout << rgbColor(keyC) << key << RESET;
        int usedSpaces = 2 + static_cast<int>(key.length() + val.length());
        int spaces = width - 2 - usedSpaces;
        for (int i = 0; i < spaces; ++i) std::cout << " ";
        std::cout << rgbColor(valC) << val << rgbColor(borderC) << " ║\n" << RESET;
    }

    inline void drawCardDivider(Color borderC, int width = 60) {
        std::cout << rgbColor(borderC) << "╟";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "╢\n" << RESET;
    }

    inline void showLoadingBar(const std::string& label, Color c, int durationMs = 800) {
        std::cout << HIDE_CURSOR;
        int width = 30;
        int steps = 20;
        int sleepTime = durationMs / steps;
        
        for (int i = 0; i <= steps; ++i) {
            std::cout << "\r" << rgbColor(c) << " " << label << " [";
            int pos = (i * width) / steps;
            for (int j = 0; j < width; ++j) {
                if (j < pos) std::cout << "█";
                else if (j == pos) std::cout << "▒";
                else std::cout << "░";
            }
            std::cout << "] " << (i * 100) / steps << "%" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
        std::cout << RESET << SHOW_CURSOR << "\n";
    }

    inline void logo() {
        clearScreen();
        std::vector<std::string> art = {
            "  █████╗ ███╗   ██╗████████╗██╗ ██████╗ ██████╗  █████╗ ██╗   ██╗",
            " ██╔══██╗████╗  ██║╚══██╔══╝██║██╔════╝ ██╔══██╗██╔══██╗╚██╗ ██╔╝",
            " ███████║██╔██╗ ██║   ██║   ██║██║  ███╗██████╔╝███████║ ╚████╔╝ ",
            " ██╔══██║██║╚██╗██║   ██║   ██║██║   ██║██╔══██╗██╔══██║  ╚██╔╝  ",
            " ██║  ██║██║ ╚████║   ██║   ██║╚██████╔╝██║  ██║██║  ██║   ██║   ",
            " ╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ",
            "                                                                 ",
            "               [ FUTURISTIC NEON BANKING PORTAL ]                "
        };
        
        Color pink = NEON_PINK;
        Color cyan = NEON_CYAN;
        
        for (size_t i = 0; i < art.size(); ++i) {
            float factor = static_cast<float>(i) / (art.size() - 1);
            Color lineC = interpolate(pink, cyan, factor);
            std::cout << rgbColor(lineC) << art[i] << "\n";
        }
        std::cout << RESET << "\n";
    }
}

#endif
