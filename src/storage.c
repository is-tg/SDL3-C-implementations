#include "storage.h"

#define SAVE_FILE_NAME "save0.sav"
#define APP_ID "osu-clone"
#define APP_NAME "Circle Game"

bool LoadHighScore(int *outScore)
{
    SDL_Storage *user = SDL_OpenUserStorage(APP_ID, APP_NAME, 0);
    if (!user)
        return false;

    while (!SDL_StorageReady(user)) {
        SDL_Delay(1);
    }

    Uint64 size = 0;
    bool success = false;

    if (SDL_GetStorageFileSize(user, SAVE_FILE_NAME, &size) && size == sizeof(int)) {
        int temp = 0;
        if (SDL_ReadStorageFile(user, SAVE_FILE_NAME, &temp, sizeof(int))) {
            *outScore = temp;
            success = true;
        }
    }

    SDL_CloseStorage(user);
    return success;
}

bool SaveHighScore(int score)
{
    SDL_Storage *user = SDL_OpenUserStorage(APP_ID, APP_NAME, 0);
    if (!user)
        return false;

    while (!SDL_StorageReady(user)) {
        SDL_Delay(1);
    }

    bool success = SDL_WriteStorageFile(user, SAVE_FILE_NAME, &score, sizeof(int));
    SDL_CloseStorage(user);
    return success;
}
