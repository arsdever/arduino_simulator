@echo off
set outputdir=%cd%/build/
set srcdir=%cd%/src/
set wkdir=%cd%/wkdir/
devenv ArduinoSimulator.sln /projectconfig "debug|x64"