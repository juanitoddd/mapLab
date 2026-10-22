#include "ofMain.h"
#include "gui/GuiApp.h"
#include "projector/ProjectorApp.h"
#include "state/AppState.h"

//========================================================================

int main() {

    auto state = std::make_shared<AppState>();

    ofGLFWWindowSettings guiSettings;
    guiSettings.setSize(1280, 800);
    guiSettings.setGLVersion(3, 3);
    guiSettings.windowMode = OF_WINDOW;
    guiSettings.title = "ProjectionMappingLab - GUI";

    auto guiWindow = ofCreateWindow(guiSettings);

    ofGLFWWindowSettings projectorSettings;
    projectorSettings.setSize(1920, 1080);
    projectorSettings.setGLVersion(3, 3);
    projectorSettings.windowMode = OF_FULLSCREEN;
    projectorSettings.monitor = 1;
    projectorSettings.title = "ProjectionMappingLab - Projector";

    projectorSettings.shareContextWith = guiWindow;

    auto projectorWindow = ofCreateWindow(projectorSettings);
    ofRunApp(guiWindow, std::make_shared<GuiApp>(state));
    ofRunApp(projectorWindow, std::make_shared<ProjectorApp>(state));

    ofRunMainLoop();

		#if defined(OF_GL_VERSION_MAJOR) && defined(OF_GL_VERSION_MINOR)
			settings.setGLVersion(OF_GL_VERSION_MAJOR, OF_GL_VERSION_MINOR); // set via projectGenerator OpenGL version
		#endif
}