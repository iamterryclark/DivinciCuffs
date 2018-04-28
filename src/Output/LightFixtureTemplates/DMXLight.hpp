//
//  DMXLight.hpp
//  KimaticaOSCDMX
//
//  Created by Terry Clark on 09/06/2017.
//
//

#pragma once

#include "ofMain.h"
#include "ofxDmx.h"
//#include "ofxCenteredTrueTypeFont.h"

class DMXLight
{
private:
protected:
    int address;
    ofxDmx * dmx;
//    ofxCenteredTrueTypeFont lightName;
    string name;
public:
    DMXLight(){
//        lightName.load("Font/Dosis/Dosis-Bold.ttf", 50);
    }
    
    ~DMXLight(){
        dmx->clear();
        dmx->update(true);
    }
    
    virtual void update() = 0;
    virtual void display(float num) = 0;
    
    void setup(int _address, ofxDmx *_dmxPtr){
        address = _address;
        dmx = _dmxPtr;
    }
    
    void setName(string _name){
        name = _name;
    };

};
