#pragma once
#include "ofMain.h"
#include <string>

class Stepper {
public:
    virtual ~Stepper() = default;
    virtual bool step() = 0;
    virtual float getProgress() const = 0;
    virtual void reset() = 0;

    ofEvent<void> onComplete;
    ofEvent<std::string> onWarning;
};
