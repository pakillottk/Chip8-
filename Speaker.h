/* 
 * File:   Speaker.h
 * Author: USUARIO
 *
 * Created on 18 de mayo de 2018, 19:23
 */

#ifndef SPEAKER_H
#define	SPEAKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <iostream>

const unsigned int AMPLITUDE = 28000;
const unsigned int FREQUENCY = 44100;
class Speaker {        
    public:
        unsigned sample_nr;
        int currentDelta;
        Speaker();
        virtual ~Speaker();
        void Play( int deltaTime );
        void Stop();
};

#endif	/* SPEAKER_H */

