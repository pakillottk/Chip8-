/* 
 * File:   main.cpp
 * Author: USUARIO
 *
 * Created on 17 de mayo de 2018, 20:54
 */
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include "Display.h"
#include "Chip8.h"

//MinGW fix: compiler options -static-libstdc++

const unsigned int FPS = 60;
int main(int argc, char* argv[]) {   
    if( argc < 2 ) {
        std::cout << "Usage: chip8++ <rom_path>" << std::endl;
        exit(1);
    }
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
        SDL_Quit();
        
        std::cout << "FATAL ERROR: Video couldn't init..." << std::endl;
        exit(1);
    }
    SDL_Window* window = SDL_CreateWindow(
                "Chip8++", 
                SDL_WINDOWPOS_CENTERED, 
                SDL_WINDOWPOS_CENTERED,
                SCREEN_W * SCREEN_SCALE,
                SCREEN_H * SCREEN_SCALE,
                SDL_WINDOW_OPENGL
            );     
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);    
    
    Display display( window, renderer );
    Chip8 machine(&display);
    
    bool run = true;
    
    //Start the Chip8
    machine.powerUp();
    
    //Load the ROM
    if( machine.loadProgram(argv[1])){
        std::cout << "ROM Loaded, running machine" << std::endl;  
        
        //MAIN LOOP
        char c;
        int last_ticks = SDL_GetTicks();
        int last_delta = 0, step_delta = 0, render_delta = 0;
        for(;;) {                 
            /* Update timers. */
            last_delta = SDL_GetTicks() - last_ticks;
            last_ticks = SDL_GetTicks();
            step_delta += last_delta;
            render_delta += last_delta;

            //run n instructions
            if( run ) { 
                while( step_delta >= 1 ) {
                    machine.runCycle();
                    step_delta--;
                }
            }
            
            //update timers
            machine.updateTimers( last_delta, FPS );
            
            //render
            while (render_delta >= (1000 / FPS)) {
                if( machine.needUpdateGraphics() ) {
                    machine.updateGraphics();
                }
                render_delta -= (1000 / FPS);
            }
            
            //handle keypad            
            c = machine.updateInput(); //exit or pause code
            if( c == 'p' ) {
                run = !run; //skip updates, pause all
            } else if( c == 'e' ) {
                break;; //close the emulator
            } else if( c == 'r' ) { //resets
                machine.reset();
            }            
            
            //delay next exectuion, reduces CPU usage
            SDL_Delay(1);
        }        
    }
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    
    return 0;
}

