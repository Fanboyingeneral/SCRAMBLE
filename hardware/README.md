# 🔩 Hardware

| Folder | Contents |
|---|---|
| [`stl-print-ready/`](stl-print-ready/) | ⭐ **The STLs we printed.** Exported 2 Jul 2025. `Claw2.stl` is our redesigned claw |
| [`freecad-source/`](freecad-source/) | FreeCAD 0.17 sources for each part. Most are identical to the *Arduino-RubikSolver* project's `CAD/Final cuts` |
| [`blueprints/`](blueprints/) | Assembly and part drawings (PDF) from the same upstream project |

## One arm = 6 parts (×4 arms)

| Part | Job |
|---|---|
| `Base-bottom` / `Base-top` | Frame that holds the base servo and guides the rack |
| `Gear` | Pinion on the base servo horn |
| `Rack` | Slides in and out as the gear turns, turning **rotation into linear motion** |
| `Turret` | Rides on the rack and holds the turret servo |
| `Claw` | Mounted on the turret servo horn; grips and twists one face |

`servo-base.FCStd` and `cube.FCStd` (a reference cube for fitting) are in `freecad-source/`. An earlier STL export of every part, including the cube, is in [`../archive/hardware/stl-first-export/`](../archive/hardware/stl-first-export/).
