//
//  Profile.hpp
//  KimaticaOSCDMX
//
//  Created by Terry Clark on 23/06/2017.
//
//

#pragma once

#include "DMXLight.hpp"

class Wash : public DMXLight {
protected:
    ofColor color;
    ofVec2f position;
public:
    void setColor(ofColor col) { color.set(col); }
    void setPosition(ofVec2f pos) { position.set(pos); }
    void display(){
        ofSetColor(color);
        ofDrawCircle(position.x, position.y, 30,30);
    }
};
