#pragma once
#include "Stepper.h"
#include "../../state/AppState.h"
#include "../types/CameraCalibration.h"
#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include <vector>

class CameraCalibrationEngine : public Stepper {
public:
    CameraCalibrationEngine(ofVideoGrabber& grabber);

    // Stepper interface
    bool step() override;              // capture one frame; always returns false
    float getProgress() const override; // captureCount / minRequired
    void reset() override;

    // Camera cal specific
    int getCaptureCount() const { return static_cast<int>(objectPoints_.size()); }
    double getLastReprojectionError() const { return lastReprojectionError_; }
    bool hasValidCorners() const { return lastCornersFound_; }
    std::vector<cv::Point2f> getLastCorners() const { return lastCorners_; }

    bool solve();                       // run calibration with current captures
    bool isReadyToSolve() const { return getCaptureCount() >= minCaptures_; }

    CameraCalibration getCalibration() const { return calibration_; }

    static constexpr int minCaptures_ = 5;
    static const cv::Size patternSize_;
    static constexpr float squareSize_ = 25.0f; // mm

private:
    ofVideoGrabber& grabber_;

    std::vector<std::vector<cv::Point3f>> objectPoints_;
    std::vector<std::vector<cv::Point2f>> imagePoints_;

    std::vector<cv::Point2f> lastCorners_;
    bool lastCornersFound_ = false;
    double lastReprojectionError_ = -1.0;

    CameraCalibration calibration_;
    cv::Size imageSize_;
};
