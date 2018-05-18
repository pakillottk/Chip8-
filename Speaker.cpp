#include "Speaker.h"

#include <stdlib.h>
#include <math.h>

void fillAudio( void* udata, Uint8* stream, int len ) {
    Sint16 *buffer = (Sint16*)stream;
    int length = len / 2; // 2 bytes per sample for 
    Speaker* speaker = (Speaker*)udata;
    
    for(int i = 0; i < length; i++, (speaker->sample_nr)++) {
        double time = (double)(speaker->sample_nr) / (double)FREQUENCY;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}

Speaker::Speaker() {
    SDL_AudioSpec wanted;
    wanted.freq = FREQUENCY;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1; 
    wanted.samples = 512;    
    wanted.callback = fillAudio;
    wanted.userdata = this;
    
    sample_nr = 0;
    if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
        std::cout << "FATAL ERROR: Audio couldn't init..." << std::endl;
        exit(1);
    }
}

Speaker::~Speaker() {
    SDL_CloseAudio();
}

void Speaker::Play( int deltaTime ) {
    currentDelta = deltaTime;
    SDL_PauseAudio(0);
}

void Speaker::Stop() {
    SDL_PauseAudio(1);
}