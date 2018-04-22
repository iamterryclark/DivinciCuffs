//
//  MachineLearning.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#include "MachineLearning.hpp"

MachineLearning::MachineLearning()
{
    //Gui Setup
    mlGui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    mlGui->setAssetPath(""); //this has to be done due to bug with gui
    
    mlGui->setPosition(ofGetWidth() - mlGui->getWidth(),0);
    mlGui->addHeader("Machine Learning");
    mlGui->addLabel(" :: Gesture Recognition Records :: ");
    mlGui->addToggle("Gesture 0");
    mlGui->addToggle("Gesture 1");
    mlGui->addToggle("Gesture 2");
    mlGui->addToggle("Gesture 3");
    mlGui->addButton("Gesture Train & Run");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Clear Out :: ");
    mlGui->addButton("Clear All");
    mlGui->addBreak()->setHeight(10.0f);
    
    mlGui->onButtonEvent(this, &MachineLearning::onButtonEvent);
    mlGui->onToggleEvent(this, &MachineLearning::onToggleEvent);
}



MachineLearning::~MachineLearning() {
    
}

void MachineLearning::update(MyoManager &myo) {
    if(ofGetFrameNum() % 3 == 0){
        for (int i = 0; i < myo.feature.size(); i++){
            vector<double> featureVector;
            
            for (int j = 0; j < 28; j++){
                featureVector.push_back(myo.feature[i].emgRMSRatio[j]);
            }
            
            for (int j = 0; j < 3; j++){
                featureVector.push_back(myo.feature[i].acc[j]);
            }
        
            //
            // :: Testing / Recording Phase ::
            //

            //If we are recording a gesture
            if (bCaptureGesture){
                
                trainingExample tempEMGExample;
                tempEMGExample.input = { featureVector };
                tempEMGExample.output = { (double)gestureNum };
                trainingEMG.push_back(tempEMGExample);

     

//                tempSeries.input.push_back();
//                trainingSet.push_back(tempSeries);
//
//                tempSeries.input.push_back({ featureVector });
//                tempSeries.label = ofToString(gestureNum);
//
                bRunGesture = false;
            }
        
            //
            // :: Training Phase ::
            //
        
            if (bTrainGestures) {
                if(trainingEMG.size() > 0) {
                    classifierEMG.train(trainingEMG);
                    cout << "Gesture Number " + ofToString(gestureNum) + " Trained" << endl;
                    bTrainGestures = false;
                    bRunGesture = true;
                }

                //
//                if(trainingQuat.size() > 0) {
//                    classifierQuat.train(trainingQuat);
//                    cout << "Gesture Number " + ofToString(gestureNum) + " Trained" << endl;
//                    bTrainGestures = false;
//                    bRunGesture = true;
//                }
//
//                    trainingSet.push_back(tempSeries);
//                    tempSeries.input.clear();
//                    classifierDTW.train(trainingSet);
//                    bTrainGestures = false;
                
            
//             else {
//                    cout << "Gestures Not Trained" << endl;
//                }
            }
        
            //
            // :: Run Phase ::
            //
            if (bRunGesture) {
                classLabelEMG = classifierEMG.run({featureVector})[0];
//                classLabelQuat = classifierQuat.run({ myo.feature[i].acc.x, myo.feature[i].acc.y,  myo.feature[i].acc.z })[0];
//                classLabel = classifierDTW.runContinuous(featureVector);
//
//                if (classLabelEMG == 0){
//                    color = ofColor(255,0,0);
//                    cout << "Class: " << classLabelEMG << endl;
//                } else if (classLabelEMG == 1) {
//                    color = ofColor(255,255,0);
//                    cout << "Class: " << classLabelEMG << endl;
//                } else if (classLabelEMG == 2) {
//                    color = ofColor(255,0,255);
//                    cout << "Class: " << classLabelEMG << endl;
//                } else if (classLabelEMG == 3){
//                    color = ofColor(0,255,0);
//                    cout << "Class: " << classLabelEMG << endl;
//                }
//
//                if (classLabelEMG == classLabelQuat){
            
                switch(classLabelEMG){
                    case 0:
                        color = ofColor(255,0,0);
                        cout << "Class: " << classLabelEMG << endl;
                        break;
                    case 1:
                        color = ofColor(255,255,0);
                        cout << "Class: " << classLabelEMG << endl;
                        break;
                    case 2:
                        color = ofColor(255,0,255);
                        cout << "Class: " << classLabelEMG << endl;
                        break;
                    case 3:
                        color = ofColor(0,255,0);
                        cout << "Class: " << classLabelEMG << endl;
                        break;
                 }
            }
        }
    }
}

void MachineLearning::draw() {
    ofSetColor(color);
    ofDrawBitmapString("Training EMG: " + ofToString(trainingEMG.size()), 100, 700);
//    ofDrawBitmapString("Training Quat: " + ofToString(trainingQuat.size()), 100, 720);
//    ofDrawBitmapString("Training Series: " + ofToString(trainingSet.size()), 100, 740);
    ofDrawRectangle(ofGetWidth()/2, ofGetHeight()/2, 100,100);
    
}

void MachineLearning::drawGui() {
    
}

void MachineLearning::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
//    mlStatus = guiLabel + " - Activated";
    if (guiLabel == "Gesture 0"){
        gestureNum = 0;
        bCaptureGesture = e.target->getChecked();
    } else if( guiLabel == "Gesture 1"){
        gestureNum = 1;
        bCaptureGesture = e.target->getChecked();
    } else if( guiLabel == "Gesture 2"){
        gestureNum = 2;
        bCaptureGesture = e.target->getChecked();
    } else if( guiLabel == "Gesture 3"){
        gestureNum = 3;
        bCaptureGesture = e.target->getChecked();
    } else if (guiLabel == "Gesture Run") {
        bRunGesture = e.target->getChecked();
    }
}

void MachineLearning::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    //mlStatus = guiLabel + " - Activated";
    
    if (guiLabel == "Clear All" ){
        bRunGesture = false;
        bCaptureGesture = false;
        classifierEMG.reset();
//        classifierQuat.reset();
//        classifierDTW.reset();
        trainingEMG.clear();
//        trainingQuat.clear();
//        trainingSet.clear();
    } else if (guiLabel == "Gesture Train & Run"){
        bTrainGestures = true;
    }
}
