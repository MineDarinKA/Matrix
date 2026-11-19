#pragma once

#include <chrono>
#include <utility>
#include "ArrayList.h"
#include "Character.h"
#include "Figure.h"

// Represents a radial wave made of random console characters
class Explosion : public Figure
{
public:
    using Clock = std::chrono::steady_clock;

    Explosion(int centerX, int centerY,
        int minRadius, int maxRadius, Clock::time_point startTime);

    // Advances the wave at a fixed rate of two radius steps per second
    bool update(Clock::time_point currentTime);

    // Indicates that the final wave has disappeared
    bool isFinished() const;

    // Renders one character of the current wave if present
    bool renderAt(int x, int y) const override;

    // Records both the previous and the new wave positions
    void collectChangedPositions(
        ArrayList<std::pair<int, int>>& positions
    ) const override;

private:
    // Stores one explosion character and its console position
    struct ExplosionCharacter
    {
        int x;
        int y;
        Character character;
    };

    // Generates a wave outline
    void generateWave();

    void addCharacter(int x, int y);
    Character createCharacter() const;

    int centerX;
    int centerY;
    int targetRadius;
    int currentRadius;
    Clock::time_point nextStepTime;
    bool finished;

    ArrayList<ExplosionCharacter> characters;
    ArrayList<std::pair<int, int>> previousPositions;
};
