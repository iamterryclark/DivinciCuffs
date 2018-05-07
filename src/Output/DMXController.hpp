//
//  DMXController.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 27/04/2018.
//

#pragma once

#include "ofMain.h"
//#include "ofxDmx.h"
#include "ProfilePlus.hpp"

class DMXController{
public:
    DMXController();
    void update();
    void draw();
    //DMX object
    ofxDmx dmx;
    
    //Light Profiles
    ProfilePlus tProfPlus1, tProfPlus2;
    
    vector<Profile*> profiles;
    
    vector<DMXLight*> lights;
    
    ofEasyCam camera;
};


