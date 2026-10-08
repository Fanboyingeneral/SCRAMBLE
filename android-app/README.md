# 📱 SCRAMBLE Android app

The phone app used in the demo. It scans the cube, gets a solution from the laptop server and relays it to the rig over Bluetooth.

> The project and package are still called `RubiksCubeSolverWithoutCamera` / `com.example.rubikscubesolverwithoutcamera` for historical reasons. It started as a manual colour-entry app ([archive](../archive/android/02-manual-color-entry/)) and gained the camera later. Renaming the package is safe if you want to.

## Flow

```
Scan face ──▶ Confirm grid ──▶ (Retake | Accept) ──▶ … ×6 ──▶ GET solver ──▶ Solution screen
                                                                              ├─ Program  (sends "P")
                                                                              ├─ Send next move (e.g. "F2", last one gets ")")
                                                                              └─ Execute  (sends "S")
```

| Stage | Implementation |
|---|---|
| Camera | CameraX `Preview` + `ImageAnalysis` (keep-only-latest) |
| Sampling | 9 square ROIs at the centre of each grid cell, averaged RGB |
| Classification | Nearest neighbour (Euclidean RGB) against 6 reference colours |
| Face order | **U R F D L B**, which is Kociemba's facelet order |
| Colour scheme | U white · R blue · F red · D yellow · L green · B orange |
| Server call | OkHttp `GET http://<ip>:8080/<54 chars>`, solution parsed from `<body>` |
| Bluetooth | Bonded device named **`HC-05`**, insecure RFCOMM, SPP UUID `00001101-…-00805F9B34FB` |

## Configure

| What | Where |
|---|---|
| Laptop IP | `MainActivity.kt` → `sendRequestToServer()` (currently `172.20.10.12`) |
| Reference colours | `MainActivity.kt` → `referenceColorsRgb`. Tune these for your cube and lighting |
| Bluetooth name | `MainActivity.kt` → `sendBluetoothCommand()` → `deviceName` |

Cleartext HTTP to the LAN server is allowed through `res/xml/network_security_config.xml`.

## Build

Open this folder in Android Studio (compile/target SDK 36, min SDK 24) and run. Android Studio regenerates `local.properties`, which is git-ignored.
