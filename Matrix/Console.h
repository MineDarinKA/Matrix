#pragma once

#include "Character.h"

// Provides functions for console output and cursor control
class Console
{
public:
    static void setCursorPosition(int x, int y);
    static void setColor(CharacterColor color);
    static void writeCharacter(const Character& character);
    static void clearCharacter(int x, int y);

    static int getWidth();
    static int getHeight();
};