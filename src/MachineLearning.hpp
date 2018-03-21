//
//  MachineLearning.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxRapidLib.h"
#include "MyoManager.hpp"

class Feature;

class MachineLearning {
private:
    
public:
    MachineLearning();
    ~MachineLearning();
    
    void update(MyoManager &myo);
    void draw();
    void drawGui();
    
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    
    ofxDatGui * mlGui;

};
