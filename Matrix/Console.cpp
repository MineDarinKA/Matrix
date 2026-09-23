#include "Console.h"
#include <windows.h>
#include <iostream>

void Console::setCursorPosition(int x, int y)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD position;
    position.X = static_cast<SHORT>(x);
    position.Y = static_cast<SHORT>(y);

    SetConsoleCursorPosition(consoleHandle, position);
}

void Console::setColor(CharacterColor color)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (color)
    {
    case CharacterColor::Green:
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN);
        break;

    case CharacterColor::Red:
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED);
        break;

    case CharacterColor::Blue:
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE);
        break;

    case CharacterColor::Cyan:
        SetConsoleTextAttribute(
            consoleHandle,
            FOREGROUND_GREEN | FOREGROUND_BLUE
        );
        break;

    case CharacterColor::Yellow:
        SetConsoleTextAttribute(
            consoleHandle,
            FOREGROUND_RED | FOREGROUND_GREEN
        );
        break;

    case CharacterColor::Magenta:
        SetConsoleTextAttribute(
            consoleHandle,
            FOREGROUND_RED | FOREGROUND_BLUE
        );
        break;

    case CharacterColor::White:
        SetConsoleTextAttribute(
            consoleHandle,
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
        );
        break;
    }
}

void Console::writeCharacter(const Character& character)
{
    setColor(character.getColor());
    std::cout << character.getSymbol();
}

void Console::clearCharacter(int x, int y)
{
    setCursorPosition(x, y);
    std::cout << ' ';
}