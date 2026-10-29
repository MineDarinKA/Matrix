#pragma once

#include <chrono>
#include <map>
#include <utility>
#include <vector>
#include "Character.h"
#include "Figure.h"

// Represents a radial wave made of random console characters
class Explosion : public Figure
{
public:
    using Clock = std::chrono::steady_clock;

    Explosion(int centerX, int centerY, int minRadius, int maxRadius, Clock::time_point startTime);

    // Advances the wave at a fixed rate of two radius steps per second
    bool update(Clock::time_point currentTime);

    // Indicates that the final wave has disappeared
    bool isFinished() const;

    // Renders one character of the current wave if present
    bool renderAt(int x, int y) const override;

    // Records both the previous and the new wave positions
    void collectChangedPositions(
        std::vector<std::pair<int, int>>& positions
    ) const override;

private:
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

    std::map<std::pair<int, int>, Character> characters;
    std::vector<std::pair<int, int>> previousPositions;
};
