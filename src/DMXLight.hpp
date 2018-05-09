//
//  DMXLight.hpp
//  KimaticaOSCDMX
//
// Created by Terry Clark on 09/06/2017.
// GoldsmithsDMX
//

#pragma once

#include "ofMain.h"
#include "ofxDmx.h"

class DMXLight
{
private:
protected:
    int address;
    ofxDmx * dmx;
public:
    DMXLight(){
    }
    
    ~DMXLight(){
        dmx->clear();
        dmx->update(true);
    }
    
    virtual void update() = 0;
    virtual void display() = 0;
    
    void setup(int _address, ofxDmx *_dmxPtr){
        address = _address;
        dmx = _dmxPtr;
    }
    
    int effectNum;
    float fader;

};
