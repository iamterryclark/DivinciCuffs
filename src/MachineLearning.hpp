//
//  MachineLearning.hpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#pragma once

#include "ofMain.h"
#include "ofxRapidLib.h"
//#include "xmm.h"
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
//    xmm::TrainingSet ts();
//    ts.dimension.set(3);
//    std::vector<float> observation(3);
//    std::string label_a(static_cast<std::string>("a"));
//    std::string label_b(static_cast<std::string>("b"));
//    ts.addPhrase(0, label_a);
//    ts.addPhrase(1, label_b);
//    for (unsigned int i = 0; i < 100; i++) {
//        observation[0] = float(i) / 100.;
//        observation[1] = pow(float(i) / 100., 2.);
//        observation[2] = pow(float(i) / 100., 3.);
//        ts.getPhrase(0)->record(observation);
//        observation[0] -= 1.;
//        observation[1] -= 1.;
//        observation[2] -= 1.;
//        ts.getPhrase(1)->record(observation);
//    }
    
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
