//
//  feature.h
//  BioPerformanceTool
//
//  Created by Terry Clark on 24/04/2018.
//

#pragma once

struct EMG {
    EMG(){
        //Raw Inits
        raw = vector<double>(8, 0.0);
        rawNormalised = vector<double>(8, 0.0);
        rms = vector<double>(8, 0.0);
        rmsRatio = vector<double>(28, 0.0);
        bayesRMS = vector<double>(8, 0.0);
        bayesRatio = vector<double>(28, 0.0);
        stdDev = vector<double>(8, 0.0);
        zeroXs = vector<int>(8, 0);
        mean = vector<double>(8, 0.0);
        min = vector<double>(8, 0.0);
        max = vector<double>(8, 0.0);
    }
    
    vector<double> raw;
    vector<double> rawNormalised;
    vector<double> rms;
    vector<double> bayesRMS;
    vector<double> rmsRatio;
    vector<double> bayesRatio;
    vector<double> stdDev;
    vector<double> min;
    vector<double> max;
    vector<double> mean;
    vector<int> zeroXs;
};

struct Accel {
    
    Accel(){
        //Raw Inits
        pitch = 0.0;
        yaw = 0.0;
        roll = 0.0;
        raw = ofVec3f(0,0,0);
        rawNormalised = ofVec3f(0,0,0);
        gyro = ofVec3f(0,0,0);
        quat = ofQuaternion(0,0,0,0);
        
        //Fancy Inits
        fod = ofVec3f(0,0,0);
        fodRMS = ofVec3f(0,0,0);
        zeroXs = vector<int>(3, 0);
        
        //Min/Max Inits
        min = vector<double>(8, 0.0);
        max = vector<double>(8, 0.0);
    }
    
    double pitch;
    double yaw;
    double roll;
    ofVec3f raw;
    ofVec3f rawNormalised;
    ofVec3f gyro;
    ofQuaternion quat;
    vector<double> min;
    vector<double> max;
    ofVec3f fod;
    ofVec3f fodRMS;
    vector<int> zeroXs;
};

class Feature {
public:
    
    Feature(int _id){
        identity = _id;
    }
    
    EMG emg;
    Accel acc;
    
    int identity;
};
