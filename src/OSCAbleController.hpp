//
//  OSCController.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 22/03/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "MyoManager.hpp"

//class MyoManager;

#define HOST "localhost"
#define PORTOUT 9005
#define PORTRTN 9006
#define TOTALSCENES 6

class OSCAbleController {
private:
    int scene = 0;
    int track = 0;
    int device = 0;
    int param = 0;
    
    ofxOscSender oscSend;
    ofxOscReceiver oscRec;
    
    string paramOut;
    
public:
    OSCAbleController();
    ~OSCAbleController();
    
    void update(MyoManager &myo);
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
