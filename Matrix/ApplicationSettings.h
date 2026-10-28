#pragma once

// Stores application parameters
struct ApplicationSettings
{
    int frequency;
    int speed;
    int length;
    char epilepsyMode;

    // Explosion probability in thousandths
    int explosionProbability;
    int minExplosionRadius;
    int maxExplosionRadius;
};