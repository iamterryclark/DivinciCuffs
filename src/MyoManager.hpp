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
    
    ofVec3f acc;
    ofVec3f gyro;
    
    double roll;
    double pitch;
    double yaw;
    
    //8 EMG Channels
    vector<double> emg;
};

class MyoManager {
private:
    ofxMyo::Myo myo;
//
public:
    MyoManager();
    ~MyoManager();
    void update();
    void draw();
    void drawGui(ofVec2f pos);
    
    void drawEmg(ofVec2f pos, int id);
    void drawGyro(ofVec2f pos, int id);
    void drawAccel(ofVec2f pos, int id);
    
    void onToggleEvent(ofxDatGuiToggleEvent e);
    
    int windowSize = 25;
    bool myoActivated = false;
    
    vector <Feature> feature;
    
    rapidStream<double> emgStream0 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream1 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream2 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream3 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream4 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream5 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream6 = rapidStream<double>(windowSize);
    rapidStream<double> emgStream7 = rapidStream<double>(windowSize);
    
    //Gui Interface
    ofxDatGui* myoGui;
    
    ofxAssimpModelLoader model;
    
    ofLight light;
};
