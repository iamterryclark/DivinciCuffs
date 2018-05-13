#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    myoManager = new MyoManager();
    interactiveML = new MachineLearning(myoManager);
    sceneControl = new SceneOSCController(interactiveML, myoManager);
}

//--------------------------------------------------------------
void ofApp::update(){
    myoManager->update();
    interactiveML->update();
    sceneControl->update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    myoManager->draw();
    sceneControl->draw( ofVec2f( 50, ofGetHeight() - 100) );
    interactiveML->draw();
   
    ofSetColor(255);
    ofDrawBitmapString("FrameRate: " + ofToString((int)ofGetFrameRate()), ofGetWidth()-200, ofGetHeight()-10);
}

void ofApp::exit(){
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    sceneControl->keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
