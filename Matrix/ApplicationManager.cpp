#include "ApplicationManager.h"
#include "Console.h"
#include "Figure.h"
#include <algorithm>
#include <cstddef>
#include <chrono>
#include <cstdlib>
#include <unordered_set>
#include <ctime>
#include <utility>
#include <windows.h>

// Creates an application manager with the specified parameters
ApplicationManager::ApplicationManager(const ApplicationSettings& settings)
    : settings(settings)
{
    // Initialize the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// Generates sorted random appearance times within one second
std::vector<Line::Clock::time_point> ApplicationManager::generateSpawnSchedule(
    Line::Clock::time_point secondStart
)
{
    std::vector<Line::Clock::time_point> spawnTimes;
    spawnTimes.reserve(static_cast<std::size_t>(settings.frequency));

    for (int i = 0; i < settings.frequency; ++i)
    {
        int offset = std::rand() % 1000;

        spawnTimes.push_back(
            secondStart + std::chrono::milliseconds(offset)
        );
    }

    std::sort(spawnTimes.begin(), spawnTimes.end());

    return spawnTimes;
}

// Runs the Matrix application
void ApplicationManager::run()
{
    Console::clearScreen();

    auto secondStart = Line::Clock::now();
    auto spawnTimes = generateSpawnSchedule(secondStart);
    std::size_t nextSpawnIndex = 0;

    std::vector<std::pair<int, int>> changedPositions;
    std::unordered_set<std::size_t> movedLineIds;

    while (true)
    {
        const auto currentTime = Line::Clock::now();
        bool updated = false;

        changedPositions.clear();

        // Creates all lines whose scheduled appearance time has passed
        while (true)
        {
            // Keep line generation aligned with fixed one-second intervals
            if (nextSpawnIndex == spawnTimes.size())
            {
                secondStart += std::chrono::seconds(1);
                spawnTimes = generateSpawnSchedule(secondStart);
                nextSpawnIndex = 0;
            }

            if (currentTime < spawnTimes[nextSpawnIndex])
            {
                break;
            }

            createLine(spawnTimes[nextSpawnIndex]);
            ++nextSpawnIndex;
            updated = true;
        }

        // Updates all lines and records changed positions
        for (Line& activeLine : lines)
        {
            if (!activeLine.update(currentTime))
            {
                continue;
            }

            movedLineIds.insert(activeLine.getId());

            const Figure& figure = activeLine;
            figure.collectChangedPositions(changedPositions);

            // Create only the first explosion of each line
            if (activeLine.didExplode())
            {
                createExplosion(
                    activeLine.getId(),
                    activeLine.getNewCharacterX(),
                    activeLine.getNewCharacterY(),
                    currentTime
                );
            }

            updated = true;
        }

        // Updates explosions and checks collisions with their source lines
        for (Explosion& activeExplosion : explosions)
        {
            const bool waveUpdated = activeExplosion.update(currentTime);

            if (waveUpdated)
            {
                const Figure& figure = activeExplosion;
                figure.collectChangedPositions(changedPositions);
                updated = true;
            }

            // Check collisions only when the wave or its line has moved
            if (!waveUpdated &&
                !movedLineIds.contains(activeExplosion.getSourceLineId()))
            {
                continue;
            }

            const auto sourceLine = std::find_if(
                lines.begin(),
                lines.end(),
                [&activeExplosion](const Line& activeLine)
                {
                    return activeLine.getId() ==
                        activeExplosion.getSourceLineId();
                }
            );

            if (sourceLine != lines.end())
            {
                sourceLine->removeCharactersInExplosion(
                    activeExplosion,
                    changedPositions
                );
            }
        }

        // Removes lines that have left the screen or been fully destroyed
        std::erase_if(lines, [this](const Line& activeLine)
            {
                return isLineFinished(activeLine);
            });

        // Removes finished explosions after collecting their cleared positions
        std::erase_if(explosions, [](const Explosion& activeExplosion)
            {
                return activeExplosion.isFinished();
            });

        // Redraws each changed position only once
        std::sort(changedPositions.begin(), changedPositions.end());

        changedPositions.erase(
            std::unique(changedPositions.begin(), changedPositions.end()),
            changedPositions.end()
        );

        for (const auto& position : changedPositions)
        {
            renderPosition(position.first, position.second);
        }

        if (!updated)
        {
            Sleep(1);
        }
    }
}

// Creates a new line at an unoccupied position when possible
void ApplicationManager::createLine(Line::Clock::time_point startTime)
{
    const int consoleWidth = Console::getWidth();

    // Leave space for zig-zag offsets near the right edge
    const int positionCount = consoleWidth > 2 ? consoleWidth - 2 : 1;

    std::vector<bool> occupiedPositions(
        static_cast<std::size_t>(positionCount), false
    );

    for (const Line& activeLine : lines)
    {
        const int activeX = activeLine.getX();

        if (activeX >= 0 && activeX < positionCount)
        {
            occupiedPositions[activeX] = true;
        }
    }

    std::vector<int> freePositions;

    for (int candidateX = 0; candidateX < positionCount; ++candidateX)
    {
        if (!occupiedPositions[candidateX])
        {
            freePositions.push_back(candidateX);
        }
    }

    int x = 0;

    if (!freePositions.empty())
    {
        const int randomIndex = std::rand() % static_cast<int>(freePositions.size());
        x = freePositions[randomIndex];
    }
    else
    {
        // Reuses an occupied position if no free positions remain
        x = std::rand() % positionCount;
    }

    const int y = Console::getHeight();

    lines.emplace_back(
        nextLineId++,
        settings.length,
        x,
        y,
        settings.epilepsyMode,
        settings.speed,
        settings.explosionProbability,
        startTime
    );
}

// Creates an explosion at the specified position
void ApplicationManager::createExplosion(
    std::size_t sourceLineId,
    int centerX,
    int centerY,
    Line::Clock::time_point startTime
)
{
    explosions.emplace_back(
        sourceLineId,
        centerX,
        centerY,
        settings.minExplosionRadius,
        settings.maxExplosionRadius,
        startTime
    );
}

// Renders the visible character at a console position
void ApplicationManager::renderPosition(int x, int y) const
{
    if (x < 0 || x >= Console::getWidth() ||
        y < 0 || y >= Console::getHeight())
    {
        return;
    }

    // Draw explosions over lines, giving newer explosions priority
    for (auto explosionIterator = explosions.rbegin();
        explosionIterator != explosions.rend();
        ++explosionIterator)
    {
        const Figure& figure = *explosionIterator;

        if (figure.renderAt(x, y))
        {
            return;
        }
    }

    // Check newer lines first to preserve correct rendering of overlaps
    for (auto lineIterator = lines.rbegin();
        lineIterator != lines.rend();
        ++lineIterator)
    {
        const Figure& figure = *lineIterator;

        if (figure.renderAt(x, y))
        {
            return;
        }
    }

    Console::clearCharacter(x, y);
}


// Checks whether a line has left the screen or been fully destroyed
bool ApplicationManager::isLineFinished(const Line& activeLine) const
{
    return activeLine.getLength() == 0 ||
        activeLine.getY() + activeLine.getLength() <= 0;
}