//
//  OSCController.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 22/03/2018.
//

#include "OSCController.hpp"

OSCController::OSCController(){
    //Send to LiveOSC
    oscSend.setup("127.0.0.1", 9001);
}

OSCController::~OSCController(){
    
}

void OSCController::update(){
    if (ofGetFrameNum() % 10 == 0){
//        streamToInstrumentDevice("/live/device", 1, 1, 1, 255);c
        cout << "aasasdas" << endl;
        streamToInstrumentDevice("/live/device", 6, 0, 8, ofMap(ofGetMouseY(), 0,  ofGetWidth(), 0 , 200));
    }
}

void OSCController::streamToInstrumentDevice(string address, int track, int device, int parameter, float value){
    ofxOscMessage m;
    m.setAddress(address);
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    m.addFloatArg(value); // Value
    oscSend.sendMessage(m);
    m.clear();
}

void ofApp::streamToInstrumentDevice(string address, int track, int device, int parameter, float value){
    ofxOscMessage m;
    m.setAddress(address);
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    m.addFloatArg(value); // Value
    oscSender.sendMessage(m);
    m.clear();
}

void ofApp::returnValFromInstrumentAllDevices(int track, int device){
    ofxOscMessage m;
    m.setAddress("/live/device/param");
    m.addIntArg(track); //Track
    m.addIntArg(device);
    oscSender.sendMessage(m);
    m.clear();
}


void ofApp::returnValFromInstrumentDevice(int track, int device, int parameter){
    ofxOscMessage m;
    m.setAddress("/live/device/range");
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    oscSender.sendMessage(m);
    m.clear();
}


void ofApp::nextScene(){
    scene++;
    scene = abs(scene % sceneNum);
    ofxOscMessage m;
    m.setAddress("/live/play/scene");
    m.addIntArg(scene);
    oscSender.sendMessage(m);
    m.clear();
    
    cout << "Scene :" << scene << endl;
}

void ofApp::prevScene(){
    scene--;
    scene = abs(scene % sceneNum);
    ofxOscMessage m;
    m.setAddress("/live/play/scene");
    m.addIntArg(scene);
    oscSender.sendMessage(m);
    m.clear();
    
    cout << "Scene :" << scene << endl;
    
}

void ofApp::playScene(){
    ofxOscMessage m;
    m.setAddress("/live/play");
    oscSender.sendMessage(m);
    m.clear();
}

void ofApp::stopScene(){
    ofxOscMessage m;
    m.setAddress("/live/stop");
    oscSender.sendMessage(m);
    m.clear();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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
    
    cout << "Track:" << track << " Device:" << device <<  " Param:" << param << endl;
}
