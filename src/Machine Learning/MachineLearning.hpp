//
//  MachineLearning.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#pragma once

#include "ofMain.h"
#include <algorithm>

//OF Addons
#include "ofxXmlSettings.h"

//My Classes
#include "MyoManager.hpp"
#define TOTALSCENES 9

class MachineLearning {
private:
    
public:
    MachineLearning();
    ~MachineLearning();
    
    void update(MyoManager &myo);
    void draw();
    void addDataToXML(vector<trainingExample> example, string addToName = "");
    void saveDataToXML();
    void loadDataFromXML();
    void dataParseXML(string searchTerm,vector<trainingExample> &trainingVec);

    //Gui Events
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onMatrixEvent(ofxDatGuiMatrixEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    
    //Gui Params
    ofxDatGui * mlGui;
    string fileName;
    ofxXmlSettings mlSettings;
    int lastTagNumber;
    
    //KNN Recognition
    classification knnClassifierEMG;
    vector<trainingExample> knnTrainingEMG;
    bool bCaptureKNN, bTrainKNN, bRunKNN;
    int knnClassLabelEMG = 0;
    int knnClassLabelAcc = 0;
    int knnGestureNum = 0;
    
    //XMM Recognition
    rapidmix::xmmTemporalClassification xmmClassifierEMG;
    rapidmix::trainingData xmmTempData;
    vector<double> xmmPhrases;
    bool bCaptureXMM, bTrainXMM, bRunXMM;
    int xmmGestureNum;
    
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
    vector<bool> sceneActivate = vector<bool>(4, false);
};
