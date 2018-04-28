//
//  ProfilePlus.cpp
//  KimaticaOSCDMX
//
//  Created by Terry Clark on 09/06/2017.
//
//

#include "ProfilePlus.hpp"

ProfilePlus::ProfilePlus() {
}

ProfilePlus::~ProfilePlus(){
    
}

void ProfilePlus::update(){
    
    switch(effectNum){
        case 49:
            UVOn();
            break;
        case 50:
            fade();
            break;
        case 51:
            fadeInOut();
            break;
        case 52:
            strobe();
            break;
        case 53:
            pulseOut();
            break;
    }
    
}

void ProfilePlus::setUV(float _uv){
    uv = _uv;
}

void ProfilePlus::setColor(ofVec4f col){
    col = color;
}

void ProfilePlus::UVOn(){
    dmx->setLevel(address, 0);     // R
    dmx->setLevel(address + 1, 0); // G
    dmx->setLevel(address + 2, 0); // B
    dmx->setLevel(address + 3, 255); // uv
    dmx->setLevel(address + 4, 255); // Strobe
    dmx->setLevel(address + 5, 255); // Dimmer
}

void ProfilePlus::fade(){
    
    float fadeDimmer = ofMap(ofGetMouseX(), 0 , ofGetWidth(), 0, 255);
    fadeDimmer = ofClamp(fadeDimmer, 0, 255);
    
    dmx->setLevel(address, 0);     // R
    dmx->setLevel(address + 1, 0); // G
    dmx->setLevel(address + 2, 0); // B
    dmx->setLevel(address + 3, 255);     // uv
    dmx->setLevel(address + 4, 255);     // Strobe
    dmx->setLevel(address + 5, fadeDimmer); // Dimmer
}

void ProfilePlus::fadeInOut(){
    
    float fadeIO = ofMap(sin(timer), 0, 1, 0, 255);
    fadeIO = ofClamp(fadeIO, 0, 255);
    
    dmx->setLevel(address, 0);
    dmx->setLevel(address + 1, 0);
    dmx->setLevel(address + 2, 0);
    dmx->setLevel(address + 3, 255);//uv
    dmx->setLevel(address + 4, 255);
    dmx->setLevel(address + 5, fadeIO);
    
    float pulse = ofMap(ofGetMouseX(), 0 , ofGetWidth(), 0.01, 0.7);
    timer += pulse;
}

void ProfilePlus::pulseOut(){    
    dmx->setLevel(address, 0);
    dmx->setLevel(address + 1, 0);
    dmx->setLevel(address + 2, 0);
    dmx->setLevel(address + 3, 255);//uv
    dmx->setLevel(address + 4, 255);
    dmx->setLevel(address + 5, timer);
    
    timer -= 2.8;
    
    if(timer <= 0) timer = 0;
    

}

void ProfilePlus::strobe(){
    
    float speed = ofMap(ofGetMouseX(), 0, ofGetWidth(), 64, 95);
    speed = ofClamp(speed, 64, 95);

    dmx->setLevel(address, 0);
    dmx->setLevel(address + 1, 0);
    dmx->setLevel(address + 2, 0);
    dmx->setLevel(address + 3, 255);
    dmx->setLevel(address + 4, speed);
    dmx->setLevel(address + 5, 255);
}


