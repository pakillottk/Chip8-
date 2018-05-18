#include "Display.h"
#include <stdlib.h>

#define DEBUG_SCREEN false

Display::Display( SDL_Window* window, SDL_Renderer* renderer ) {     
    this->window = window;
    this->renderer = renderer;
    clear();
}

Display::~Display() {
}

unsigned Display::transformX( unsigned v ) {
    int h, w;
    SDL_GetWindowSize( window, &w, &h );
    return normalizeX( v ) * w;
}

unsigned Display::transformY( unsigned v ) {
    int h,w;
    SDL_GetWindowSize( window, &w, &h );
    return normalizeY( v ) * h;
}

float Display::normalizeX( unsigned v ) {
    return (1.0f * v) / SCREEN_W;
}

float Display::normalizeY( unsigned v ) {
    return (1.0f * v) / SCREEN_H;
}

SDL_Rect Display::getBoundedSquare( unsigned center_x, unsigned center_y, unsigned size ) {
    unsigned halfSize = size / 2;
    SDL_Rect output;
    
    output.x = center_x - halfSize;
    output.y = center_y - halfSize;
    output.w = size;
    output.h = size;
    
    return output;
}

void Display::clear() {
    for( unsigned i = 0; i < SCREEN_H; i++  ) {
        for( unsigned j = 0; j < SCREEN_W; j++ ) {
            screen_pixels[i][j] = 0;            
        }
    }      
    
    #if DEBUG_SCREEN
        std::cout << "SCREEN CLEARED" << std::endl;
        show_pixelsCLI();
    #endif
}

void Display::updateScreen() {   
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    for( unsigned i = 0; i < SCREEN_H; i++  ) {
        for( unsigned j = 0; j < SCREEN_W; j++ ) {  
            if( screen_pixels[i][j] ) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
                SDL_Rect pixelRect = getBoundedSquare( transformX(j), transformY(i), SCREEN_SCALE );
                SDL_RenderFillRect(renderer, &pixelRect);
                SDL_RenderDrawRect( renderer, &pixelRect );
            }
            
        }
    }    
    SDL_RenderPresent(renderer); 
}


bool Display::drawSprite(uint8_t x, uint8_t y, uint8_t* sprite_data, uint8_t nbytes) {
    uint8_t currentByte;
    uint8_t i;
    unsigned j;
    unsigned xIndex, yIndex;
    char current_bit;
    bool collision = false;
    
    #if DEBUG_SCREEN
        std::cout << std::dec << "Draw at x: " << (int)x << " y: " << (int)y << std::endl;
    #endif

    for( i = 0; i < nbytes; i++ ) {
        yIndex = (y + i)%SCREEN_H;
        currentByte = sprite_data[ i ];        
        for( j = 0; j < 8; j++ ) {
            xIndex = (x + j)%SCREEN_W;
            current_bit = (( currentByte << j ) & 0x80) >> 7;
            
            #if DEBUG_SCREEN
                std::cout << (int)current_bit;
            #endif

            if( 
                current_bit == screen_pixels[yIndex][xIndex] 
                && screen_pixels[yIndex][xIndex] > 0
            ) {
              collision = true;  
            }
            screen_pixels[yIndex][xIndex] = screen_pixels[yIndex][xIndex]^current_bit;                
        }
        #if DEBUG_SCREEN
            std::cout << std::endl;
        #endif
    }
    
    #if DEBUG_SCREEN
        show_pixelsCLI();
    #endif

    return collision;
}

void Display::show_pixelsCLI() {
    std::cout << "================================================" << std::endl;
    for( unsigned i = 0; i < SCREEN_H; i++  ) {
        for( unsigned j = 0; j < SCREEN_W; j++ ) {
            std::cout << (int)screen_pixels[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "================================================" << std::endl;
}

