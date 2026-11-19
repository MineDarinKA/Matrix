#include "ApplicationManager.h"
#include "Console.h"
#include "Figure.h"
#include <algorithm>
#include <cstddef>
#include <chrono>
#include <cstdlib>
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
ArrayList<Line::Clock::time_point> ApplicationManager::generateSpawnSchedule(
    Line::Clock::time_point secondStart
)
{
    ArrayList<Line::Clock::time_point> spawnTimes;

    // Reserves storage for all scheduled line appearances
    spawnTimes.reserve(static_cast<std::size_t>(settings.frequency));

    for (int i = 0; i < settings.frequency; ++i)
    {
        const int offset = std::rand() % 1000;

        spawnTimes.pushBack(
            secondStart + std::chrono::milliseconds(offset)
        );
    }

    // Sorts appearance times without changing their values
    if (spawnTimes.size() > 1)
    {
        std::sort(spawnTimes.begin(), spawnTimes.end());
    }

    return spawnTimes;
}

// Runs the Matrix application
void ApplicationManager::run()
{
    Console::clearScreen();

    auto secondStart = Line::Clock::now();
    auto spawnTimes = generateSpawnSchedule(secondStart);
    std::size_t nextSpawnIndex = 0;

    ArrayList<std::pair<int, int>> changedPositions;

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

            if (activeLine.didExplode())
            {
                createExplosion(
                    activeLine.getNewCharacterX(),
                    activeLine.getNewCharacterY(),
                    currentTime
                );

                activeLine.explode();
            }

            const Figure& figure = activeLine;
            figure.collectChangedPositions(changedPositions);

            updated = true;
        }

        // Updates all explosions independently of lines
        for (Explosion& activeExplosion : explosions)
        {
            if (activeExplosion.update(currentTime))
            {
                const Figure& figure = activeExplosion;
                figure.collectChangedPositions(changedPositions);

                updated = true;
            }
        }

        // Removes lines that have left the screen or been fully destroyed
        for (auto lineIterator = lines.begin();
            lineIterator != lines.end();)
        {
            if (isLineFinished(*lineIterator))
            {
                lineIterator = lines.erase(lineIterator);
            }
            else
            {
                ++lineIterator;
            }
        }

        // Removes finished explosions after collecting their cleared positions
        for (auto explosionIterator = explosions.begin();
            explosionIterator != explosions.end();)
        {
            if (explosionIterator->isFinished())
            {
                explosionIterator = explosions.erase(explosionIterator);
            }
            else
            {
                ++explosionIterator;
            }
        }

        // Redraws each changed position only once
        if (changedPositions.size() > 1)
        {
            std::sort(changedPositions.begin(), changedPositions.end());

            auto uniqueEnd = std::unique(
                changedPositions.begin(),
                changedPositions.end()
            );

            changedPositions.erase(
                uniqueEnd,
                changedPositions.end()
            );
        }

        // Renders every affected console position
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

    // Marks all possible line positions as initially unoccupied
    ArrayList<bool> occupiedPositions;
    occupiedPositions.reserve(static_cast<std::size_t>(positionCount));

    for (int i = 0; i < positionCount; ++i)
    {
        occupiedPositions.pushBack(false);
    }

    for (const Line& activeLine : lines)
    {
        const int activeX = activeLine.getX();

        if (activeX >= 0 && activeX < positionCount)
        {
            occupiedPositions[static_cast<std::size_t>(activeX)] = true;
        }
    }

    // Collects all currently unoccupied line positions
    ArrayList<int> freePositions;
    freePositions.reserve(static_cast<std::size_t>(positionCount));

    for (int candidateX = 0; candidateX < positionCount; ++candidateX)
    {
        if (!occupiedPositions[static_cast<std::size_t>(candidateX)])
        {
            freePositions.pushBack(candidateX);
        }
    }

    int x = 0;

    if (!freePositions.empty())
    {
        const int randomIndex =
            std::rand() % static_cast<int>(freePositions.size());

        x = freePositions[static_cast<std::size_t>(randomIndex)];
    }
    else
    {
        // Reuses an occupied position if no free positions remain
        x = std::rand() % positionCount;
    }

    const int y = Console::getHeight();

    lines.emplaceBack(
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
    int centerX,
    int centerY,
    Line::Clock::time_point startTime
)
{
    explosions.emplaceBack(
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