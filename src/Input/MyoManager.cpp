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
    
    myoGui->setPosition(ofGetWidth() - myoGui->getWidth(), 400);
    myoGui->setAssetPath("");
    
    myoGui->addHeader(" Myo Functions ");
    myoGui->addBreak()->setHeight(10.0f);
    myoGui->addLabel(" :: Sensor Psrameters :: ");
    myoGui->addSlider("Lower Threshold", 0, 100);
    myoGui->addSlider("Higher Threshold", 0, 100);
//    myoGui->addSlider("WindowSize", 1, 60);
    myoGui->addLabel(" :: Bayes Filter Params :: ");
    myoGui->addSlider("Bayes JumpRate", 1, 10);
    myoGui->addSlider("Bayes MVC", 1, 100);
    myoGui->addSlider("Bayes Diffusion", 1, 10);
    
    //Setup Myo Events Listeners
    myoGui->onButtonEvent(this, &MyoManager::onButtonEvent);
    myoGui->onSliderEvent(this, &MyoManager::onSliderEvent);

    for (int i = 0; i < 1; i++){
        Feature feat(i);
        feature.push_back(feat);
        
        for (int j = 0; j < 8; j++){
            //Set window size of all 8 emg streams
            emgRawStreams.push_back(new rapidStream<double>(windowSize));
//            emgNormStreams.push_back(new rapidStream<double>(windowSize));
            emgRawStreams[j]->bayesSetJumpRate(0);
            emgRawStreams[j]->bayesSetMVC(0);
            emgRawStreams[j]->bayesSetDiffusion(0);
        }
        
        for (int j = 0; j < 3; j++){
            accNormStreams.push_back(new rapidStream<double>(windowSize));
            accFODStreams.push_back(new rapidStream<double>(windowSize));
        }
    }
    
    //Load Myo Model
    model.loadModel("./MyoModel/myo.obj");
    model.setPosition(ofGetWidth() * 0.25, ofGetHeight() - (ofGetHeight() * 0.25), 0);
    model.setScale(0.2,0.2,0.2);
    
    light.enable();
}

//-------------------------------

MyoManager::~MyoManager(){
    myo.stop();
}

//-------------------------------

void MyoManager::update(){
    for ( int i=0; i<myo.getDevices().size(); i++ ) {
        
        feature[i].identity = myo.getDevices()[i]->getId();
        
        //For the model
        feature[i].acc.pitch = myo.getDevices()[i]->getPitch();
        feature[i].acc.yaw = myo.getDevices()[i]->getYaw();
        feature[i].acc.roll = myo.getDevices()[i]->getRoll();

        feature[i].acc.raw = myo.getDevices()[i]->getAccel();
        feature[i].acc.quat = myo.getDevices()[i]->getQuaternion();
        
        for( int j = 0; j < 3; j++){
            //Add accellerometer data to the rapidStream
            accNormStreams[j]->pushToWindow(feature[i].acc.quat[j]);
            
            //Capture the First Order Difference of Quat
            feature[i].acc.fod[j] = accNormStreams[j]->acceleration();
            
            //Push the FOD to another rapidstream object for smoothing, based on its windowSize
            accFODStreams[j]->pushToWindow(feature[i].acc.fod[j]);
            feature[i].acc.fodRMS[j] = accFODStreams[j]->rms();
        }
        
        //Push raw emg values into rapidStream to utilise other functions RMS, Zero Crossings etc
        for (int j = 0; j < 8; j++){
            //Capture raw EMG
            feature[i].emg.raw[j] = (double)myo.getDevices()[i]->getEmgSamples()[j];
            
            //Pass to rapidstream to get calculated rms, and other statistical analysis
            emgRawStreams[j]->pushToWindow(feature[i].emg.raw[j]);
            
            //Capture the statistical analysis for later passing to machine learning algorithm
            feature[i].emg.mean[j] = emgRawStreams[j]->mean();
            feature[i].emg.stdDev[j] = emgRawStreams[j]->standardDeviation();
//            feature[i].emg.zeroXs[j] = emgRawStreams[j]->numZeroCrossings();
//            cout << feature[i].emg.zeroXs[j] << endl;
           
            //Normalise EMG Values = (current - mean) / Standard Dev
//            feature[i].emg.rawNormalised[j] = (feature[i].emg.raw[j] - feature[i].emg.mean[j] ) / feature[i].emg.stdDev[j];
        
            feature[i].emg.rms[j] = emgRawStreams[j]->rms();
            feature[i].emg.min[j] = emgRawStreams[j]->minimum();
            feature[i].emg.max[j] = emgRawStreams[j]->maximum();
            feature[i].emg.bayesRMS[j] = emgRawStreams[j]->bayesFilter( feature[i].emg.rms[j] );
        }
        
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
//              cout << den << endl;
                //cout << index << endl;
                den = num + k + 1;
                feature[i].emg.rmsRatio[index] = feature[i].emg.rms[num] / feature[i].emg.rms[den];
                feature[i].emg.bayesRatio[index] = feature[i].emg.bayesRMS[num] / feature[i].emg.bayesRMS[den];
                index++;
            }
            num++;
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
    
    for ( int i = 0; i < myo.getDevices().size(); i++ ) {
        ofDrawBitmapString("Myo: " + ofToString(myo.getDevices()[i]->getId()), pos.x, pos.y + 20);
        
        drawGraph(ofVec2f(pos.x, pos.y + 50),  "1) RMS", feature[i].emg.rms, 0, 127);
        drawGraph(ofVec2f(pos.x, pos.y + 150), "2) BayesFitler", feature[i].emg.bayesRMS, 0, 1);
        drawGraph(ofVec2f(pos.x + 200, pos.y + 50), "3) RMSRatio", feature[i].emg.rmsRatio, 0, 10);
        drawGraph(ofVec2f(pos.x + 350, pos.y + 50), "4) BayesRatio", feature[i].emg.bayesRatio, 0, 10);
        drawGraph(ofVec2f(pos.x + 200, pos.y + 250), "5) RMSStDev", feature[i].emg.stdDev, 0, 100);
        drawGraph(ofVec2f(pos.x, pos.y + 250), "6) Acc", feature[i].acc.raw, -1, 1);
        drawGraph(ofVec2f(pos.x + 350, pos.y + 250), "6) AccFODRMS", feature[i].acc.fodRMS, -1, 1);
        drawGraph(ofVec2f(pos.x + 650, pos.y + 250), "6) ZeroXs", feature[i].acc.zeroXs, 0, 10);
        
        drawModel(ofVec2f(pos.x, pos.y + 250), i);
    }
}

//-------------------------------

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
            ofTranslate( pos.x + model.getPosition().x, pos.y + model.getPosition().y);
            ofRotate(ofRadToDeg(feature[id].acc.quat.x()), 0, 0, 1);
            ofRotate(ofRadToDeg(feature[id].acc.quat.y()), 1, 0, 0);
            ofRotate(ofRadToDeg(feature[id].acc.quat.z()), 0, 1, 0);
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
    }
}

//-------------------------------

void MyoManager::drawGraph(ofVec2f pos, string title, vector<int> values, int min, int max){
    ofDrawBitmapString(title, pos.x, pos.y);
    
    ofSetColor(255);
    for (int i = 0; i < values.size(); i++){
        double valueMapped = ofMap(values[i], min, max, 0, 100);
        ofDrawRectangle(pos.x, 20 + pos.y + (5 * i), valueMapped, 5);
    }
}

//-------------------------------

void MyoManager::drawGraph(ofVec2f pos, string title, ofVec3f values, int min, int max){
    ofDrawBitmapString(title, pos.x, pos.y);
    
    ofSetColor(255);
    
    for (int i = 0; i < 3; i++) {
        double valueMapped = ofMap(values[i], min, max, -50, 50);
        ofDrawRectangle(pos.x + 50, 20 + pos.y + (5 * i), valueMapped, 5);
    }
}

//-------------------------------

void MyoManager::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
}

//-------------------------------
void MyoManager::onSliderEvent(ofxDatGuiSliderEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Lower Threshold") {
        lowerThresh = e.target->getValue();
    } else if (guiLabel == "Higher Threshold") {
        higherThresh = e.target->getValue();
    } else if (guiLabel == "Bayes JumpRate") {
        float value = e.target->getValue();
        
        for (int i = 0; i < 8; i++){
            emgRawStreams[i]->bayesSetJumpRate(value);
        }
    } else if (guiLabel == "Bayes Diffusion") {
        float value = e.target->getValue();
        
        for (int i = 0; i < 8; i++){
            emgRawStreams[i]->bayesSetDiffusion(value);
        }
        
    } else if (guiLabel == "Bayes MVC") {
        float value = e.target->getValue();
       
        for (int i = 0; i < 8; i++){
            emgRawStreams[i]->bayesSetMVC(value);
        }
    } else if (guiLabel == "Bayes Signal Divider") {
        bayesSigDiv = (int)e.target->getValue();
    }
    
    
//    if (guiLabel == "WindowSize") {
//        for (int i = 0; i < emgStreams.size(); i++){
//            emgStreams[i]->setWindowSize((int)windowSize);
//        }
//        for (int i = 0; i < emgStreams.size(); i++){
//            accStreams[i]->setWindowSize((int)windowSize);
//        }
//        for (int i = 0; i < emgStreams.size(); i++){
//            accFODStreams[i]->setWindowSize((int)windowSize);
//        }
//    }
}

        
