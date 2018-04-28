//
//  ProfilePlus.hpp
//  KimaticaOSCDMX
//
//  Created by Terry Clark on 09/06/2017.
//
//

#pragma once

#include <ofMain.h>
#include "Profile.hpp"

class ProfilePlus : public Profile {
protected:
    float uv;
    
public:
    ProfilePlus();
    ~ProfilePlus();
    
    void update();
    void setUV(float _uv);
    void setColor(ofVec4f col);
    void UVOn();
    void fade();
    void fadeInOut();
    void strobe();
    void pulseOut();
    float timer = 0;
};
