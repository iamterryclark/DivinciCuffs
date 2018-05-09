//
//  DMXController.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 27/04/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxDmx.h"
#include "ProfilePlus.hpp"
#include "MachineLearning.hpp"

class DMXController{
public:
    DMXController();
    void update(MachineLearning &interactiveML);
    void draw();
    //DMX object
    ofxDmx dmx;
    
    vector<Profile*> profiles;
    
    vector<DMXLight*> lights;
    
    ofEasyCam camera;
};


