#pragma once
#include "ofMain.h"
#include <opencv2/opencv.hpp>
#include <string>

class ProjectorCalibration {
public:
    ProjectorCalibration() = default;
    bool load(const std::string& path);
    bool save(const std::string& path) const;
    bool isValid() const;
    cv::Mat getCameraMatrix() const;
    cv::Mat getDistCoeffs() const;
    cv::Size getImageSize() const;
    cv::Mat getR() const;
    cv::Mat getT() const;
    double getReprojectionError() const;
    void setCameraMatrix(const cv::Mat& m);
    void setDistCoeffs(const cv::Mat& d);
    void setImageSize(const cv::Size& s);
    void setR(const cv::Mat& r);
    void setT(const cv::Mat& t);
    void setReprojectionError(double e);
private:
    cv::Mat cameraMatrix_;
    cv::Mat distCoeffs_;
    cv::Size imageSize_;
    cv::Mat R_;
    cv::Mat T_;
    double reprojectionError_ = -1.0;
};
