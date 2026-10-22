#include "ProjectorApp.h"

void ProjectorApp::setup() {
    stateListener = appState->stateChanged.newListener([this](int& index) {
        onStateChanged(index);
    });
}

void ProjectorApp::onStateChanged(int& index) {
    ofLogNotice("ProjectorApp") << "State changed to " << index;
    if (engine_) {
        engine_->activate(static_cast<State>(index));
    }
}

void ProjectorApp::draw() {
    ofSetBackgroundColor(ofColor::black);
    if (engine_) {
        auto& fbo = engine_->getProjectorOutput();
        if (fbo.isAllocated()) {
            int winW = ofGetWidth();
            int winH = ofGetHeight();
            int texW = fbo.getWidth();
            int texH = fbo.getHeight();
            float scale = std::min(static_cast<float>(winW) / texW,
                                    static_cast<float>(winH) / texH);
            int drawW = static_cast<int>(texW * scale);
            int drawH = static_cast<int>(texH * scale);
            int x = (winW - drawW) / 2;
            int y = (winH - drawH) / 2;
            fbo.draw(x, y, drawW, drawH);
        }
    }
}
