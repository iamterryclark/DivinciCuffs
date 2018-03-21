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
    mlGui->addToggle("Play Track");
    mlGui->addToggle("Skip Forward");
    mlGui->addToggle("Skip Back");
    mlGui->addToggle("Modify Parameters");
    mlGui->addButton("Gesture Train");
    mlGui->addToggle("Gesture Run");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Regression :: ");
    mlGui->addToggle("Regression Record");
    mlGui->addButton("Regression Train");
    mlGui->addToggle("Regression Run");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Clear Out :: ");
    mlGui->addButton("Clear All");
    mlGui->addBreak()->setHeight(10.0f);
    
    mlGui->onButtonEvent(this, &MachineLearning::onButtonEvent);
    mlGui->onToggleEvent(this, &MachineLearning::onToggleEvent);}

MachineLearning::~MachineLearning() {
}

void MachineLearning::update(MyoManager &myo) {
    
}

void MachineLearning::draw() {
    
}

void MachineLearning::drawGui() {
    
}

void MachineLearning::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    //mlStatus = guiLabel + " - Activated";
    if (guiLabel == "Record Voice"){
        //gestureNum = 0;
        //bCaptureGesture = e.target->getChecked();
    } else if( guiLabel == "Set Playhead"){
        //gestureNum = 1;
        //bCaptureGesture = e.target->getChecked();
    } else if( guiLabel == "Manipulate Synth"){
        //gestureNum = 2;
        //bCaptureGesture = e.target->getChecked();
    } else if( guiLabel == "Exit Mode"){
        //gestureNum = 3;
        //bCaptureGesture = e.target->getChecked();
    } else if (guiLabel == "Gesture Run") {
        //bRunToggle = e.target->getChecked();
    } else if (guiLabel == "Regression Record"){
        //gestureNum = 4;
       // bRegression = e.target->getChecked();
    } else if (guiLabel == "Regression Run"){
        //bModelControl = e.target->getChecked();
    }
}

void MachineLearning::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    //mlStatus = guiLabel + " - Activated";
    
    if (guiLabel == "Clear All" ){
//        classifier.reset();
//        trainingSetReg.clear();
//        tempSeries.input.clear();
//        trainingSet.clear();
    } else if (guiLabel == "Regression Train"){
//        bTrainRegression = true;
    } else if (guiLabel == "Gesture Train"){
//        bTrainGestures = true;
    }
}
