#pragma once

#include <vector>
#include "Character.h"

// Represents a line moving from bottom to top
class Line
{
public:
    Line(int length, int x, int y, char epilepsyMode);

    void move();

    int getLength() const;
    int getX() const;
    int getY() const;
    int getCharacterOffset(int index) const;
    const std::vector<Character>& getCharacters() const;

private:
    // Creates characters for the line
    void createCharacters();

    int length;
    int x;
    int y;
    char epilepsyMode;
    std::vector<Character> characters;
};