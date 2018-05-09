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
    mlGui->addTextInput("Filename");
    mlGui->addButton("Save All");
    mlGui->addButton("Load Preset");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Posture Recognition (KNN) :: ");
    mlGui->addMatrix("KNN Record", 7, true);
    mlGui->addButton("KNN Train & Run");
    mlGui->addButton("Clear KNN");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Gesture Recognition (XMM) :: ");
    mlGui->addMatrix("XMM Record", 7, true);
    mlGui->addButton("XMM Train & Run");
    mlGui->addButton("Clear XMM");
    mlGui->addBreak()->setHeight(10.0f);
    mlGui->addLabel(" :: Regression (Neural Network) :: ");
    mlGui->addToggle("NN Record");
    mlGui->addButton("NN Train & Run");
    mlGui->addButton("Stop NN");
    mlGui->addMatrix("Scene", TOTALSCENES, true);
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
    mlGui->onTextInputEvent(this, &MachineLearning::onTextInputEvent);
    
    rapidmix::xmmConfig xcfg;
    xcfg.relativeRegularization = 0.1;
    xcfg.states = 7;
    xcfg.likelihoodWindow = 10;

    xmmClassifierEMG = rapidmix::xmmTemporalClassification(xcfg);
    
    for (int i = 0; i < TOTALSCENES; i++){
        regression nn;
        nn.setNumHiddenLayers(2);
        nn.setNumHiddenNodes(3);
        nn.setNumEpochs(100);
        nnAllReg.push_back(nn);
        
        vector<trainingExample> teVec;
        nnAllTrainingSets.push_back(teVec);
        
        trainingExample te;
        nnAllTempExamples.push_back(te);
    }
}

MachineLearning::~MachineLearning() {
    delete mlGui;
}

void MachineLearning::addDataToXML(vector<trainingExample> examples, string title){

    lastTagNumber = mlSettings.addTag(title);

    if( mlSettings.pushTag(title, lastTagNumber) ){

        for (int i = 0; i < examples.size(); i++) {
            int tagNum = mlSettings.addTag("example");
            
            mlSettings.pushTag("example", i);
            
            for (int j = 0; j < examples[i].input.size(); j++) {
                mlSettings.addValue("input", examples[i].input[j]);
            }
            
            //Seperate Process for Regression outputs
            if (title == "nnEMG_" + ofToString(sceneNum)){
                for (int k = 0; k < examples[i].output.size(); k++){
                    mlSettings.addValue("output", examples[i].output[k]);
//                    cout << examples[i].output[k] << endl;
                }
            } else {
                mlSettings.addValue("label", (int)examples[i].output[i]);
            }
            
            mlSettings.popTag();
        }
        mlSettings.popTag();
    }
}

void MachineLearning::saveDataToXML(){
    mlSettings.clear();
    
    if (knnTrainingEMG.size() > 0)
        addDataToXML(knnTrainingEMG, "knnEMG");
    
    for (int i = 0; i < TOTALSCENES; i++){
         if (nnAllTrainingSets[i].size() > 0)
             addDataToXML(nnAllTrainingSets[i], "nnEMG_" + ofToString(i));
    }
   
    mlSettings.save(fileName + ".xml");
}

void MachineLearning::loadDataFromXML(){
    knnTrainingEMG.clear();

    for (int i = 0; i < TOTALSCENES; i++){
        nnAllTrainingSets[i].clear();
    }
    
    if( mlSettings.loadFile(fileName + ".xml") ){
        if (mlSettings.getNumTags("knnEMG") > 0) {
            dataParseXML("knnEMG", knnTrainingEMG);
        }
        
        for (int i = 0; i < TOTALSCENES; i++){
            if (mlSettings.getNumTags("nnEMG_" + ofToString(i)) > 0){
                dataParseXML("nnEMG_" + ofToString(i), nnAllTrainingSets[i]);
            }
        }
        
    } else {
        ofLogError("File did not load!");
    }
}

//Reference: http://openframeworks.cc/documentation/ofxXmlSettings/ofxXmlSettings/

void MachineLearning::dataParseXML(string searchTerm, vector<trainingExample> &trainingVec){
    mlSettings.pushTag(searchTerm);
    int numExTags = mlSettings.getNumTags("example");
    for(int i = 0; i < numExTags; i++){
        
        mlSettings.pushTag("example", i);
        
        int inputNum = mlSettings.getNumTags("input");
        
        trainingExample example;
       
        for (int j = 0; j < inputNum; j++ ){
             example.input.push_back(mlSettings.getValue("input", j));
        }
        
        if (searchTerm == "nnEMG_" + ofToString(sceneNum)){
            int numOutputs = mlSettings.getNumTags("output");
            
            for (int k = 0; k < numOutputs; k++){
                example.output.push_back(mlSettings.getValue("output",double(0.0), k));
                cout << mlSettings.getValue("output", double(0.0), k) << endl;
            }
        } else {
            example.output.push_back(mlSettings.getValue("label", 0, 0));
        }
        
        trainingVec.push_back(example);
    
        mlSettings.popTag();
    }
    mlSettings.popTag(); //pop position
}


bool MachineLearning::segmentLimit(vector<double> inputCheck, float threshold) {
    
    for (int i = 0; i < inputCheck.size(); i++){
        if (inputCheck[i] > threshold){
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void MachineLearning::update(MyoManager &myo) {
    for (int i = 0; i < myo.feature.size(); i++){
        if(ofGetFrameNum() % 2 == 0){
          Feature feature = myo.feature[i];
            //Collect data from the myo ready for input into the mahcine learning algorithms
            vector<double> emgRMS, emgSTD, emgRMSRatio, emgSTDRatio, emgBayesRatio, gyroFeature;
            
            emgRMS = feature.emg.rms;
            emgSTD = feature.emg.stdDev;
            emgSTDRatio = feature.emg.stdRatio;
            emgRMSRatio = feature.emg.rmsRatio;
            emgBayesRatio = feature.emg.bayesRatio;
            gyroFeature = {
                feature.acc.gyro[0],
                feature.acc.gyro[1],
                feature.acc.gyro[2],
            };
            
            //
            // :: Testing / Recording Phase ::
            //

            //If we are recording a gesture
            if (bCaptureKNN){
                
                //Record EMG
                trainingExample tempEMGExample;
                tempEMGExample.input = emgBayesRatio;
                tempEMGExample.output = { (double)knnGestureNum };
                knnTrainingEMG.push_back(tempEMGExample);
                
                //Do not run Classifer whilst recording
                bRunKNN = false;
            }
            
            
            if (bCaptureXMM){
                if (bStartXMMRecording){
                    //Following Implemtation from Tests folder on RapidMix API
                    xmmTempData.startRecording(ofToString(xmmGestureNum));
                    cout << "Recording Started : " << xmmGestureNum << endl;
                }

                vector<double> label = {(double)xmmGestureNum};
                
                //x y z gyro
                xmmTempData.addElement(gyroFeature, label);
                cout << xmmTempData.trainingSet.size() << endl;
                bStartXMMRecording = false;
                bRunXMM = false;
                xmmTempData.stopRecording();
            }
            
            if (bCaptureReg){
                bRunReg = false;
                int i = sceneNum;
                
                nnAllTempExamples[i].input = { emgSTDRatio };
                nnAllTempExamples[i].output = {
                    mlGui->getSlider("Ableton Param 1")->getValue(),
                    mlGui->getSlider("Ableton Param 2")->getValue(),
                    mlGui->getSlider("Ableton Param 3")->getValue(),
                    mlGui->getSlider("Ableton Param 4")->getValue(),
                    mlGui->getSlider("Ableton Param 5")->getValue(),
                    mlGui->getSlider("Ableton Param 6")->getValue()
                };
            
                nnAllTrainingSets[i].push_back(nnAllTempExamples[i]);
            }
        
            //
            // :: Training Phase ::
            //
        
            if (bTrainKNN) {
                if(knnTrainingEMG.size() > 0) {
                    knnClassifierEMG.train(knnTrainingEMG);
                    bTrainKNN = false;
                    bRunKNN = true;
                }
            }
            
            if (bTrainXMM) {
                //Training Phase for NN Regression
                xmmClassifierEMG.train(xmmTempData);
                bRunXMM = true;
                bTrainXMM = false;
                cout << "XMM Trained!" << endl;
            }
            
            if (bTrainReg) {
                //Training Phase for NN Regression
                if(nnAllTrainingSets[sceneNum].size() > 0){
                    nnAllReg[i].train(nnAllTrainingSets[sceneNum]);
                    cout << "Regression_" + ofToString(sceneNum) + " Trained" << endl;
                } else {
                    cout << "Regression_" + ofToString(sceneNum) + " Not Trained" << endl;
                }
                bTrainReg = false;
                bRunReg = true;
            }

            //
            // :: Run Phase ::
            //
            if (bRunKNN) {
                //Quick and dirty segmentation will make this user friendly at some point
//                if(segmentLimit(emgBayesRatio, 0.2)){
                    int classLabel = knnClassifierEMG.run({emgBayesRatio})[0];
                    colorKNN = colors[classLabel];
//                }
            }
            
            if (bRunXMM) {
                cout << ofToString(xmmClassifierEMG.run(gyroFeature)) << endl;
            }
            
            if (bRunReg) {
                if (nnAllTrainingSets[sceneNum].size() > 0){
                    regressVals = nnAllReg[sceneNum].run({ emgSTDRatio });
                    //For Visual Purposes
                    mlGui->getSlider("Ableton Param 1")->setValue(regressVals[0]);
                    mlGui->getSlider("Ableton Param 2")->setValue(regressVals[1]);
                    mlGui->getSlider("Ableton Param 3")->setValue(regressVals[2]);
                    mlGui->getSlider("Ableton Param 4")->setValue(regressVals[3]);
                    mlGui->getSlider("Ableton Param 5")->setValue(regressVals[4]);
                    mlGui->getSlider("Ableton Param 6")->setValue(regressVals[5]);
                }
            }
        }
    }
}

void MachineLearning::draw() {
    
    //KNN Indicator
    ofSetColor(255);
    ofDrawBitmapString("KNN Examples (EMG): " + ofToString(knnTrainingEMG.size()), 50, 350);

    ofPushStyle();
    ofSetColor(colorKNN);
    ofDrawRectangle(250, 320, 80,80);
    ofSetColor(0);
    ofDrawBitmapString("KNN", 255, 345);
    ofDrawBitmapString("Class: " + ofToString(knnClassLabelEMG), 255, 365);
    ofPopStyle();
    
    //XMM Indicator
    ofSetColor(255);
    ofDrawBitmapString("XMM Training Data: " + ofToString(xmmTempData.trainingSet.size()), 50, 460);
    
    //NN Indicator
    for (int i = 0; i < TOTALSCENES; i++){
        if (i == sceneNum){
            ofDrawBitmapString("NN_" + ofToString(i) + " Set Num: " + ofToString(nnAllTrainingSets[i].size()), 50, 570);
        }
    }
}

void MachineLearning::onButtonEvent(ofxDatGuiButtonEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Clear KNN"){
        bRunKNN = false;
        bCaptureKNN = false;
        knnTrainingEMG.clear();
        knnClassifierEMG.reset();
    }
    
    if (guiLabel == "Clear XMM"){
        bRunXMM = false;
        bCaptureXMM = false;
        xmmClassifierEMG.reset();
        xmmTempData.trainingSet.clear();
    }
    
    if (guiLabel == "Clear NN"){
        bRunReg = false;
        bCaptureReg = false;
        
        for (int i = 0; i < TOTALSCENES; i++){
            nnAllTrainingSets[i].clear();
            nnAllReg[i].reset();
        }
    }
    
    if (guiLabel == "KNN Train & Run") {
        bTrainKNN = true;
    } else if (guiLabel == "NN Train & Run") {
        bTrainReg = true;
    } else if (guiLabel == "Stop NN") {
        bRunReg = false;
    } else if (guiLabel == "XMM Train & Run") {
        bTrainXMM = true;
    } else if (guiLabel == "Save All"){
        saveDataToXML();
    } else if (guiLabel == "Load Preset"){
        loadDataFromXML();
    }
    
}

void MachineLearning::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "NN Record") {
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
        bRunKNN = false;
    } else if (guiLabel == "XMM Record"){
        xmmGestureNum = whichButton;
        bCaptureXMM = e.enabled;
        bStartXMMRecording = true;
    } else if (guiLabel == "Scene"){
        bRunReg = false;
        sceneNum = whichButton;

        //For GUI Feedback
        for(int i =0; i < TOTALSCENES; i++){
            if ( i == sceneNum){
                mlGui->getMatrix("Scene")->getChildAt(i)->setSelected(true);
            } else {
                mlGui->getMatrix("Scene")->getChildAt(i)->setSelected(false);
            }
        }
        bRunReg = true;
    }
}

void MachineLearning::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    string guiLabel = e.target->getLabel();
    
    if (guiLabel == "Filename") {
        
        //Make sure the filename isnt affected by case sensitivity.
        string fName = e.target->getText();
        transform(fName.begin(), fName.end(), fName.begin(), ::tolower);
        fileName = fName; //A little bit of error handling
        
    }
}
