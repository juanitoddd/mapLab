#pragma once
#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include <string>

class CameraCalibration {
public:
    CameraCalibration() = default;

    bool load(const std::string& path);
    bool save(const std::string& path) const;
    bool isValid() const;

    cv::Mat getCameraMatrix() const;
    cv::Mat getDistCoeffs() const;
    cv::Size getImageSize() const;
    double getReprojectionError() const;

    void setCameraMatrix(const cv::Mat& m);
    void setDistCoeffs(const cv::Mat& d);
    void setImageSize(const cv::Size& s);
    void setReprojectionError(double e);

private:
    cv::Mat cameraMatrix_;
    cv::Mat distCoeffs_;
    cv::Size imageSize_;
    double reprojectionError_ = -1.0;
};
