#include "Explosion.h"
#include "Console.h"
#include <algorithm>
#include <cstdlib>

// Chooses the final radius independently for each explosion
Explosion::Explosion(int centerX, int centerY,
    int minRadius, int maxRadius, Clock::time_point startTime)
    : centerX(centerX),
    centerY(centerY),
    targetRadius(minRadius + std::rand() % (maxRadius - minRadius + 1)),
    currentRadius(0),
    nextStepTime(startTime),
    finished(false)
{
}

// Advances the wave at a fixed rate of two radius steps per second
bool Explosion::update(Clock::time_point currentTime)
{
    if (finished || currentTime < nextStepTime)
    {
        return false;
    }

    previousPositions.clear();

    for (const auto& entry : characters)
    {
        previousPositions.emplaceBack(entry.x, entry.y);
    }

    characters.clear();

    if (currentRadius == targetRadius)
    {
        finished = true;
    }
    else
    {
        ++currentRadius;
        generateWave();
    }

    // Keep each radius visible for 500 milliseconds
    nextStepTime += std::chrono::milliseconds(500);
    return true;
}

bool Explosion::isFinished() const
{
    return finished;
}

// Renders one character of the current wave if present
bool Explosion::renderAt(int x, int y) const
{
    // Searches for a character at the requested position
    for (const auto& entry : characters)
    {
        if (entry.x == x && entry.y == y)
        {
            Console::setCursorPosition(x, y);
            Console::writeCharacter(entry.character);
            return true;
        }
    }

    return false;
}

// Records both the previous and the new wave positions
void Explosion::collectChangedPositions(
    ArrayList<std::pair<int, int>>& positions
) const
{
    // Records positions occupied by the previous wave
    for (const auto& position : previousPositions)
    {
        positions.pushBack(position);
    }

    // Records positions occupied by the current wave
    for (const auto& entry : characters)
    {
        positions.emplaceBack(entry.x, entry.y);
    }
}

// Generates a wave outline
void Explosion::generateWave()
{
    const int capWidth = currentRadius / 2;

    for (int offsetY = -currentRadius; offsetY <= currentRadius; ++offsetY)
    {
        const int extent = currentRadius -
            std::max(0, std::abs(offsetY) - capWidth);

        if (std::abs(offsetY) == currentRadius)
        {
            for (int offsetX = -extent; offsetX <= extent; ++offsetX)
            {
                addCharacter(centerX + offsetX, centerY + offsetY);
            }
        }
        else
        {
            addCharacter(centerX - extent, centerY + offsetY);
            addCharacter(centerX + extent, centerY + offsetY);
        }
    }
}

// Adds a character only if its position is not already occupied
void Explosion::addCharacter(int x, int y)
{
    // Prevents duplicate positions within the same explosion wave
    for (const auto& entry : characters)
    {
        if (entry.x == x && entry.y == y)
        {
            return;
        }
    }

    characters.pushBack(
        ExplosionCharacter{ x, y, createCharacter() }
    );
}

// Uses printable ASCII symbols and random colors independently of epilepsy mode
Character Explosion::createCharacter() const
{
    const char symbol = static_cast<char>('!' + std::rand() % ('~' - '!' + 1));
    const CharacterColor color = static_cast<CharacterColor>(std::rand() % 7);
    return Character(symbol, color);
}
