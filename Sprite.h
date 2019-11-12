#ifndef _SPRITE_
#define _SPRITE_

#include <SDL.h>

class Sprite {
    public:
        int x;
        int y;
        int speedX, speedY;
        SDL_Surface * image;
        bool visible;
        bool available;

    public:
        //default constructor
        Sprite(): x(0), y(0), speedX(0), speedY(0), visible(true), available(true) {}

        //overloaded constructor
        Sprite(int x, int y, int sx, int sy, bool v = true, bool a = true) {
            this->x   = x;
            this->y   = y;
            speedX    = sx;
            speedY    = sy;
            visible   = v;
            available = a;
        }

        //getters
        int getX() {
            return x;
        }

        int getY() {
            return y;
        }

        int getSpeedX() {
            return speedX;
        }

        int getSpeedY() {
            return speedY;
        }

        SDL_Surface * getImage() {
            return image;
        }

        bool isVisible() {
            return visible;
        }

        bool isAvailable() {
            return available;
        }

        //setters
        void setX(int x) {
            this->x = x;
        }

        void setY(int y) {
            this->y = y;
        }

        void setSpeedX(int sx) {
            speedX = sx;
        }

        void setSpeedY(int sy) {
            speedY = sy;
        }

        void setImage(SDL_Surface * i) {
            image = i;
        }

        void setVisible(bool v) {
            visible = v;
        }

        void setAvailable(bool a) {
            available = a;
        }

        //additional method to move the sprite
        void moveLeft() {
            x = x - speedX;
        }

        void moveRight() {
            x = x + speedX;
        }

        void moveUp() {
            y = y - speedY;
        }

        void moveDown() {
            y = y + speedY;
        }

        void moveXY() {
            x = x + speedX;
            y = y + speedY;
        }

        //drawing sprite
        void draw(SDL_Surface * screen) {
            if (!visible) {
                return; //no need to draw for invisible sprite
            }

            SDL_Rect pos;
            pos.x = x;
            pos.y = y;

            //blit to the screen
            SDL_BlitSurface(image, NULL, screen, &pos);
        }

        //collision detection
        bool isHit(SDL_Rect rect2, int vb) {
            //make your own rectangle
            SDL_Rect rect1;
            rect1.x = this->x;
            rect1.y = this->y;

            //set the rect1 dimension
            rect1.w = image->w;
            rect1.h = image->h;

            //use AABB approach to check for the collision
            if (rect1.x >= rect2.x + rect2.w) {
                return false;
            }

            if (rect1.y >= rect2.y + rect2.h) {
                return false;
            }

            if (rect2.x >= rect1.x + rect1.w) {
                return false;
            }

            if (rect2.y >= rect1.y + rect1.h) {
                return false;
            }

            //if the above four cases not fulfilled, then HIT
            return true;
        }
};

class AnimatedSprite: public Sprite {
    private:
        int width;        //frame width
        int height;       //frame height
        int maxFrame;     //total frame
        int currentFrame; //current frame
        int frameDelay;   //how long to change to next frame
        int frameCounter; //counter purpose
        int direction;    //forward or backward
        int loop;

    public:
        //overloaded constructors
        AnimatedSprite(int x, int y, int sx, int sy, int w, int h, int mf, int fd, int d = 1, int l = 1): Sprite(x, y, sx, sy) {
            width        = w;
            height       = h;
            maxFrame     = mf;
            frameDelay   = fd;
            currentFrame = 0;
            frameCounter = 0;
            direction    = d;
            loop         = l;
        }

        int getFrameWidth() {
            return width;
        }

        int getFrameHeight() {
            return height;
        }

        int getDirection() {
            return direction;
        }

        void setDirection(int d) {
            direction = d;
        }

        void drawImageFrame(SDL_Surface * backBuffer) {
            //check the sprite condition
            if (!visible) {
                return;
            }

            frameCounter++;

            if (frameCounter > frameDelay) {
                frameCounter = 0;

                if (direction == 1) {
                    currentFrame++; //forward
                } else {
                    currentFrame--; //backward
                }
            }

            if (direction == 1) {
                if  (currentFrame > maxFrame - 1) {
                    if (loop == 1) {
                        currentFrame = 0; //first frame
                    } else {
                        visible = false;
                    }
                }
            } else {
                if (currentFrame < 0) {
                    if (loop == 1) {
                        currentFrame = maxFrame - 1;
                    } else {
                        visible = false;
                    }
                }
            }

            extractImageFrame(backBuffer, currentFrame);
        }

        void extractImageFrame(SDL_Surface * backBuffer, int frame) {
            SDL_Rect destRect;
            destRect.x = x;
            destRect.y = y;

            int columns = image->w / width;

            SDL_Rect sourceRect;
            sourceRect.y = (frame / columns) * height;
            sourceRect.x = (frame % columns) * width;
            sourceRect.w = width;
            sourceRect.h = height;

            //start draw the image
            SDL_BlitSurface(image, &sourceRect, backBuffer, &destRect);
        }

        bool isHit(SDL_Rect rect2, int vb) {
            //make your own rectangle
            SDL_Rect rect1;
            rect1.x = this->x;
            rect1.y = this->y;

            //set the rect1 dimension
            rect1.w = width;
            rect1.h = height;

            //use AABB approach to check for the collision
            if (rect1.x >= rect2.x + rect2.w) {
                return false;
            }

            if (rect1.y >= rect2.y + rect2.h) {
                return false;
            }

            if (rect2.x >= rect1.x + rect1.w) {
                return false;
            }

            if (rect2.y >= rect1.y + rect1.h) {
                return false;
            }

            //if the above four cases not fulfilled, then HIT
            return true;
        }
};

#endif // _SPRITE_
