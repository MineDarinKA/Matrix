#include "Line.h"
#include "Console.h"
#include "Explosion.h"
#include <cstddef>
#include <cstdlib>

// Creates a line with the specified parameters
Line::Line(std::size_t id, int length, int x, int y, char epilepsyMode,
    int speed, int explosionProbability, Clock::time_point startTime)
    : id(id),
    length(length),
    x(x),
    y(y),
    epilepsyMode(epilepsyMode),
    explosionProbability(explosionProbability),
    stepInterval(std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(1.0 / speed))),
    nextMoveTime(startTime),
    exploded(false),
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

    move();
    explosionTriggered = false;

    // A line can explode only once
    if (!exploded && std::rand() % 1000 < explosionProbability)
    {
        exploded = true;
        explosionTriggered = true;

        // Remove the head immediately at the explosion center
        characters.front().reset();
        removedPositions.emplace_back(
            getNewCharacterX(),
            getNewCharacterY()
        );

        --length;
        trimCharacters();
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

std::size_t Line::getId() const
{
    return id;
}

// Moves the line one step upward
void Line::move()
{
    --y;
    removedPositions.clear();

    // Grow from the head and remove excess characters from the tail
    characters.push_front(createCharacter());
    trimCharacters();
}

// Removes excess tail characters and records their positions
void Line::trimCharacters()
{
    while (characters.size() > static_cast<std::size_t>(length))
    {
        const int removedY = y + static_cast<int>(characters.size()) - 1;
        const int removedX = x + getCharacterOffset(removedY);

        removedPositions.emplace_back(removedX, removedY);
        characters.pop_back();
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

    const auto& character = characters[static_cast<std::size_t>(characterIndex)];

    if (!character.has_value())
    {
        return nullptr;
    }

    return &character.value();
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

    positions.insert(
        positions.end(),
        removedPositions.begin(),
        removedPositions.end()
    );
}

// Removes this line's characters touched by its explosion wave
void Line::removeCharactersInExplosion(
    const Explosion& explosion,
    std::vector<std::pair<int, int>>& changedPositions
)
{
    for (std::size_t index = 0; index < characters.size(); ++index)
    {
        if (!characters[index].has_value())
        {
            continue;
        }

        const int characterY = y + static_cast<int>(index);
        const int characterX = x + getCharacterOffset(characterY);

        if (explosion.occupiesPosition(characterX, characterY))
        {
            characters[index].reset();
            changedPositions.emplace_back(characterX, characterY);
        }
    }
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