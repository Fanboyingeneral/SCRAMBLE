# 🧭 Provenance: raw folder → repo

This repo was assembled from the original, unorganised `CSE 316/` working folder, which is preserved untouched. The tables below show where everything came from and what was deliberately left out.

## Copied

| Raw path | Repo path |
|---|---|
| `maxresdefault.jpg` | `docs/assets/banner.jpg` |
| `Project/Project Proposal Presentation.pdf`, `Project/Updated Proposal.pdf` | `docs/proposal/` |
| `Project/Arduino-RubikSolver-master/arduino files/a_constants/master_controller.c` | `firmware/master/` |
| `Project/Arduino-RubikSolver-master/arduino files/a_constants/slave_controller.c` | `firmware/slave/` |
| `RubiksCubeSolverWithoutCamera/` | `android-app/` |
| `Project_Web_Server/RubiksCube-TwophaseSolver-master/` | `solver-server/` (`README.md` → `UPSTREAM_README.md`) |
| `Project_Web_Server/test_client.py` | `solver-server/test_client.py` |
| `Project/Print Ready 3D Files/` | `hardware/stl-print-ready/` |
| `Project/Original 3D Files/` | `hardware/freecad-source/` |
| `Project/rubick-*.pdf` | `hardware/blueprints/` |
| `Project_Android_App/` | `archive/android/01-camera-hsv-client/` |
| `FinalVersionAttempt_v1/` | `archive/android/02-manual-color-entry/` |
| `BluetoothTestApp/` | `archive/android/03-bluetooth-test/` |
| `…/a_constants/*_atmega32.c` (except motor tests) | `archive/firmware/atmega32-cfop-port/` |
| `…/a_constants/{top_motors,base_motors,new_task}_atmega32.c` | `archive/firmware/motor-tests/` |
| `…/arduino files/Solver/solver.py` | `archive/firmware/solver-smoke-test/` |
| `Project/3D Files/All3D_stl/` | `archive/hardware/stl-first-export/` |
| `Project/6190212806561875445.jpg`, `Project/c3RlZCZ3PTc0MA.png` | `archive/proposal-assets/` |
| `Project/Arduino-RubikSolver-master/` (sources, CAD, blueprints, photos) | `archive/upstream-reference/Arduino-RubikSolver/` |
| `Online Prep/Online 1/*.asm`, `Online q.pdf` | `coursework/assembly-online-1/` |
| `Offlines/*.pdf` | `coursework/assembly-offlines/` |
| `Experiments/*.pdf`, `demo.pptx` | `coursework/experiments/` |

## Not copied (still in the raw folder)

| Raw path | Why |
|---|---|
| `Project/Final Demo.mov` (569 MB) | Over GitHub's 100 MB limit. It's [on YouTube](https://youtu.be/4RdSLmTVIiQ) |
| `Project/arduino-ide_nightly-…zip` (196 MB) | Third-party installer |
| `emu8086/`, `emu8086v408r.exe` | Third-party tool installation |
| `Experiments/Utilities*` | Third-party binary |
| `Project/IMG_20190305_233856.jpg` | Duplicate of an upstream photo (kept in `archive/upstream-reference/…/CAD/photos/`) |
| `Project/3D Files/All3D_stl.zip` | Zip of `All3D_stl/`, which is copied unzipped |
| `Project/Atmel/` | Empty |
| Upstream `android App/java/` + native libs (~1.5 GB) | Bundled OpenCV Android SDK |
| Upstream `CAD/CNC cutting media/*.mp4` (~230 MB) | Upstream build videos |
| Upstream `arduino files/Solver/` tables, `Project_Web_Server/…/twophase/` (~70 MB each) | Generated solver tables; rebuilt on first run |
| `Online Prep/Online 1/noname.exe*` | emu8086 build artifacts and backups |
| `build/`, `.gradle/`, `.idea/`, `.kotlin/`, `__pycache__/`, `local.properties` | Generated or machine-specific |
