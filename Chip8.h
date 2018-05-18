/* 
 * File:   Chip8.h
 * Author: USUARIO
 *
 * Created on 17 de mayo de 2018, 20:55
 */

#ifndef CHIP8_H
#define	CHIP8_H

#include <iostream>
#include <stdint.h>
#include "Display.h"
#include "Speaker.h"
#include "Keypad.h"

const uint16_t PGR_START = 0x200;
const unsigned int STACK_SIZE = 16;
const unsigned int MEM_SIZE = 4096; //4KB 
class Chip8 {
    protected:         
        Display* display;
        Keypad keypad;
        Speaker speaker;
        
        uint8_t MEMORY[ MEM_SIZE ];
        uint8_t V[16];
        uint16_t I;
        
        uint8_t delay_timer;
        uint8_t sound_timer;
        
        uint16_t PC;
        
        uint8_t SP;
        uint16_t Stack[ STACK_SIZE ];       
        
        bool PGR_loaded;
        bool waitingForInput;
        bool updateDisplay;
        unsigned accumDelta; //Accums delta time between frames, for timers update
        
        //stack helpers
        void push( uint16_t v );
        uint16_t pop();        
    public:
        Chip8( Display* display );
        void powerUp();
        void reset();
        bool loadProgram( const char* path );
        void decode( uint16_t opcode );
        void runCycle();   
        bool needUpdateGraphics();
        void updateGraphics();
        char updateInput();
        void print_state();
        void updateTimers( int deltaTime, unsigned FPS );
};
#endif	/* CHIP8_H */

