//
//  OSCController.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 22/03/2018.
//This code has been used on my second year project 'ReactGlow' and adapted for this year

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "MachineLearning.hpp"
#include "DMXController.hpp"
#include "MyoManager.hpp"

//#define HOST "172.17.0.189"
#define HOST "localhost"
#define PORTOUT 9005
#define PORTRTN 9006

class SceneOSCController {
private:
    int track = 0;
    int device = 0;
    int param = 0;
    
    ofxOscSender oscSend;
    ofxOscReceiver oscRec;
    
    string paramOut;
    
    once_flag flag1;
    
public:
    SceneOSCController(MachineLearning *_ml, MyoManager *_myo);
    ~SceneOSCController();
    
    void update();
    void draw(ofVec2f pos);
    void callPlayScene();
    void streamToInstrumentDevice(string address, int track, int device, int parameter, float value);
    void returnValFromInstrumentAllDevices(int track, int device);
    
    void returnValFromInstrumentDevice(int track, int device, int parameter);
    
    void playScene(int scene);
    void stopScene(int scene);
    void keyPressed(int key);
    
    MachineLearning * ml;
    MyoManager * myo;
    DMXController dmxControl;
};
