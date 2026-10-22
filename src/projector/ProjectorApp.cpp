#include "ProjectorApp.h"

void ProjectorApp::setup() {
  stateListener = appState->stateChanged.newListener([this](int& index) {
    onStateChanged(index);
  });
}

void ProjectorApp::onStateChanged(int& index) {
    ofLogNotice("ProjectorApp") << "State changed to " << index;
    // Update projector resources if necessary.
}

void ProjectorApp::draw(){      
  ofSetBackgroundColor(ofColor(114, 144, 154));

  switch (appState->getState()) {

    case State::STATE_IDLE:
      ofSetBackgroundColor(ofColor(114, 255, 154));
      break;

    case State::STATE_CAMERA_CALIB:
      ofSetBackgroundColor(ofColor(78, 144, 154));
      // Draw calibration pattern
      break;

    case State::STATE_SCAN:
      ofSetBackgroundColor(ofColor(114, 144, 98));
      // Draw structured-light pattern
      break;

    case State::STATE_PROJECTOR_CALIB:
      ofSetBackgroundColor(ofColor(114, 144, 78));
      // Render projection mapping
      break;
  }
}