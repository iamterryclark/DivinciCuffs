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
    void drawGui();

    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    
    ofxDatGui * mlGui;
    
    //KNN Recognition
    classification classifierEMG, classifierQuat;
    vector<trainingExample> trainingEMG;
//    vector<trainingExample> trainingQuat;
    
    int classLabelEMG = 0;
//    int classLabelQuat = 0;
    int gestureNum;
    
    bool bTrainGestures, bCaptureGesture;
    bool bRunGesture;
    
    //XMM Hidden Markiv Model
    //xmmTemporalClassification hMM;

    //DTW Recognition
//    seriesClassification classifierDTW;
//    std::vector<trainingSeries> trainingSet;
//    trainingSeries tempSeries;
//    bool trained;
//    string classLabel;
//
    //Regression
//    regression myNN;
//    std::vector<trainingExample> trainingSet;
    
    ofColor color;
};
