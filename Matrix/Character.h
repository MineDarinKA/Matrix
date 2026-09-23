#pragma once

// Defines available character colors
enum class CharacterColor
{
    Green
};

// Represents one matrix character
class Character
{
public:
    Character(char symbol, CharacterColor color);

    char getSymbol() const;
    CharacterColor getColor() const;

private:
    char symbol;
    CharacterColor color;
};