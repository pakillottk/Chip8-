#include "Chip8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>

#define DEBUG false

Chip8::Chip8( Display* display ) {
    this->display = display;
    PGR_loaded = false;
}

void Chip8::push(uint16_t v) {    
    Stack[ SP++ ] = v;    
    SP = SP & 0x0F;
}

uint16_t Chip8::pop() {
    uint16_t out = Stack[ --SP ];
    SP = SP & 0x0F;
    return out;
}

const uint8_t font_sprites[0x80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F    
};

void Chip8::reset() {
    unsigned int i;
    waitingForInput = updateDisplay = false;
    I = delay_timer = sound_timer = SP = accumDelta = 0;
    memset( Stack, 0, sizeof Stack );
    memset( V, 0, sizeof V );       
    display->clear();

    PC = 0x200;
    
    speaker.Stop();
}

void Chip8::powerUp(){
    waitingForInput = updateDisplay = false;
    I = delay_timer = sound_timer = SP = accumDelta = 0;
    unsigned int i;
    
    memset( MEMORY, 0, sizeof MEMORY );
    memset( Stack, 0, sizeof Stack );
    memset( V, 0, sizeof V );
    
    PC = 0x200;    //Initialize font sprites
    memcpy( MEMORY, font_sprites, sizeof font_sprites );
    
    speaker.Stop();
}

bool Chip8::needUpdateGraphics() {
    return updateDisplay;
}

void Chip8::updateGraphics() {
    display->updateScreen();
    updateDisplay = false;
}

char Chip8::updateInput() {
    return keypad.update();
}

bool Chip8::loadProgram(const char* path) {
    char readByte;
    uint16_t currentAddress = PGR_START;
    std::ifstream file (path, std::ios::in|std::ios::binary);
    
    if( file.is_open() ) {
        while( !file.eof() ) {
            file.get( readByte );
            MEMORY[ currentAddress ] = (int)readByte;
            currentAddress++; 
        }
        PGR_loaded = true;
        file.close();
        return true;
    }
    
    return false;
}

void Chip8::decode(uint16_t opcode){
    #if DEBUG
        std::cout << "Processing: " << std::hex << (int)opcode  << std::endl;
    #endif
    //Extract all possible parameters from opcode
    uint16_t addr = opcode & 0x0FFF;
    uint8_t x = (opcode >> 8) & 0x0F;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;
    #if DEBUG
        std::cout << "nnn: " << std::hex << (int)addr  << std::endl;
        std::cout << "x: " << std::hex << (int)x  << std::endl;
        std::cout << "y: " << std::hex << (int)y  << std::endl;
        std::cout << "kk: " << std::hex << (int)kk << std::endl;
        std::cout << "n: " << std::hex << (int)n  << std::endl;
    #endif
    
    //Test first nibble
    switch( opcode >> 12 ) {
        case 0: { //Multiple
            if( kk == 0xE0 ) { //CLS
                //Clears the display
                display->clear();
                updateDisplay = true;
                
                #if DEBUG       
                    std::cout << "CLS" << std::endl;
                #endif
            } else if( kk == 0xEE ) { //RET
                PC = pop();
                #if DEBUG       
                    std::cout << "RET" << std::endl;
                    std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
                #endif
            }
            break;
        }
        case 1: { //JP addr            
            PC = addr-1;
            #if DEBUG       
                std::cout << "JP addr" << std::endl;
                std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
           #endif
            break;
        }
        case 2: { //CALL addr   
            push( PC );
            PC = addr - 1;
            #if DEBUG       
                std::cout << "CALL addr" << std::endl;
                std::cout << "PC set to: " << std::hex << (int)addr+1 << std::endl;
           #endif
            break;
        }
        case 3: { //SE Vx, byte            
            if( V[x] == kk ){
                PC += 2;
            }            
            #if DEBUG       
                std::cout << "SE Vx, byte" << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                std::cout << "kk=" << std::hex << (int)kk << std::endl;
                std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
           #endif
           break;
        }
        case 4: { //SNE Vx, byte            
            if( V[x] != kk ){
                PC += 2;
            }            
            #if DEBUG       
                std::cout << "SNE Vx, byte" << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                std::cout << "kk=" << std::hex << (int)kk << std::endl;
                std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
           #endif
           break;
        }
        case 5: { //SE Vx, Vy            
            if( V[x] == V[y] ){
                PC += 2;
            }   
            #if DEBUG       
                std::cout << "SE Vx, Vy" << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
           #endif
            break;
        }
        case 6: { //LD Vx, byte            
           V[ x ] = kk;
           #if DEBUG       
                std::cout << "LD Vx, byte" << std::endl;
                std::cout << "x=" << std::hex << (int)x << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
           #endif
           break;
        }
        case 7: { //ADD Vx, byte
            V[x] = V[x] + kk;
            #if DEBUG       
                std::cout << "ADD Vx, byte" << std::endl;
                std::cout << "kk=" << std::hex << (int)kk << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
           #endif
           break;
        }
        case 8: { //Multiple
            switch( n ) {
                case 0: { //LD Vx, Vy
                    V[x] = V[y];
                    #if DEBUG       
                        std::cout << "LD Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                   #endif
                   break;
                }
                case 1: { //OR Vx, Vy
                    V[x] = V[x] | V[y];
                    #if DEBUG       
                        std::cout << "OR Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                   #endif
                    break;
                }
                case 2: { //AND Vx, Vy
                    V[x] = V[x] & V[y];
                    #if DEBUG       
                        std::cout << "AND Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                   #endif
                    break;
                }
                case 3: { //XOR Vx, Vy
                    V[x] = V[x]^ V[y];
                    #if DEBUG       
                        std::cout << "XOR Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                   #endif
                   break;
                }
                case 4: { //ADD Vx, Vy
                    unsigned result = V[x] + V[y];
                    V[0xF] = result > 0xFF;
                    V[x] = result & 0xFF;
                    #if DEBUG       
                        std::cout << "ADD Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                        std::cout << "VF=" << (int)V[0xF] << std::endl;
                   #endif
                   break;
                }
                case 5: { //SUB Vx, Vy
                    V[0xF] = V[x] > V[y];          
                    V[x] = V[x] - V[y];
                    #if DEBUG       
                        std::cout << "SUB Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                        std::cout << "VF=" << (int)V[0xF] << std::endl;
                   #endif
                   break;
                }
                case 6: { //SHR Vx {, Vy}
                    V[0xF] = (V[x] & 0x01) > 0;          
                    V[x] = V[x] / 2;
                    #if DEBUG       
                        std::cout << "SHR Vx {, Vy}" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "VF=" << (int)V[0xF] << std::endl;
                   #endif
                   break;
                }
                case 7: { //SUBN Vx, Vy
                    V[0xF] = V[y] > V[x];          
                    V[x] = V[y] - V[x];
                    #if DEBUG       
                        std::cout << "SUBN Vx, Vy" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                        std::cout << "VF=" << (int)V[0xF] << std::endl;
                   #endif
                   break;
                }
                case 0xE: { //SHL Vx {, Vy}
                    V[0xF] = (V[x] & 0x80) > 0;          
                    V[x] = V[x] * 2;
                    #if DEBUG       
                        std::cout << "SHL Vx {, Vy}" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "VF=" << (int)V[0xF] << std::endl;
                   #endif
                   break;
                }
            }
            break;
        }
        case 9: { //SNE Vx, Vy
            if( V[x] != V[y] ) {
                PC += 2;
            }
            #if DEBUG       
                std::cout << "SNE Vx, Vy" << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                std::cout << "V" << std::hex << (int)y << "=" << (int)V[y] << std::endl;
                std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
           #endif
            break;
        }
        case 0xA: { //LD I, addr
            I = addr;
            #if DEBUG       
                std::cout << "LD I, addr" << std::endl;
                std::cout << "I=" << std::hex << (int)I << std::endl;
           #endif
            break;
        }
        case 0xB: { //JP V0, addr
            PC = addr + V[0] - 1;
            #if DEBUG       
                std::cout << "JP V0, addr" << std::endl;
                std::cout << "V0=" << (int)V[0] << std::endl;
                std::cout << "PC set to: " << std::hex << (int)PC << std::endl;
           #endif
            break;
        }
        case 0xC: { //RND Vx, byte
            uint8_t random = rand()%256;
            V[x] = random & kk;
            #if DEBUG       
                std::cout << "RND Vx, byte" << std::endl;
                std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
           #endif
            break;
        }
        case 0xD: { //DRW Vx, Vy, nibble            
            uint8_t* sprite_data = (uint8_t*)malloc( n );
            memcpy( sprite_data, &MEMORY[ I ], sizeof(uint8_t) * n );            
            V[0xF] = display->drawSprite( V[x], V[y], sprite_data, n );
            free( sprite_data );                
            updateDisplay = true;      
            
            #if DEBUG       
                std::cout << "DRW Vx, Vy, nibble" << std::endl;
                std::cout << "VF=" << std::hex << (int)V[0xF] << std::endl;
           #endif
            break;
        }
        case 0xE: { //Multiple
            if( kk == 0x9E ) { //SKP Vx
                /*
                 * Checks the keyboard, and if the key corresponding to the 
                 * value of Vx is currently in the down position, PC is increased 
                 * by 2.
                 * */                
                if( keypad.getKeyState(V[x]) ) {
                    PC+=2;
                }
                
                #if DEBUG       
                        std::cout << "SKP Vx" << std::endl;
                #endif
            } else if( kk == 0xA1 ) { //SKNP Vx
                /*
                 * Checks the keyboard, and if the key corresponding to the 
                 * value of Vx is currently in the up position, PC is increased 
                 * by 2.
                 * */                
                if( !keypad.getKeyState(V[x]) ) {
                    PC+=2;
                }
                
                #if DEBUG       
                        std::cout << "SKNP Vx" << std::endl;
                #endif
            }
            break;
        }
        case 0xF: { //Multiple
            switch( kk ) {
                case 0x07: { //LD Vx, DT
                    V[x] = delay_timer;
                    #if DEBUG       
                        std::cout << "LD Vx, DT" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "delay_timer=" << std::hex << (int)delay_timer  << std::endl;
                   #endif
                    break;
                }
                case 0x0A: { //LD Vx, K
                    //Wait for a key press, store the value of the key in Vx.
                    /*
                        All execution stops until a key is pressed, then the 
                     *  value of that key is stored in Vx.
                    */      
                    waitingForInput = true;
                    for( unsigned i = 0; i < 16; i++ ) {
                        if( keypad.getKeyState(i) ) {                            
                            V[x] = i;
                            waitingForInput = false;
                            break;
                        }                        
                    }
                    #if DEBUG       
                        std::cout << "LD Vx, K" << std::endl;
                    #endif   
                    break;
                }
                case 0x15: { //LD DT, Vx
                    delay_timer = V[x];
                    
                    #if DEBUG       
                        std::cout << "LD DT, Vx" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "delay_timer=" << std::hex << (int)delay_timer  << std::endl;
                   #endif
                    break;
                }                
                case 0x18: { //LD ST, Vx
                    sound_timer = V[x];
                    
                    #if DEBUG       
                        std::cout << "LD ST, Vx" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "sound_timer=" << std::hex << (int)sound_timer  << std::endl;
                   #endif
                    break;
                } 
                case 0x1E: { //ADD I, Vx
                    I = I + V[x];
                    
                    #if DEBUG       
                        std::cout << "ADD I, Vx" << std::endl;
                        std::cout << "V" << std::hex << (int)x << "=" << (int)V[x] << std::endl;
                        std::cout << "I=" << std::hex << (int)I  << std::endl;
                   #endif
                    break;
                }
                case 0x29: { //LD F, Vx
                    switch( V[x] ) {
                        case 0x0: {
                            I = 0; 
                            break;
                        }
                        case 0x1: {
                            I = 5; 
                            break;
                        }
                        case 0x2: {
                            I = 10; 
                            break;
                        }
                        case 0x3: {
                            I = 15; 
                            break;
                        }
                        case 0x04: {
                            I = 20; 
                            break;
                        }
                        case 0x05: {
                            I = 25; 
                            break;
                        }
                        case 0x06: {
                            I = 30; 
                            break;
                        }
                        case 0x7: {
                            I = 35; 
                            break;
                        }
                        case 0x08: {
                            I = 40; 
                            break;
                        }
                        case 0x09: {
                            I = 45; 
                            break;
                        }
                        case 0x0A: {
                            I = 50; 
                            break;
                        }
                        case 0x0B: {
                            I = 55; 
                            break;
                        }
                        case 0x0C: {
                            I = 60; 
                            break;
                        }
                        case 0x0D: {
                            I = 65; 
                            break;
                        }
                        case 0x0E: {
                            I = 70; 
                            break;
                        }
                        case 0x0F: {
                            I = 75; 
                            break;
                        }
                    }
                    
                    #if DEBUG       
                        std::cout << "LD F, Vx" << std::endl;    
                        std::cout << "I=" << std::hex << (int)V[x] << std::endl;
                    #endif
                    break;
                }
                case 0x33: { //LD B, Vx
                    unsigned bcd = V[x];
                    uint8_t bcd_hundreds = bcd/100;
                    uint8_t bcd_tens = (bcd/10)%10;
                    uint8_t bcd_ones = bcd%10;

                    MEMORY[ I ] = bcd_hundreds;
                    MEMORY[ I + 1 ] = bcd_tens;
                    MEMORY[ I + 2] = bcd_ones;

                    #if DEBUG       
                        std::cout << "LD B, Vx" << std::endl;
                        std::cout << "V" << std::hex << (int)x <<"=" << (int)V[x] << std::endl;
                        std::cout << "bcd=" << std::dec << bcd << std::endl;
                        std::cout << "MEMORY:(" << 
                                (int)MEMORY[ I ]   << ", " << 
                                (int)MEMORY[ I+1 ] << ", " << 
                                (int)MEMORY[ I+2 ] <<") " << std::endl;
                    #endif
                    break;
                }
                case 0x55: { //LD [I], Vx
                    unsigned i;
                    for( i = 0; i < x; i++ ) {
                        MEMORY[ I + i ] = V[i];
                    }
                    #if DEBUG       
                        std::cout << "LD [I], Vx" << std::endl;
                        std::cout << "I=" << std::hex << (int)I<< std::endl;
                        for( i = 0; i < x; i++ ) {
                            std::cout << "V" << i << "=" << std::hex << (int)V[i] << std::endl;
                            std::cout << std::hex << (int)(I+i) << "=" << 
                                         std::hex << (int)MEMORY[ I + 1 ] << std::endl;
                        }
                    #endif
                    break;
                }
                case 0x65: { //LD Vx, [I]
                    unsigned i;
                    for( i = 0; i < x; i++ ) {
                        V[i] = MEMORY[ I + i ];
                    }
                    #if DEBUG       
                        std::cout << "LD Vx, [I]" << std::endl;
                        std::cout << "I=" << std::hex << (int)I<< std::endl;
                        for( i = 0; i < x; i++ ) {
                            std::cout << "V" << i << "=" << std::hex << (int)V[i] << std::endl;
                            std::cout << std::hex << (int)(I+i) << "=" << 
                                         std::hex << (int)MEMORY[ I + 1 ] << std::endl;
                        }
                    #endif
                    break;
                }               
            }
            break;
        }
        default: {
            std::cout << "FATAL ERROR: Unknown OPCODE" << std::endl;
            exit(1);
        }
    }
}

void Chip8::runCycle() {
    /*if( !PGR_loaded ) {
        std::cout << "No program loaded... shutting down" << std::endl;
        return;
    }*/
    #if DEBUG
        std::cout << "Fetching at PC: " << std::hex << (int)PC << std::endl;
    #endif    
        
    decode( (MEMORY[ PC ] << 8) | MEMORY[++PC] );
    PC++; 
    
    if( waitingForInput ) {
        #if DEBUG
            std::cout << "Waiting for user input..." << std::endl;
        #endif
        PC -= 2;
    }
}

void Chip8::updateTimers( int deltaTime, unsigned FPS ) {
    accumDelta += deltaTime;
    while( accumDelta > (1000 / FPS) ) {
        accumDelta -= ( 1000 / FPS );
        if( delay_timer > 0) {
            delay_timer--;
        }
        if( sound_timer > 0 ) {
            speaker.Play( deltaTime );
            sound_timer--;
        } else {
            speaker.Stop();
        }
    }
}

void Chip8::print_state() {
    unsigned int i;
    
    std::cout << "=================" << std::endl;
    std::cout << "====  CPU  ======" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << "PC: " << std::hex << (int)PC << std::endl; 
    for( i = 0; i < 16; i++ ) {
        std::cout << "V" << std::hex << i << ": " << (int)V[i] << std::endl; 
    }
    std::cout << "I: " << std::hex << (int)I << std::endl; 
    std::cout << "DELAY_TIMER: " << std::hex << (int)delay_timer << std::endl; 
    std::cout << "SOUND_TIMER: " << std::hex << (int)sound_timer << std::endl; 
    
    std::cout << "=================" << std::endl;
}