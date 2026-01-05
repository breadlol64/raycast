#include <math.h>
#include <stdio.h>
#include <SDL3/SDL.h>

#define CELL_W 64.0f

float playerX, playerY;
float playerDirX, playerDirY;
float playerRot;
float moveSpeed;
float rotationSpeed;

int map[8][8] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,0,0,1,1},
    {1,1,0,0,0,0,0,1},
    {1,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1},
};

void drawPlayer(SDL_Renderer* renderer) {
    SDL_FRect rect = {
        playerX, playerY,
        10.0f, 10.0f
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderLine(renderer, playerX, playerY, 25*playerDirX+playerX, 25*playerDirY+playerY);
}

void drawMap(SDL_Renderer* renderer) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (map[y][x]) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }
            SDL_RenderFillRect(renderer, &(SDL_FRect){ x*50, y*50, 50.0f, 50.0f });
        }
    }
    
}

int main() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("raycast", 800, 600, 0);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderVSync(renderer, 1);

    playerX = 400.0;
    playerY = 300.0;

    playerDirX = 1.0;
    playerDirY = 0.0;
    playerRot = 0.0;

    moveSpeed = 2;
    rotationSpeed = 2;

    SDL_Event event;
    int running = 1;
    while(running) {
        Uint64 current_frame_start = SDL_GetPerformanceCounter();
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                case SDLK_W:
                    playerX += playerDirX * moveSpeed;
                    playerY += playerDirY * moveSpeed;
                    break;
                case SDLK_S:
                    playerX -= playerDirX * moveSpeed;
                    playerY -= playerDirY * moveSpeed;
                    break;
                case SDLK_A:
                    playerRot += rotationSpeed;
                    break;
                case SDLK_D:
                    playerRot -= rotationSpeed;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        playerDirX = sin(playerRot);
        playerDirY = cos(playerRot);

        drawMap(renderer);
        drawPlayer(renderer);

        SDL_RenderPresent(renderer);

        Uint64 current_frame_end = SDL_GetPerformanceCounter();
        float elapsed_seconds = (float)(current_frame_end - current_frame_start) / (float)SDL_GetPerformanceFrequency();

        if (elapsed_seconds > 0) {
            float fps = 1.0f / elapsed_seconds;
            printf("\rFPS: %f ", fps);
            fflush(stdout);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}