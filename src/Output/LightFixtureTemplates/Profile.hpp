//
//  Profile.hpp
//  KimaticaOSCDMX
//
//  Created by Terry Clark on 23/06/2017.
//
//

#pragma once

#include "ofMain.h"
#include "DMXLight.hpp"

class Profile : public DMXLight {
protected:
    ofVec4f color;
public:
    int effectNum = 1;    
    void display(ofVec2f pos){
        ofPushMatrix();
        {
            //Shape
            ofSetColor(color.x, color.y, color.z, color.w);
            ofDrawCircle(pos, 0.5);
        }
        ofPopMatrix();
    }
    
    void setColor(ofVec4f col){ color.set(col); }
};
