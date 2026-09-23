#pragma once

#include <memory>
#include "Line.h"

// Manages the Matrix application
class ApplicationManager
{
public:
    ApplicationManager(int speed, int length, char epilepsyMode);

    void run();

private:
    // Creates a new line at a random horizontal position
    void createLine();

    // Renders the newly generated character
    void renderNewCharacter();

    // Clears the character removed from the end
    void clearRemovedCharacter();

    // Checks whether the line has completely left the console
    bool isLineFinished() const;

    int speed;
    int length;
    char epilepsyMode;
    std::unique_ptr<Line> line;
};