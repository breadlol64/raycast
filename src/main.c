#include <math.h>
#include <stdio.h>
#include <SDL3/SDL.h>

#define CELL_W 50.0
#define SCREEN_W 800
#define SCREEN_H 600

double playerX, playerY;
double playerDirX, playerDirY;
double playerRot;
double moveSpeed;
double rotationSpeed;
double planeX, planeY;
double planeRot;

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
        playerX * CELL_W, playerY * CELL_W,
        10.0f, 10.0f
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderLine(
        renderer, playerX * CELL_W + 5, playerY * CELL_W + 5, 25 * playerDirX+playerX * CELL_W, 25 * playerDirY + playerY * CELL_W
    );
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

    SDL_Window* window = SDL_CreateWindow("raycast", SCREEN_W, SCREEN_H, 0);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderVSync(renderer, 1);

    playerX = 3;
    playerY = 4;

    playerDirX = 1.0;
    playerDirY = 0.0;
    playerRot = 0.0;

    planeX = 0;
    planeY = 0.66; // 66 degrees FOV
    planeRot = 0;

    moveSpeed = 0.03;
    rotationSpeed = 0.1;

    SDL_Event event;
    int running = 1;
    while(running) {
        Uint64 current_frame_start = SDL_GetPerformanceCounter();
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            default:
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        const bool *keys = SDL_GetKeyboardState(NULL);

        if(keys[SDL_SCANCODE_A]) {
            double oldDirX = playerDirX;
            playerDirX = playerDirX * cos(-rotationSpeed) - playerDirY * sin(-rotationSpeed);
            playerDirY = oldDirX * sin(-rotationSpeed) + playerDirY * cos(-rotationSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotationSpeed) - planeY * sin(-rotationSpeed);
            planeY = oldPlaneX * sin(-rotationSpeed) + planeY * cos(-rotationSpeed);
        }
        if(keys[SDL_SCANCODE_D]) {
            double oldDirX = playerDirX;
            playerDirX = playerDirX * cos(rotationSpeed) - playerDirY * sin(rotationSpeed);
            playerDirY = oldDirX * sin(rotationSpeed) + playerDirY * cos(rotationSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotationSpeed) - planeY * sin(rotationSpeed);
            planeY = oldPlaneX * sin(rotationSpeed) + planeY * cos(rotationSpeed);
        }

        if (keys[SDL_SCANCODE_W]) {
            if (!map[(int)(playerY + playerDirY * moveSpeed)][(int)playerX])
                playerY += playerDirY * moveSpeed;
            if (!map[(int)playerY][(int)(playerX + playerDirX * moveSpeed)])
                playerX += playerDirX * moveSpeed;
        }
        if (keys[SDL_SCANCODE_S]) {
            playerX -= playerDirX * moveSpeed;
            playerY -= playerDirY * moveSpeed;
        }

        for(int x = 0; x < SCREEN_W; x++) {
            double cameraX = 2.0 * (double)x / (double)SCREEN_W - 1;
            double rayDirX = playerDirX + planeX * cameraX;
            double rayDirY = playerDirY + planeY * cameraX;

            int mapX = (int)playerX;
            int mapY = (int)playerY;
            double sideDistX, sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
            double perpWallDist;

            int stepX, stepY;

            int hit = 0;
            int side;

            if(rayDirX < 0) {
                stepX = -1;
                sideDistX = (playerX - mapX) * deltaDistX;
            } else {
                stepX = 1;
                sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
            }

            if(rayDirY < 0) {
                stepY = -1;
                sideDistY = (playerY - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
            }

            while(!hit) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                if(map[mapY][mapX]) hit = 1;
            }

            if(side == 0) perpWallDist = (sideDistX - deltaDistX);
            else          perpWallDist = (sideDistY - deltaDistY);

            int lineHeight = (int)(SCREEN_H / perpWallDist);
            int lineStart = -lineHeight / 2 + SCREEN_H / 2;
            if(lineStart < 0) lineStart = 0;
            int lineEnd = lineHeight / 2 + SCREEN_H / 2;
            if(lineEnd >= SCREEN_H) lineEnd = SCREEN_H - 1;
            
            int color = lineHeight * 0.5;
            if (color < 0)   color = 0;
            if (color > 255) color = 255;

            SDL_SetRenderDrawColor(renderer, color, color, color, 255);
            SDL_RenderLine(renderer, x, lineStart, x, lineEnd);
        }

        //drawMap(renderer);
        //drawPlayer(renderer);

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