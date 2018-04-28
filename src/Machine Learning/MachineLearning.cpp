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
    mlGui->addMatrix("Scene", 4, true);
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Posture Recognition (KNN) :: ");
    mlGui->addMatrix("KNN Record", 7, true);
    mlGui->addButton("KNN Train & Run");
    mlGui->addButton("Clear KNN");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Gesture Recognition (DTW) :: ");
    mlGui->addMatrix("DTW Record", 7, true);
    mlGui->addButton("DTW Train & Run");
    mlGui->addButton("Clear DTW");
//    mlGui->addBreak()->setHeight(10.0f);
//    mlGui->addLabel(" :: Gesture Recognition (HMM) :: ");
//    mlGui->addMatrix("HMM Record", 7, true);
//    mlGui->addButton("HMM Train & Run");
//    mlGui->addButton("Clear HMM");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Regression (Neural Network) :: ");
    mlGui->addToggle("NN Record");
    mlGui->addButton("NN Train & Run");
    mlGui->addSlider("Ableton Param 1", 0, 1);
    mlGui->addSlider("Ableton Param 2", 0, 1);
    mlGui->addSlider("Ableton Param 3", 0, 1);
    mlGui->addSlider("Ableton Param 4", 0, 1);
    mlGui->addSlider("Ableton Param 5", 0, 1);
    mlGui->addSlider("Ableton Param 6", 0, 1);
    mlGui->addButton("Clear NN");
    
    mlGui->onButtonEvent(this, &MachineLearning::onButtonEvent);
    mlGui->onToggleEvent(this, &MachineLearning::onToggleEvent);
    mlGui->onSliderEvent(this, &MachineLearning::onSliderEvent);
    mlGui->onMatrixEvent(this, &MachineLearning::onMatrixEvent);
}

MachineLearning::~MachineLearning() {
    delete mlGui;
}

void MachineLearning::update(MyoManager &myo) {
    for (int i = 0; i < myo.feature.size(); i++){
        Feature feature = myo.feature[i];
        
        if(ofGetFrameNum() % 5 == 0){
            
            //Collect data from the myo ready for input into the mahcine learning algorithms
            vector<double> emgFeature, accFeature;
            vector<double> emgBayesFeature;
            
            emgFeature = feature.emg.rawNormalised;
            
            emgBayesFeature = feature.emg.bayesRMS;
            
            accFeature = {
                feature.acc.quat[0],
                feature.acc.quat[1],
                feature.acc.quat[2],
            };
            
            //
            // :: Testing / Recording Phase ::
            //

            //If we are recording a gesture
            if (bCaptureKNN){
               
                //Record EMG
                trainingExample tempEMGExample;
                tempEMGExample.input = { emgFeature };
                tempEMGExample.output = { (double)knnGestureNum };
                knnTrainingEMG.push_back(tempEMGExample);
                
                //Record Acc
                trainingExample tempAccExample;
                tempAccExample.input = { accFeature };
                tempAccExample.output = { (double)knnGestureNum };
                knnTrainingAcc.push_back(tempAccExample);
                
                //Do not run Classifer whilst recording
                bRunKNN = false;
            }
            
            if (bCaptureDTW){
                //Record EMG
                tempSeriesEMG.input.push_back( { emgFeature } );
                tempSeriesEMG.label = ofToString(dtwGestureNum);
                
                //Record Acc
                tempSeriesAcc.input.push_back( { accFeature } );
                tempSeriesAcc.label = ofToString(dtwGestureNum);
                
                bRunDTW = false;
            }
            
            if (bCaptureReg){
//                cout << "Captureing Reg" << endl;
                nnTempExample.input = { emgBayesFeature };
                nnTempExample.output = {
                    mlGui->getSlider("Ableton Param 1")->getValue(),
                    mlGui->getSlider("Ableton Param 2")->getValue(),
                    mlGui->getSlider("Ableton Param 3")->getValue(),
                    mlGui->getSlider("Ableton Param 4")->getValue(),
                    mlGui->getSlider("Ableton Param 5")->getValue(),
                    mlGui->getSlider("Ableton Param 6")->getValue()
                };
                
                nnTrainingSet.push_back(nnTempExample);
                bRunReg = false;
            }
        
            //
            // :: Training Phase ::
            //
        
            if (bTrainKNN) {
                if(knnTrainingEMG.size() > 0 && knnTrainingAcc.size() > 0) {
                    knnClassifierEMG.train(knnTrainingEMG);
                    knnClassifierAcc.train(knnTrainingAcc);
                    bTrainKNN = false;
                    bRunKNN = true;
                }
            }
            
            if (bTrainDTW) {
                if(tempSeriesAcc.input.size() > 0) {
                    trainingSetAcc.push_back(tempSeriesAcc);
                    dtwClassifierAcc.train(trainingSetAcc);
                    tempSeriesAcc.input.clear();
                    bTrainDTW = false;
                    bRunDTW = true;
                }
            }
            
            if (bTrainReg) {
                //Training Phase for NN Regression
                if(nnTrainingSet.size() > 0){
                    nnReg.train(nnTrainingSet);
                    cout << "Regression Trained " << endl;
                    bTrainReg = false;
                    bRunReg = true;
                } else {
                    cout << "Regression Not Trained" << endl;
                }
            }
                
        
            //
            // :: Run Phase ::
            //
            if (bRunKNN) {
                if (myo.accNormStreams[0]->velocity() > 0.1){
                
                    knnClassLabelEMG = knnClassifierEMG.run({emgFeature})[0];
                    knnClassLabelAcc = knnClassifierAcc.run({accFeature})[0];

                    if ( knnClassLabelEMG == knnClassLabelAcc){
                        colorKNN = colors[knnClassLabelEMG];
                    }
                }
            }
            
            if (bRunDTW) {
                dtwClassLabelAcc = dtwClassifierAcc.run({accFeature})[0];
                
                colorDTW = colors[ofToInt(dtwClassLabelAcc)];

            }
            
            if (bRunReg) {
                vector<double> regresVals = nnReg.run({ emgBayesFeature });
               
                //For Visual Purposes
                mlGui->getSlider("Ableton Param 1")->setValue(regresVals[0]);
                mlGui->getSlider("Ableton Param 2")->setValue(regresVals[1]);
                mlGui->getSlider("Ableton Param 3")->setValue(regresVals[2]);
                mlGui->getSlider("Ableton Param 4")->setValue(regresVals[3]);
                mlGui->getSlider("Ableton Param 5")->setValue(regresVals[4]);
                mlGui->getSlider("Ableton Param 6")->setValue(regresVals[5]);
                
                //Actually Changing the Values
                for (int i = 0; i < regresVals.size(); i++){
                    outputVals[i] = regresVals[i];
                }
            }
        }
    }
}

void MachineLearning::draw() {
    
    //KNN Indicator
    ofSetColor(255);
    ofDrawBitmapString("KNN Examples EMG: " + ofToString(knnTrainingEMG.size()), 50, 350);
    ofDrawBitmapString("KNN Examples Acc: " + ofToString(knnTrainingAcc.size()), 50, 370);

    ofPushStyle();
    ofSetColor(colorKNN);
    ofDrawRectangle(250, 320, 80,80);
    ofSetColor(0);
    ofDrawBitmapString("KNN", 255, 345);
    ofDrawBitmapString("Class: " + ofToString(knnClassLabelEMG), 255, 365);
    ofPopStyle();
    
    //DTW Indicator
    ofSetColor(255);
    ofDrawBitmapString("DTW Examples EMG: " + ofToString(tempSeriesEMG.input.size()), 50, 460);
    ofDrawBitmapString("DTW Examples Acc: " + ofToString(tempSeriesAcc.input.size()), 50, 490);
    ofDrawBitmapString("DTW Set Num EMG: " + ofToString(trainingSetEMG.size()), 50, 510);
    ofDrawBitmapString("DTW Set Num Acc: " + ofToString(trainingSetAcc.size()), 50, 540);
    ofDrawBitmapString("NN Set Num: " + ofToString(nnTrainingSet.size()), 50, 570);
    
    ofPushStyle();
    ofSetColor(colorDTW);
    ofDrawRectangle(250, 450, 80,80);
    ofSetColor(0);
    ofDrawBitmapString("DTW", 250 + 5, 460 + 25);
    ofDrawBitmapString("Class: " + ofToString(dtwClassLabelEMG), 255, 505);
    ofPopStyle();
}

void MachineLearning::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Clear KNN"){
        bRunKNN = false;
        bCaptureKNN = false;
        knnClassifierEMG.reset();
        knnClassifierAcc.reset();
        knnTrainingEMG.clear();
        knnTrainingAcc.clear();
    } else if (guiLabel == "Clear DTW"){
        bRunDTW = false;
        bCaptureDTW = false;
        dtwClassifierEMG.reset();
        dtwClassifierAcc.reset();
        trainingSetEMG.clear();
        trainingSetAcc.clear();
    } else if (guiLabel == "Clear NN"){
        bRunReg = false;
        bCaptureReg = false;
        nnReg.reset();
        nnTrainingSet.clear();
    } else if (guiLabel == "KNN Train & Run") {
        bTrainKNN = true;
    } else if (guiLabel == "NN Train & Run") {
        bTrainReg = true;
    } else if (guiLabel == "DTW Train & Run") {
        bTrainDTW = true;
    }
}

void MachineLearning::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "NN Record"){
        bCaptureReg = e.target->getChecked();
    }
}

void MachineLearning::onSliderEvent(ofxDatGuiSliderEvent e){
    string guiLabel = e.target->getLabel();
}

void MachineLearning::onMatrixEvent(ofxDatGuiMatrixEvent e){
    string guiLabel = e.target->getLabel();
    int whichButton = e.child;
    
    if (guiLabel == "KNN Record"){
        knnGestureNum = whichButton;
        bCaptureKNN = e.enabled;
    } else if (guiLabel == "DTW Record"){
        dtwGestureNum = whichButton;
        bCaptureDTW = e.enabled;
    } else if (guiLabel == "Scene"){
        sceneNum = whichButton;
        sceneActivate = e.enabled;
    }
}
