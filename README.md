# JUCE Projects

A collection of small audio DSP projects built with [JUCE](https://juce.com/) and C++. Each project lives in its own folder under `projects/` and keeps its original commit history.

## Projects

| Project | Description |
| --- | --- |
| [`JUCE_audioPlayer`](projects/JUCE_audioPlayer) | Audio file player. |
| [`JUCE_delay`](projects/JUCE_delay) | Delay effect. |
| [`JUCE_spectrogram`](projects/JUCE_spectrogram) | Spectrogram visualizer for audio signals. |
| [`JUCE_mySimpleEQ1`](projects/JUCE_mySimpleEQ1) | First simple equalizer experiment. |
| [`JUCE_SimpleEQ_HighPass_LowPass_PeakFilter`](projects/JUCE_SimpleEQ_HighPass_LowPass_PeakFilter) | EQ with high-pass, low-pass and peak (bell) filters. |
| [`JUCE_SimpleEQ_Compressor_HighPass_LowPass_PeakFilter`](projects/JUCE_SimpleEQ_Compressor_HighPass_LowPass_PeakFilter) | The same EQ with an added compressor. |

## Repository layout

```
JUCE_projects/
└── projects/
    ├── JUCE_audioPlayer/
    ├── JUCE_delay/
    ├── JUCE_spectrogram/
    ├── JUCE_mySimpleEQ1/
    ├── JUCE_SimpleEQ_HighPass_LowPass_PeakFilter/
    └── JUCE_SimpleEQ_Compressor_HighPass_LowPass_PeakFilter/
```

Each project folder contains a `Source/` directory with the C++ code and a Projucer project file (`.jucer`).

## Getting started

### Requirements

- [JUCE](https://juce.com/get-juce/) (includes the Projucer)
- A C++ toolchain: Visual Studio on Windows, Xcode on macOS, or a Linux build setup

### Build a project

1. Clone the repository:
   ```bash
   git clone https://github.com/didemugur/JUCE_projects.git
   ```
2. Open the `.jucer` file of the project you want in the Projucer.
3. Check that the JUCE module paths in the Projucer point to your local JUCE installation.
4. Click **Save and Open in IDE**, then build and run from Visual Studio / Xcode.

> The projects were moved into subfolders when they were merged into this repository. If the Projucer reports missing modules, update the module paths and re-save the project.

## Repository history

These projects were originally separate repositories and were merged here with `git subtree`, so the full commit history of each one is preserved. The original repositories are archived.

## Author

[@didemugur](https://github.com/didemugur)
