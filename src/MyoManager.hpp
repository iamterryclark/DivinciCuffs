//
//  MyoManager.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxMyo.h"
#include "ofxDatGui.h"
#include "ofxRapidLib.h"
#include "ofxAssimpModelLoader.h"

struct Feature {
    
    Feature(){
        emgRaw = vector<double>(8, 0.0);
        std::cout << emgRaw[0];
    }
    
    
    int id;
    
    //Raw Features
    double pitch = 0.0;
    double yaw = 0.0;
    double roll = 0.0;
    ofVec3f acc = ofVec3f(0,0,0);
    ofVec3f gyro = ofVec3f(0,0,0);
    ofQuaternion quat = ofQuaternion(0,0,0,0);
    
    //Fancy Features
    ofVec3f accFOD = ofVec3f(0,0,0);
    vector<int> zeroCrosses;
    vector<float> bayesFilter;
 
    //Initialised in Constructor
    vector<double> emgRaw;
    vector<double> emgRMS;
    vector<double> emgRMSRatio;
    vector<double> emgBayes;
    
    vector<double> emgMin;
    vector<double> emgMax;
};

class MyoManager {
private:
    ofxMyo::Myo myo;
    bool calibrateActivate = false;
    
public:
    MyoManager();
    ~MyoManager();
    
    void update();
    void draw();
//    void calibrate(Feature & feat);
    
    void drawGui(ofVec2f pos);
    void drawModel(ofVec2f pos, int id);
    void drawGraph(ofVec2f pos, string title, vector<double> values, int min, int max);
    void drawGraph(ofVec2f pos, string title, ofVec3f values, int min, int max);
    void drawGraph(ofVec2f pos, string title, ofQuaternion values, int min, int max);
    
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    
    vector<Feature> feature;
    
    vector< rapidStream<double> *> emgStreams;
    vector< rapidStream<double> *> accStreams;
    vector< rapidStream<double> *> accFODStreams;

    //Gui Interface
    ofxDatGui* myoGui;
    
    ofxAssimpModelLoader model;
    
    ofLight light;
    
    float lowerThresh = 0;
    float higherThresh = 30;
    
//    ofxJSONElement savedGestures;
};
