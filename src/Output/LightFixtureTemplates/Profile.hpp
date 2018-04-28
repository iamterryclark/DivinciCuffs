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
    void display(float num){
        ofPushMatrix();
        {
            float x = 4.0 * cos(num);
            float y = 4.0 * sin(num);
        
            //Shape
            ofSetColor(color.x, color.y, color.z, color.w);
            ofDrawCircle(x,y, 0.5);
        
            //Text
            ofSetColor(100);
            ofTranslate(x,y);
            ofRotate(180,0,1,0);
            ofDrawAxis(0.5);
            ofScale(0.005,0.005);
//            lightName.drawStringCentered(ofToString(num + 1), 0, 0, false);
        }
        ofPopMatrix();
    }
    
    void setColor(ofVec4f col){ color.set(col); }
};
