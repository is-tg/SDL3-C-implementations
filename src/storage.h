#ifndef STORAGE_H
#define STORAGE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

bool LoadHighScore(int *outScore);
bool SaveHighScore(int score);

#endif // STORAGE_H
