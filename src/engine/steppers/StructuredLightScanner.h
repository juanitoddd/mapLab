#pragma once
#include "Stepper.h"
#include "ofMain.h"
#include <memory>
#include <vector>

class GrayCodeGenerator {
public:
    GrayCodeGenerator(int width, int height);

    int getTotalPairs() const;
    int getCurrentPairIndex() const { return currentPair_; }
    bool isFinished() const { return currentPair_ >= getTotalPairs(); }

    ofFbo& getNormalPattern() { return normalPattern_; }
    ofFbo& getInversePattern() { return inversePattern_; }

    void advance();
    void reset();

private:
    void generatePair(int pairIndex);
    int grayCode(int n) const;

    int width_;
    int height_;
    int hBits_;
    int vBits_;
    int currentPair_;

    ofFbo normalPattern_;
    ofFbo inversePattern_;
};

class StructuredLightScanner : public Stepper {
public:
    StructuredLightScanner(int projectorWidth, int projectorHeight);

    bool step() override;
    float getProgress() const override;
    void reset() override;

    void setResolution(int w, int h);
    int getCurrentPair() const;
    int getTotalPairs() const;

    // Rendering
    void renderPatternTo(ofFbo& target);
    bool isHorizontalBit() const; // true if current pair is horizontal
    int getCurrentBitIndex() const;

    int getResolutionX() const { return resolutionX_; }
    int getResolutionY() const { return resolutionY_; }

private:
    void updatePatterns();

    int projectorWidth_;
    int projectorHeight_;
    int resolutionX_;
    int resolutionY_;

    std::unique_ptr<GrayCodeGenerator> generator_;
    bool stepped_ = false;
};
