# Custom OS

## Background

Based on [this](https://www.youtube.com/watch?v=FkrpUaGThTQ&list=PLZQftyCk7_SeZRitx5MjBKzTtvk0pHMtp&index=1) tutorial by CodePulse and [this](https://github.com/pritamzope/OS/tree/master/Kernel/Keyboard) project by pritamzope.

## Setup

Build image for docker env:

- `docker build docker -t os-buildenv`

## Build

- Linux or MacOS: `docker run --rm -it -v "$pwd":/root/env os-buildenv`
- Windows (cmd): `docker run --rm -it -v "%cd%":/root/env os-buildenv`
- Windows (Powershell): `docker run --rm -it -v "${pwd}:/root/env" os-buildenv`

Build for x86:

- `node Makefile.js`

to leave the build environment type `exit`

## Emulate

Emulate the OS using Qemu

- `qemu-system-x86_64 -cdrom dist/kernel.iso`

Use this one if you have added to path and the previous doesn't work

- `"C:\Program Files\qemu\qemu-system-x86_64.exe" -cdrom dist/kernel.iso`
