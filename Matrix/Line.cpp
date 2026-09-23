#include "Line.h"
#include <cstdlib>

Line::Line(int length, int x, int y, char epilepsyMode)
    : length(length),
    x(x),
    y(y),
    epilepsyMode(epilepsyMode)
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

int Line::getCharacterOffset(int index) const
{
    return index % 2 == 0 ? 1 : 0;
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
        CharacterColor color = CharacterColor::Green;

        if (epilepsyMode == 'Y' || epilepsyMode == 'y')
        {
            int colorIndex = std::rand() % 7;
            color = static_cast<CharacterColor>(colorIndex);
        }

        characters.emplace_back(symbol, color);
    }
}