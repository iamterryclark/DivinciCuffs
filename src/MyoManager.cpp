//
//  MyoManager.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#include "MyoManager.hpp"

MyoManager::MyoManager(){
    
    myo.setup();
    
    //Load GUI
    myoGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    
    myoGui->setPosition(ofGetWidth()/2,0);
    myoGui->setAssetPath("");
    myoGui->addHeader("Myo Functions");
    myoGui->addToggle("Calibrate");
    myoGui->addButton("Clear Calibration");
    myoGui->addSlider("Lower Threshold", 0, 60);
    myoGui->addSlider("Higher Threshold", 30, 100);
    myoGui->addSlider("Set RMS Window Size", 1, 60);
    
    
    //Setup GloveGui Events
    myoGui->onToggleEvent(this, &MyoManager::onToggleEvent);
    myoGui->onButtonEvent(this, &MyoManager::onButtonEvent);
    myoGui->onSliderEvent(this, &MyoManager::onSliderEvent);

    //Set window size of all 8 emg streams
    for (int i = 0; i < 1; i++){
        Feature feat;
        
        for (int j = 0; j < feat.emgRMS.size(); j++){
            feat.emgMin[j] = numeric_limits<double>::max();
            feat.emgMax[j] = numeric_limits<double>::min();
        }
        
        feature.push_back(feat);
    }
    
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
        feature[i].quat = myo.getDevices()[i]->getQuaternion();
        
        accStream0.pushToWindow(feature[i].acc.x);
        accStream1.pushToWindow(feature[i].acc.y);
        accStream2.pushToWindow(feature[i].acc.z);
        
        feature[i].accFOD[0] = accStream0.velocity();
        feature[i].accFOD[1] = accStream1.velocity();
        feature[i].accFOD[2] = accStream2.velocity();
        
        //Issue: I cannot quiet work out how to initialise a vector of rapidStream objects correctly and as a result this produces memory issues. So I have been forced to initialise them all seperately.
        
        //Capture raw emg signal
        vector<double> emgSamples;
        
        for (int j = 0; j < 8; j++){
             emgSamples.push_back( (double)myo.getDevices()[i]->getEmgSamples()[j]);
        }
                
        //Push back into rapidStream which will then acquire emg stream over N bufersize
        emgStream0.pushToWindow( double(emgSamples[0]) );
        emgStream1.pushToWindow( double(emgSamples[1]) );
        emgStream2.pushToWindow( double(emgSamples[2]) );
        emgStream3.pushToWindow( double(emgSamples[3]) );
        emgStream4.pushToWindow( double(emgSamples[4]) );
        emgStream5.pushToWindow( double(emgSamples[5]) );
        emgStream6.pushToWindow( double(emgSamples[6]) );
        emgStream7.pushToWindow( double(emgSamples[7]) );
        
        //Get Form Factor root mean squareed value of emg signal to help smooth out signal
        feature[i].emgRMS[0] = emgStream0.rms();
        feature[i].emgRMS[1] = emgStream1.rms();
        feature[i].emgRMS[2] = emgStream2.rms();
        feature[i].emgRMS[3] = emgStream3.rms();
        feature[i].emgRMS[4] = emgStream4.rms();
        feature[i].emgRMS[5] = emgStream5.rms();
        feature[i].emgRMS[6] = emgStream6.rms();
        feature[i].emgRMS[7] = emgStream7.rms();
       
        /*
            I needed to generate the below number sequence in order to
            capture more expressive information about which position
            the arm is in through the EMG channels.
         
             Note: The numbers relate to EMG channels of the Myo
                   the values of each sensor's RMS value is the propotion
                   of how much we are a muscle eithin that given sensor.
         
             1/2  1/3  1/4   1/5  1/6  1/7  1/8
             2/3  2/4  2/5   2/6  2/7  2/8
             3/4  3/5  3/6   3/7  3/8
             4/5  4/6  4/7   4/8
             5/6  5/7  5/8
             6/7  6/8
             7/8
        */
        
        int num = 0, den = 0, index = 0;
        for (int j = 7; j > 0; j--){
//            num++;
//            cout << num << endl;
            for (int k = 0; k < j; k++){
//                cout << den << endl;
                 //cout << index << endl;
                 den = num + k + 1;
                 feature[i].emgRatioRMS[index] = feature[i].emgRMS[num] / feature[i].emgRMS[den];
                 index++;
                
            }
            num++;
        }
        
    }
    
    if (calibrateActivate){
        calibrate();
    }
}

void MyoManager::draw(){
    drawGui(ofVec2f(50, 12));
}

void MyoManager::calibrate(){
    for(int i = 0; i < myo.getDevices().size(); i++){
        
        for (int j = 0; j < 8; j++){
            // Min/Max Accellerometer
            if(feature[i].emgRMS[j] < feature[i].emgMin[j]){
                feature[i].emgMin[j] = feature[i].emgRMS[j];
            }
            
            if(feature[i].emgRMS[j] > feature[i].emgMax[j]){
                feature[i].emgMax[j] = feature[i].emgRMS[j];
            }
        }
        
        for (int j = 0; j < 3; j++){
            // Min/Max Accellerometer
            if(feature[i].acc[j] < feature[i].accMin[j]){
                feature[i].accMin[j] = feature[i].acc[j];
            }
            
            if(feature[i].acc[j] > feature[i].accMax[j]){
                feature[i].accMax[j] = feature[i].acc[j];
            }
            
             // Min/Max Gyro
             if(feature[i].gyro[j] < feature[i].gyroMin[j]){
                 feature[i].gyroMin[j] = feature[i].gyro[j];
             }
             
             if(feature[i].gyro[j] > feature[i].gyroMax[j]){
                 feature[i].gyroMax[j] = feature[i].gyro[j];
             }
             
             // Min/Max Quat
             if(feature[i].quat[j] < feature[i].quatMin[j]){
                 feature[i].quatMin[j] = feature[i].quat[j];
             }
             
             if(feature[i].quat[j] > feature[i].quatMax[j]){
                 feature[i].quatMax[j] = feature[i].quat[i];
             }
        }
    }
}


//Adapted code from the ofx example in /addon/ assimpExample
void MyoManager::drawGui(ofVec2f pos){
    ofSetColor(255);
    
    for ( int i=0; i < myo.getDevices().size(); i++ ) {
        ofDrawBitmapString("Myo: " + ofToString(myo.getDevices()[i]->getId()), pos.x, pos.y + 20);
        drawEmg(ofVec2f(pos.x, pos.y + 50), i);
        drawEMGRatio(ofVec2f(pos.x, pos.y + 150), i);
        drawGyro(ofVec2f(pos.x, pos.y + 250), i);
        drawAccel(ofVec2f(pos.x, pos.y + 250), i);
        drawAccelFOD(ofVec2f(pos.x, pos.y + 350), i);
        drawQuat(ofVec2f(pos.x, pos.y + 450), i);
        
        ofPushStyle();
        {
        //Enable lighting only on the model
            ofEnableLighting();
            ofEnableSeparateSpecularLight();
            
            //Depth test renders the z axis of the model so it is whole
            ofEnableDepthTest();
            ofPushMatrix();
            {
                ofTranslate(model.getPosition().x+100, model.getPosition().y, 0);
                ofRotate(ofRadToDeg(feature[i].quat.x()), 0, 0, 1);
                ofRotate(ofRadToDeg(feature[i].quat.y()), 1, 0, 0);
                ofRotate(ofRadToDeg(feature[i].quat.z()), 0, 1, 0);
                ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
                model.drawFaces();
            }
            ofPopMatrix();
            
            //Disable everything as to not effect ofxDatGui
            ofDisableDepthTest();
            ofDisableLighting();
            ofDisableSeparateSpecularLight();
        }
        ofPopStyle();
    }
}

void MyoManager::drawEmg(ofVec2f pos, int id){
    ofDrawBitmapString("EMG", pos.x, pos.y);
    
    vector<string> labels = { "1", ".", ".", ".", ".", "", "", "7"};
    
    ofSetColor(255);
    for (int i = 0; i < 8; i++){
        double emg = ofMap(feature[id].emgRMS[i], feature[id].emgMin[i], feature[id].emgMax[i], 0, 100);
        ofDrawBitmapString(labels[i], -10 + pos.x, 15 + pos.y + ( (i+1) * 6) );
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), emg, 5);
        
        if (calibrateActivate){
            ofDrawBitmapString(int(feature[id].emgMin[i]), 100 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
            ofDrawBitmapString(int(feature[id].emgMax[i]), 140 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
        }
    }
    
    ofSetColor(255,0,0);
    ofDrawLine(pos.x + lowerThresh, pos.y+10, pos.x + lowerThresh, pos.y+60);
    
    ofSetColor(0,255,0);
    ofDrawLine(pos.x + higherThresh, pos.y+10, pos.x + higherThresh, pos.y+60);
}


void MyoManager::drawEMGRatio(ofVec2f pos, int id){
    ofDrawBitmapString("EMGRatio", pos.x, pos.y);
    
    ofSetColor(255);
    for (int i = 0; i < 28; i++){
        double emgRatio = ofMap(feature[id].emgRatioRMS[i], 0, 15, 0, 100);
//        ofDrawBitmapString((int)feature[id].emgRatioRMS[i], pos.x * i, -10 + pos.y );
        ofDrawRectangle(pos.x + (5 * i), pos.y, 5, emgRatio);

    }
    
//    ofSetColor(255,0,0);
//    ofDrawLine(pos.x + lowerThresh, pos.y+10, pos.x + lowerThresh, pos.y+60);
//
//    ofSetColor(0,255,0);
//    ofDrawLine(pos.x + higherThresh, pos.y+10, pos.x + higherThresh, pos.y+60);
//
}
                             
void MyoManager::drawGyro(ofVec2f pos, int id){
    ofDrawBitmapString("Gyro", pos.x, pos.y);
    
    vector<string> labels = { "x", "y", "z"};
    
    for (int i = 0; i < 3; i++){
       float gyro = ofMap(feature[id].gyro[i], feature[id].gyroMin[i], feature[id].gyroMax[i], -50, 50);
        
        ofDrawBitmapString(labels[i], -10 + pos.x, 10 + pos.y + ( (i+1) *10));
        ofDrawRectangle(50 + pos.x, 20 + pos.y + (5 * i), gyro, 5);
        
        if (calibrateActivate){
            ofDrawBitmapString(int(feature[id].gyroMin[i]), 100 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
            ofDrawBitmapString(int(feature[id].gyroMax[i]), 140 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
        }
    }
}

void MyoManager::drawAccel(ofVec2f pos, int id){
    ofDrawBitmapString("Accel", pos.x, pos.y);
    
    vector<string> labels = { "x", "y", "z"};
 
    for (int i = 0; i < 3; i++){
       float accel = ofMap(feature[id].acc[i], -1, 1, -50, 50);
        ofDrawBitmapString(labels[i], -10 + pos.x, 10 + pos.y + ( (i+1) *10));
        ofDrawRectangle(50 + pos.x, 20 + pos.y + (5 * i), accel, 5);
        
        if (calibrateActivate){
            ofDrawBitmapString(int(feature[id].accMin[i]), 100 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
            ofDrawBitmapString(int(feature[id].accMax[i]), 140 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
        }
    }
}

void MyoManager::drawAccelFOD(ofVec2f pos, int id){
    ofDrawBitmapString("Accel", pos.x, pos.y);
    
    vector<string> labels = { "x", "y", "z"};
    
    for (int i = 0; i < 3; i++){
        float accel = ofMap(feature[id].accFOD[i], -1, 1, -50, 50);
        ofDrawBitmapString(labels[i], -10 + pos.x, 10 + pos.y + ( (i+1) *10));
        ofDrawRectangle(50 + pos.x, 20 + pos.y + (5 * i), accel, 5);
    }
}

void MyoManager::drawQuat(ofVec2f pos, int id){
    ofDrawBitmapString("Qauternion", pos.x, pos.y);
    
    vector<string> labels = { "x", "y", "z"};
    
    for (int i = 0; i < 3; i++){
        float quat = ofMap(feature[id].quat[i], feature[id].quatMin[i], feature[id].quatMax[i], -50, 50);
        
        ofDrawBitmapString(labels[i], -10 + pos.x, 10 + pos.y + ( (i+1) *10));
        ofDrawRectangle(50 + pos.x, 20 + pos.y + (5 * i), quat, 5);
        
        if (calibrateActivate){
            //The quaternions are between -1 and 1 so wante to see only 2 decimal points
            //Reference: https://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
            float rdQuatMin = floorf(feature[id].quatMin[i] * 100) / 100;
            float rdQuatMax = floorf(feature[id].quatMax[i] * 100) / 100;
            
            ofDrawBitmapString(rdQuatMin, 100 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
            ofDrawBitmapString(rdQuatMax, 140 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
        }
    }
}

void MyoManager::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Clear Calibration"){
        for (int i = 0; i < feature.size(); i ++){
            feature[i].emgMin = {0,0,0,0,0,0,0,0};
            feature[i].emgMax = {0,0,0,0,0,0,0,0};
            feature[i].accMin = ofVec3f(0,0,0);
            feature[i].accMin = ofVec3f(0,0,0);
            feature[i].gyroMin = ofVec3f(0,0,0);
            feature[i].gyroMax = ofVec3f(0,0,0);
            feature[i].quatMin = ofQuaternion(0,0,0,0);
            feature[i].quatMax = ofQuaternion(0,0,0,0);
        }
    }
}

void MyoManager::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    if (guiLabel == "Calibrate") calibrateActivate = e.target->getChecked();
}


void MyoManager::onSliderEvent(ofxDatGuiSliderEvent e){
    string guiLabel = e.target->getLabel();
    if (guiLabel == "Lower Threshold") lowerThresh = e.target->getValue();
    if (guiLabel == "Higher Threshold") higherThresh = e.target->getValue();
    if(guiLabel == "Set RMS Window Size") windowSize = e.target->getValue();
}

        
