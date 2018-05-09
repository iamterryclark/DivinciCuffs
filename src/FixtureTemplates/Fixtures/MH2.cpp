//
//  MH2.cpp
//  GoldsmithsDMX
//
//  Created by Terry Clark on 09/06/2017.
//
//

#include "MH2.hpp"

MH2::MH2() {
}

MH2::~MH2(){
    
}

void MH2::update(){
    float mouse;
    switch(effectNum){
        case 1:
            mouse = ofMap(ofGetMouseX(), 0 ,ofGetWidth(), 0, 255);
            mouse = ofClamp(mouse, 0, 255);
            on(mouse);
            break;
        case 2:
            fadeIn();
            break;
        case 3:
            mouse = ofMap(ofGetMouseX(), 0 ,ofGetWidth(), 0.01, 1.7);
            pulseSlow(mouse);
            break;
        case 4:
            fadeOut();
            break;
    }
}

void MH2::on(float mouse){
    dmx->setLevel(address, 255); //Shutter
    dmx->setLevel(address + 1, mouse); //Dimmer
    dmx->setLevel(address + 2, position.x); //Pan
    dmx->setLevel(address + 4, position.y); //Tilt
    dmx->setLevel(address + 7, 0); //Cholor Wheel
    dmx->setLevel(address + 8, color.r); // R
    dmx->setLevel(address + 9, color.g); // G
    dmx->setLevel(address + 10, color.b); // B
}

void MH2::fadeIn(){
    dmx->setLevel(address, 255); //Shutter
    dmx->setLevel(address + 1, ofClamp(fader, 0 ,255)); //Dimmer
    dmx->setLevel(address + 2, position.x); //Pan
    dmx->setLevel(address + 4, position.y); //Tilt
    dmx->setLevel(address + 7, 0); //Cholor Wheel
    dmx->setLevel(address + 8, color.r); // R
    dmx->setLevel(address + 9, color.g); // G
    dmx->setLevel(address + 10, color.b); // B
        
    fader+=1.5;
    if(fader >= 255) fader = 255;
}

void MH2::fadeOut(){
    dmx->setLevel(address, 255); //Shutter
    dmx->setLevel(address + 1, ofClamp(fader, 0,255)); //Dimmer
    dmx->setLevel(address + 2, position.x); //Pan
    dmx->setLevel(address + 4, position.y); //Tilt
    dmx->setLevel(address + 7, 0); //Cholor Wheel
    dmx->setLevel(address + 8, color.r); // R
    dmx->setLevel(address + 9, color.g); // G
    dmx->setLevel(address + 10, color.b); // B
    
    fader-=1.5;
    if(fader <= 0) fader = 0;
}

void MH2::pulseSlow(float speed){
    
    float fade = ofMap(sin(fader), -1, 1, 0, 255);
    
    dmx->setLevel(address, 255); //Shutter
    dmx->setLevel(address + 1, ofClamp(fade, 0 ,255)); //Dimmer
    dmx->setLevel(address + 2, position.x); //Pan
    dmx->setLevel(address + 4, position.y); //Tilt
    dmx->setLevel(address + 7, 0); //Cholor Wheel
    dmx->setLevel(address + 8, color.r); // R
    dmx->setLevel(address + 9, color.g); // G
    dmx->setLevel(address + 10, color.b); // B
    
    fader -= speed;
}


