#pragma once

#include "ofMain.h"
#include "../state/AppState.h"
#include "../engine/Engine.h"
#include <memory>

class ProjectorApp : public ofBaseApp {
public:
    ProjectorApp(std::shared_ptr<AppState> state, std::shared_ptr<Engine> engine)
        : appState(std::move(state))
        , engine_(std::move(engine)) {}

    void setup() override;
    void draw() override;

    void onStateChanged(int& index);

private:
    std::shared_ptr<AppState> appState;
    std::shared_ptr<Engine> engine_;
    ofEventListener stateListener;
};
