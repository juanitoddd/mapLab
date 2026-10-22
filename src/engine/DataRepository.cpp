#include "DataRepository.h"
#include "ofUtils.h"

void DataRepository::setCameraCalibration(const CameraCalibration& cal) {
    std::lock_guard<std::mutex> lock(mtx_);
    cameraCal_ = cal;
    ofNotifyEvent(onCameraCalibrationChanged);
}

std::optional<CameraCalibration> DataRepository::getCameraCalibration() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return cameraCal_;
}

bool DataRepository::hasCameraCalibration() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return cameraCal_.has_value();
}

void DataRepository::setProjectorCalibration(const ProjectorCalibration& cal) {
    std::lock_guard<std::mutex> lock(mtx_);
    projectorCal_ = cal;
    ofNotifyEvent(onProjectorCalibrationChanged);
}

std::optional<ProjectorCalibration> DataRepository::getProjectorCalibration() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return projectorCal_;
}

bool DataRepository::hasProjectorCalibration() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return projectorCal_.has_value();
}

void DataRepository::setMesh(const ofVboMesh& mesh) {
    std::lock_guard<std::mutex> lock(mtx_);
    mesh_ = mesh;
    ofNotifyEvent(onMeshChanged);
}

std::optional<ofVboMesh> DataRepository::getMesh() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return mesh_;
}

bool DataRepository::hasMesh() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return mesh_.has_value();
}

std::string DataRepository::getCameraCalibrationPath() {
    return ofToDataPath("calibrations/camera.yml", true);
}

std::string DataRepository::getProjectorCalibrationPath() {
    return ofToDataPath("calibrations/projector.yml", true);
}
