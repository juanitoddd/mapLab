#pragma once
#include "../state/AppState.h"
#include "ofMain.h"
#include "ofxImGui.h"
#include "ui/UiTheme.h"

class GuiApp : public ofBaseApp {
public:
	explicit GuiApp(std::shared_ptr<AppState> state)
		: appState(std::move(state)) { }

	void setup() override;
	void update() override;
	void draw() override;
	void exit() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void mouseMoved(int x, int y) override;
	void mouseDragged(int x, int y, int button) override;
	void mousePressed(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
	void mouseEntered(int x, int y) override;
	void mouseExited(int x, int y) override;
	void windowResized(int w, int h) override;
	void dragEvent(ofDragInfo dragInfo) override;
	void gotMessage(ofMessage msg) override;

	ImVec4 backgroundColor;
	bool show_test_window;
	bool show_another_window;
	float floatValue;

private:
	std::shared_ptr<AppState> appState;
	ofxImGui::Gui gui;
	// Hardware
	ofVideoGrabber grabber;
};