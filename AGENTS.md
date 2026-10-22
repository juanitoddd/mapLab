# ProjectionMappingLab — Agent Brief

OpenFrameworks app for camera-projector structured-light scanning, 3D reconstruction, and projection mapping onto physical surfaces.

## Architecture

Tha application consists of two apps GuiApp `src/gui/*` and ProjectorApp `src/projector/*` that share state and events through `src/state/AppState.h` and also share a compute engine `src/engine` which takes care of the scanning, calibrations, mesh mapping and rest of tasks.

### App States

from `src/state/AppState.h`, we have following states:

- STATE_IDLE
- STATE_CAMERA_CALIB
- STATE_PROJECTOR_CALIB
- STATE_SCAN
- STATE_MESH_VIEW
- STATE_VERIFY
- STATE_MAP
- STATE_PERFORM

### Engine

This is where the classes that manage the computer vision, structured-light scanning, 3D reconstruction, and projection mapping are located

This includes, camera-projector calibration, mapping editor, mesh viewer, structured light scanner and rest of additional classes and helpers.

## Agent skills

### Issue tracker

Issues live as GitHub issues in `juanitoddd/ProjectionMappingLab`. See `docs/agents/issue-tracker.md`.

### Triage labels

Five canonical roles, unmapped (`needs-triage` through `wontfix`). See `docs/agents/triage-labels.md`.

### Domain docs

Single-context layout: one `GLOSSARY.md` + `docs/adr/` at repo root. See `docs/agents/domain.md`.