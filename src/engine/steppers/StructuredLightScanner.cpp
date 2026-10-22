#include "StructuredLightScanner.h"
#include "ofGraphics.h"

// ---------------------------------------------------------------------------
// GrayCodeGenerator
// ---------------------------------------------------------------------------

GrayCodeGenerator::GrayCodeGenerator(int width, int height)
    : width_(width), height_(height)
    , hBits_(0), vBits_(0), currentPair_(0) {
    // Calculate bits needed: ceil(log2(width)) and ceil(log2(height))
    int tmpW = width_ - 1;
    while (tmpW > 0) { hBits_++; tmpW >>= 1; }
    if (hBits_ == 0) hBits_ = 1;

    int tmpH = height_ - 1;
    while (tmpH > 0) { vBits_++; tmpH >>= 1; }
    if (vBits_ == 0) vBits_ = 1;

    normalPattern_.allocate(width_, height_, GL_RGBA);
    inversePattern_.allocate(width_, height_, GL_RGBA);
}

int GrayCodeGenerator::getTotalPairs() const {
    return hBits_ + vBits_;
}

void GrayCodeGenerator::advance() {
    if (currentPair_ < getTotalPairs()) {
        currentPair_++;
        if (currentPair_ < getTotalPairs()) {
            generatePair(currentPair_);
        }
    }
}

void GrayCodeGenerator::reset() {
    currentPair_ = 0;
    generatePair(0);
}

int GrayCodeGenerator::grayCode(int n) const {
    return n ^ (n >> 1);
}

void GrayCodeGenerator::generatePair(int pairIndex) {
    normalPattern_.begin();
    ofClear(0, 0, 0, 255);

    if (pairIndex < hBits_) {
        // Horizontal bit: encode x position
        int bitPos = hBits_ - 1 - pairIndex; // MSB first
        for (int x = 0; x < width_; ++x) {
            int g = grayCode(x);
            bool bit = ((g >> bitPos) & 1) != 0;
            if (bit) {
                ofSetColor(255, 255, 255);
                ofDrawLine(x, 0, x, height_);
            }
        }
    } else {
        // Vertical bit: encode y position
        int bitPos = vBits_ - 1 - (pairIndex - hBits_); // MSB first
        for (int y = 0; y < height_; ++y) {
            int g = grayCode(y);
            bool bit = ((g >> bitPos) & 1) != 0;
            if (bit) {
                ofSetColor(255, 255, 255);
                ofDrawLine(0, y, width_, y);
            }
        }
    }
    normalPattern_.end();

    // Inverse: swap black/white
    inversePattern_.begin();
    ofClear(255, 255, 255, 255);
    if (pairIndex < hBits_) {
        int bitPos = hBits_ - 1 - pairIndex;
        for (int x = 0; x < width_; ++x) {
            int g = grayCode(x);
            bool bit = ((g >> bitPos) & 1) != 0;
            if (bit) {
                ofSetColor(0, 0, 0);
                ofDrawLine(x, 0, x, height_);
            }
        }
    } else {
        int bitPos = vBits_ - 1 - (pairIndex - hBits_);
        for (int y = 0; y < height_; ++y) {
            int g = grayCode(y);
            bool bit = ((g >> bitPos) & 1) != 0;
            if (bit) {
                ofSetColor(0, 0, 0);
                ofDrawLine(0, y, width_, y);
            }
        }
    }
    inversePattern_.end();
}

// ---------------------------------------------------------------------------
// StructuredLightScanner
// ---------------------------------------------------------------------------

StructuredLightScanner::StructuredLightScanner(int projectorWidth, int projectorHeight)
    : projectorWidth_(projectorWidth)
    , projectorHeight_(projectorHeight)
    , resolutionX_(1024)
    , resolutionY_(768) {
    generator_ = std::make_unique<GrayCodeGenerator>(resolutionX_, resolutionY_);
}

void StructuredLightScanner::setResolution(int w, int h) {
    resolutionX_ = w;
    resolutionY_ = h;
    generator_ = std::make_unique<GrayCodeGenerator>(resolutionX_, resolutionY_);
    reset();
}

bool StructuredLightScanner::step() {
    // Capture the current pair index then advance
    if (generator_->isFinished()) {
        ofNotifyEvent(onComplete);
        return true;
    }

    // Advance to next pair
    generator_->advance();

    if (generator_->isFinished()) {
        ofNotifyEvent(onComplete);
        return true;
    }

    return false;
}

float StructuredLightScanner::getProgress() const {
    int total = generator_->getTotalPairs();
    if (total <= 0) return 1.0f;
    return static_cast<float>(generator_->getCurrentPairIndex()) / static_cast<float>(total);
}

void StructuredLightScanner::reset() {
    stepped_ = false;
    generator_->reset();
}

void StructuredLightScanner::renderPatternTo(ofFbo& target) {
    if (!generator_) return;

    target.begin();
    ofClear(0, 0, 0, 255);
    generator_->getNormalPattern().draw(0, 0, target.getWidth(), target.getHeight());
    target.end();
}

int StructuredLightScanner::getCurrentPair() const {
    return generator_->getCurrentPairIndex();
}

int StructuredLightScanner::getTotalPairs() const {
    return generator_->getTotalPairs();
}

bool StructuredLightScanner::isHorizontalBit() const {
    int hBits = 0;
    int tmp = resolutionX_ - 1;
    while (tmp > 0) { hBits++; tmp >>= 1; }
    if (hBits == 0) hBits = 1;
    return getCurrentPair() < hBits;
}

int StructuredLightScanner::getCurrentBitIndex() const {
    int hBits = 0;
    int tmp = resolutionX_ - 1;
    while (tmp > 0) { hBits++; tmp >>= 1; }
    if (hBits == 0) hBits = 1;
    if (getCurrentPair() < hBits) {
        return getCurrentPair();
    } else {
        return getCurrentPair() - hBits;
    }
}

void StructuredLightScanner::updatePatterns() {
    // Patterns are generated on-demand in generatePair
}
