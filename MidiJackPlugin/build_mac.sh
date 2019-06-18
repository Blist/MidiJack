#!/bin/sh
g++ -fPIC -shared -Wall -D__MACOSX_CORE__ -o ../Assets/MidiJack/Plugins/MidiJackPlugin.bundle PluginEntry.cpp RtMidi.cpp -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
