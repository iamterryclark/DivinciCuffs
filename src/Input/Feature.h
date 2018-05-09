//
//  feature.h
//  BioPerformanceTool
//
//  Created by Terry Clark on 24/04/2018.
//

#pragma once

struct EMG {
    vector<double> raw = vector<double>(8, 0.0);
    vector<double> rms = vector<double>(8, 0.0);
    vector<double> bayes = vector<double>(8, 0.0);
    vector<double> stdDev = vector<double>(8, 0.0);
    vector<double> min = vector<double>(8, 0.0);
    vector<double> max = vector<double>(8, 0.0);
    vector<double> mean = vector<double>(8, 0.0);
    
    vector<double> bayesRatio = vector<double>(28, 0.0);
    vector<double> rmsRatio = vector<double>(28, 0.0);
    vector<double> stdRatio = vector<double>(28, 0.0);
};

struct Accel {
    double pitch = 0.0;
    double yaw = 0.0;
    double roll = 0.0;
    
    ofVec3f raw = ofVec3f(0,0,0);
    ofVec3f gyro = ofVec3f(0,0,0);
    ofVec3f fod = ofVec3f(0,0,0);
    ofVec3f fodRMS = ofVec3f(0,0,0);
    
    ofQuaternion quat = ofQuaternion(0,0,0,0);
    
    vector<double> min = vector<double>(8, 0.0);
    vector<double> max = vector<double>(8, 0.0);
  
};

//Class Init here to capture all myo data into one data structure

class Feature {
public:
    
    Feature(int _id){
        identity = _id;
    }
    
    EMG emg;
    Accel acc;
    
    int identity;
};
