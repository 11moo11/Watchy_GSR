# Watchy_GSR LLM Codebase Guide

This document provides a high-level overview of the Watchy_GSR codebase to help LLMs (and humans) quickly understand the architecture, key concepts, and file structure.

## Overview

**Watchy_GSR** is a custom firmware for the Watchy smartwatch, an ESP32-based e-paper watch. The codebase is written in C++ using the Arduino framework and is built using PlatformIO. It provides a robust, customizable, and battery-efficient core with support for custom watch faces, a dynamic menu system, web-based configuration, and various integrations (weather, NTP, steps via BMA accelerometer).

## Technology Stack & Environment

- **Platform:** ESP32 (Support for `esp32dev` for v1/v2 and `esp32s3` for v3)
- **Framework:** Arduino
- **Build System:** PlatformIO (`platformio.ini`)
- **Display:** GxEPD2 library for e-paper display manipulation
- **Storage:** Non-Volatile Storage (NVS) via `ArduinoNvs`

## Architecture & Core Components

The firmware is designed around a core class `WatchyGSR` which handles the heavy lifting of power management, sensor communication, and the base UI loop. Customizations and watch faces are injected by creating a derived class.

### The Entry Point (`src/GSR.ino`)
- This is the main entry point of the application.
- It defines `class OverrideGSR : public WatchyGSR`.
- By overriding virtual methods like `InsertAddWatchStyles`, `InsertInitWatchStyle`, and `InsertDrawWatchStyle`, you can implement custom watch faces and behaviors without modifying the core `Watchy_GSR` code.
- `setup()` initializes the `watchy` instance and `loop()` remains empty (since ESP32 deep sleeps and wakes up on interrupts).

### Core Framework (`src/Watchy_GSR.h` & `src/Watchy_GSR.cpp`)
- **`WatchyGSR` Class:** The heart of the OS.
- Manages the RTC (Real-Time Clock), deep sleep cycles, WiFi, NTP synchronization, and weather fetching.
- Defines base drawing operations and the state machine (`GSR_WATCHON`, `GSR_MENUON`, `GSR_GAMEON`).
- Manages the captive portal AP for initial WiFi and settings configuration.

### Menu & UI System (`src/UISimple_GSR.h` & `src/UISimple_GSR.cpp`)
- Handles the full-screen modular menu UI (enabled via `GSR_SIMPLE_UI` in `Defines_GSR.h`).
- **`GSRUIModule` Class:** Contains all the rendering logic for the menus, including headers, list items, footers, scrollbars, and dithering effects.
- Relies heavily on the e-paper's partial refresh capabilities.

### Important Configuration Files
- **`src/Defines_GSR.h`:** Contains macros, constants, state definitions, and feature flags (e.g., `GSR_SIMPLE_UI`).
- **`src/Locale_GSR.h`:** Handles localization and language strings.
- **`src/Fonts_*.h` & `src/Icons_GSR.h`:** Byte arrays containing pre-compiled fonts and bitmaps (weather icons, UI elements).
- **`platformio.ini`:** Contains environments for different hardware versions (v1/v2, v3). Dependencies are strictly managed here using GitHub URLs and specific commits for stability.

## Development Workflows & Best Practices

1. **Adding a New Watch Face:**
   - Do NOT modify `Watchy_GSR.cpp` directly.
   - Open `src/GSR.ino`.
   - Implement `InsertAddWatchStyles()` to register the face name.
   - Implement `InsertInitWatchStyle()` to define layout parameters (positions, fonts, colors).
   - Implement `InsertDrawWatchStyle()` to write the actual drawing logic using the GxEPD2 APIs (`display`).

2. **UI Changes:**
   - Visual modifications to the menu should be done in `UISimple_GSR.cpp`.
   - The UI uses structured dithering (`drawDitherSidebar`, `drawFooterDitherAndPill`) to mimic grayscale on the black-and-white e-paper display.
   - Always consider partial refresh constraints when changing the UI.

3. **Power Management:**
   - The ESP32 spends most of its time in deep sleep.
   - Do not write blocking code or infinite loops (`delay()`, `while(true)`). 
   - Variables that need to survive sleep must be marked with `RTC_DATA_ATTR`.
   - Utilize the built-in wake-up stubs rather than executing the entire `setup()` if full wake is not necessary.

4. **Debugging:**
   - Use the `watchy-v1-v2` or `watchy-v3` PlatformIO environments depending on the user's board.
   - The system utilizes `GSRLogOutput` for structured logging.

## Summary for LLMs

When tasked with modifying this codebase:
- Start by checking if the task can be achieved via `OverrideGSR` in `GSR.ino`.
- If modifying the menu UI, go straight to `UISimple_GSR.cpp`.
- If changing core logic or adding hardware integrations, look at `Watchy_GSR.cpp`.
- Always verify board version via `platformio.ini` when dealing with compilation issues.
