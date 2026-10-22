#include "CameraCalibration.h"

bool CameraCalibration::load(const std::string& path) {
    cv::FileStorage fs(path, cv::FileStorage::READ);
    if (!fs.isOpened()) return false;
    fs["cameraMatrix"] >> cameraMatrix_;
    fs["distCoeffs"] >> distCoeffs_;
    fs["imageSize_width"] >> imageSize_.width;
    fs["imageSize_height"] >> imageSize_.height;
    fs["reprojectionError"] >> reprojectionError_;
    fs.release();
    return isValid();
}

bool CameraCalibration::save(const std::string& path) const {
    cv::FileStorage fs(path, cv::FileStorage::WRITE);
    if (!fs.isOpened()) return false;
    fs << "cameraMatrix" << cameraMatrix_;
    fs << "distCoeffs" << distCoeffs_;
    fs << "imageSize_width" << imageSize_.width;
    fs << "imageSize_height" << imageSize_.height;
    fs << "reprojectionError" << reprojectionError_;
    fs.release();
    return true;
}

bool CameraCalibration::isValid() const {
    if (cameraMatrix_.empty() || cameraMatrix_.rows != 3 || cameraMatrix_.cols != 3) return false;
    if (imageSize_.width <= 0 || imageSize_.height <= 0) return false;
    if (reprojectionError_ < 0) return false;
    return true;
}

cv::Mat CameraCalibration::getCameraMatrix() const { return cameraMatrix_.clone(); }
cv::Mat CameraCalibration::getDistCoeffs() const { return distCoeffs_.clone(); }
cv::Size CameraCalibration::getImageSize() const { return imageSize_; }
double CameraCalibration::getReprojectionError() const { return reprojectionError_; }

void CameraCalibration::setCameraMatrix(const cv::Mat& m) { cameraMatrix_ = m.clone(); }
void CameraCalibration::setDistCoeffs(const cv::Mat& d) { distCoeffs_ = d.clone(); }
void CameraCalibration::setImageSize(const cv::Size& s) { imageSize_ = s; }
void CameraCalibration::setReprojectionError(double e) { reprojectionError_ = e; }
