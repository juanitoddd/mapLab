#pragma once
#include "ofMain.h"

class Stepper {
public:
    virtual ~Stepper() = default;
    virtual bool step() = 0;              // return true when complete
    virtual float getProgress() const = 0; // 0..1
    virtual void reset() = 0;

    ofEvent<void> onComplete;
    ofEvent<std::string> onWarning;
};
