#!/bin/sh
g++ -fPIC -shared -Wall -D__LINUX_JACK__ -o ../Assets/MidiJack/Plugins/x64/MidiJackPlugin.so PluginEntry.cpp RtMidi.cpp -ljack -std=c++11
