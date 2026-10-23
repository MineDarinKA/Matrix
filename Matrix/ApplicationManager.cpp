#include "ApplicationManager.h"
#include "Console.h"
#include <algorithm>
#include <cstddef>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <windows.h>

// Creates an application manager with the specified parameters
ApplicationManager::ApplicationManager(int frequency, int speed, int length, char epilepsyMode)
    : frequency(frequency),
    speed(speed),
    length(length),
    epilepsyMode(epilepsyMode)
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
    spawnTimes.reserve(static_cast<std::size_t>(frequency));

    for (int i = 0; i < frequency; ++i)
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

    while (true)
    {
        const auto currentTime = Line::Clock::now();
        bool updated = false;

        // Creates all lines whose scheduled appearance time has passed
        while (true)
        {
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

        // Updates every active line independently
        for (Line& activeLine : lines)
        {
            if (!activeLine.update(currentTime))
            {
                continue;
            }

            renderNewCharacter(activeLine);
            clearRemovedCharacter(activeLine);
            updated = true;
        }

        // Removes lines that have completely left the console
        std::erase_if(lines, [this](const Line& activeLine)
            {
                return isLineFinished(activeLine);
            });

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
        length, x, y, epilepsyMode, speed, startTime
    );
}

// Renders the newly generated character of a line
void ApplicationManager::renderNewCharacter(const Line& activeLine)
{
    int characterX = activeLine.getNewCharacterX();
    int characterY = activeLine.getNewCharacterY();

    if (characterX >= 0 && characterX < Console::getWidth() &&
        characterY >= 0 && characterY < Console::getHeight())
    {
        Console::setCursorPosition(characterX, characterY);
        Console::writeCharacter(activeLine.getNewCharacter());
    }
}

// Clears the character removed from a line
void ApplicationManager::clearRemovedCharacter(const Line& activeLine)
{
    if (!activeLine.hasRemovedCharacter())
    {
        return;
    }

    int characterX = activeLine.getRemovedCharacterX();
    int characterY = activeLine.getRemovedCharacterY();

    if (characterX >= 0 && characterX < Console::getWidth() &&
        characterY >= 0 && characterY < Console::getHeight())
    {
        Console::clearCharacter(characterX, characterY);
    }
}

// Checks whether a line has completely left the console
bool ApplicationManager::isLineFinished(const Line& activeLine) const
{
    return activeLine.getY() + activeLine.getLength() <= 0;
}