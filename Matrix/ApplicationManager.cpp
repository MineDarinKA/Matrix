#include "ApplicationManager.h"
#include "Console.h"
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

// Runs the Matrix application
void ApplicationManager::run()
{
    Console::clearScreen();

    // Sets a temporary uniform interval between line generations
    const auto spawnInterval = std::chrono::duration_cast<Line::Clock::duration>(
        std::chrono::duration<double>(1.0 / frequency)
    );

    auto nextSpawnTime = Line::Clock::now();

    while (true)
    {
        const auto currentTime = Line::Clock::now();
        bool updated = false;

        // Creates all lines scheduled up to the current moment
        while (currentTime >= nextSpawnTime)
        {
            createLine(nextSpawnTime);
            nextSpawnTime += spawnInterval;
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

// Creates a new line at a random horizontal position
void ApplicationManager::createLine(Line::Clock::time_point startTime)
{
    int consoleWidth = Console::getWidth();
    int x = 0;

    if (consoleWidth > 2)
    {
        x = std::rand() % (consoleWidth - 2);
    }

    int y = Console::getHeight();

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