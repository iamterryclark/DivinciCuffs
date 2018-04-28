//
//  DataKeeper.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 28/04/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class DataKeeper {
public:
    DataKeeper();
    
    void calculateText();
    
    ofxXmlSettings XML;
    ofTrueTypeFont ttf;
    ofPath path;
    string text;
};
