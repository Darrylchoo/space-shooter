#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include "library.h"
#include "Sprite.h"

bool loadMusic();
bool loadSounds();

Mix_Music * music     = NULL;
Mix_Music * GameMusic = NULL;
Mix_Chunk * StartFX   = NULL;
Mix_Chunk * ShootFX   = NULL;
Mix_Chunk * explosion = NULL;

int noteChannels[2];

//using frame rate concept to delay the animation
const int FPS = 30; //60 Frames Per Second
const int FRAME_DELAY = 1000 / FPS;
const int CHAR_SIZE = 16;
const int TOTAL_ENEMY = 10;

int enemyCount = 10;

int main(int argc, char * args[]) {
    srand(time(0)); //get random value

    SDL_Surface * background     = NULL;
    SDL_Surface * text           = NULL;
    SDL_Surface * background2    = NULL;
    SDL_Surface * background3    = NULL;
    SDL_Surface * spaceship      = NULL;
    SDL_Surface * imgenemy       = NULL;
    SDL_Surface * bullet         = NULL;
    SDL_Surface * bullet1        = NULL;
    SDL_Surface * enemy_explode  = NULL;
    SDL_Surface * player_explode = NULL;

    AnimatedSprite * ship;
    AnimatedSprite * enemy[TOTAL_ENEMY];
    Sprite * b;
    Sprite * b1;

    TTF_Font * font  = NULL;
    TTF_Font * font1 = NULL;

    int x1 = 0, y1 = 0, x2 = 0, y2 = -600;
    int p = 0;
    bool running   = true;
    bool gamestart = false;
    bool shoot     = false;
    bool shoot1    = false;
    char * score;

    Uint8 * keyState = (Uint8 *) SDL_GetKeyboardState(NULL);

    //text location
    SDL_Rect spritePos;
    spritePos.x = 50;
    spritePos.y = 400;

    //initialize game
    if (!initGame("Assignment 2", 10, 10, 800, 600)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Fail to init game system!", NULL);
        SDL_Quit();
        return -1;
    }

    //initialize sound system
    if(!initSoundSystem()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Failed to init sound system!", NULL);
        SDL_Quit();
        return -1;
    }

    //initialize music
    if (!loadMusic()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Failed to sound files", NULL);
        SDL_Quit();
        return -1;
    }

    //initialize the font library
    if (TTF_Init() == -1) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Fail to open font library!", NULL);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    //open the font file
    font  = TTF_OpenFont("fonts/Alfphabet.ttf", 20);
    font1 = TTF_OpenFont("fonts/Alfphabet.ttf", 50);

    if (font == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Fail to open font file!", NULL);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    //load images
    background     = loadImageFile("graphics/startscreen.bmp");
    text           = loadImageFile("graphics/prestart.bmp");
    background2    = loadImageFile("graphics/space.bmp");
    background3    = loadImageFile("graphics/space.bmp");
    spaceship      = loadImageFile("graphics/Player_Ship.bmp");
    imgenemy       = loadImageFile("graphics/Enemy_Ship.bmp");
    bullet         = loadImageFile("graphics/Bullet.bmp");
    bullet1        = loadImageFile("graphics/Bullet.bmp");
    enemy_explode  = loadImageFile("graphics/explosion.bmp");
    player_explode = loadImageFile("graphics/Player_Ship_Explode.bmp");

    if (background     == NULL) return -1;
    if (text           == NULL) return -1;
    if (background2    == NULL) return -1;
    if (background3    == NULL) return -1;
    if (spaceship      == NULL) return -1;
    if (imgenemy       == NULL) return -1;
    if (bullet         == NULL) return -1;
    if (bullet1        == NULL) return -1;
    if (enemy_explode  == NULL) return -1;
    if (player_explode == NULL) return -1;

    //create sprites and animated sprites
    ship = new AnimatedSprite(350, 500, 10, 10, 95, 100, 4, 3, 1);
    b    = new Sprite(350, 700, 0, 50);
    b1   = new Sprite(350, 700, 0, 50);

    //set images for sprites and animated sprites
    ship->setImage(spaceship);
    b   ->setImage(bullet);
    b1  ->setImage(bullet1);

    //setup enemy
    for (int i = 0; i < TOTAL_ENEMY; i++) {
        int x = -(rand() % 500 + 50);
        int y = -(rand() % 400 + 50);

        enemy[i] = new AnimatedSprite(x, y, 5, 5, 49, 46, 3, 3, 1);
        enemy[i]->setImage(imgenemy);
    }

    Mix_PlayMusic(music, -1);

    enemyCount = TOTAL_ENEMY;

    //game loop
    while (running) {
        SDL_Event event;

        if(gamestart == false){
            SDL_BlitSurface(background,NULL, backBuffer, NULL);
            SDL_BlitSurface(text, NULL, backBuffer, &spritePos);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }

                if (event.key.keysym.sym == SDLK_BACKSPACE){
                    Mix_HaltMusic(); //stop music
                }

                if (event.key.keysym.sym == SDLK_m) {
                    if (Mix_PausedMusic()) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                }

                if (event.key.keysym.sym == SDLK_RETURN && gamestart == false) {
                    gamestart = true;
                    noteChannels[0] = Mix_PlayChannel(-1, StartFX, 0);
                    Mix_PlayMusic(GameMusic, -1);
                }

                if (gamestart == true) {
                    if (event.key.keysym.sym == SDLK_SPACE && shoot == false && ship->isAvailable() && p < 500) {
                        b->setX(ship->getX() + 35);
                        b->setY(ship->getY() + 30);
                        b->setVisible(true);

                        shoot = true;

                        noteChannels[1] = Mix_PlayChannel(-1, ShootFX, 0);
                    } else if (event.key.keysym.sym == SDLK_SPACE && shoot1 == false && ship->isAvailable() && p >= 500) {
                        b->setX(ship->getX() + 20);
                        b->setY(ship->getY() + 30);
                        b->setVisible(true);

                        b1->setX(ship->getX() + 51);
                        b1->setY(ship->getY() + 30);
                        b1->setVisible(true);

                        shoot1 = true;

                        noteChannels[1] = Mix_PlayChannel(-1, ShootFX, 0);
                    }
                }
            }
        }

        //we record the first frame timing
        int frameStartTime = SDL_GetTicks();

        //gamestart
        if (gamestart == true) {
            drawImage(background2, backBuffer, x1, y1++);
            drawImage(background3, backBuffer, x2, y2++);

            //parallax background scrolling
            if (y1 >= 600) {
                y1 = -600;
            }

            if (y2 >= 600) {
                y2 = -600;
            }
            //end parallax background scrolling

            //bullet shooting
            if (shoot) {
                b->moveUp();

                if (b->getY() <= -20) {
                    shoot = false;
                }
            }

            if (shoot1) {
                b->moveUp();
                b1->moveUp();

                if (b->getY() <= -20) {
                    shoot1 = false;
                }

                if (b1->getY() <= -20) {
                    shoot1 = false;
                }
            }
            //end bullet shooting

            //player ship movement
            if (keyState[SDL_SCANCODE_UP]) {
                ship->moveUp();
            }

            if (keyState[SDL_SCANCODE_DOWN]) {
                ship->moveDown();
            }

            if (keyState[SDL_SCANCODE_RIGHT]) {
                ship->moveRight();
            }

            if (keyState[SDL_SCANCODE_LEFT]) {
                ship->moveLeft();
            }
            //end player ship movement

            //player ship movement for within the screen
            if (ship->getY() > (600 - spaceship->h)) {
                ship->setY(600 - spaceship->h);
            } else if (ship->getY() < 0) {
                ship->setY(0);
            }

            if (ship->getX() > (800 - spaceship->w / 4)) {
                ship->setX(800 - spaceship->w / 4);
            } else if (ship->getX() < 0) {
                ship->setX(0);
            }
            //end player ship movement for within the screen

            ship->drawImageFrame(backBuffer);
            b   ->draw(backBuffer);
            b1  ->draw(backBuffer);

            //draw enemies and determine their movement
            for (int i = 0; i < TOTAL_ENEMY; i++) {
                enemy[i]->drawImageFrame(backBuffer);

                switch (i) {
                    case 0:
                        enemy[i]->moveDown();
                        if (enemy[i]->getY() > 600) {
                            enemy[i]->setX(rand() % 600);
                            enemy[i]->setY(-100);
                        }
                        break;

                    case 1:
                        enemy[i]->moveLeft();
                        if (enemy[i]->getX() < 0) {
                            enemy[i]->setX(950);
                            enemy[i]->setY(rand() % 500);
                        }
                        break;

                    case 2:
                        enemy[i]->moveRight();
                        if (enemy[i]->getX() > 800) {
                            enemy[i]->setX(-100);
                            enemy[i]->setY(rand() % 500);
                        }
                        break;

                    case 3:
                        enemy[i]->moveXY();
                        if (enemy[i]->getX() >= 800 || enemy[i]->getY() >= 600) {
                            enemy[i]->setX(rand() % 700);
                            enemy[i]->setY(-100);
                        }
                        break;

                    case 4:
                        enemy[i]->moveDown();
                        if (enemy[i]->getY() > 600) {
                            enemy[i]->setX(rand() % 700);
                            enemy[i]->setY(-100);
                        }
                        break;

                    case 5:
                        enemy[i]->moveLeft();
                        if (enemy[i]->getX() < 0) {
                            enemy[i]->setX(850);
                            enemy[i]->setY(rand() % 400);
                        }
                        break;

                    case 6:
                        enemy[i]->moveRight();
                        if (enemy[i]->getX() > 800) {
                            enemy[i]->setX(-100);
                            enemy[i]->setY(rand() % 400);
                        }
                        break;

                    case 7:
                        enemy[i]->moveXY();
                        if (enemy[i]->getX() >= 800 || enemy[i]->getY() >= 600) {
                            enemy[i]->setX(rand() % 700);
                            enemy[i]->setY(-100);
                        }
                        break;

                    case 8:
                        enemy[i]->moveLeft();
                        if (enemy[i]->getX() < 0) {
                            enemy[i]->setX(900);
                            enemy[i]->setY(rand() % 300);
                        }
                        break;

                    case 9:
                        enemy[i]->moveRight();
                        if (enemy[i]->getX() > 800) {
                            enemy[i]->setX(-100);
                            enemy[i]->setY(rand() % 300);
                        }
                        break;
                }
            } //end for loop for drawing enemies and movement

            //check the collision between player ship, enemies and bullets
            for (int i = 0; i < TOTAL_ENEMY; i++) {
                if (enemy[i]->isVisible()) {
                    SDL_Rect rect;
                    rect.x = enemy[i]->getX();
                    rect.y = enemy[i]->getY();
                    rect.w = rect.h = 32;

                    //if hit
                    if (enemy[i]->isAvailable()) {
                        if (b->isHit(rect, 0)) {
                            if (enemy[i]->getY() >= 0 && b->getY() >= 0) {
                                //get the current location
                                int x = enemy[i]->getX();
                                int y = enemy[i]->getY();

                                enemy[i] = new AnimatedSprite(x, y, 0, 0, 32, 32, 6, 3, 1, 0);
                                enemy[i]->setImage(enemy_explode);

                                //set it to false to avoid collision detection
                                enemy[i]->setAvailable(false);

                                b->setVisible(false);
                                b->setY(-15);
                                b1->setVisible(false);
                                b1->setY(-15);

                                noteChannels[2] = Mix_PlayChannel(-1, explosion, 0);

                                p += 100;

                                enemyCount--;
                            }
                        } else if (b1->isHit(rect, 0)) {
                            if (enemy[i]->getY() >= 0 && b1->getY() >= 0) {
                                //get the current location
                                int x = enemy[i]->getX();
                                int y = enemy[i]->getY();

                                enemy[i] = new AnimatedSprite(x, y, 0, 0, 32, 32, 6, 3, 1, 0);
                                enemy[i]->setImage(enemy_explode);

                                //set it to false to avoid collision detection
                                enemy[i]->setAvailable(false);

                                b->setVisible(false);
                                b->setY(-15);
                                b1->setVisible(false);
                                b1->setY(-15);

                                noteChannels[2] = Mix_PlayChannel(-1, explosion, 0);

                                p += 100;

                                enemyCount--;
                            }
                        }

                        if (ship->isAvailable()) {
                            if (ship->isHit(rect, 0)) {
                                //get the current location
                                int x2 = ship->getX();
                                int y2 = ship->getY();

                                ship = new AnimatedSprite(x2, y2, 0, 0, 95, 100, 7, 2, 1, 0);
                                ship->setImage(player_explode);
                                noteChannels[2] = Mix_PlayChannel(-1, explosion, 0);

                                //set it to false to avoid collision detection
                                ship->setAvailable(false);
                            }
                        } else if (!ship->isAvailable()) {
                            drawOutlineText(backBuffer, "GAME OVER", 230, 280, font1, 255, 255, 255);
                        }
                    }
                }
            } //end for loop for collision detection

            //spawn enemies if enemyCount = 0
            if (enemyCount == 0) {
                for (int i = 0; i < TOTAL_ENEMY; i++) {
                    if (enemyCount < TOTAL_ENEMY) {
                        int x = -(rand() % 500 + 50);
                        int y = -(rand() % 400 + 50);

                        enemy[i] = new AnimatedSprite(x, y, 5, 5, 49, 46, 3, 3, 1);
                        enemy[i]->setImage(imgenemy);

                        enemyCount++;
                    }
                }
            }

            std::stringstream ss;
            std::string str;

            ss << p;

            str = ss.str();

            char * cstr = &str[0u];

            score = cstr;

            drawOutlineText(backBuffer, "SCORE:", 20, 20, font, 255, 255, 255);
            drawOutlineText(backBuffer, score, 120, 20, font, 255, 255, 255);
        }
        //end gamestart

        int frameEndTime = SDL_GetTicks();
        int currentDelay = frameEndTime - frameStartTime;

        //to delay the frame rate
        if (currentDelay < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - currentDelay);
        }

        SDL_UpdateWindowSurface(window);
    } //end game loop

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

bool loadMusic(){
    //load musics and sound effects
    music     = Mix_LoadMUS("audio/StartBackground.ogg");
    GameMusic = Mix_LoadMUS("audio/PlayBackground.ogg");
    StartFX   = Mix_LoadWAV("notes/SFX1.wav");
    ShootFX   = Mix_LoadWAV("notes/ShootFX.wav");
    explosion = Mix_LoadWAV("notes/explosion.wav");

    if (music == NULL || GameMusic == NULL || StartFX == NULL || ShootFX == NULL || explosion == NULL) {
        return false;  //failed to load the sound
    }

    return true; //success
}
