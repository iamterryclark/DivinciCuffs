#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
    myoManager.update();
    
    interactiveML.update(myoManager);
    
    oscControl.update(interactiveML);
    dmxControl.update(interactiveML);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    myoManager.draw();
    
    oscControl.draw( ofVec2f( 50, ofGetHeight() - 100) );
    
    interactiveML.draw();
    dmxControl.draw();

    ofDrawBitmapString("FrameRate: " + ofToString((int)ofGetFrameRate()), ofGetWidth()-200, ofGetHeight()-10);
}

void ofApp::exit(){
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    oscControl.keyPressed(key);
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
