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
		if (pix.isAllocated()) {
			cameraImage_.setFromPixels(pix);
		}
	}
}

void GuiApp::draw() {
	ofSetBackgroundColor(backgroundColor);
	gui.begin();

	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);

	if (engine_) {
		ImGui::Begin("Camera Preview");
		auto tex = cameraImage_.getTexture();
		bool allocated = tex.isAllocated() && tex.getWidth() > 0 && tex.getHeight() > 0;
		if (allocated) {
			float w = ImGui::GetWindowWidth() - 20;
			float h = w * (720.0f / 1280.0f);
			ImGui::Image((ImTextureID)(uintptr_t)tex.getTextureData().textureID, ImVec2(w, h));
			ImGui::Text("Texture: %dx%d (id=%u)", tex.getWidth(), tex.getHeight(), tex.getTextureData().textureID);
		} else {
			ImGui::Text("No camera feed");
		}
		ImGui::End();
	}

	if (ImGui::BeginMainMenuBar()) {
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
					// activate() handled by ProjectorApp stateChanged listener
				}
			}
			ImGui::EndMenu();
		}
		if (engine_ && !engine_->getBlockReason().empty()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[Blocked: %s]", engine_->getBlockReason().c_str());
		}
		ImGui::EndMainMenuBar();
	}

	gui.end();
}

void GuiApp::exit() { }
void GuiApp::keyPressed(int key) { }
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
