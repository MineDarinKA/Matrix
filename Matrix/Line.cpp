#include "Line.h"
#include "Console.h"
#include <cstddef>
#include <cstdlib>

// Creates a line with the specified parameters
Line::Line(int length, int x, int y, char epilepsyMode, int speed, Clock::time_point startTime)
    : length(length),
    x(x),
    y(y),
    epilepsyMode(epilepsyMode),
    stepInterval(std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(1.0 / speed))),
    nextMoveTime(startTime),
    removedCharacterY(0),
    characterRemoved(false)
{
}

// Updates the line when its next movement step is due
bool Line::update(Clock::time_point currentTime)
{
    if (currentTime < nextMoveTime)
    {
        return false;
    }

    move();

    // Advance the scheduled time instead of current time to avoid drift
    nextMoveTime += stepInterval;

    return true;
}

// Returns the scheduled time of the next movement step
Line::Clock::time_point Line::getNextMoveTime() const
{
    return nextMoveTime;
}

// Moves the line one step upward
void Line::move()
{
    --y;
    characterRemoved = false;

    // Grow from the head and remove the oldest character when exceeding the length
    characters.push_front(createCharacter());

    if (static_cast<int>(characters.size()) > length)
    {
        removedCharacterY = y + length;
        characters.pop_back();
        characterRemoved = true;
    }
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

// Returns the character at a specified position, or nullptr if absent
const Character* Line::getCharacterAt(int characterX, int characterY) const
{
    // Convert the screen position into an index within the line
    const int characterIndex = characterY - y;

    if (characterIndex < 0 ||
        characterIndex >= static_cast<int>(characters.size()))
    {
        return nullptr;
    }

    if (characterX != x + getCharacterOffset(characterY))
    {
        return nullptr;
    }

    return &characters[static_cast<std::size_t>(characterIndex)];
}

// Renders the line character at the specified position
bool Line::renderAt(int x, int y) const
{
    const Character* character = getCharacterAt(x, y);

    if (character == nullptr)
    {
        return false;
    }

    Console::setCursorPosition(x, y);
    Console::writeCharacter(*character);

    return true;
}

// Records positions changed during the latest movement step
void Line::collectChangedPositions(
    std::vector<std::pair<int, int>>& positions
) const
{
    positions.emplace_back(
        getNewCharacterX(),
        getNewCharacterY()
    );

    if (hasRemovedCharacter())
    {
        positions.emplace_back(
            getRemovedCharacterX(),
            getRemovedCharacterY()
        );
    }
}

const Character& Line::getNewCharacter() const
{
    return characters.front();
}

int Line::getNewCharacterX() const
{
    return x + getCharacterOffset(y);
}

int Line::getNewCharacterY() const
{
    return y;
}

bool Line::hasRemovedCharacter() const
{
    return characterRemoved;
}

int Line::getRemovedCharacterX() const
{
    return x + getCharacterOffset(removedCharacterY);
}

int Line::getRemovedCharacterY() const
{
    return removedCharacterY;
}

// Creates a new character with the current color mode
Character Line::createCharacter() const
{
    char symbol = static_cast<char>('A' + std::rand() % 26);
    CharacterColor color = CharacterColor::Green;

    if (epilepsyMode == 'Y' || epilepsyMode == 'y')
    {
        int colorIndex = std::rand() % 7;
        color = static_cast<CharacterColor>(colorIndex);
    }

    return Character(symbol, color);
}

// Calculates the horizontal offset of a character in the zig-zag line
int Line::getCharacterOffset(int characterY) const
{
    return characterY % 2 == 0 ? 1 : 0;
}