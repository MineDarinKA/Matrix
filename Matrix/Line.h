#pragma once

#include <chrono>
#include <cstddef>
#include <deque>
#include <optional>
#include <utility>
#include <vector>
#include "Character.h"
#include "Figure.h"

class Explosion;

// Represents a line moving from bottom to top
class Line : public Figure
{
public:
    using Clock = std::chrono::steady_clock;

    Line(std::size_t id, int length, int x, int y, char epilepsyMode,
        int speed, int explosionProbability, Clock::time_point startTime);

    // Renders the line character at the specified position
    bool renderAt(int x, int y) const override;

    // Records positions changed during the latest movement step
    void collectChangedPositions(
        std::vector<std::pair<int, int>>& positions
    ) const override;

    // Updates the line when its next movement step is due
    bool update(Clock::time_point currentTime);

    // Returns the scheduled time of the next movement step
    Clock::time_point getNextMoveTime() const;

    // Returns whether the latest movement step triggered an explosion
    bool didExplode() const;

    // Removes this line's characters touched by its explosion wave
    void removeCharactersInExplosion(
        const Explosion& explosion,
        std::vector<std::pair<int, int>>& changedPositions
    );

    std::size_t getId() const;
    int getLength() const;
    int getX() const;
    int getY() const;

    // Returns the character at a specified position, or nullptr if absent
    const Character* getCharacterAt(int characterX, int characterY) const;

    int getNewCharacterX() const;
    int getNewCharacterY() const;

private:
    // Moves the line one step upward and updates its characters
    void move();

    // Removes excess tail characters and records their positions
    void trimCharacters();

    // Creates a new character with the current color mode
    Character createCharacter() const;

    // Calculates the horizontal offset of a character in the zig-zag line
    int getCharacterOffset(int characterY) const;
    
    std::size_t id;
    int length;
    int x;
    int y;
    char epilepsyMode;
    int explosionProbability;

    // Time interval between consecutive movement steps
    Clock::duration stepInterval;

    // Scheduled time of the next movement step
    Clock::time_point nextMoveTime;

    // Empty slots preserve positions of characters destroyed by the wave
    std::deque<std::optional<Character>> characters;

    // Positions of characters removed during the latest step
    std::vector<std::pair<int, int>> removedPositions;

    bool exploded;
    bool explosionTriggered;
};