# Da-Vinci Cuffs

A biosignal performance software to create gestures and regession models to interact with Ableton 9.2 or lower.

A third year Creative Computing Project created by Yours Truly.

## Why Ableton 9.2?
- Because this software interfaces with Ableton 9.2 via OSC and LiveOSC.
- You can read about this setup [here](https://livecontrol.q3f.org/ableton-liveapi/liveosc/).
- Download the plugin [here](https://github.com/dinchak/LiveOSC).
- Full list of commands [here](https://github.com/q-depot/LiveOSC/blob/master/OSCAPI.txt).

## OFX Addons:
- ofxAssimpModelLoader
- ofxDatGui
- ofxDmx
- ofxMyo - Requires a little setup
- ofxOsc
- ofxXmlSettings
- Rapid Mix API - Requires further setup

## Basic Setup
- Assuming here you have install and worked with [OFX](www.openFrameworks.cc) before.
- Install any ofx addons you do not have, listed above.
- Open the project in ofx Project Generator.

## ofxMyo and MyoFramework
- Download [myo connect](https://support.getmyo.com/hc/en-us/articles/202667496-Getting-starting-with-Myo-on-Mac-OS-X).
- Link Myo Framework (found in ofxMyo) to xCode [video tutorial here](https://www.youtube.com/watch?v=tAUayhchkm0).


## Finally! Rapid Mix API Installation

Download the full RapidMix API folder and store in libs folder within this project. You may need to compile your own version of the library into a Dynamic Link Library.

- Follow instructions [here](https://www.doc.gold.ac.uk/eavi/rapidmixapi.com/index.php/getting-started/download-and-setup/) also look on page for further rapid mix funcationality
- You may also have issues compileing due to some templating typename things. If you do during the 'make' command in compilation then you will need to adjust the source code of the library.
- Go to RAPID-MIX_API > dependencies > RapidLib >  seriesClassification.cpp - change both MinMax functions to '**typename seriesClassificationTemplate<T>::template minMax<T>**' and recompile.
- Drag in to Build Phrase -> Link to binaries the Dynamic link library you have now created.

### All changes within Targets under xCode Project
- Add to header search path : **$(PROJECT_DIR)/libs/RAPID_MIX-API/src/**
- Add to header search path : **$(PROJECT_DIR)/libs/RAPID_MIX-API/dependencies/** - Recursive
- Possibly delete from header search paths : src/inputs, src/machinelearning, src/output
- If you have issues with OSC Network Utils then : RAPID_MIX-API > dependencies/Maximillian and delete openFrameworks and any example tests as this clashes with ofxOSC you had added onto the project at the beginning.

# Run xcode and Bang you have a fully compiled project!

Enjoy! x TC

