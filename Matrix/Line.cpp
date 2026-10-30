#include "Line.h"
#include "Console.h"
#include <cstddef>
#include <cstdlib>

// Creates a line with the specified parameters
Line::Line(int length, int x, int y, char epilepsyMode,
    int speed, int explosionProbability, Clock::time_point startTime)
    : length(length),
    x(x),
    y(y),
    epilepsyMode(epilepsyMode),
    explosionProbability(explosionProbability),
    stepInterval(std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(1.0 / speed))),
    nextMoveTime(startTime),
    explosionTriggered(false)
{
}

// Updates the line when its next movement step is due
bool Line::update(Clock::time_point currentTime)
{
    if (length == 0 || currentTime < nextMoveTime)
    {
        return false;
    }

    removedPositions.clear();
    explosionTriggered = false;

    move();

    // Check the explosion probability at every movement step
    if (std::rand() % 1000 < explosionProbability)
    {
        explosionTriggered = true;
    }

    // Advance the scheduled time instead of current time to avoid drift
    nextMoveTime += stepInterval;

    return true;
}

// Returns the scheduled time of the next movement step
Line::Clock::time_point Line::getNextMoveTime() const
{
    return nextMoveTime;
}

// Returns whether the latest movement step triggered an explosion
bool Line::didExplode() const
{
    return explosionTriggered;
}

// Destroys the current head and reduces the maximum length
void Line::explode()
{
    removedPositions.emplace_back(
        getNewCharacterX(),
        getNewCharacterY()
    );

    characters.pop_front();
    ++y;
    --length;
}

// Moves the line one step upward
void Line::move()
{
    --y;

    characters.push_front(createCharacter());

    if (characters.size() > static_cast<std::size_t>(length))
    {
        removeTail();
    }
}

// Removes the last character of the line
void Line::removeTail()
{
    const int tailY = y + static_cast<int>(characters.size()) - 1;
    const int tailX = x + getCharacterOffset(tailY);

    removedPositions.emplace_back(tailX, tailY);
    characters.pop_back();
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
    if (!characters.empty())
    {
        positions.emplace_back(
            getNewCharacterX(),
            getNewCharacterY()
        );
    }

    positions.insert(
        positions.end(),
        removedPositions.begin(),
        removedPositions.end()
    );
}

int Line::getNewCharacterX() const
{
    return x + getCharacterOffset(y);
}

int Line::getNewCharacterY() const
{
    return y;
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