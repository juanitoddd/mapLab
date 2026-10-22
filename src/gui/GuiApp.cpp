#include "GuiApp.h"

void GuiApp::setup() {
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	gui.setup();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().MouseDrawCursor = false;
	backgroundColor = ofColor(114, 144, 154);
	show_test_window = false;
	show_another_window = false;
	floatValue = 0.0f;
}

void GuiApp::update() {
	if (engine_) {
		engine_->update();
		const ofPixels& pix = engine_->getLatestFrame();
		if (pix.isAllocated() && pix.getWidth() > 0 && pix.getHeight() > 0) {
			cameraImage_.setFromPixels(pix);
			cameraImage_.update();
		}
	}
}

void GuiApp::draw() {
	ofSetBackgroundColor(backgroundColor);

	// Camera preview position
	float previewX = 10;
	float previewY = 40;
	float previewW = 640;
	float previewH = 360;

	// Draw camera preview window
	bool showPreview = cameraImage_.isAllocated() && cameraImage_.getWidth() > 0;
	if (showPreview) {
		ofPushStyle();
		ofSetColor(40, 40, 40);
		ofDrawRectangle(previewX - 2, previewY - 22, previewW + 4, previewH + 24);
		ofSetColor(60, 60, 60);
		ofDrawRectangle(previewX - 2, previewY - 22, previewW + 4, 20);
		ofSetColor(255);
		ofDrawBitmapString("Camera Preview", previewX + 4, previewY - 8);
		ofSetColor(255);
		cameraImage_.draw(previewX, previewY, previewW, previewH);

		// Corner overlay in STATE_CAMERA_CALIB
		if (appState->getState() == State::STATE_CAMERA_CALIB && engine_) {
			auto* calib = engine_->getCameraCalibrationEngine();
			if (calib) {
				auto corners = calib->getLastCorners();
				bool found = calib->hasValidCorners();
				float sx = previewW / static_cast<float>(cameraImage_.getWidth());
				float sy = previewH / static_cast<float>(cameraImage_.getHeight());
				if (found) {
					ofSetColor(0, 255, 0);
					for (const auto& c : corners) {
						float px = previewX + c.x * sx;
						float py = previewY + c.y * sy;
						ofDrawCircle(px, py, 3);
					}
				} else {
					ofSetColor(255, 0, 0);
					ofDrawBitmapString("X", previewX + 4, previewY + 14);
				}
			}
		}
		ofPopStyle();
	}

	// ImGui overlay
	gui.begin();

	if (appState->getState() == State::STATE_CAMERA_CALIB && engine_) {
		auto* calib = engine_->getCameraCalibrationEngine();
		if (calib) {
			ImGui::Begin("Camera Calibration");
			int count = calib->getCaptureCount();
			ImGui::Text("Captures: %d", count);
			ImGui::ProgressBar(calib->getProgress(), ImVec2(-1, 0), "Progress");

			double rms = calib->getLastReprojectionError();
			if (rms >= 0) {
				ImGui::Text("Reprojection Error: %.4f px", rms);
			}

			if (ImGui::Button("Capture")) {
				calib->step();
			}

			ImGui::SameLine();
			if (calib->isReadyToSolve()) {
				if (ImGui::Button("Finish")) {
					calib->solve();
				}
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
				ImGui::Button("Finish (locked)");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text("(need %d)", CameraCalibrationEngine::minCaptures_);
			}

			ImGui::Separator();

			if (ImGui::Button("Reset")) {
				calib->reset();
			}
			ImGui::SameLine();
			if (ImGui::Button("Save As...")) {
				engine_->saveCameraCalibration(ofToDataPath("calibrations/camera_manual.yml", true));
			}
			ImGui::SameLine();
			if (ImGui::Button("Load...")) {
				engine_->loadCameraCalibration(ofToDataPath("calibrations/camera_manual.yml", true));
			}
			ImGui::End();
		}
	}

	if (ImGui::BeginMainMenuBar()) {
		ImGui::Text("| %s |", appState->toString(appState->mode).data());
		if (ImGui::BeginMenu("State")) {
			for (int i = 0; i <= static_cast<int>(State::STATE_PERFORM); ++i) {
				State state = static_cast<State>(i);
				bool canEnter = engine_ ? engine_->canEnter(state) : true;
				if (!canEnter) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
					ImGui::MenuItem(appState->toString(state).data(), nullptr, false, false);
					ImGui::PopStyleColor();
				} else if (ImGui::MenuItem(appState->toString(state).data())) {
					appState->setState(static_cast<int>(state));
				}
			}
			ImGui::EndMenu();
		}
		if (engine_ && !engine_->getBlockReason().empty()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
			                   "[Blocked: %s]", engine_->getBlockReason().c_str());
		}
		ImGui::EndMainMenuBar();
	}

	gui.end();
}

void GuiApp::keyPressed(int key) {
	if (key == 's') {
		if (engine_) {
			auto tex = engine_->getLatestFrameTexture();
			ofLogNotice("GuiApp") << "Grabber texture: " << tex.getWidth() << "x" << tex.getHeight();
			ofLogNotice("GuiApp") << "Image texture: " << cameraImage_.getTexture().getWidth()
			                      << "x" << cameraImage_.getTexture().getHeight()
			                      << " allocated=" << cameraImage_.getTexture().isAllocated();
		}
	}
}
void GuiApp::keyReleased(int key) { }
void GuiApp::mouseMoved(int x, int y) { }
void GuiApp::mouseDragged(int x, int y, int button) { }
void GuiApp::mousePressed(int x, int y, int button) { }
void GuiApp::mouseReleased(int x, int y, int button) { }
void GuiApp::mouseScrolled(int x, int y, float scrollX, float scrollY) { }
void GuiApp::mouseEntered(int x, int y) { }
void GuiApp::mouseExited(int x, int y) { }
void GuiApp::windowResized(int w, int h) { }
void GuiApp::gotMessage(ofMessage msg) { }
void GuiApp::dragEvent(ofDragInfo dragInfo) { }
void GuiApp::exit() { }
