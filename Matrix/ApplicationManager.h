#pragma once

#include <vector>
#include "Line.h"

// Manages the Matrix application
class ApplicationManager
{
public:
    ApplicationManager(int frequency, int speed, int length, char epilepsyMode);

    void run();

private:
    // Generates sorted random appearance times within one second
    std::vector<Line::Clock::time_point> generateSpawnSchedule(
        Line::Clock::time_point secondStart
    );

    // Creates a new line at a random horizontal position
    void createLine(Line::Clock::time_point startTime);

    // Renders the visible character at a console position
    void renderPosition(int x, int y) const;

    // Checks whether the line has completely left the console
    bool isLineFinished(const Line& activeLine) const;

    int frequency;
    int speed;
    int length;
    char epilepsyMode;
    std::vector<Line> lines;
};