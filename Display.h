/* 
 * File:   Display.h
 * Author: USUARIO
 *
 * Created on 17 de mayo de 2018, 23:49
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <iostream>

const unsigned int SCREEN_W = 64;
const unsigned int SCREEN_H = 32;
const unsigned int SCREEN_SCALE = 10;
class Display {
    protected:
        char screen_pixels[SCREEN_H][SCREEN_W];
        SDL_Window* window;
        SDL_Renderer* renderer;
        
        float normalizeX( unsigned v );
        float normalizeY( unsigned v );
        unsigned transformX( unsigned v );
        unsigned transformY( unsigned v );
        SDL_Rect getBoundedSquare( unsigned center_x, unsigned center_y, unsigned size );
    public:
        Display( SDL_Window* window, SDL_Renderer* renderer );
        virtual ~Display();
        void clear();
        bool drawSprite( uint8_t x, uint8_t y, uint8_t* sprite_data, uint8_t nbytes );
        void show_pixelsCLI();
        void updateScreen();
};

#endif	/* DISPLAY_H */

