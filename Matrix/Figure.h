#pragma once

#include <utility>
#include <vector>

// Defines the common interface for rendering console figures
class Figure
{
public:
    virtual ~Figure() = default;

    // Renders a character at the position and returns true if occupied
    virtual bool renderAt(int x, int y) const = 0;

    // Records positions affected by the latest figure update
    virtual void collectChangedPositions(
        std::vector<std::pair<int, int>>& positions
    ) const = 0;
};

