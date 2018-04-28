//
//  OSCController.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 22/03/2018.

#include "SceneOSCController.hpp"

SceneOSCController::SceneOSCController(){
    //Send to LiveOSC
    oscSend.setup(HOST, PORTOUT);
    oscRec.setup(PORTRTN);
}

SceneOSCController::~SceneOSCController(){
    
}

void SceneOSCController::update(MachineLearning &ml){
    
    if (ofGetFrameNum() % 2 == 0){
        if (!ml.sceneActivate){
            double value1, value2, value3, value4;
            
            switch(ml.sceneNum){
                case 1:
                    value1 = ml.mlGui->getSlider("Ableton Param 1")->getValue();
                    value2 = ml.mlGui->getSlider("Ableton Param 2")->getValue();
                    value3 = ml.mlGui->getSlider("Ableton Param 3")->getValue();
                    value4 = ofMap((int)ml.mlGui->getSlider("Ableton Param 4")->getValue(), 0, 1, 0, 20);
                    
                    streamToInstrumentDevice("/live/device", 1, 0, 2, value1);
                    streamToInstrumentDevice("/live/device", 1, 0, 3, value2);
                    streamToInstrumentDevice("/live/device", 1, 0, 6, value3);
                    streamToInstrumentDevice("/live/device", 1, 0, 13, value4);
                    break;
                case 2:
                    value1 = ofMap(ml.outputVals[0], 0, 254, 0, 1);
                    value2 = ofMap(ml.outputVals[1], 0, 254, 0, 1);
                    
                    streamToInstrumentDevice("/live/device", 2, 1, 5, value1);
                    streamToInstrumentDevice("/live/device", 2, 1, 6, value2);
                    break;
                default:
                    
                    break;
            }
            
            while(oscRec.hasWaitingMessages())
            {
                // get the next message
                ofxOscMessage m;
                oscRec.getNextMessage(m);
                
                // check for mouse moved message
                if(m.getAddress() == "/live/device/param")
                {
                    //Show the message to screen when ableton interacts
                    paramOut =  ofToString(m.getArgAsInt32(0)) + " | " +
                    ofToString(m.getArgAsInt32(1)) + " | " +
                    ofToString(m.getArgAsInt32(2)) + " | " +
                    ofToString(m.getArgAsFloat(3)) + " | " +
                    m.getArgAsString(4);
                }
            }
        } else {
            double value1, value2, value3, value4;

            switch(ml.sceneNum){
                case 1:
                    value1 = ofMap(ml.outputVals[0], 0, 255, 0, 1);
                    value2 = ofMap(ml.outputVals[1], 0, 255, 0, 1);
                    value3 = ofMap(ml.outputVals[2], 0, 255, 0, 1);
                    value4 = (int)ofMap(ml.outputVals[3], 0, 255, 0, 20);
                    
                    streamToInstrumentDevice("/live/device", 1, 0, 2, value1);
                    streamToInstrumentDevice("/live/device", 1, 0, 3, value2);
                    streamToInstrumentDevice("/live/device", 1, 0, 13, value3);
                    break;
                case 2:
                    value1 = ofMap(ml.outputVals[0], 0, 254, 0, 1);
                    value2 = ofMap(ml.outputVals[1], 0, 254, 0, 1);
                    
                    streamToInstrumentDevice("/live/device", 2, 1, 5, value1);
                    streamToInstrumentDevice("/live/device", 2, 1, 6, value2);
                    break;
                case 3:
                    
                    break;
                default:
                    
                    break;
            }
        }
    }
}

void SceneOSCController::draw(ofVec2f pos){
    ofSetColor(255);
    ofDrawBitmapString("Ablton OSC", pos.x, pos.y);
    ofDrawBitmapString("Track " + ofToString(track), pos.x, pos.y + 20);
    ofDrawBitmapString("Device " + ofToString(device), pos.x, pos.y + 40);
    ofDrawBitmapString("Param " + ofToString(param), pos.x, pos.y + 60);
    
    ofDrawBitmapString(paramOut, pos.x, pos.y + 80 );
}

void SceneOSCController::streamToInstrumentDevice(string address, int track, int device, int parameter, float value){
    ofxOscMessage m;
    m.setAddress(address);
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    m.addFloatArg(value); // Value
    oscSend.sendMessage(m);
    m.clear();
}

void SceneOSCController::returnValFromInstrumentAllDevices(int track, int device){
    ofxOscMessage m;
    m.setAddress("/live/device/param");
    m.addIntArg(track); //Track
    m.addIntArg(device);
    oscSend.sendMessage(m);
    m.clear();
}


void SceneOSCController::returnValFromInstrumentDevice(int track, int device, int parameter){
    ofxOscMessage m;
    m.setAddress("/live/device/range");
    m.addIntArg(track); //Track
    m.addIntArg(device);
    m.addIntArg(parameter);
    oscSend.sendMessage(m);
    m.clear();
}


void SceneOSCController::nextScene(){
    scene++;
    scene = abs(scene % TOTALSCENES);
    ofxOscMessage m;
    m.setAddress("/live/play/scene");
    m.addIntArg(scene);
    oscSend.sendMessage(m);
    m.clear();
    
    cout << "Scene :" << scene << endl;
}

void SceneOSCController::prevScene(){
    scene--;
    scene = abs(scene % TOTALSCENES);
    ofxOscMessage m;
    m.setAddress("/live/play/scene");
    m.addIntArg(scene);
    oscSend.sendMessage(m);
    m.clear();
    
    cout << "Scene :" << scene << endl;
}

void SceneOSCController::playScene(){
    ofxOscMessage m;
    m.setAddress("/live/play");
    oscSend.sendMessage(m);
    m.clear();
}

void SceneOSCController::stopScene(){
    ofxOscMessage m;
    m.setAddress("/live/stop");
    oscSend.sendMessage(m);
    m.clear();
}

//--------------------------------------------------------------
void SceneOSCController::keyPressed(int key){
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
