#pragma once
#include <array>
#include <string_view>
#include "ofMain.h"

enum State {
    STATE_IDLE,
    STATE_CAMERA_CALIB,
    STATE_PROJECTOR_CALIB,
    STATE_SCAN,
    STATE_MESH_VIEW,
    STATE_VERIFY,
    STATE_MAP,
    STATE_PERFORM
};

inline constexpr std::array<std::string_view, 8> StateNames = {
    "Idle",
    "Camera Calibration",
    "Projector Calibration",
    "Scanning",
    "Mesh View",
    "Verification",
    "Mapping",
    "Performing"
};

class AppState {
public:
    // Global State
    State mode = State::STATE_IDLE;    
    ofEvent<int> stateChanged;    

    // Get State names
    inline std::string_view toString(State mode) {
      auto index = static_cast<size_t>(mode);
      if (index >= StateNames.size()) return "Unknown";
      return StateNames[index];
    }

    void setState(int index) {
      mode = static_cast<State>(index);
      ofLogNotice("AppState") << "State changed to " << toString(mode);
      ofNotifyEvent(stateChanged, index);
    }

    // Read the current mode
    State getState() const {
      return mode;
    }
};