//
//  OSCController.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 22/03/2018.
//

//This code has bee used on my second year project and adapted for this year a reference can be found here:

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "MachineLearning.hpp"

#define HOST "172.20.10.10"
#define PORTOUT 9005
#define PORTRTN 9006
#define TOTALSCENES 6

class SceneOSCController {
private:
    int scene = 0;
    int track = 0;
    int device = 0;
    int param = 0;
    
    ofxOscSender oscSend;
    ofxOscReceiver oscRec;
    
    string paramOut;
    
public:
    SceneOSCController();
    ~SceneOSCController();
    
    void update(MachineLearning &ml);
    void draw(ofVec2f pos);
    void streamToInstrumentDevice(string address, int track, int device, int parameter, float value);
    void returnValFromInstrumentAllDevices(int track, int device);
    
    void returnValFromInstrumentDevice(int track, int device, int parameter);
    
    void nextScene();
    void prevScene();
    void playScene();
    void stopScene();
    void keyPressed(int key);
};
