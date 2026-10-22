#include "ofMain.h"
#include "gui/GuiApp.h"
#include "projector/ProjectorApp.h"
#include "state/AppState.h"
#include "engine/Engine.h"

//========================================================================

int main() {

    auto state = std::make_shared<AppState>();

    // Create the engine with default camera settings
    auto engine = std::make_shared<Engine>(
        state,
        0,           // camera device ID
        1280, 720,   // camera resolution
        1920, 1080   // projector resolution
    );

    // Initialize the engine before apps are created
    engine->setup();

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
    ofRunApp(guiWindow, std::make_shared<GuiApp>(state, engine));
    ofRunApp(projectorWindow, std::make_shared<ProjectorApp>(state, engine));

    ofRunMainLoop();
}
