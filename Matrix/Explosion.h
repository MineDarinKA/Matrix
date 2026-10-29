#pragma once

#include <chrono>
#include <cstddef>
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

    Explosion(std::size_t sourceLineId, int centerX, int centerY,
        int minRadius, int maxRadius, Clock::time_point startTime);

    // Advances the wave at a fixed rate of two radius steps per second
    bool update(Clock::time_point currentTime);

    // Indicates that the final wave has disappeared
    bool isFinished() const;

    // Returns the identifier of the line that created this explosion
    std::size_t getSourceLineId() const;

    // Checks whether the current wave occupies a console position
    bool occupiesPosition(int x, int y) const;

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

    std::size_t sourceLineId;
    int centerX;
    int centerY;
    int targetRadius;
    int currentRadius;
    Clock::time_point nextStepTime;
    bool finished;

    std::map<std::pair<int, int>, Character> characters;
    std::vector<std::pair<int, int>> previousPositions;
};
