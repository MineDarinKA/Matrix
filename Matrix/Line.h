#pragma once

#include <chrono>
#include <deque>
#include "Character.h"

// Represents a line moving from bottom to top
class Line
{
public:
    using Clock = std::chrono::steady_clock;

    Line(int length, int x, int y, char epilepsyMode, int speed, Clock::time_point startTime);

    // Updates the line when its next movement step is due
    bool update(Clock::time_point currentTime);

    // Returns the scheduled time of the next movement step
    Clock::time_point getNextMoveTime() const;

    int getLength() const;
    int getX() const;
    int getY() const;

    // Returns the character at a specified position, or nullptr if absent
    const Character* getCharacterAt(int characterX, int characterY) const;

    const Character& getNewCharacter() const;
    int getNewCharacterX() const;
    int getNewCharacterY() const;

    bool hasRemovedCharacter() const;
    int getRemovedCharacterX() const;
    int getRemovedCharacterY() const;

private:
    // Moves the line one step upward and updates its characters
    void move();

    // Creates a new character with the current color mode
    Character createCharacter() const;

    // Calculates the horizontal offset of a character in the zig-zag line
    int getCharacterOffset(int characterY) const;

    int length;
    int x;
    int y;
    char epilepsyMode;

    // Time interval between consecutive movement steps
    Clock::duration stepInterval;

    // Scheduled time of the next movement step
    Clock::time_point nextMoveTime;

    std::deque<Character> characters;

    int removedCharacterY;
    bool characterRemoved;
};