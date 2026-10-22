#pragma once

#include "ofMain.h"
#include "../state/AppState.h"

class ProjectorApp : public ofBaseApp {
public:
    explicit ProjectorApp(std::shared_ptr<AppState> state)
        : appState(std::move(state)) {}

    void setup() override;
    void draw() override;

    void onPatternChanged(int& index);
    void onStateChanged(int& index);

private:
    std::shared_ptr<AppState> appState;
    ofEventListener patternListener;
    ofEventListener stateListener;
};