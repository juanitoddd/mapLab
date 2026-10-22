#include "GuiApp.h"

void GuiApp::setup() {
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	// Gui
	gui.setup();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().MouseDrawCursor = false;

	backgroundColor = ofColor(114, 144, 154);
	show_test_window = false;
	show_another_window = false;
	floatValue = 0.0f;
}

//--------------------------------------------------------------
void GuiApp::update() {
	if (engine_) {
		engine_->update();
	}
}

void GuiApp::draw() {
	ofSetBackgroundColor(backgroundColor);

	gui.begin();

	// Main docking space
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);

	ImGui::BeginMainMenuBar();
	ImGui::Text("| %s |", appState->toString(appState->mode).data());
	if (ImGui::BeginMenu("State")) {
		for (int i = 0; i < static_cast<int>(State::STATE_PERFORM) + 1; ++i) {
			State state = static_cast<State>(i);
			bool canEnter = engine_ ? engine_->canEnter(state) : true;
			if (!canEnter) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
				ImGui::MenuItem(appState->toString(state).data(), nullptr, false, false);
				ImGui::PopStyleColor();
			} else if (ImGui::MenuItem(appState->toString(state).data())) {
				appState->setState(static_cast<int>(state));
				if (engine_) engine_->activate(state);
			}
		}
		ImGui::EndMenu();
	}
	if (engine_ && !engine_->getBlockReason().empty()) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[Blocked: %s]", engine_->getBlockReason().c_str());
	}
	ImGui::EndMainMenuBar();

	// State-specific UI
	switch (appState->getState()) {
	case State::STATE_IDLE: {
		ImGuiIO & io = ImGui::GetIO();
		ImGui::Begin("Input Debug");
		ImGui::Text("Mouse: %.1f, %.1f", io.MousePos.x, io.MousePos.y);
		ImGui::Text("Left button: %d", io.MouseDown[0]);
		ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
		ImGui::Text("Hovered: %d", ImGui::IsWindowHovered());
		if (ImGui::Button("TEST BUTTON")) {
			ofLogNotice() << "ImGui button clicked!";
		}
		ImGui::End();
		break;
	}

	case State::STATE_CAMERA_CALIB: {
		drawCameraCalibPanel();
		break;
	}

	case State::STATE_SCAN: {
		drawScanPanel();
		break;
	}
	case State::STATE_PROJECTOR_CALIB:
	case State::STATE_MESH_VIEW:
	case State::STATE_VERIFY:
	case State::STATE_MAP:
	case State::STATE_PERFORM:
		// Placeholder for future tickets
		break;
	}

	gui.end();
}

void GuiApp::drawCameraCalibPanel() {
	if (!engine_) return;

	// Camera preview with overlay
	ImGui::Begin("Camera Calibration");

	auto tex = engine_->getLatestFrameTexture();
	if (tex.isAllocated()) {
		float w = ImGui::GetWindowWidth() - 20;
		float h = w * (720.0f / 1280.0f);

		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)(uintptr_t)tex.getTextureData().textureID, ImVec2(w, h));

		// Draw corner overlay
		auto* calib = engine_->getCameraCalibrationEngine();
		if (calib) {
			auto corners = calib->getLastCorners();
			bool found = calib->hasValidCorners();

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			for (const auto& c : corners) {
				float px = imagePos.x + c.x * (w / 1280.0f);
				float py = imagePos.y + c.y * (h / 720.0f);
				if (found) {
					drawList->AddCircleFilled(ImVec2(px, py), 3.0f, IM_COL32(0, 255, 0, 255));
				}
			}

			if (!found && !corners.empty()) {
				drawList->AddText(imagePos, IM_COL32(255, 0, 0, 255), "X");
			}
		}
	} else {
		ImGui::Text("No camera feed");
	}

	ImGui::Separator();

	auto* calib = engine_->getCameraCalibrationEngine();
	if (calib) {
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

		bool canFinish = calib->isReadyToSolve();
		if (canFinish) {
			if (ImGui::Button("Finish")) {
				if (calib->solve()) {
					ofLogNotice("GuiApp") << "Camera calibration solved";
				}
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
			// Simple save dialog placeholder — use fixed path for now
			engine_->saveCameraCalibration(ofToDataPath("calibrations/camera_manual.yml", true));
		}

		ImGui::SameLine();
		if (ImGui::Button("Load...")) {
			engine_->loadCameraCalibration(ofToDataPath("calibrations/camera_manual.yml", true));
		}
	}

	ImGui::End();
}

void GuiApp::drawScanPanel() {
	if (!engine_) return;

	auto* scanner = engine_->getScanner();
	if (!scanner) return;

	ImGui::Begin("Structured Light Scanning");

	// Pattern preview
	auto& projFbo = engine_->getProjectorOutput();
	if (projFbo.isAllocated()) {
		float w = ImGui::GetWindowWidth() - 20;
		float h = w * (static_cast<float>(projFbo.getHeight()) / projFbo.getWidth());
		ImGui::Image((ImTextureID)(uintptr_t)projFbo.getTexture().getTextureData().textureID, ImVec2(w, h));
	}

	ImGui::Separator();

	int pair = scanner->getCurrentPair();
	int total = scanner->getTotalPairs();
	ImGui::Text("Pair: %d / %d", pair + 1, total);
	ImGui::ProgressBar(scanner->getProgress(), ImVec2(-1, 0));

	// Resolution spinner
	static int resX = scanner->getResolutionX();
	static int resY = scanner->getResolutionY();
	if (pair == 0) {
		ImGui::InputInt("Resolution X", &resX, 256);
		ImGui::InputInt("Resolution Y", &resY, 256);
		if (ImGui::Button("Apply Resolution")) {
			scanner->setResolution(resX, resY);
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Capture")) {
		if (scanner->step()) {
			ofLogNotice("GuiApp") << "Scan complete";
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		scanner->reset();
	}

	ImGui::End();
}

//--------------------------------------------------------------
void GuiApp::exit() {
}

//--------------------------------------------------------------
void GuiApp::keyPressed(int key) {
	ofLogVerbose(__FUNCTION__) << key;
	switch (key) {
	case 's': {
		break;
	}
	}
}

//--------------------------------------------------------------
void GuiApp::keyReleased(int key) {
	ofLogVerbose(__FUNCTION__) << key;
}

//--------------------------------------------------------------
void GuiApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void GuiApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void GuiApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void GuiApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void GuiApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	ofLogVerbose(__FUNCTION__) << "x: " << x << " y: " << y << " scrollX: " << scrollX << " scrollY: " << scrollY;
}

//--------------------------------------------------------------
void GuiApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void GuiApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void GuiApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void GuiApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void GuiApp::dragEvent(ofDragInfo dragInfo) {
}
