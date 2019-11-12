#ifndef _LIBRARY_
#define _LIBRARY_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>

//global variable
SDL_Window * window = NULL; //window object
SDL_Surface * backBuffer = NULL; //surface

//function prototype
bool initGame(const char * title, int x, int y, int w, int h);

void fillRectangle(SDL_Surface * buffer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);

//game loop
bool gameIsRunning();

//load optimized image
SDL_Surface * loadImageFile(const char * file);

//draw image is used to blit image to screen
void drawImage(SDL_Surface *, SDL_Surface *, int x, int y);

bool initSoundSystem();

void drawImageFrame(SDL_Surface * image, SDL_Surface * dest, int x, int y, int width, int height, int frame);

void drawRasterFont(SDL_Surface * surface, SDL_Surface * destSurface, char text[], int x, int y, int charSize);

void drawRect(SDL_Surface * surface, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b);

void drawOutlineText(SDL_Surface * backBuffer, char * text, int x, int y, TTF_Font * font, int r, int g, int b);

//function body
bool initGame(const char * title, int x, int y, int w, int h) {
    //initialize the SDL system
    int result = SDL_Init(SDL_INIT_EVERYTHING);

    if (result < 0) {
        return false;
    }

    //initialize the window
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        return false;
    }

    //get the surface from window object
    backBuffer = SDL_GetWindowSurface(window);

    return true;
}

void fillRectangle(SDL_Surface * buffer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
    //draw a simple color to the window
    Uint32 color = SDL_MapRGB(buffer->format, r, g, b);

    SDL_Rect rect; //rectangle object

    //initialize the rectangle
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_FillRect(buffer, &rect, color);
}

//this is a game loop
bool gameIsRunning() {
    SDL_Event event;

    bool running = true; //end the game loop

    //check any event triggered by users
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
    }

    return running;
}

SDL_Surface * loadImageFile(const char * file){
    SDL_Surface * image = NULL;
    SDL_Surface * processedImage = NULL;

    //load the image
    image = IMG_Load(file);

    if (image == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Fail to get image!", NULL);
    } else {
        //create optimum image
        processedImage = SDL_ConvertSurface(image, backBuffer->format, 0);

        //remove first image
        SDL_FreeSurface(image);

        if(processedImage != NULL) {
            //set color key
            Uint32 colorKey = SDL_MapRGB(processedImage->format, 255, 0, 255);

            //color key to transparency color
            SDL_SetColorKey(processedImage,SDL_TRUE,colorKey);
        }
    }

    return processedImage;
}

void drawImage(SDL_Surface * image, SDL_Surface * screen, int x, int y) {
    //define the sprite location
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    //write the image to the screen
    SDL_BlitSurface(image, NULL, screen, &dest);
}

bool initSoundSystem() {
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "SDL", "Failed to open audio device!", NULL);

        return false;
    }

    return true;
}

void drawImageFrame(SDL_Surface * image, SDL_Surface * dest, int x, int y, int width, int height, int frame) {
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    int columns = image->w / width;

    SDL_Rect sourceRect;
    sourceRect.y = (frame / columns) * height;
    sourceRect.x = (frame % columns) * width;
    sourceRect.w = width;
    sourceRect.h = height;

    SDL_BlitSurface(image, &sourceRect, dest, &destRect);
}

void drawRasterFont(SDL_Surface * surface, SDL_Surface * destSurface, char text[], int x, int y, int charSize) {
    for (int i = 0; i < strlen(text); i++) {
        drawImageFrame(surface, destSurface, x + i * charSize, y, charSize, charSize, text[i] - 32);
    }
}

void drawRect(SDL_Surface * surface, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b) {
    fillRectangle(surface, x, y, width, 1, r, g, b);
    fillRectangle(surface, x, y + height - 1, width, 1, r, g, b);
    fillRectangle(surface, x, y, 1, height, r, g, b);
    fillRectangle(surface, x + width - 1, y, 1, height, r, g, b);
}

void drawOutlineText(SDL_Surface * backBuffer, char * text, int x, int y, TTF_Font * font, int r, int g, int b) {
    SDL_Surface * renderedText = NULL;
    SDL_Color color;

    if (font == NULL) return;

    color.r = r;
    color.g = g;
    color.b = b;

    //create the font surface
    renderedText = TTF_RenderText_Solid(font, text, color);

    SDL_Rect pos;
    pos.x = x;
    pos.y = y;

    SDL_BlitSurface(renderedText, NULL, backBuffer, &pos);
    SDL_FreeSurface(renderedText);
}

#endif // _LIBRARY_
