#pragma once
#include "DataRepository.h"
#include "steppers/CameraCalibrationEngine.h"
#include "steppers/StructuredLightScanner.h"
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

    // State guard
    bool canEnter(State s) const;
    std::string getBlockReason() const;

    void activate(State s);

    // Camera
    const ofPixels& getLatestFrame() const;
    ofTexture getLatestFrameTexture() const;

    // Projector output
    ofFbo& getProjectorOutput();

    // Events
    ofEvent<State> onRequestState;

    // Data repository
    DataRepository& getRepository() { return repository_; }

    // Mouse forwarding
    void onMousePressed(int viewportId, int x, int y, int button);
    void onMouseDragged(int viewportId, int x, int y, int button);
    void onMouseReleased(int viewportId, int x, int y, int button);
    void onMouseScrolled(int viewportId, float scrollX, float scrollY);

    // Steppers / SubEngines
    CameraCalibrationEngine* getCameraCalibrationEngine() { return cameraCalibEngine_.get(); }
    StructuredLightScanner* getScanner() { return scanner_.get(); }

    // Manual calibration persistence
    bool saveCameraCalibration(const std::string& path);
    bool loadCameraCalibration(const std::string& path);

private:
    void ensureDirectories();
    void tryLoadCameraCalibration();
    void setupEventListeners();

    std::shared_ptr<AppState> appState_;
    std::string dataRoot_;
    mutable std::string blockReason_;

    // Hardware
    ofVideoGrabber grabber_;
    ofFbo projectorOutput_;

    // Data
    DataRepository repository_;

    // Dimensions
    int projectorWidth_;
    int projectorHeight_;

    // Steppers
    std::unique_ptr<CameraCalibrationEngine> cameraCalibEngine_;
    std::unique_ptr<StructuredLightScanner> scanner_;

    // Event listeners
    ofEventListener camCalCompleteListener_;
    ofEventListener camCalWarningListener_;
    ofEventListener scannerCompleteListener_;
    ofEventListener scannerWarningListener_;

    State activeState_ = State::STATE_IDLE;
    bool projectorPatternDirty_ = true;


};
