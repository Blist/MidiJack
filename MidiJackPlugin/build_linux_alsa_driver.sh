#!/bin/sh
g++ -fPIC -shared -Wall -D__LINUX_ALSA__ -o ../Assets/MidiJack/Plugins/x64/MidiJackPlugin.so PluginEntry.cpp RtMidi.cpp -lasound -lpthread -std=c++11
