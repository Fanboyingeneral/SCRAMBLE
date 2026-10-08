# ⚙️ Firmware: ATmega32 master + slave

Bare-metal AVR C with direct register access and no Arduino core. Both chips run on the **internal 1 MHz RC oscillator** (factory fuses).

| File | Chip | Drives |
|---|---|---|
| [`master/master_controller.c`](master/master_controller.c) | Master | HC-05 UART, 4 turret servos, 4 lines to the slave, move choreography |
| [`slave/slave_controller.c`](slave/slave_controller.c) | Slave | 4 base (grip) servos, mirroring the master's 4 lines |

## Build and flash

Requires `avr-gcc`, `avr-libc` and `avrdude` (or Microchip Studio).

```bash
make                 # → master.hex, slave.hex
make flash-master    # default programmer: USBasp
make flash-slave
make flash-master PROGRAMMER=stk500v1 PORT=COM5   # override
```

## Why 4 servos per chip

The ATmega32 has four hardware PWM outputs: **OC0** (PB3), **OC1A** (PD5), **OC1B** (PD4) and **OC2** (PD7). Timer 1 is 16-bit and drives two of them. Timers 0 and 2 are 8-bit and drive one each. Eight servos therefore need two chips.

## PWM configuration

```c
// Timer 1: Fast PWM, TOP = ICR1 (mode 14), prescaler 8 → 8 µs/tick, 2049 ticks ≈ 16.4 ms
TCCR1A = (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
ICR1   = 2048;

// Timer 2: Fast PWM 8-bit, prescaler 64 → 64 µs/tick, 256 ticks ≈ 16.4 ms
TCCR2 = (1<<WGM21) | (1<<WGM20) | (1<<COM21) | (1<<CS22);

// Timer 0: Fast PWM 8-bit, prescaler 64 → same 16.4 ms frame
TCCR0 = (1<<WGM01) | (1<<WGM00) | (1<<COM01) | (1<<CS01) | (1<<CS00);
```

Pulse width = `OCRx × tick`. For example, `OCR1B = 75` gives 600 µs and `OCR1B = 210` gives 1.68 ms.

## Calibration constants

Measured on our rig. **Re-measure these for yours.**

| Servo | Register | 0° / min | 90° / max |
|---|---|:-:|:-:|
| Turret Front | master `OCR1B` | 75 | 210 |
| Turret Back | master `OCR1A` | 78 | 204 |
| Turret Left | master `OCR2` | 7 | 24 |
| Turret Right | master `OCR0` | 8 | 24 |
| Base Front | slave `OCR1B` | 70 | 230 |
| Base Back | slave `OCR1A` | 120 | 280 |
| Base Left | slave `OCR2` | 10 | 30 |
| Base Right | slave `OCR0` | 10 | 30 |

Use the single-byte jog commands (`F`/`f`, `W`/`w`, …) from a Bluetooth serial terminal to find each value.

## Master → slave wiring

| Master | Slave | Base servo |
|:-:|:-:|---|
| PC0 | PC0 | Front |
| PC1 | PC1 | Back |
| PA2 | PC2 | Left |
| PA3 | PC3 | Right |
| GND | GND | *common ground* |

> The header comment in `master_controller.c` says PC0–PC3, but the code drives **PC0, PC1, PA2, PA3**. The table above follows the code.

## Move notation

The solver outputs moves as `<face><n>`, where `n` is the number of clockwise quarter turns (`1` = 90°, `2` = 180°, `3` = 270°, i.e. counter-clockwise). `execute_sequence()` parses that format directly and stops at `(` (the `(20f)` length suffix).

| Move | Implementation |
|---|---|
| `F` `B` `L` `R` + 1/2/3 | Release → pre-rotate → grip → twist, using the face's own arm |
| `U` + n | `Rotate_top()` → `F` + n → `Rotate_bottom()` |
| `D` + n | `Rotate_bottom()` → `F` + n → `Rotate_top()` |

Each primitive step is followed by `_delay_ms(900)`.
