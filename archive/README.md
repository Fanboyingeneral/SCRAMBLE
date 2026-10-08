# 🧪 Archive: explorations and dead ends

**Nothing in this folder is part of the final SCRAMBLE build.** These are the experiments, ports and test apps that led to it. They are kept because they show how the design evolved, and some (like the motor tests) are handy for bring-up.

Legend: 🟢 directly led to the final design · 🟡 partly reused · 🔴 abandoned · 📖 third-party reference

---

## 📱 `android/`

| Folder | Date | Status | What it is |
|---|---|:-:|---|
| [`01-camera-hsv-client/`](android/01-camera-hsv-client/) | 23 Jul | 🟡 | **First app** (`RubiksSolverClient`). CameraX scanner that *learns* reference colours in a calibration pass (HSV), then scans. Scanning only: no solver call and no Bluetooth yet. The camera and ROI code were carried into the final app |
| [`02-manual-color-entry/`](android/02-manual-color-entry/) | 23–26 Jul | 🟡 | **`FinalVersionAttempt_v1`**. Drops the camera: you tap stickers on a grid with a colour palette. Adds the solver HTTP call and HC-05 Bluetooth sending. The final app is this project plus the camera back. That's where the name *RubiksCubeSolverWithoutCamera* comes from |
| [`03-bluetooth-test/`](android/03-bluetooth-test/) | 26 Jul | 🟢 | Minimal one-button app that sends `Z` to a paired module (originally `HC-06`). Used to prove the phone → UART link |

## 🔌 `firmware/`

| Folder | Date | Status | What it is |
|---|---|:-:|---|
| [`motor-tests/`](firmware/motor-tests/) | 13 Jul | 🟢 | Single-chip bring-up for the servos. `new_task_atmega32.c` sweeps each turret servo. `top_motors_atmega32.c` jogs turret servos over Bluetooth. `base_motors_atmega32.c` jogs base servos with 2-char commands. These became the master and slave |
| [`atmega32-cfop-port/`](firmware/atmega32-cfop-port/) | 10 Jul | 🔴 | A line-by-line port of the Arduino reference to AVR C: on-chip cube model, CFOP solver (Cross/F2L/OLL/PLL), colour-sensor reading, PCA9685 servo driver over I²C, serial menu. Dropped in favour of off-board Kociemba solving and native timer PWM on two chips |
| [`solver-smoke-test/`](firmware/solver-smoke-test/) | Jul | 🟡 | Three-line script that called `twophase.solver` directly before the HTTP server was used |

## 🛠 `hardware/`

| Folder | Status | What it is |
|---|:-:|---|
| [`stl-first-export/`](hardware/stl-first-export/) | 🟡 | First STL export of all parts, including the reference cube. Superseded by `hardware/stl-print-ready/` |

## 🖼 `proposal-assets/`

Mood-board images used in the proposal deck. `robot-arm-freepik.png` is Freepik stock (attribution in the image).

## 📖 `upstream-reference/Arduino-RubikSolver/`

A **source-only snapshot** of the open-source *Arduino-RubikSolver* project (public domain, see its [`LICENSE`](upstream-reference/Arduino-RubikSolver/LICENSE)). This is where we started. It contains:

- `arduino/`: the original `.ino` sketches (the source of the `atmega32-cfop-port` above)
- `android-app/src/`: its Kotlin app (camera colour sensing + Bluetooth)
- `CAD/`: FreeCAD 2D/3D files and photos of CNC-cut parts
- `blueprints/`: assembly drawings

Stripped from the snapshot to keep the repo small: the bundled OpenCV Android SDK (~1.5 GB), build outputs, and ~230 MB of CNC cutting videos. These are still available in the raw source folder.
