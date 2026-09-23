#pragma once

#include <vector>
#include "Character.h"

// Represents a line moving from bottom to top
class Line
{
public:
    Line(int length, int x, int y);

    void move();

    int getLength() const;
    int getX() const;
    int getY() const;
    const std::vector<Character>& getCharacters() const;

private:
    // Creates characters for the line
    void createCharacters();

    // Returns horizontal offset for a character in the zig-zag line
    int getCharacterOffset(int index) const;

    int length;
    int x;
    int y;
    std::vector<Character> characters;
};