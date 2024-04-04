#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#define BORDER_WIDTH 5

typedef struct {
    SDL_Window* window;
    const char* title;
} SDL_WindowInfo;

SDL_WindowInfo windows[10]; // Adjust the size as needed

int numWindows = 0;

// Function to draw a border around a window
void drawBorder(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_Rect borderRect = {0, 0, 0, 0};
    SDL_GetWindowSize(window, &borderRect.w, &borderRect.h);
    borderRect.w -= 1;
    borderRect.h -= 1;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for border
    SDL_RenderDrawRect(renderer, &borderRect);
    SDL_RenderPresent(renderer);
}

// Function to find a window by title
SDL_Window* findWindowByTitle(const char* title) {
    for (int i = 0; i < numWindows; ++i) {
        if (strcmp(windows[i].title, title) == 0) {
            return windows[i].window;
        }
    }
    return NULL;
}

// Function to add a window to the list
void addWindow(SDL_Window* window, const char* title) {
    if (numWindows >= sizeof(windows) / sizeof(windows[0])) {
        fprintf(stderr, "Exceeded maximum number of windows\n");
        exit(1);
    }
    windows[numWindows].window = window;
    windows[numWindows].title = title;
    numWindows++;
}

// Function to remove a window from the list
void removeWindow(SDL_Window* window) {
    for (int i = 0; i < numWindows; ++i) {
        if (windows[i].window == window) {
            // Shift remaining windows to fill the gap
            for (int j = i; j < numWindows - 1; ++j) {
                windows[j] = windows[j + 1];
            }
            numWindows--;
            break;
        }
    }
}

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    printf("Preloader application started\n");

    // Find the window of the target application by its title
    SDL_Window* targetWindow = findWindowByTitle("VLC media player");
    if (!targetWindow) {
        fprintf(stderr, "Target window not found!\n");
        SDL_Quit();
        return 1;
    }

    // Create a renderer for the target window
    SDL_Renderer* targetRenderer = SDL_CreateRenderer(targetWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!targetRenderer) {
        fprintf(stderr, "Renderer could not be created for target window! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Draw border around the target window
    drawBorder(targetWindow, targetRenderer);

    // Event loop
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    // Cleanup
    removeWindow(targetWindow);
    SDL_DestroyRenderer(targetRenderer);
    SDL_Quit();

    return 0;
}
