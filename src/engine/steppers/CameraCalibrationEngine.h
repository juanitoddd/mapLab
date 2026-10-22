#pragma once
#include "Stepper.h"
#include "../types/CameraCalibration.h"
#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include <vector>

class CameraCalibrationEngine : public Stepper {
public:
    CameraCalibrationEngine(ofVideoGrabber& grabber);

    bool step() override;
    float getProgress() const override;
    void reset() override;

    int getCaptureCount() const;
    double getLastReprojectionError() const;
    bool hasValidCorners() const;
    std::vector<cv::Point2f> getLastCorners() const;

    bool solve();
    bool isReadyToSolve() const;
    CameraCalibration getCalibration() const;

    static const int minCaptures_;
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
