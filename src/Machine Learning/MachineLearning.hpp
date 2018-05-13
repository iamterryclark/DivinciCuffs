//
//  MachineLearning.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//dd

#pragma once

#include "ofMain.h"
#include <algorithm>

//OF Addons
#include "ofxXmlSettings.h"

//My Classes
#include "MyoManager.hpp"

//Just change this number to total scenes in Ableton, everything will format
#define TOTALSCENES 11

class MachineLearning {
private:
    
public:
    MachineLearning(MyoManager *_myo);
    ~MachineLearning();
    
    void update();
    void draw();
    void addDataToXML(vector<trainingExample> example, string addToName = "");
    void saveDataToXML();
    void loadDataFromXML();
    void dataParseXML(string searchTerm,vector<trainingExample> &trainingVec);
    bool segmentLimit(vector<double> inputCheck);
    bool playSceneTrigger();

    //Gui Events
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onMatrixEvent(ofxDatGuiMatrixEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    
    //Myo Instance
    MyoManager * myo;
    
    //Gui Params
    ofxDatGui * mlGui;
    string fileName;
    ofxXmlSettings mlSettings;
    int lastTagNumber;
    
    //KNN Recognition
    classification knnClassifierEMG, knnClassifierAcc;
    vector<trainingExample> knnTrainingEMG, knnTrainingAcc;
    bool bCaptureKNN, bTrainKNN, bRunKNN;
    int knnClassLabelEMG = 0;
    int knnClassLabelAcc = 0;
    int knnGestureNum = 0;
    float segmentThreshold = 0.0;

//    XMM Recognition
rapidmix::xmmTemporalClassification xmmClassifierEMG;
    rapidmix::trainingData xmmTempData;
    bool bCaptureXMM, bTrainXMM, bRunXMM;
    bool bStartXMMRecording = false;
    int xmmGestureNum = 0;
    vector<vector<double>> xmmPhrase;

    //Regression
    vector<regression> nnAllReg;
    vector <vector<trainingExample>> nnAllTrainingSets;
    vector<trainingExample> nnAllTempExamples;
    bool bCaptureReg, bTrainReg, bRunReg;
    vector<double> regressVals;
    vector<double> outputReg = {0,0,0,0,0,0,0,0,0};
    
    //Output Visualisor for classification type labels
    ofColor colorKNN, colorXMM;
    vector<ofColor> colors = {
        ofColor::silver,
        ofColor::indigo,
        ofColor::blueSteel,
        ofColor::darkorange,
        ofColor::indianRed,
        ofColor::hotPink,
        ofColor::yellow,
    };
    
    //Scene Information
    int sceneNum = 0;
    bool sceneActivate = 0;
    
    //For playing the scene
    bool oneShotTrigger = false;

};
