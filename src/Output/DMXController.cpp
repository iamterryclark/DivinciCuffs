//
//  DMXController.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 27/04/2018.
//

#include "DMXController.hpp"

DMXController::DMXController(){
    dmx.connect(0, 60); //Serial Port Number
    dmx.update(true); //Resets the lights to 0 position in all channel
    
    washes.push_back(new MH2);
    washes.push_back(new MH2);
    
    // Setup address of each light
    washes[0]->setup(1, &dmx);
    washes[1]->setup(7, &dmx);
    
    washes[0]->setPosition(ofVec2f(600,ofGetHeight()-100));
    washes[1]->setPosition(ofVec2f(700,ofGetHeight()-100));
}

DMXController::~DMXController(){
}

void DMXController::update(float pVal1, float pVal2, ofColor knnClass){
    pVal1 = ofMap(pVal1, 0 ,1 ,0 ,255);
    pVal2 = ofMap(pVal2, 0 ,1 ,0 ,255);
    
    knnClass.a = pVal2;
    
    for (auto wash : washes){
        wash->setColor(ofColor(knnClass));
    }

    if (dmx.isConnected()) dmx.update(); // Gets next dmx message
}

void DMXController::draw(){
    ofDrawBitmapString("DMX OutPut EMG", 580, ofGetHeight()-150);
    for (int i = 0 ; i < washes.size(); i++){
        washes[i]->display();
    }
}
