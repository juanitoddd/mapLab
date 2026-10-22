#pragma once
#include "DataRepository.h"
#include "steppers/CameraCalibrationEngine.h"
#include "../state/AppState.h"
#include "ofMain.h"
#include <memory>
#include <string>

class Engine {
public:
    Engine(std::shared_ptr<AppState> state,
           int cameraDeviceId,
           int cameraWidth, int cameraHeight,
           int projectorWidth, int projectorHeight,
           const std::string& dataRoot = "data/");

    void setup();
    void update();

    bool canEnter(State s) const;
    std::string getBlockReason() const;
    void activate(State s);

    const ofPixels& getLatestFrame() const;
    ofTexture getLatestFrameTexture() const;
    ofFbo& getProjectorOutput();

    ofEvent<State> onRequestState;
    DataRepository& getRepository() { return repository_; }

    // Steppers
    CameraCalibrationEngine* getCameraCalibrationEngine() { return cameraCalibEngine_.get(); }

    // Calibration persistence
    bool saveCameraCalibration(const std::string& path);
    bool loadCameraCalibration(const std::string& path);

private:
    void ensureDirectories();
    void tryLoadCameraCalibration();
    void setupEventListeners();

    std::shared_ptr<AppState> appState_;
    std::string dataRoot_;
    mutable std::string blockReason_;
    ofVideoGrabber grabber_;
    ofFbo projectorOutput_;
    DataRepository repository_;
    int projectorWidth_;
    int projectorHeight_;

    // Steppers
    std::unique_ptr<CameraCalibrationEngine> cameraCalibEngine_;

    // Event listeners
    ofEventListener camCalCompleteListener_;
    ofEventListener camCalWarningListener_;
};
