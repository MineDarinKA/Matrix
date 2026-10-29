#include "Explosion.h"
#include "Console.h"
#include <algorithm>
#include <cstdlib>

// Chooses the final radius independently for each explosion
Explosion::Explosion(std::size_t sourceLineId, int centerX, int centerY,
    int minRadius, int maxRadius, Clock::time_point startTime)
    : sourceLineId(sourceLineId),
    centerX(centerX),
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
        previousPositions.push_back(entry.first);
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

// Returns the identifier of the line that created this explosion
std::size_t Explosion::getSourceLineId() const
{
    return sourceLineId;
}

// Checks whether the current wave occupies a console position
bool Explosion::occupiesPosition(int x, int y) const
{
    return characters.contains({ x, y });
}

// Renders one character of the current wave if present
bool Explosion::renderAt(int x, int y) const
{
    const auto character = characters.find({ x, y });

    if (character == characters.end())
    {
        return false;
    }

    Console::setCursorPosition(x, y);
    Console::writeCharacter(character->second);
    return true;
}

// Records both the previous and the new wave positions
void Explosion::collectChangedPositions(
    std::vector<std::pair<int, int>>& positions
) const
{
    positions.insert(
        positions.end(),
        previousPositions.begin(),
        previousPositions.end()
    );

    for (const auto& entry : characters)
    {
        positions.push_back(entry.first);
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

void Explosion::addCharacter(int x, int y)
{
    characters.emplace(std::make_pair(x, y), createCharacter());
}

// Uses printable ASCII symbols and random colors independently of epilepsy mode
Character Explosion::createCharacter() const
{
    const char symbol = static_cast<char>('!' + std::rand() % ('~' - '!' + 1));
    const CharacterColor color = static_cast<CharacterColor>(std::rand() % 7);
    return Character(symbol, color);
}
