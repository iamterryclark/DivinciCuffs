//
//  MyoManager.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#include "MyoManager.hpp"

MyoManager::MyoManager(){
    
    myo.setup();
    //Set window size of all 8 emg streams
    for (int i = 0; i < 2; i++){
        Feature feat;
        feat.acc = ofVec3f(0,0,0);
        feat.gyro = ofVec3f(0,0,0);
        feat.roll = 0.0;
        feat.pitch = 0.0;
        feat.yaw = 0.0;
        feat.emg = { 0.0,0.0,0.0,0.0,
            0.0,0.0,0.0,0.0 };
        
        feature.push_back(feat);
    }
    
    //Setup GloveGui Events

    
    //Load Myo Model
    model.loadModel("./MyoModel/myo.obj");
    model.setPosition(ofGetWidth() * 0.25, ofGetHeight() * 0.25, 0);
    model.setScale(0.3,0.3,0.3);
    
    light.enable();
}

MyoManager::~MyoManager(){
    myo.stop();
}

void MyoManager::update(){
    for ( int i=0; i<myo.getDevices().size(); i++ ) {
        
        feature[i].id = myo.getDevices()[i]->getId();
        feature[i].acc = myo.getDevices()[i]->getAccel();
        feature[i].gyro = myo.getDevices()[i]->getGyro();
        feature[i].roll = myo.getDevices()[i]->getRoll();
        feature[i].pitch = myo.getDevices()[i]->getPitch();
        feature[i].yaw = myo.getDevices()[i]->getYaw();
        
        //Issue: I cannot quiet work out how to initialise a vector of rapidStream objects correctly and as a result this produces memory issues. So I have been forced to initialise them all seperately.
        
        //Capture raw emg signal
        double emgSample0 = (double)myo.getDevices()[i]->getEmgSamples()[0];
        double emgSample1 = (double)myo.getDevices()[i]->getEmgSamples()[1];
        double emgSample2 = (double)myo.getDevices()[i]->getEmgSamples()[2];
        double emgSample3 = (double)myo.getDevices()[i]->getEmgSamples()[3];
        double emgSample4 = (double)myo.getDevices()[i]->getEmgSamples()[4];
        double emgSample5 = (double)myo.getDevices()[i]->getEmgSamples()[5];
        double emgSample6 = (double)myo.getDevices()[i]->getEmgSamples()[6];
        double emgSample7 = (double)myo.getDevices()[i]->getEmgSamples()[7];
        
        //Push back into rapidStream which will then acquire emg stream over N bufersize
        emgStream0.pushToWindow(double(emgSample0));
        emgStream1.pushToWindow(double(emgSample1));
        emgStream2.pushToWindow(double(emgSample2));
        emgStream3.pushToWindow(double(emgSample3));
        emgStream4.pushToWindow(double(emgSample4));
        emgStream5.pushToWindow(double(emgSample5));
        emgStream6.pushToWindow(double(emgSample6));
        emgStream7.pushToWindow(double(emgSample7));
        
        // get root mean squareed value of emg signal to help smooth out signal
        feature[i].emg[0] = emgStream0.rms();
        feature[i].emg[1] = emgStream1.rms();
        feature[i].emg[2] = emgStream2.rms();
        feature[i].emg[3] = emgStream3.rms();
        feature[i].emg[4] = emgStream4.rms();
        feature[i].emg[5] = emgStream5.rms();
        feature[i].emg[6] = emgStream6.rms();
        feature[i].emg[7] = emgStream7.rms();
    }
}

void MyoManager::draw(){
    drawGui(ofVec2f(50, 12));
}


//Adapted code from the ofx example in /addon/ assimpExample
void MyoManager::drawGui(ofVec2f pos){
    ofSetColor(255);
    
    for ( int i=0; i < myo.getDevices().size(); i++ ) {
        ofDrawBitmapString("Myo: " + ofToString(myo.getDevices()[i]->getId()), pos.x, pos.y + 80);
        drawEmg(ofVec2f(pos.x, pos.y + 100), i);
        drawGyro(ofVec2f(pos.x, pos.y + 200), i);
        drawAccel(ofVec2f(pos.x, pos.y + 300), i);
        
        //Enable lighting only on the model
        ofEnableLighting();
        ofEnableSeparateSpecularLight();
        
        //Depth test renders the z axis of the model so it is whole
        ofEnableDepthTest();
        ofPushMatrix();
        {
            ofTranslate(model.getPosition().x+100, model.getPosition().y, 0);
            ofRotate(ofRadToDeg(feature[i].pitch), 1, 0, 0);
            ofRotate(ofRadToDeg(feature[i].yaw+45),   0, 1, 0);
            ofRotate(ofRadToDeg(feature[i].roll),   0, 0, 1);
            ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
            model.drawFaces();
        }
        ofPopMatrix();
        
        //Disable everything as to not effect ofxDatGui
        ofDisableDepthTest();
        ofDisableLighting();
        ofDisableSeparateSpecularLight();
    }
    
}

void MyoManager::drawEmg(ofVec2f pos, int id){
    ofDrawBitmapString("EMG", pos.x, pos.y);
    
    for (int i = 0; i < 8; i++){
        ofDrawBitmapString(i, -10 + pos.x, 10 + pos.y + ( (i+1) *10) );
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), feature[id].emg[i], 5);
    }
}
                             
void MyoManager::drawGyro(ofVec2f pos, int id){
    ofDrawBitmapString("Gyro", pos.x, pos.y);
    
    vector<string> labels = { "x", "y", "z"};
     
    for (int i = 0; i < 3; i++){
        float gyro = ofMap(feature[id].gyro[i], 0, 300, 0, 50);
        ofDrawBitmapString(labels[i], -10 + pos.x, 10 + pos.y + ( (i+1) *10));
        ofDrawRectangle(50 + pos.x, 20 + pos.y + (5 * i), gyro, 5);
    }
    
}
                             
void MyoManager::drawAccel(ofVec2f pos, int id){
    ofDrawBitmapString("Accel", pos.x, pos.y);
    
    vector<string> labels = { "x", "y", "z"};
 
    for (int i = 0; i < 3; i++){
        float accel = ofMap(feature[id].acc[i], 0, 4, 0, 100);
        ofDrawBitmapString(labels[i], -10 + pos.x, 10 + pos.y + ( (i+1) *10));
        ofDrawRectangle(50 + pos.x, 20 + pos.y + (5 * i), accel, 5);
    }
}

void MyoManager::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
 
    if (guiLabel == "Activate")
        myoActivated = e.target->getChecked();
}
        

        
