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
    , projectorHeight_(projectorHeight) {

    grabber_.setDeviceID(cameraDeviceId);
    grabber_.setDesiredFrameRate(30);
    grabber_.setup(cameraWidth, cameraHeight);
}

void Engine::setup() {
    ensureDirectories();
    tryLoadCameraCalibration();

    projectorOutput_.allocate(projectorWidth_, projectorHeight_, GL_RGBA);
    projectorOutput_.begin();
    ofClear(0, 0, 0, 255);
    projectorOutput_.end();
}

void Engine::update() {
    grabber_.update();
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
            // Additional checks for later states will be added in subsequent tickets
            if (s == STATE_MAP || s == STATE_PERFORM || s == STATE_VERIFY) {
                if (!repository_.hasProjectorCalibration()) {
                    blockReason_ = "Projector calibration required. Complete scanning first.";
                    return false;
                }
                if (s == STATE_MAP || s == STATE_PERFORM || s == STATE_VERIFY) {
                    if (!repository_.hasMesh()) {
                        blockReason_ = "Mesh required. Complete scanning and mesh generation first.";
                        return false;
                    }
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
    // Currently a placeholder - sub-engine activation will be added in subsequent tickets
    projectorOutput_.begin();
    ofClear(0, 0, 0, 255);
    projectorOutput_.end();
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
    // Placeholder for future SubEngine dispatch
}

void Engine::onMouseDragged(int viewportId, int x, int y, int button) {
    // Placeholder
}

void Engine::onMouseReleased(int viewportId, int x, int y, int button) {
    // Placeholder
}

void Engine::onMouseScrolled(int viewportId, float scrollX, float scrollY) {
    // Placeholder
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
