#include "Line.h"

Line::Line(int length, int x, int y)
    : length(length), x(x), y(y)
{
    createCharacters();
}

void Line::move()
{
    --y;
}

int Line::getLength() const
{
    return length;
}

int Line::getX() const
{
    return x;
}

int Line::getY() const
{
    return y;
}

const std::vector<Character>& Line::getCharacters() const
{
    return characters;
}

void Line::createCharacters()
{
    characters.clear();

    for (int index = 0; index < length; ++index)
    {
        char symbol = static_cast<char>('A' + index % 26);
        characters.emplace_back(symbol, CharacterColor::Green);
    }
}

int Line::getCharacterOffset(int index) const
{
    return index % 2 == 0 ? 1 : 0;
}