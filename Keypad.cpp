#include "Keypad.h"
#include <stdlib.h>
#include <iostream>

#define DEBUG_KEYS false

Keypad::Keypad() {
    for( int i = 0; i < 16; i++ ) {
        keyState[i] = 0;
    }
}

Keypad::~Keypad() {
    
}

char Keypad::getKeyState(uint8_t key) {
    return keyState[ key ];
}

char Keypad::update() {
    SDL_Event event;
     while( SDL_PollEvent( &event ) ){
        switch( event.type ){
          case SDL_KEYDOWN:
          case SDL_KEYUP:
              return handleKeyEvent( &event.key );
        }
      }
    
    return 0;
}

const unsigned keyMap[0x10] = {
 //     0       1      2       3
    SDLK_2, SDLK_q, SDLK_w, SDLK_e,
 //     4       5      6       7
    SDLK_a, SDLK_s, SDLK_d, SDLK_z,
 //     8       9      A       B
    SDLK_x, SDLK_c, SDLK_1, SDLK_3,
 //     C      D      E       F
    SDLK_r, SDLK_f, SDLK_v, SDLK_4
};

char Keypad::handleKeyEvent(SDL_KeyboardEvent* key) {
    //std::cout << SDL_GetKeyName( key->keysym.sym ) << " is " << (key->type == SDL_KEYUP ? "released" : "pressed") << std::endl;  
    short i;
    char released = key->type == SDL_KEYUP;
    switch( key->keysym.sym ) {
        case SDLK_RETURN: {
            if( !released ) {
                return 'p';
            }
            break;
        }
        case SDLK_ESCAPE: {
            if( !released ) {
                return 'e';
            }
            break;
        }        
        case SDLK_SPACE: {
            if( !released ) {
                return 'r';
            }
            break;
        }
        default: {
            for( i = 0; i < 0x10; i++ ) {
                if( key->keysym.sym == keyMap[ i ] ) {
                    keyState[ i ] = !released;
                }
            }
        }
    }
    
    #if DEBUG_KEYS
        print_keys();
    #endif
    
    return 0;
}

void Keypad::print_keys() {
    for( unsigned i = 0; i < 16; i++ ) {
        std::cout << (int)keyState[i];                         
    }
    std::cout << std::endl;
}