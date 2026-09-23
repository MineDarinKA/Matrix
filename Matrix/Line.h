#pragma once

#include <deque>
#include "Character.h"

// Represents a line moving from bottom to top
class Line
{
public:
    Line(int length, int x, int y, char epilepsyMode);

    void move();

    int getLength() const;
    int getY() const;

    const Character& getNewCharacter() const;
    int getNewCharacterX() const;
    int getNewCharacterY() const;

    bool hasRemovedCharacter() const;
    int getRemovedCharacterX() const;
    int getRemovedCharacterY() const;

private:
    // Creates a new character with the current color mode
    Character createCharacter() const;

    // Calculates the horizontal offset of a character in the zig-zag line
    int getCharacterOffset(int characterY) const;

    int length;
    int x;
    int y;
    char epilepsyMode;

    std::deque<Character> characters;

    int removedCharacterY;
    bool characterRemoved;
};