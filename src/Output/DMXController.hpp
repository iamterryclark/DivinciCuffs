//
//  DMXController.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 27/04/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxDmx.h"
#include "MH2.hpp"

class DMXController{
public:
    DMXController();
    ~DMXController();
    void update(float pVal1, float pVal2, ofColor knnClass);
    void draw();
    //DMX object
    ofxDmx dmx;
    vector<MH2*> washes;
//    vector<DMXLight*> lights;
};


