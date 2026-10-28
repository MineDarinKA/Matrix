#pragma once

#include <vector>
#include "ApplicationSettings.h"
#include "Line.h"

// Manages the Matrix application
class ApplicationManager
{
public:
    explicit ApplicationManager(const ApplicationSettings& settings);

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

    ApplicationSettings settings;
    std::vector<Line> lines;
};