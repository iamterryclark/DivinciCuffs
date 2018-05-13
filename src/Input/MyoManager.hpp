//
//  MyoManager.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#pragma once

#include "ofMain.h"

//Addon Classes
#include "ofxMyo.h"
#include "ofxDatGui.h"
#include "ofxAssimpModelLoader.h"

//Rapid Mix
#include "rapidmix.h"

//My Classes
#include "Feature.h"

class MyoManager {
private:
    ofxMyo::Myo myo;
public:
    MyoManager();
    ~MyoManager();
  
    void update();
    void draw();
    void drawGui(ofVec2f pos);
    void drawModel(ofVec2f pos, int id);
    void drawGraph(ofVec2f pos, string title, vector<double> values, int min, int max);
    void drawGraph(ofVec2f pos, string title, vector<int> values, int min, int max);
    void drawGraph(ofVec2f pos, string title, ofVec3f values, int min, int max);
    void drawGraph(ofVec2f pos, string title, ofQuaternion values, int min, int max);
    
    //Myo Gui Events
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    
    //Store Myo data here
    vector<Feature> feature;
    
    //Rapid Streams
    vector< rapidStream<double> *> emgRawStreams;
    vector< rapidStream<double> *> accStreams;
    vector< rapidStream<double> *> accFODStreams;

    //Gui Interface
    ofxDatGui* myoGui;
    float lowerThresh = 0;
    float higherThresh = 30;
    int windowSize = 20;
    
    //For Model Display
    ofxAssimpModelLoader model;
    ofLight light;
    
    
};
