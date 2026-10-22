#include "GuiApp.h"

void GuiApp::setup() {
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	// Cam Grabber
	grabber.setDeviceID(0);
	grabber.setDesiredFrameRate(30);
	grabber.setup(1280, 720);

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
	grabber.update();
}

void GuiApp::draw() {
	ofSetBackgroundColor(backgroundColor);

	//required to call this at beginning
	gui.begin();

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
		break;
	}

	case State::STATE_CAMERA_CALIB: {
		grabber.draw(0, 0);
		// Draw calibration pattern
		break;
	}

	case State::STATE_SCAN: {
		// Draw structured-light pattern
		break;
	}

	case State::STATE_PROJECTOR_CALIB: {
		// Render projection mapping
		break;
	}
	}

	if (ImGui::BeginMainMenuBar()) {
		ImGui::Text("| %s |", appState->toString(appState->mode).data());
		if (ImGui::BeginMenu("State")) {
			for (int i = 0; i < static_cast<int>(State::STATE_PERFORM) + 1; ++i) {
				State state = static_cast<State>(i);
				if (ImGui::MenuItem(appState->toString(state).data())) {
					appState->setState(static_cast<int>(state));
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::End();
	gui.end();
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
	// ofLogNotice("ControlApp") << "Mouse pressed: " << x << ", " << y << " button: " << button;
}

//--------------------------------------------------------------
void GuiApp::mouseReleased(int x, int y, int button) {
	// ofLogNotice("ControlApp") << "Mouse released: " << x << ", " << y << " button: " << button;
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