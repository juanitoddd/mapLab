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

	// Render camera preview into dedicated FBO
	if (cameraImage_.isAllocated() && cameraImage_.getWidth() > 0) {
		int imgW = static_cast<int>(cameraImage_.getWidth());
		int imgH = static_cast<int>(cameraImage_.getHeight());
		if (!cameraPreviewFbo_.isAllocated() ||
		    static_cast<int>(cameraPreviewFbo_.getWidth()) != imgW ||
		    static_cast<int>(cameraPreviewFbo_.getHeight()) != imgH) {
			cameraPreviewFbo_.allocate(imgW, imgH, GL_RGB);
		}
		cameraPreviewFbo_.begin();
		ofClear(0, 0, 0, 255);
		cameraImage_.draw(0, 0);
		cameraPreviewFbo_.end();
	}

	gui.begin();

	if (engine_) {
		ImGui::Begin("Camera Preview");
		bool hasFbo = cameraPreviewFbo_.isAllocated()
		              && cameraPreviewFbo_.getWidth() > 0;
		if (hasFbo) {
			float w = ImGui::GetWindowContentRegionWidth() - 10;
			float h = cameraPreviewFbo_.getHeight() * (w / cameraPreviewFbo_.getWidth());
			ImGui::Image(
			    (ImTextureID)(uintptr_t)cameraPreviewFbo_.getTexture().getTextureData().textureID,
			    ImVec2(w, h)
			);
			ImGui::Text("%dx%d",
			            (int)cameraPreviewFbo_.getWidth(),
			            (int)cameraPreviewFbo_.getHeight());
		} else {
			ImGui::Text("No camera feed");
		}
		ImGui::End();
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

	// Debug: native OF draw bypassing ImGui entirely
	if (debugDrawCamera_ && cameraImage_.isAllocated() && cameraImage_.getWidth() > 0) {
		ofPushStyle();
		ofSetColor(255);
		cameraImage_.draw(10, 100, 640, 360);
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("DEBUG: Native OF draw (press 'd' to toggle)", 12, 95);
		ofPopStyle();
	}
}

void GuiApp::exit() { }
void GuiApp::keyPressed(int key) {
	if (key == 'd' || key == 'D') {
		debugDrawCamera_ = !debugDrawCamera_;
		ofLogNotice("GuiApp") << "Debug camera draw: " << (debugDrawCamera_ ? "ON" : "OFF");
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
