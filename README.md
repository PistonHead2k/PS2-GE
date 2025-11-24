# PS2-GE
PS2 Game Engine

<div align="center">

<img src="https://logo.png" alt="PS2-GE" width="600" />

[![Release](https://img.shields.io/github/release/noisymime/speeduino.svg)](https://github.com/noisymime/speeduino/releases/latest)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](https://github.com/noisymime/speeduino/blob/master/LICENSE)
[![Build Status](https://img.shields.io/github/actions/workflow/status/noisymime/speeduino/build-firmware.yml?label=Build%20Status&branch=master)](https://github.com/noisymime/speeduino/actions/workflows/build-firmware.yml)
[![Unit Tests](https://img.shields.io/github/actions/workflow/status/noisymime/speeduino/unit-tests.yml?label=Unit%20Tests&branch=master)](https://github.com/noisymime/speeduino/actions/workflows/unit-tests.yml)
![badge](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/noisymime/d8a449a3f6d3307dab457431512502f9/raw/misra_results.json)
[![Sponsors](https://img.shields.io/github/sponsors/noisymime)](https://github.com/sponsors/noisymime)
[![GitHub commits](https://img.shields.io/github/commits-since/noisymime/speeduino/202310.svg)](https://github.com/noisymime/speeduino/compare/202310...master)
[![https://img.shields.io/discord/879495735912071269 ](https://img.shields.io/discord/879495735912071269?label=Discord&logo=Discord)](https://discord.gg/YWCEexaNDe)

##### Free and low overhead PS2 game engine
</div>


## PS2-GE
Game Engine for the PS2, currently doing a demo game to test its functionality

## Current Status

- [x] Pushing Cube to GS
- [x] Input Handler
- [ ] LookAt Transformation
- [ ] True Asset Importer
- [ ] Texturing
- [ ] Using Vector Units for Linear Transformations

## Documentation
-

## How to play
Real PS2: Download the release ISO, burn it on a disc, and you can use FreeMCBoot to load it.


PC: Download PCSX2 or any other PS2 emulator and load the iso.

## For Devs
VSCode: I use the linux subsystem with gcc to compile (Make sure to have ps2dev library)
PCSX2: Make sure to add it to the Path enviroment variable, press F5 in vscode to have it run.

## Dependencys:
PS2SDK - PS2DEV - Makefile - Linux Subsystem or Bash

