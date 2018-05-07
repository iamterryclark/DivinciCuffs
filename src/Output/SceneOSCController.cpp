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
        vector<double> paramValues(6);
        
        paramValues[0] = ml.mlGui->getSlider("Ableton Param 1")->getValue();
        paramValues[1] = ml.mlGui->getSlider("Ableton Param 2")->getValue();
        paramValues[2] = ml.mlGui->getSlider("Ableton Param 3")->getValue();
        paramValues[3] = ml.mlGui->getSlider("Ableton Param 4")->getValue();
        paramValues[4] = ml.mlGui->getSlider("Ableton Param 5")->getValue();
        paramValues[5] = ml.mlGui->getSlider("Ableton Param 6")->getValue();
        
        if (!(ml.sceneActivate[ml.sceneNum])){
            switch(ml.sceneNum){
                case 0:
                    
                    paramValues[3] = ofMap(paramValues[3], 0 , 1, 0, 20);
                    
                    streamToInstrumentDevice("/live/device", 1, 0, 2, paramValues[0]);
                    streamToInstrumentDevice("/live/device", 1, 0, 3, paramValues[1]);
                    streamToInstrumentDevice("/live/device", 1, 0, 6, paramValues[2]);
                    streamToInstrumentDevice("/live/device", 1, 0, 13, paramValues[3]);
                    
                    break;
                case 1:
                    paramValues[0] = ofMap(paramValues[0], 0, 1, 0, 127);
                    paramValues[1] = ofMap(paramValues[1], 0, 1, 0, 127);
                    
                    streamToInstrumentDevice("/live/device", 2, 1, 5, paramValues[0]);
                    streamToInstrumentDevice("/live/device", 2, 1, 6, paramValues[1]);
                    break;
                case 2:
                    streamToInstrumentDevice("/live/device", 3, 0, 159, paramValues[0]);
                    streamToInstrumentDevice("/live/device", 3, 0, 115, paramValues[1]);
                    streamToInstrumentDevice("/live/device", 3, 0, 117, paramValues[2]);
                    streamToInstrumentDevice("/live/device", 3, 0, 138, paramValues[3]);
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
            
            switch(ml.sceneNum){
                case 0:
                    paramValues[3]  = (int)ofMap(paramValues[3], 0, 1, 0, 20);
                    
                    streamToInstrumentDevice("/live/device", 1, 0, 2, paramValues[0]);
                    streamToInstrumentDevice("/live/device", 1, 0, 3, paramValues[1]);
                    streamToInstrumentDevice("/live/device", 1, 0, 6, paramValues[2]);
                    streamToInstrumentDevice("/live/device", 1, 0, 13, paramValues[3]);
                    break;
                case 1:
                    paramValues[0] = ofMap(paramValues[0], 0, 1, 0, 127);
                    paramValues[1] = ofMap(paramValues[1], 0, 1, 0, 100);
                    
                    streamToInstrumentDevice("/live/device", 2, 1, 5, paramValues[0]);
                    streamToInstrumentDevice("/live/device", 2, 1, 6, paramValues[1]);
                    break;
                case 2:
                    
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
