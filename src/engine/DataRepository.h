#pragma once
#include "types/CameraCalibration.h"
#include "types/ProjectorCalibration.h"
#include "ofMain.h"
#include <mutex>
#include <optional>

class DataRepository {
public:
    void setCameraCalibration(const CameraCalibration& cal);
    std::optional<CameraCalibration> getCameraCalibration() const;
    bool hasCameraCalibration() const;
    ofEvent<void> onCameraCalibrationChanged;

    void setProjectorCalibration(const ProjectorCalibration& cal);
    std::optional<ProjectorCalibration> getProjectorCalibration() const;
    bool hasProjectorCalibration() const;
    ofEvent<void> onProjectorCalibrationChanged;

    static std::string getCameraCalibrationPath();
    static std::string getProjectorCalibrationPath();

private:
    mutable std::mutex mtx_;
    std::optional<CameraCalibration> cameraCal_;
    std::optional<ProjectorCalibration> projectorCal_;
};
