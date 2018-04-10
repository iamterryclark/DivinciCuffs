//
//  OSCController.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 22/03/2018.

#include "OSCAbleController.hpp"

OSCAbleController::OSCAbleController(){
    //Send to LiveOSC
    oscSend.setup(HOST, PORTOUT);
    oscRec.setup(PORTRTN);
}

OSCAbleController::~OSCAbleController(){
    
}

void OSCAbleController::update(MyoManager &myo){
    
    if (ofGetFrameNum() % 3 == 0){
        
        float value = ofMap(myo.feature[0].emgRMS[0], myo.feature[0].emgMin[0], myo.feature[0].emgMax[0], 0, 255);
        streamToInstrumentDevice("/live/device", track, device, param, value);
        
        while(oscRec.hasWaitingMessages())
        {
            // get the next message
            ofxOscMessage m;
            oscRec.getNextMessage(m);
            
            // check for mouse moved message
            if(m.getAddress() == "/live/device/param")
            {
                //Returns int 32
                paramOut =  ofToString(m.getArgAsFloat(0)) + " | " +
                            ofToString(m.getArgAsFloat(1)) + " | " +
                            ofToString(m.getArgAsFloat(2)) + " | " +
                            ofToString(m.getArgAsFloat(3)) + " | " +
                            m.getArgAsString(4);
            }
        }
    }
}

void OSCAbleController::draw(ofVec2f pos){
    ofSetColor(255);
    ofDrawBitmapString("Ablton OSC", pos.x, pos.y);
    ofDrawBitmapString("Track " + ofToString(track), pos.x, pos.y + 20);
    ofDrawBitmapString("Device " + ofToString(device), pos.x, pos.y + 40);
    ofDrawBitmapString("Param " + ofToString(param), pos.x, pos.y + 60);
    
    ofDrawBitmapString(paramOut, pos.x, pos.y + 80 );
}

void OSCAbleController::streamToInstrumentDevice(string address, int track, int device, int parameter, float value){
    ofxOscMessage m;
    m.setAddress(address);
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    m.addFloatArg(value); // Value
    oscSend.sendMessage(m);
    m.clear();
}

void OSCAbleController::returnValFromInstrumentAllDevices(int track, int device){
    ofxOscMessage m;
    m.setAddress("/live/device/param");
    m.addIntArg(track); //Track
    m.addIntArg(device);
    oscSend.sendMessage(m);
    m.clear();
}


void OSCAbleController::returnValFromInstrumentDevice(int track, int device, int parameter){
    ofxOscMessage m;
    m.setAddress("/live/device/range");
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    oscSend.sendMessage(m);
    m.clear();
}


void OSCAbleController::nextScene(){
    scene++;
    scene = abs(scene % TOTALSCENES);
    ofxOscMessage m;
    m.setAddress("/live/play/scene");
    m.addIntArg(scene);
    oscSend.sendMessage(m);
    m.clear();
    
    cout << "Scene :" << scene << endl;
}

void OSCAbleController::prevScene(){
    scene--;
    scene = abs(scene % TOTALSCENES);
    ofxOscMessage m;
    m.setAddress("/live/play/scene");
    m.addIntArg(scene);
    oscSend.sendMessage(m);
    m.clear();
    
    cout << "Scene :" << scene << endl;
    
}

void OSCAbleController::playScene(){
    ofxOscMessage m;
    m.setAddress("/live/play");
    oscSend.sendMessage(m);
    m.clear();
}

void OSCAbleController::stopScene(){
    ofxOscMessage m;
    m.setAddress("/live/stop");
    oscSend.sendMessage(m);
    m.clear();
}

//--------------------------------------------------------------
void OSCAbleController::keyPressed(int key){
    switch(key) {
        case 'w': nextScene(); break;
        case 'q': prevScene(); break;
        case ' ': playScene(); break;
        case 's': stopScene(); break;
        case 't': track++; break;
        case 'g': track--; break;
        case 'd': device++; break;
        case 'c': device--; break;
        case 'p': param++; break;
        case 'l': param--; break;
        case 'k': returnValFromInstrumentDevice(track, device, param); break;
        case 'j': returnValFromInstrumentAllDevices(track, device); break;
    }
    
//    cout << "Track:" << track << " Device:" << device <<  " Param:" << param << endl;
}
