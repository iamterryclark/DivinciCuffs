//
//  MachineLearning.cpp
//  BioPerformanceTool
//
//  Created by Terry Clark on 06/03/2018.
//

#include "MachineLearning.hpp"

MachineLearning::MachineLearning(MyoManager *_myo) :
myo(_myo)
{
    bCaptureKNN = false;
    bCaptureReg = false;
    bCaptureXMM = false;
    
    bTrainKNN = false;
    bTrainReg = false;
    bTrainXMM = false;
    
    bRunKNN = false;
    bRunReg = false;
    bRunXMM = false;
    
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
    mlGui->addSlider("Segment Threshold", 0, 100);
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
    mlGui->addSlider("DMX Param 1", 0, 1);
    mlGui->addSlider("DMX Param 2", 0, 1);
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
        nn.setNumHiddenLayers(1);
        nn.setNumHiddenNodes(3);
        nn.setNumEpochs(200);
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

void MachineLearning::addDataToXML(vector<trainingExample> examples, int nnNum, string title){

    lastTagNumber = mlSettings.addTag(title);

    if( mlSettings.pushTag(title, lastTagNumber) ){

        for (int i = 0; i < examples.size(); i++) {
            int tagNum = mlSettings.addTag("example");
            
            mlSettings.pushTag("example", i);
            
            for (int j = 0; j < examples[i].input.size(); j++) {
                mlSettings.addValue("input", examples[i].input[j]);
            }
            
            //Seperate Process for Regression outputs
            if (title == "nnEMG_" + ofToString(nnNum)){
                for (int j = 0; j < examples[i].output.size(); j++){
                    mlSettings.addValue("output", (float)examples[i].output[j]);
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
        addDataToXML(knnTrainingEMG, 0, "knnEMG");
    
    if (knnTrainingAcc.size() > 0)
        addDataToXML(knnTrainingAcc, 0, "knnAcc");
    
    for (int i = 0; i < TOTALSCENES; i++){
        if (nnAllTrainingSets[i].size() > 0){
             addDataToXML(nnAllTrainingSets[i], i, "nnEMG_" + ofToString(i));
        }
    }
   
    mlSettings.save("savedGestures/" + fileName + ".xml");
}

void MachineLearning::loadDataFromXML(){
    knnTrainingEMG.clear();
    knnTrainingAcc.clear();

    for (int i = 0; i < TOTALSCENES; i++)
        nnAllTrainingSets[i].clear();
    
    if( mlSettings.loadFile("savedGestures/" + fileName + ".xml") ){
        if (mlSettings.getNumTags("knnEMG") > 0){
            dataParseXML("knnEMG", 0, knnTrainingEMG);
        }
        
        if (mlSettings.getNumTags("knnAcc") > 0){
            dataParseXML("knnAcc", 0, knnTrainingAcc);
        }
        
        for (int i = 0; i < TOTALSCENES; i++){
            if (mlSettings.getNumTags("nnEMG_" + ofToString(i)) > 0){
                dataParseXML("nnEMG_" + ofToString(i), i, nnAllTrainingSets[i]);
            }
        }

        
    } else {
        ofLogError("File did not load!");
    }
}

//Reference: http://openframeworks.cc/documentation/ofxXmlSettings/ofxXmlSettings/

void MachineLearning::dataParseXML(string searchTerm, int nnNum, vector<trainingExample> &trainingVec){
    mlSettings.pushTag(searchTerm);
    int numExTags = mlSettings.getNumTags("example");
    for(int i = 0; i < numExTags; i++){
        mlSettings.pushTag("example", i);
        
        int inputNum = mlSettings.getNumTags("input");
        
        trainingExample example;
       
        for (int j = 0; j < inputNum; j++ ){
             example.input.push_back(mlSettings.getValue("input", j));
        }
        
        if (searchTerm == "nnEMG_" + ofToString(nnNum)){
            int numOutputs = mlSettings.getNumTags("output");
            
            for (int k = 0; k < numOutputs; k++){
                example.output.push_back(mlSettings.getValue("output", double(0.0), k));
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


bool MachineLearning::segmentLimit(vector<double> inputCheck) {
    for (int i = 0; i < inputCheck.size(); i++){
        //Threshold is manipulated by slider.
        if (inputCheck[i] > segmentThreshold){
            return true;
        }
    }
    return false;
}

void MachineLearning::update() {
    for (int i = 0; i < myo->feature.size(); i++){
        if(ofGetFrameNum() % 3 == 0){
          Feature feature = myo->feature[i];
            
            //Collect data from the myo ready for input into the mahcine learning algorithms
            vector<double> emgSTD, emgSTDRatio, emgRMSRatio, gyro, PitYawRol, quat;
           
            //Testing all sorts of inputs
            emgSTD = feature.emg.stdDev;
            emgSTDRatio = feature.emg.stdRatio;
            emgRMSRatio = feature.emg.rmsRatio;
            gyro = {
                feature.acc.gyro[0],
                feature.acc.gyro[1],
                feature.acc.gyro[2],
            };
            quat = {
                feature.acc.quat.x(),
                feature.acc.quat.y(),
                feature.acc.quat.z(),
                feature.acc.quat.w()
            };
            PitYawRol = {
                feature.acc.pitch,
                feature.acc.yaw,
                feature.acc.roll
            };
            
            //
            // :: Testing / Recording Phase ::
            //
            //If we are recording a gesture
            if (bCaptureKNN){
                
                //Record EMG
                trainingExample tempEMGExampleEmg;
                tempEMGExampleEmg.input = emgSTDRatio;
                tempEMGExampleEmg.output = { (double)knnGestureNum };
                knnTrainingEMG.push_back(tempEMGExampleEmg);
                
                trainingExample tempEMGExampleAcc;
                tempEMGExampleAcc.input = PitYawRol;
                tempEMGExampleAcc.output = { (double)knnGestureNum };
                knnTrainingAcc.push_back(tempEMGExampleAcc);
                
                //Do not run Classifer whilst recording
                bRunKNN = false;
            }
            
            
            //Currently this feature needs work in understanding the implementation. Still unresolved in the GDS also
            if (bCaptureXMM){
//                //Run once before adding all elements to training Data
//                if (bStartXMMRecording){
//
//                    //Following Implemtation from Tests folder on RapidMix API
//                    xmmTempData.startRecording(ofToString(xmmGestureNum));
//                    cout << "Recording Started : " << xmmGestureNum << endl;
//                }
                xmmTempData.startRecording(ofToString(xmmGestureNum));

                vector<double> label = {(double)xmmGestureNum};
                //x y z gyro
                xmmTempData.addElement(gyro, label);
//                bStartXMMRecording = false;
                bRunXMM = false;
//                xmmTempData.stopRecording();
            }
            
            //Captureing Regression
            if (bCaptureReg){
                bRunReg = false;
                vector<double> inputVec;
                //Depending on scene number regression shall take different inputs
                switch(sceneNum){
                    case 2:
                        inputVec = emgSTDRatio;
                        inputVec.insert( inputVec.end(), quat.begin(), quat.end() );
                        break;
                    case 3:
                        inputVec = emgSTDRatio;
                        break;
                    case 4:
                        inputVec = PitYawRol;
                        break;
                    case 5:
                        inputVec = PitYawRol;
                        break;
                    case 6:
                        inputVec = PitYawRol;
                        break;
                    case 7:
                        inputVec = PitYawRol;
                        break;
                    case 9:
                        inputVec = PitYawRol;
                        break;
                    default:
                        inputVec = emgSTDRatio;
                        break;
                }
        
                nnAllTempExamples[sceneNum].input = inputVec;
        
                //Get values from the sliders to set as outputs
                nnAllTempExamples[sceneNum].output = {
                    mlGui->getSlider("Ableton Param 1")->getValue(),
                    mlGui->getSlider("Ableton Param 2")->getValue(),
                    mlGui->getSlider("Ableton Param 3")->getValue(),
                    mlGui->getSlider("Ableton Param 4")->getValue(),
                    mlGui->getSlider("DMX Param 1")->getValue(),
                    mlGui->getSlider("DMX Param 2")->getValue()
                };
                
                //Push back all respective examples into the training sets
                //ready to be trained
                nnAllTrainingSets[sceneNum].push_back(nnAllTempExamples[sceneNum]);
            }
        
            //
            // :: Training Phase ::
            //
        
            if (bTrainKNN) {
                if(knnTrainingEMG.size() > 0)
                    knnClassifierEMG.train(knnTrainingEMG);
                
                if (knnTrainingAcc.size() > 0)
                    knnClassifierAcc.train(knnTrainingAcc);
                
                bTrainKNN = false;
                bRunKNN = true;
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
                    nnAllReg[sceneNum].train(nnAllTrainingSets[sceneNum]);
                    cout << "Regression_" + ofToString(sceneNum) + " Trained" << endl;
                } else {
                    cout << "Regression_" + ofToString(sceneNum) + " Not Trained" << endl;
                }
                bTrainReg = false;
                bRunReg = true;
            }

            //
            // :: Run Phase ::
            
            if (bRunKNN) {
                //Quick and dirty segmentation
                if(segmentLimit(emgSTD)){
                    
                    //Capture class values for both emg and acc
                    int classLabelEmg = knnClassifierEMG.run({emgSTDRatio})[0];
                    int classLabelAcc = knnClassifierAcc.run({PitYawRol})[0];
                    
                    //If they are both the same class
                    if (classLabelEmg == classLabelAcc){
                        //Output the class
                        colorKNN = colors[classLabelEmg];
                    } else {
                        colorKNN = ofColor(0);
                    }
                }
            }
            
            if (bRunXMM) {
//                cout << ofToString(xmmClassifierEMG.run(gyroFeature)) << endl;
            }
            
            if (bRunReg) {
                if (nnAllTrainingSets[sceneNum].size() > 0){
                    vector<double> currentInput;
                    switch(sceneNum){
                        case 2:
                            currentInput = emgSTDRatio;
                            currentInput.insert( currentInput.end(), quat.begin(), quat.end() );
                            break;
                        case 3:
                            currentInput = emgSTDRatio;
                            break;
                        case 4:
                            currentInput = PitYawRol;
                            break;
                        case 5:
                            currentInput = PitYawRol;
                            break;
                        case 6:
                            currentInput = PitYawRol;
                            break;
                        case 7:
                            currentInput = PitYawRol;
                            break;
                        case 9:
                            currentInput = PitYawRol;
                            break;
                        default:
                            currentInput = emgSTDRatio;
                            break;
                    }
                    
                    regressVals = nnAllReg[sceneNum].run( currentInput );
                    
                    //For Visual Purposes
                    mlGui->getSlider("Ableton Param 1")->setValue(regressVals[0]);
                    mlGui->getSlider("Ableton Param 2")->setValue(regressVals[1]);
                    mlGui->getSlider("Ableton Param 3")->setValue(regressVals[2]);
                    mlGui->getSlider("Ableton Param 4")->setValue(regressVals[3]);
                    mlGui->getSlider("DMX Param 1")->setValue(regressVals[4]);
                    mlGui->getSlider("DMX Param 2")->setValue(regressVals[5]);
                }
            }
        }
    }
}

//Need to trigger this from sceneCotnrol
bool MachineLearning::playSceneTrigger(){
    if (oneShotTrigger){
        return oneShotTrigger;
    }
    oneShotTrigger = false;
    return oneShotTrigger;
}

void MachineLearning::draw() {
    //KNN Indicator
    ofSetColor(255);
    ofDrawBitmapString("KNN Examples (EMG): " + ofToString(knnTrainingEMG.size()), 50, 350);
    ofDrawBitmapString("KNN Examples (Acc): " + ofToString(knnTrainingAcc.size()), 50, 370);
    
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
        knnTrainingAcc.clear();
        knnClassifierEMG.reset();
        knnClassifierAcc.reset();
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
            nnAllReg[i].setNumHiddenLayers(1);
            nnAllReg[i].setNumHiddenNodes(3);
            nnAllReg[i].setNumEpochs(200);
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
    if (guiLabel == "Segment Threshold") segmentThreshold = e.target->getValue();
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
        
        oneShotTrigger = true;
        playSceneTrigger();
        
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
