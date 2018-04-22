//
//  MyoManager.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#include "MyoManager.hpp"

MyoManager::MyoManager()
{

    myo.setup();
    
    //Load GUI
    myoGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    
    myoGui->setPosition(ofGetWidth()/2,0);
    myoGui->setAssetPath("");
    
    myoGui->addHeader("Myo Functions");
    myoGui->addToggle("Calibrate Sensors");
    myoGui->addButton("Clear Calibration");

    myoGui->addSlider("Lower Threshold", 0, 60);
    myoGui->addSlider("Higher Threshold", 30, 100);
    
    //Setup GloveGui Events
    myoGui->onToggleEvent(this, &MyoManager::onToggleEvent);
    myoGui->onButtonEvent(this, &MyoManager::onButtonEvent);
    myoGui->onSliderEvent(this, &MyoManager::onSliderEvent);

    for (int i = 0; i < 1; i++){
        Feature feat;
        
        //Need to intialise vectors in data structure
        for (int j = 0; j < 28; j++)
            feat.emgRMSRatio.push_back(0);
        
        for (int j = 0; j < 8; j++){
            feat.emgRaw.push_back(0);
            feat.emgRMS.push_back(0);
            feat.emgMin.push_back(0);
            feat.emgMax.push_back(0);
            feat.emgBayes.push_back(0);
            
            //Set window size of all 8 emg streams
            emgStreams.push_back(new rapidStream<double>(10));
            emgStreams[j]->bayesSetJumpRate(5.0);
            emgStreams[j]->bayesSetMVC(-1);
        }
        
        feature.push_back(feat);
        
        for (int j = 0; j < 3; j++)
            accStreams.push_back(new rapidStream<double>(10));
        
        
    }
    
    //Load Myo Model
    model.loadModel("./MyoModel/myo.obj");
    model.setPosition(ofGetWidth() * 0.25, ofGetHeight() * 0.25, 0);
    model.setScale(0.3,0.3,0.3);
    
    light.enable();
}

//-------------------------------

MyoManager::~MyoManager(){
    myo.stop();
}

//-------------------------------

void MyoManager::update(){
    if (ofGetFrameNum() % 3 == 0){
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            
            feature[i].id = myo.getDevices()[i]->getId();
            
            feature[i].pitch = myo.getDevices()[i]->getPitch();
            feature[i].yaw = myo.getDevices()[i]->getYaw();
            feature[i].roll = myo.getDevices()[i]->getRoll();
            
            feature[i].acc = myo.getDevices()[i]->getAccel();
            feature[i].gyro = myo.getDevices()[i]->getGyro();
            feature[i].quat = myo.getDevices()[i]->getQuaternion();
            
            for( int j = 0; j < 3; j++){
                accStreams[j]->pushToWindow(feature[i].acc[j]);
                feature[i].accFOD[j] = accStreams[j]->acceleration();
            }
            
            //Push raw emg values into rapidStream to utilise other functions RMS, Zero Crossings etc
            for (int j = 0; j < 8; j++){
                
                feature[i].emgRaw[j] = (double)myo.getDevices()[i]->getEmgSamples()[j];
                emgStreams[j]->pushToWindow( feature[i].emgRaw[j] );
                feature[i].emgRMS[j] = emgStreams[j]->rms();
                feature[i].emgMin[j] = emgStreams[j]->minimum();
                feature[i].emgMax[j] = emgStreams[j]->maximum();
    //            feature[i].zeroCrosses[j] = emgStreams[j]->numZeroCrossings();
    //            for (int k = 0; k < 100; k++){
                feature[i].emgBayes[j] = emgStreams[j]->bayesFilter( feature[i].emgRMS[j]/100 );
                
    //            }
            }
            
//            cout << feature[i].emgBayes[0] << endl;
            
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
                     feature[i].emgRMSRatio[index] = feature[i].emgBayes[num] / feature[i].emgBayes[den];
                     index++;
                    
                }
                num++;
            }
            
            if (calibrateActivate){
    //            calibrate(feature[i]);
            }
        }
    }
}

//-------------------------------

void MyoManager::draw(){
    drawGui(ofVec2f(50, 12));
}

//-------------------------------

//Adapted code from the ofx example in /addon/ assimpExample
void MyoManager::drawGui(ofVec2f pos){
    ofSetColor(255);
    
    for ( int i=0; i < myo.getDevices().size(); i++ ) {
        ofDrawBitmapString("Myo: " + ofToString(myo.getDevices()[i]->getId()), pos.x, pos.y + 20);
        
        drawGraph(ofVec2f(pos.x, pos.y + 50),  "RMS", feature[i].emgRMS, 0, 127);
        drawGraph(ofVec2f(pos.x, pos.y + 150), "RMSRatio", feature[i].emgRMSRatio, 0, 50);
        drawGraph(ofVec2f(pos.x, pos.y + 350), "BayesFitler", feature[i].emgBayes, 0, 1);
        drawGraph(ofVec2f(pos.x, pos.y + 450), "Gyro", feature[i].gyro, -50, 50);
        drawGraph(ofVec2f(pos.x, pos.y + 550), "Acc", feature[i].acc, -10, 10);
        drawGraph(ofVec2f(pos.x, pos.y + 650), "Acc 1st Order", feature[i].accFOD, -10, 10);

        drawModel(pos, i);
    }
}

void MyoManager::drawModel(ofVec2f pos, int id){
    ofPushStyle();
    {
        //Enable lighting only on the model
        ofEnableLighting();
        ofEnableSeparateSpecularLight();
        
        //Depth test renders the z axis of the model so it is whole
        ofEnableDepthTest();
        
        ofPushMatrix();
        {
            ofTranslate( model.getPosition().x, 500 + model.getPosition().y);
            ofRotate(ofRadToDeg(feature[id].quat.x()), 0, 0, 1);
            ofRotate(ofRadToDeg(feature[id].quat.y()), 1, 0, 0);
            ofRotate(ofRadToDeg(feature[id].quat.z()), 0, 1, 0);
            ofTranslate(-model.getPosition().x, -model.getPosition().y);
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

//-------------------------------

void MyoManager::drawGraph(ofVec2f pos, string title, vector<double> values, int min, int max){
    ofDrawBitmapString(title, pos.x, pos.y);
    
    ofSetColor(255);
    for (int i = 0; i < values.size(); i++){
        double valueMapped = ofMap(values[i], min, max, 0, 100);
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), valueMapped, 5);
        
//        if (title == "RMS" && calibrateActivate) {
////            ofDrawBitmapString(int(feature[id].emgMin[i]), 100 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
////            ofDrawBitmapString(int(feature[id].emgMax[i]), 140 + pos.x, 15 + pos.y + ( (i+1) * 10)    );
//        }
    }
    
//    if (title == "RMS") {
//        ofPushStyle();
//        {
//            ofSetColor(255,0,0);
//            ofDrawLine(pos.x + lowerThresh, pos.y+10, pos.x + lowerThresh, pos.y+60);
//            
//            ofSetColor(0,255,0);
//            ofDrawLine(pos.x + higherThresh, pos.y+10, pos.x + higherThresh, pos.y+60);
//        }
//        ofPopStyle();
//    }
}

void MyoManager::drawGraph(ofVec2f pos, string title, ofVec3f values, int min, int max){
    ofDrawBitmapString(title, pos.x, pos.y);
    
    ofSetColor(255);
    
    for (int i = 0; i < 3; i++){
        double valueMapped = ofMap(values[i], min, max, -50, 50);
        ofDrawRectangle(pos.x + 50, 20 + pos.y + (5 * i), valueMapped, 5);
    }
}

//-------------------------------

void MyoManager::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Clear Calibration"){
//        for (int i = 0; i < feature.size(); i ++){
//            feature[i].emgMin = {0,0,0,0,0,0,0,0};
//            feature[i].emgMax = {0,0,0,0,0,0,0,0};
//            feature[i].accMin = ofVec3f(0,0,0);
//            feature[i].accMin = ofVec3f(0,0,0);
//            feature[i].gyroMin = ofVec3f(0,0,0);
//            feature[i].gyroMax = ofVec3f(0,0,0);
//            feature[i].quatMin = ofQuaternion(0,0,0,0);
//            feature[i].quatMax = ofQuaternion(0,0,0,0);
//        }
    }
}

//-------------------------------

void MyoManager::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Calibrate") calibrateActivate = e.target->getChecked();
//    if (guiLabel == "Zero Crossings") seeZeros = e.target->getChecked();
//    if (guiLabel == "RMS Ratios") seeRMSRatio = e.target->getChecked();
//    if (guiLabel == "RMS") seeRMS = e.target->getChecked();
//    if (guiLabel == "Bayes") seeBayes = e.target->getChecked();
}

//-------------------------------


void MyoManager::onSliderEvent(ofxDatGuiSliderEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Lower Threshold") lowerThresh = e.target->getValue();
    if (guiLabel == "Higher Threshold") higherThresh = e.target->getValue();
    
}

        
