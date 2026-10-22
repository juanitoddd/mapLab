#include "ProjectorCalibration.h"
#include <fstream>

bool ProjectorCalibration::load(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open() || file.tellg() <= 0) return false;
    file.close();

    cv::FileStorage fs(path, cv::FileStorage::READ);
    if (!fs.isOpened()) return false;
    fs["cameraMatrix"] >> cameraMatrix_;
    fs["distCoeffs"] >> distCoeffs_;
    fs["imageSize_width"] >> imageSize_.width;
    fs["imageSize_height"] >> imageSize_.height;
    fs["R"] >> R_;
    fs["T"] >> T_;
    fs["reprojectionError"] >> reprojectionError_;
    fs.release();
    return isValid();
}

bool ProjectorCalibration::save(const std::string& path) const {
    cv::FileStorage fs(path, cv::FileStorage::WRITE);
    if (!fs.isOpened()) return false;
    fs << "cameraMatrix" << cameraMatrix_;
    fs << "distCoeffs" << distCoeffs_;
    fs << "imageSize_width" << imageSize_.width;
    fs << "imageSize_height" << imageSize_.height;
    fs << "R" << R_;
    fs << "T" << T_;
    fs << "reprojectionError" << reprojectionError_;
    fs.release();
    return true;
}

bool ProjectorCalibration::isValid() const {
    if (cameraMatrix_.empty() || cameraMatrix_.rows != 3 || cameraMatrix_.cols != 3) return false;
    if (imageSize_.width <= 0 || imageSize_.height <= 0) return false;
    if (R_.empty() || T_.empty()) return false;
    if (reprojectionError_ < 0) return false;
    return true;
}

cv::Mat ProjectorCalibration::getCameraMatrix() const { return cameraMatrix_.clone(); }
cv::Mat ProjectorCalibration::getDistCoeffs() const { return distCoeffs_.clone(); }
cv::Size ProjectorCalibration::getImageSize() const { return imageSize_; }
cv::Mat ProjectorCalibration::getR() const { return R_.clone(); }
cv::Mat ProjectorCalibration::getT() const { return T_.clone(); }
double ProjectorCalibration::getReprojectionError() const { return reprojectionError_; }

void ProjectorCalibration::setCameraMatrix(const cv::Mat& m) { cameraMatrix_ = m.clone(); }
void ProjectorCalibration::setDistCoeffs(const cv::Mat& d) { distCoeffs_ = d.clone(); }
void ProjectorCalibration::setImageSize(const cv::Size& s) { imageSize_ = s; }
void ProjectorCalibration::setR(const cv::Mat& r) { R_ = r.clone(); }
void ProjectorCalibration::setT(const cv::Mat& t) { T_ = t.clone(); }
void ProjectorCalibration::setReprojectionError(double e) { reprojectionError_ = e; }
