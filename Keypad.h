/* 
 * File:   Keypad.h
 * Author: USUARIO
 *
 * Created on 18 de mayo de 2018, 3:54
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

#include <stdint.h>
#include <SDL2/SDL.h>
class Keypad {
    protected:
        char keyState[16]; //0 - up, 1 - down
        char handleKeyEvent( SDL_KeyboardEvent* key );
    public:
        Keypad();
        virtual ~Keypad();
        char update();
        char getKeyState( uint8_t key );
        void print_keys();
};

#endif	/* KEYPAD_H */

