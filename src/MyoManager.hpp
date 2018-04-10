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
    int id;

    ofVec3f acc = ofVec3f(0,0,0);
    ofVec3f accMin = numeric_limits<ofVec3f>::max();
    ofVec3f accMax = numeric_limits<ofVec3f>::min();
    
    ofVec3f accFOD = ofVec3f(0,0,0);
    
    ofVec3f gyro = ofVec3f(0,0,0);
    ofVec3f gyroMin = numeric_limits<ofVec3f>::max();
    ofVec3f gyroMax = numeric_limits<ofVec3f>::min();
   
    ofQuaternion quat = ofQuaternion(0,0,0,0);
    ofQuaternion quatMin = numeric_limits<ofQuaternion>::max();
    ofQuaternion quatMax = numeric_limits<ofQuaternion>::min();
    
    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;
    
    //8 EMG Channels
    array<double, 8> emgRMS = {};
    array<double, 28> emgRatioRMS = {};
    
    array<double, 8> emgMin = {};
    array<double, 8> emgMax = {};
    
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
    void calibrate();
    void drawGui(ofVec2f pos);
    
    void drawEmg(ofVec2f pos, int id);
    void drawEMGRatio(ofVec2f pos, int id);
    void drawGyro(ofVec2f pos, int id);
    void drawAccel(ofVec2f pos, int id);
    void drawAccelFOD(ofVec2f pos, int id);
    void drawQuat(ofVec2f pos, int id);
    
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    
    int windowSize = 20;
    
    vector<Feature> feature;
    
    rapidStream<double> emgStream0{windowSize};
    rapidStream<double> emgStream1{windowSize};
    rapidStream<double> emgStream2{windowSize};
    rapidStream<double> emgStream3{windowSize};
    rapidStream<double> emgStream4{windowSize};
    rapidStream<double> emgStream5{windowSize};
    rapidStream<double> emgStream6{windowSize};
    rapidStream<double> emgStream7{windowSize};
    
    rapidStream<double> accStream0{windowSize};
    rapidStream<double> accStream1{windowSize};
    rapidStream<double> accStream2{windowSize};
    
    //Gui Interface
    ofxDatGui* myoGui;
    
    ofxAssimpModelLoader model;
    
    ofLight light;
    
    float lowerThresh = 0;
    float higherThresh = 30;
};
