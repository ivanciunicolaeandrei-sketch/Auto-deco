# Auto Decorator

A Geode mod for Geometry Dash that helps creators restyle levels with one editor button.

## Planned features

- One floating editor button that opens the Auto Decorator menu.
- Ten starter themes: Hell Theme, White Space, Oceanic, Amethyst, Monochrome, Nature, Nine Circles, Cyberpunk, Industrial, and Sunset.
- Per-preset intensity and glow settings.
- Sensitivity control and saved custom presets.
- A local level-style scanner that analyzes existing level objects and proposes a matching preset. This is a heuristic prototype, not a hosted generative-AI service.
- Non-destructive editing: the first implementation will prioritize previewing and applying changes only after the creator confirms.

## Target

- Geometry Dash 2.2081
- Geode
- Android 64-bit first, with Windows, macOS and iOS CI targets.

Linux users can investigate running the Windows build under Proton/Wine; this project does not claim a native Linux target yet.

## Build

Use the Geode CLI and SDK. Android 64-bit:

```sh
geode sdk install-binaries -p android64
geode build -p android64
```

The GitHub Actions workflow builds Geode mod packages (`.geode`) for the configured platform targets and uploads them as an artifact. This project does **not** build or distribute an Android APK. On Android, install the `.geode` file using Geode's mod installation flow.

## Status

Early scaffold. The editor entry button and build pipeline are the first milestone; object-level decoration, custom preset persistence and level analysis are being developed incrementally.
