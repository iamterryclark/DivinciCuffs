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
    
    profiles.push_back(&tProfPlus1);
    profiles.push_back(&tProfPlus2);
    
    //Setup address of each light
    profiles[0]->setup(1, &dmx);
    profiles[1]->setup(7, &dmx);
    
    lights.push_back(profiles[0]);
    lights.push_back(profiles[1]);
}

void DMXController::update(){
//    profiles[0]->setColor(ofVec4f(0,0,0,0));
//    profiles[1]->setColor(ofVec4f(0,0,0,0));
    
    dynamic_cast<ProfilePlus*>(profiles[0])->setUV(255);
    dynamic_cast<ProfilePlus*>(profiles[1])->setUV(255);
    
    dynamic_cast<ProfilePlus*>(profiles[0])->setColor(ofVec4f(255, 0, 0, 255));
    dynamic_cast<ProfilePlus*>(profiles[1])->setColor(ofVec4f(0, 255, 0, 255));
    
    for (auto light : lights){
        light->update();
    }
    
    if (dmx.isConnected()) dmx.update(); // Gets next dmx message
}

void DMXController::draw(){
    for (auto light : lights){
        light->update();
    }
}
