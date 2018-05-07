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
        nn.setNumHiddenLayers(3);
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
            string aNeuralNet = "nn_EMG_" + ofToString(i);
           
            if (title != aNeuralNet){
                mlSettings.addValue("label", (int)examples[i].output[i]);
            }
            mlSettings.popTag();
        }
        mlSettings.popTag();
    }
}

void MachineLearning::saveDataToXML(){
    mlSettings.clear();
    if (knnTrainingEMG.size() > 0) addDataToXML(knnTrainingEMG, "knnEMG");
    
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
        if (mlSettings.getNumTags("knnemg") > 0) {
            cout << "knnEMG " + ofToString( mlSettings.getNumTags("knnemg") )<< endl;
            dataParseXML("knnEMG", knnTrainingEMG);
        }
        
        for (int i = 0; i < TOTALSCENES; i++){
            if (mlSettings.getNumTags("nnEmg_" + ofToString(i)) > 0){
                cout << "nnEmg_ " + ofToString(i) + " " + ofToString( mlSettings.getNumTags("knnemg") )<< endl;
                dataParseXML("nnEMG_"+ ofToString(i), nnAllTrainingSets[i]);
            }
        }
    } else {
        ofLogError("Position file did not load!");
    }
}

//Reference: http://openframeworks.cc/documentation/ofxXmlSettings/ofxXmlSettings/
void MachineLearning::dataParseXML(string searchTerm, vector<trainingExample> &trainingVec){
    mlSettings.pushTag(searchTerm);
    int numData = mlSettings.getNumTags("example");
    for(int i = 0; i < numData; i++){
        mlSettings.pushTag("example", i);
        int inputNum = mlSettings.getNumTags("input");
        trainingExample example;
        for (int j = 0; j < inputNum; j++ ){
             example.input.push_back(mlSettings.getValue(searchTerm + ":input_" + ofToString(j),0, j));
        }
        example.output.push_back(mlSettings.getValue("label", 0, 0));

        trainingVec.push_back(example);
        mlSettings.popTag();
    }
    mlSettings.popTag(); //pop position
}

void MachineLearning::update(MyoManager &myo) {
    for (int i = 0; i < myo.feature.size(); i++){
        Feature feature = myo.feature[i];
        
        if(ofGetFrameNum() % 5 == 0){
            
            //Collect data from the myo ready for input into the mahcine learning algorithms
            vector<double> emgFeature, gyroFeature;
            vector<double> emgBayes;
            vector<double> emgBayesQuat;
            
            emgFeature = feature.emg.rms;
            
            emgBayes = feature.emg.bayesRMS;
            
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
                tempEMGExample.input = emgBayes;
                tempEMGExample.output = { (double)knnGestureNum };
                knnTrainingEMG.push_back(tempEMGExample);
                
                //Do not run Classifer whilst recording
                bRunKNN = false;
            }
            
            if (bCaptureXMM){
                
                //Following Implemtation from Tests folder on RapidMix API
                xmmTempData.startRecording(ofToString(xmmGestureNum));
                
                vector<double> label = {(double)xmmGestureNum};
                
                xmmTempData.addElement(gyroFeature, label);
                
                xmmTempData.stopRecording();

                bRunXMM = false;
            }
            
            if (bCaptureReg){
                bRunReg = false;
                int i = sceneNum;
                
                nnAllTempExamples[i].input = { emgBayes };
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
                bTrainXMM = false;
//                if(xmmTempData.trainingSet.size() > 0) {
                    //Training Phase for NN Regression
                xmmClassifierEMG.train(xmmTempData);
//                xmmTempData.trainingSet.clear();
                bRunXMM = true;
//                }
            }
            
            if (bTrainReg) {
                //Training Phase for NN Regression
                if(nnAllTrainingSets[sceneNum].size() > 0){
                    nnAllReg[sceneNum].train(nnAllTrainingSets[sceneNum]);
                    cout << "Regression_" + ofToString(sceneNum) + " Trained" << endl;
                    bTrainReg = false;
                    bRunReg = true;
                } else {
                    cout << "Regression_" + ofToString(sceneNum) + " Not Trained" << endl;
                }
            }

            //
            // :: Run Phase ::
            //
            if (bRunKNN) {
                int classLabel = knnClassifierEMG.run({emgBayes})[0];
                colorKNN = colors[classLabel];
            }
            
            if (bRunXMM) {
                
//                vector<rapidmix::rapidStream> xmmRS;
//                for (int i = 0; i < 3; i ++){
//                    rapidStream<double> rs((int)xmmTempData.trainingSet.size());
//                    xmmRS.push_back(rs);
//                    xmmRS[i].pushToWindow(accFeature[i]);
//                }
//                
//                
//                vector<double> progress(xmmTempData.trainingSet.size());
////                cout << progress.size() << endl;
//                for (int i = 0; i < progress.size(); i++) {
//                    
////                    xmmRS.pushToWindow(accFeature[0]);
//                    myXmmPhrase.push_back({xmmRS[0]});
//
////                    // we take the second value because run() returns
////                    // [ likelihood_1, timeProgression_1, .... likelihood_n, timeProgression_n ]
//                    progress[i] = xmmClassifierEMG.run(myXmmPhrase[i])[1];
//                    cout << "Progress_" + ofToString(i) << " : "  << progress.size() << endl;
//                }
//                
//                myXmmPhrase.clear();
//                std::vector<double> sortedProgress = progress;
//                std::sort(sortedProgress.begin(), sortedProgress.end());
                
            }
            
            if (bRunReg) {
                if (nnAllTrainingSets[sceneNum].size() > 0){
                    regressVals = nnAllReg[sceneNum].run({ emgBayes });
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
    ofDrawBitmapString("XMM Training Data: " + ofToString(xmmTempData.trainingSet.size()), 50, 460);
    ofDrawBitmapString("XMM Phrase Size: " + ofToString(xmmPhrases.size()), 50, 480);
    
    for (int i = 0; i < TOTALSCENES; i++){
        if (sceneNum == i){
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
        xmmPhrases.clear();
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
    } else if (guiLabel == "XMM Record"){
        xmmGestureNum = whichButton;
        bCaptureXMM = e.enabled;
    } else if (guiLabel == "Scene"){
        bRunReg = false;
        sceneNum = whichButton;

        for(int i =0; i < TOTALSCENES; i++){
            if (i == sceneNum){
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
        fileName = e.target->getText();
        transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
    }
}
