//
//  MachineLearning.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxRapidLib.h"
#include "MyoManager.hpp"

class MachineLearning {
private:
    
public:
    MachineLearning();
    ~MachineLearning();
    
    void update(MyoManager &myo);
    void draw();

    //Gui Events
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onMatrixEvent(ofxDatGuiMatrixEvent e);
    
    ofxDatGui * mlGui;
    
    //KNN Recognition
    classification knnClassifierEMG, knnClassifierAcc;
    vector<trainingExample> knnTrainingEMG, knnTrainingAcc;
    bool bCaptureKNN, bTrainKNN, bRunKNN;
    
    int knnClassLabelEMG = 0;
    int knnClassLabelAcc = 0;
    int knnGestureNum = 0;
    
    //DTW Recognition
    seriesClassification dtwClassifierEMG, dtwClassifierAcc;
    vector<trainingSeries> trainingSetEMG, trainingSetAcc;
    trainingSeries tempSeriesEMG, tempSeriesAcc;
    bool bCaptureDTW, bTrainDTW, bRunDTW;
    int dtwGestureNum = 0;
    string dtwClassLabelEMG = "";
    string dtwClassLabelAcc = "";
    //Regression
    vector<trainingExample> nnTrainingSet;
    trainingExample nnTempExample;
    regression nnReg;
    bool bCaptureReg, bTrainReg, bRunReg;
    
    ofColor colorKNN, colorDTW;
    
    vector<ofColor> colors = {
        ofColor::silver,
        ofColor::indigo,
        ofColor::blueSteel,
        ofColor::darkorange,
        ofColor::indianRed,
        ofColor::hotPink,
        ofColor::yellow,
    };
    
    vector<double> outputVals = {0,0,0,0,0,0,0,0,0};
    
    int sceneNum = 0;
    bool sceneActivate = false;
};
