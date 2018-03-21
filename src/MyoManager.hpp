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
    ofVec4f gyro;
    
    float roll;
    float pitch;
    float yaw;
    
    //8 EMG Channels
    vector<double> emg = {
        0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0
    };
};

class MyoManager {
private:
    
    
public:
    MyoManager();
    ~MyoManager();
    
    void update();
    void draw();
    void drawGui(ofVec2f pos);

    void drawEmg(ofVec2f pos, int id);
    void drawGyro(ofVec2f pos, int id);
    void drawAccel(ofVec2f pos, int id);
    
//    static MyoManager& get_myoInstance();
//    static void destroy_myoInstance();

    void onToggleEvent(ofxDatGuiToggleEvent e);
    
    ofxMyo::Myo  myo;
    
    vector<Feature> featureVec;
    vector<Feature> getFeatureVec();
    vector < rapidStream<double> > emgStream;
    
    int windowSize = 25;
    bool myoActivated = false;
    
    //Gui Interface
    ofxDatGui* myoGui;
    
    ofxAssimpModelLoader model ;
    
    ofLight light;

    
};
