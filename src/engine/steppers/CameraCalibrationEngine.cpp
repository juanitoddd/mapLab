#include "CameraCalibrationEngine.h"

const int CameraCalibrationEngine::minCaptures_ = 5;
const cv::Size CameraCalibrationEngine::patternSize_ = cv::Size(9, 6); // inner corners

CameraCalibrationEngine::CameraCalibrationEngine(ofVideoGrabber& grabber)
    : grabber_(grabber) {
    imageSize_ = cv::Size(grabber_.getWidth(), grabber_.getHeight());
}

bool CameraCalibrationEngine::step() {
    if (!grabber_.isInitialized()) {
        std::string msg = "Camera not initialized";
        ofNotifyEvent(onWarning, msg);
        return false;
    }

    grabber_.update();
    if (!grabber_.isFrameNew()) {
        std::string msg = "No new frame";
        ofNotifyEvent(onWarning, msg);
        return false;
    }

    // Convert OF RGB pixels to OpenCV Mat (RGB)
    ofPixels pix = grabber_.getPixels();
    cv::Mat image(grabber_.getHeight(), grabber_.getWidth(), CV_8UC3, pix.getData());
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_RGB2GRAY);

    // Find chessboard corners
    std::vector<cv::Point2f> corners;
    bool found = cv::findChessboardCornersSB(gray, patternSize_, corners,
        cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

    lastCorners_ = corners;
    lastCornersFound_ = found;

    if (!found) {
        std::string msg = "No chessboard detected — try again";
        ofNotifyEvent(onWarning, msg);
        return false;
    }

    // Refine corner positions
    cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));

    // Build object points for this frame
    std::vector<cv::Point3f> objPts;
    for (int y = 0; y < patternSize_.height; ++y) {
        for (int x = 0; x < patternSize_.width; ++x) {
            objPts.emplace_back(x * squareSize_, y * squareSize_, 0.0f);
        }
    }

    objectPoints_.push_back(objPts);
    imagePoints_.push_back(corners);

    // Compute running reprojection error if enough frames
    if (getCaptureCount() >= minCaptures_ && !objectPoints_.empty()) {
        cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat distCoeffs;
        std::vector<cv::Mat> rvecs, tvecs;
        cv::calibrateCamera(objectPoints_, imagePoints_, imageSize_,
            cameraMatrix, distCoeffs, rvecs, tvecs,
            cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);

        // Evaluate on these same frames to get RMS error
        double totalErr = 0.0;
        int totalPoints = 0;
        for (size_t i = 0; i < objectPoints_.size(); ++i) {
            std::vector<cv::Point2f> projected;
            cv::projectPoints(objectPoints_[i], rvecs[i], tvecs[i],
                cameraMatrix, distCoeffs, projected);
            for (size_t j = 0; j < projected.size(); ++j) {
                double dx = projected[j].x - imagePoints_[i][j].x;
                double dy = projected[j].y - imagePoints_[i][j].y;
                totalErr += dx * dx + dy * dy;
            }
            totalPoints += static_cast<int>(projected.size());
        }
        lastReprojectionError_ = std::sqrt(totalErr / totalPoints);
    }

    return false; // step() returns true only when explicitly completed via solve()
}

float CameraCalibrationEngine::getProgress() const {
    return std::min(1.0f, static_cast<float>(getCaptureCount()) / static_cast<float>(minCaptures_));
}

void CameraCalibrationEngine::reset() {
    objectPoints_.clear();
    imagePoints_.clear();
    lastCorners_.clear();
    lastCornersFound_ = false;
    lastReprojectionError_ = -1.0;
    calibration_ = CameraCalibration();
}

int CameraCalibrationEngine::getCaptureCount() const {
    return static_cast<int>(objectPoints_.size());
}

double CameraCalibrationEngine::getLastReprojectionError() const {
    return lastReprojectionError_;
}

bool CameraCalibrationEngine::hasValidCorners() const {
    return lastCornersFound_;
}

std::vector<cv::Point2f> CameraCalibrationEngine::getLastCorners() const {
    return lastCorners_;
}

bool CameraCalibrationEngine::solve() {
    if (getCaptureCount() < minCaptures_) {
        std::string msg = "Need at least " + std::to_string(minCaptures_) + " captures";
        ofNotifyEvent(onWarning, msg);
        return false;
    }

    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    cv::Mat distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;

    double rms = cv::calibrateCamera(objectPoints_, imagePoints_, imageSize_,
        cameraMatrix, distCoeffs, rvecs, tvecs,
        cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);

    calibration_.setCameraMatrix(cameraMatrix);
    calibration_.setDistCoeffs(distCoeffs);
    calibration_.setImageSize(imageSize_);
    calibration_.setReprojectionError(rms);

    lastReprojectionError_ = rms;

    ofNotifyEvent(onComplete);
    return true;
}

bool CameraCalibrationEngine::isReadyToSolve() const {
    return getCaptureCount() >= minCaptures_;
}

CameraCalibration CameraCalibrationEngine::getCalibration() const {
    return calibration_;
}
