#pragma once
#include "types/CameraCalibration.h"
#include "types/ProjectorCalibration.h"
#include "ofMain.h"
#include <mutex>
#include <optional>

class DataRepository {
public:
    // Camera calibration
    void setCameraCalibration(const CameraCalibration& cal);
    std::optional<CameraCalibration> getCameraCalibration() const;
    bool hasCameraCalibration() const;
    ofEvent<void> onCameraCalibrationChanged;

    // Projector calibration
    void setProjectorCalibration(const ProjectorCalibration& cal);
    std::optional<ProjectorCalibration> getProjectorCalibration() const;
    bool hasProjectorCalibration() const;
    ofEvent<void> onProjectorCalibrationChanged;

    // Mesh
    void setMesh(const ofVboMesh& mesh);
    std::optional<ofVboMesh> getMesh() const;
    bool hasMesh() const;
    ofEvent<void> onMeshChanged;

    // Persistence paths
    static std::string getCameraCalibrationPath();
    static std::string getProjectorCalibrationPath();

private:
    mutable std::mutex mtx_;
    std::optional<CameraCalibration> cameraCal_;
    std::optional<ProjectorCalibration> projectorCal_;
    std::optional<ofVboMesh> mesh_;
};
