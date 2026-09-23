#include "Character.h"

Character::Character(char symbol, CharacterColor color)
    : symbol(symbol), color(color)
{
}

char Character::getSymbol() const
{
    return symbol;
}

CharacterColor Character::getColor() const
{
    return color;
}