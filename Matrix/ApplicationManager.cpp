#include "ApplicationManager.h"
#include "Console.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <thread>

// Creates an application manager with the specified parameters
ApplicationManager::ApplicationManager(int speed, int length, char epilepsyMode)
    : speed(speed),
    length(length),
    epilepsyMode(epilepsyMode),
    line(nullptr)
{
    // Initialize the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// Runs the Matrix application
void ApplicationManager::run()
{
    while (true)
    {
        createLine();

        while (!isLineFinished())
        {
            line->move();

            renderNewCharacter();
            clearRemovedCharacter();

            // Sets the line step frequency according to the specified speed
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1000 / speed)
            );
        }

        line.reset();
    }
}

// Creates a new line at a random horizontal position
void ApplicationManager::createLine()
{
    int consoleWidth = Console::getWidth();
    int x = 0;

    if (consoleWidth > 1)
    {
        x = std::rand() % (consoleWidth - 1);
    }

    int y = Console::getHeight();

    line = std::make_unique<Line>(length, x, y, epilepsyMode);
}

// Renders the newly generated character
void ApplicationManager::renderNewCharacter()
{
    int characterY = line->getNewCharacterY();

    if (characterY >= 0 && characterY < Console::getHeight())
    {
        Console::setCursorPosition(
            line->getNewCharacterX(),
            characterY
        );

        Console::writeCharacter(line->getNewCharacter());
    }
}

// Clears the character removed from the end
void ApplicationManager::clearRemovedCharacter()
{
    if (!line->hasRemovedCharacter())
    {
        return;
    }

    int characterY = line->getRemovedCharacterY();

    if (characterY >= 0 && characterY < Console::getHeight())
    {
        Console::clearCharacter(
            line->getRemovedCharacterX(),
            characterY
        );
    }
}

// Checks whether the line has completely left the console
bool ApplicationManager::isLineFinished() const
{
    return line->getY() + line->getLength() <= 0;
}