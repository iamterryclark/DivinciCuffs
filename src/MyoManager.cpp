//
//  MyoManager.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#include "MyoManager.hpp"

MyoManager::MyoManager(){
//    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    myo.setup();
    
    ofDisableArbTex(); // we need GL_TEXTURE_2D for our models coords.
    
    //Set window size of all 8 emg streams
    emgStream = {
        windowSize,windowSize,windowSize,windowSize,
        windowSize,windowSize,windowSize,windowSize
    };
    
    featureVec.push_back(Feature());

    //Gui Setup
    myoGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    myoGui->setAssetPath(""); //this has to be done due to bug with gui
    
    myoGui->setPosition(0,0);
    myoGui->addHeader("Myo Manager");
    myoGui->addToggle("Activate");
    
    //Setup GloveGui Events
    myoGui->onToggleEvent(this, &MyoManager::onToggleEvent);
    
    //Load Myo Model
    model.loadModel("./MyoModel/myo.obj");
    model.setPosition(ofGetWidth() * 0.25, ofGetHeight() * 0.25, 0);
    model.setScale(0.3,0.3,0.3);
    
    light.enable();
    light.setPosition(model.getPosition() + ofPoint(0, 0, 100));
}

MyoManager::~MyoManager(){
    myo.stop();
}

void MyoManager::update(){
    if (myoActivated){
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            featureVec[i].id = myo.getDevices()[i]->getId();
            featureVec[i].acc = myo.getDevices()[i]->getAccel();
            featureVec[i].gyro = myo.getDevices()[i]->getGyro();
            featureVec[i].roll = myo.getDevices()[i]->getRoll();
            featureVec[i].pitch = myo.getDevices()[i]->getPitch();
            featureVec[i].yaw = myo.getDevices()[i]->getYaw();

            for (int j = 0; j < emgStream.size(); j++ ){
                //Capture raw emg signal
                featureVec[i].emg[j] = (double)myo.getDevices()[i]->getEmgSamples()[j];

                //Push back into rapidStream which will then acquire emg stream over N bufersize
                emgStream[j].pushToWindow(double(featureVec[i].emg[j]));

                //get root mean squareed value of emg signal to help smooth out signal
                //featureVec[i].emg[j] = emgStream[j].rms();
            }
        }
    }
}

vector<Feature> MyoManager::getFeatureVec(){
    return featureVec;
}

void MyoManager::draw(){
    drawGui(ofVec2f(50, 12));
}


//Adapted code from the ofx example in /addon/ assimpExample
void MyoManager::drawGui(ofVec2f pos){
    ofSetColor(255);
    
    ofEnableDepthTest();

//    light.enable();
//    ofEnableSeparateSpecularLight();
    for ( int i=0; i < myo.getDevices().size(); i++ ) {
        ofDrawBitmapString("Myo: " + ofToString(myo.getDevices()[i]->getId()), pos.x, pos.y + 80);
        drawEmg(ofVec2f(pos.x, pos.y + 200), i);
        drawGyro(ofVec2f(pos.x, pos.y + 300), i);
        drawAccel(ofVec2f(pos.x, pos.y + 400), i);
        
        ofPushMatrix();
        {
            ofTranslate(model.getPosition().x, model.getPosition().y, 0);
            
            ofRotate(ofRadToDeg(featureVec[i].pitch), 1, 0, 0);
            ofRotate(ofRadToDeg(featureVec[i].yaw),   0, 1, 0);
            ofRotate(ofRadToDeg(featureVec[i].roll),  0, 0, 1);

            ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
            
            model.drawFaces();
        }
        ofPopMatrix();
    }
//    ofDisableDepthTest();
//    light.disable();
    
//    ofDisableLighting();
//    ofDisableSeparateSpecularLight();
}

void MyoManager::drawEmg(ofVec2f pos, int id){
    ofDrawBitmapString("EMG", pos.x, 200);
    
    for (int i = 0; i < 8; i++){
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), featureVec[id].emg[i], 5);
    }
    
}

void MyoManager::drawGyro(ofVec2f pos, int id){
    ofDrawBitmapString("Gyro", pos.x, pos.y);
    
    for (int i = 0; i < 4; i++){
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), featureVec[id].gyro[i], 5);
    }
}

void MyoManager::drawAccel(ofVec2f pos, int id){
    ofDrawBitmapString("Accel", pos.x, pos.y);
    
    for (int i = 0; i < 3; i++){
        
        float accel = ofMap(featureVec[id].acc[i], 0, 10, 0, 100);
        
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), accel, 5);
    }
}


void MyoManager::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Activate")
        myoActivated = e.target->getChecked();
}

//This returns a reference to the instance;
//MyoManager& MyoManager::get_myoInstance() {

//    static MyoManager *myoManager = NULL;
//    
//    if (myoManager == NULL) {
//        myoManager = new MyoManager();
//        cout << "Myo Manager Created" << endl;
//    }
//    
//    return *myoManager;
//}
//
//void MyoManager::destroy_myoInstance() {
//    MyoManager* myoManager = &get_myoInstance();
//    
//    if (myoManager != NULL) delete myoManager;
//    
//    cout << "Myo Manager Destroyed" << endl;
//}

