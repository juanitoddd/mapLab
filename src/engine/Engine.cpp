#include "Engine.h"
#include "ofUtils.h"
#include "ofFileUtils.h"

Engine::Engine(std::shared_ptr<AppState> state,
               int cameraDeviceId,
               int cameraWidth, int cameraHeight,
               int projectorWidth, int projectorHeight,
               const std::string& dataRoot)
    : appState_(std::move(state))
    , dataRoot_(dataRoot)
    , projectorWidth_(projectorWidth)
    , projectorHeight_(projectorHeight)
    , cameraCalibEngine_(std::make_unique<CameraCalibrationEngine>(grabber_))
    , scanner_(std::make_unique<StructuredLightScanner>(projectorWidth, projectorHeight)) {

    grabber_.setDeviceID(cameraDeviceId);
    grabber_.setDesiredFrameRate(30);
    grabber_.setup(cameraWidth, cameraHeight);
}

void Engine::setup() {
    ensureDirectories();
    tryLoadCameraCalibration();
    setupEventListeners();

    projectorOutput_.allocate(projectorWidth_, projectorHeight_, GL_RGBA);
    projectorOutput_.begin();
    ofClear(0, 0, 0, 255);
    projectorOutput_.end();
}

void Engine::setupEventListeners() {
    camCalCompleteListener_ = cameraCalibEngine_->onComplete.newListener([this]() {
        auto cal = cameraCalibEngine_->getCalibration();
        if (cal.isValid()) {
            repository_.setCameraCalibration(cal);
            cal.save(DataRepository::getCameraCalibrationPath());
            ofLogNotice("Engine") << "Camera calibration solved and saved";
        }
        State idleState = State::STATE_IDLE;
        ofNotifyEvent(onRequestState, idleState, this);
    });

    camCalWarningListener_ = cameraCalibEngine_->onWarning.newListener([this](std::string& msg) {
        ofLogWarning("CameraCalib") << msg;
    });

    scannerCompleteListener_ = scanner_->onComplete.newListener([this]() {
        ofLogNotice("Scanner") << "Scan complete";
        State projCalibState = State::STATE_PROJECTOR_CALIB;
        ofNotifyEvent(onRequestState, projCalibState, this);
    });
}

void Engine::update() {
    grabber_.update();

    // Render active projector pattern
    if (activeState_ == State::STATE_SCAN && scanner_ && projectorPatternDirty_) {
        scanner_->renderPatternTo(projectorOutput_);
        projectorPatternDirty_ = false;
    }
}

bool Engine::canEnter(State s) const {
    blockReason_.clear();

    switch (s) {
        case State::STATE_IDLE:
        case State::STATE_CAMERA_CALIB:
            return true;

        case State::STATE_SCAN:
        case State::STATE_PROJECTOR_CALIB:
        case State::STATE_MESH_VIEW:
        case State::STATE_VERIFY:
        case State::STATE_MAP:
        case State::STATE_PERFORM: {
            if (!repository_.hasCameraCalibration()) {
                blockReason_ = "Camera calibration required. Run Camera Calibration first.";
                return false;
            }
            if (s == STATE_SCAN) return true;

            if (s == STATE_PROJECTOR_CALIB) {
                return true; // Allowed after scan or for review
            }

            if (s == STATE_MAP || s == STATE_PERFORM || s == STATE_VERIFY || s == STATE_MESH_VIEW) {
                if (!repository_.hasProjectorCalibration()) {
                    blockReason_ = "Projector calibration required. Complete scanning first.";
                    return false;
                }
                if (!repository_.hasMesh()) {
                    blockReason_ = "Mesh required. Complete scanning and mesh generation first.";
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

std::string Engine::getBlockReason() const {
    return blockReason_;
}

void Engine::activate(State s) {
    activeState_ = s;
    projectorPatternDirty_ = true;

    projectorOutput_.begin();
    ofClear(0, 0, 0, 255);
    projectorOutput_.end();

    switch (s) {
        case State::STATE_IDLE:
            break;
        case State::STATE_CAMERA_CALIB:
            if (cameraCalibEngine_) cameraCalibEngine_->reset();
            break;
        case State::STATE_SCAN:
            if (scanner_) {
                scanner_->reset();
                projectorPatternDirty_ = true;
            }
            break;
        case State::STATE_PROJECTOR_CALIB:
        case State::STATE_MESH_VIEW:
        case State::STATE_VERIFY:
        case State::STATE_MAP:
        case State::STATE_PERFORM:
            break;
    }
}

const ofPixels& Engine::getLatestFrame() const {
    return grabber_.getPixels();
}

ofTexture Engine::getLatestFrameTexture() const {
    return grabber_.getTexture();
}

ofFbo& Engine::getProjectorOutput() {
    return projectorOutput_;
}

void Engine::onMousePressed(int viewportId, int x, int y, int button) {
}

void Engine::onMouseDragged(int viewportId, int x, int y, int button) {
}

void Engine::onMouseReleased(int viewportId, int x, int y, int button) {
}

void Engine::onMouseScrolled(int viewportId, float scrollX, float scrollY) {
}

void Engine::ensureDirectories() {
    ofDirectory::createDirectory(ofToDataPath("calibrations", true), true, true);
    ofDirectory::createDirectory(ofToDataPath("scans", true), true, true);
    ofDirectory::createDirectory(ofToDataPath("maps", true), true, true);
    ofDirectory::createDirectory(ofToDataPath("shows", true), true, true);
}

void Engine::tryLoadCameraCalibration() {
    CameraCalibration cal;
    if (cal.load(DataRepository::getCameraCalibrationPath())) {
        repository_.setCameraCalibration(cal);
        ofLogNotice("Engine") << "Auto-loaded camera calibration";
    }
}

bool Engine::saveCameraCalibration(const std::string& path) {
    auto cal = repository_.getCameraCalibration();
    if (cal) {
        return cal->save(path);
    }
    return false;
}

bool Engine::loadCameraCalibration(const std::string& path) {
    CameraCalibration cal;
    if (cal.load(path)) {
        repository_.setCameraCalibration(cal);
        return true;
    }
    return false;
}
