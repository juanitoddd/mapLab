#include "Engine.h"
#include "ofFileUtils.h"
#include "ofUtils.h"

Engine::Engine(std::shared_ptr<AppState> state,
	int cameraDeviceId,
	int cameraWidth, int cameraHeight,
	int projectorWidth, int projectorHeight,
	const std::string & dataRoot)
	: appState_(std::move(state))
	, dataRoot_(dataRoot)
	, projectorWidth_(projectorWidth)
	, projectorHeight_(projectorHeight)
	, cameraCalibEngine_(std::make_unique<CameraCalibrationEngine>(grabber_)) {
	ofLogNotice("cameraDeviceId") << cameraDeviceId;
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
		if (s == STATE_MAP || s == STATE_PERFORM || s == STATE_VERIFY) {
			if (!repository_.hasProjectorCalibration()) {
				blockReason_ = "Projector calibration required. Complete scanning first.";
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
	projectorOutput_.begin();
	ofClear(0, 0, 0, 255);
	projectorOutput_.end();

	switch (s) {
	case State::STATE_CAMERA_CALIB:
		if (cameraCalibEngine_) cameraCalibEngine_->reset();
		break;
	default:
		break;
	}
}

const ofPixels & Engine::getLatestFrame() const {
	return grabber_.getPixels();
}

ofTexture Engine::getLatestFrameTexture() const {
	return grabber_.getTexture();
}

ofFbo & Engine::getProjectorOutput() {
	return projectorOutput_;
}

void Engine::ensureDirectories() {
	ofDirectory::createDirectory(ofToDataPath("calibrations", true), true, true);
	ofDirectory::createDirectory(ofToDataPath("scans", true), true, true);
	ofDirectory::createDirectory(ofToDataPath("maps", true), true, true);
	ofDirectory::createDirectory(ofToDataPath("shows", true), true, true);
}

void Engine::tryLoadCameraCalibration() {
	std::string path = DataRepository::getCameraCalibrationPath();
	ofLogNotice("Engine") << "Looking for camera calibration at: " << path;
	CameraCalibration cal;
	if (cal.load(path)) {
		repository_.setCameraCalibration(cal);
		ofLogNotice("Engine") << "Auto-loaded camera calibration";
	} else {
		ofLogNotice("Engine") << "No valid camera calibration found (file missing or empty)";
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
