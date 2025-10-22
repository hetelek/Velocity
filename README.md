Velocity
========

**This repository is not regularly monitored and active development ceased quite some time ago. Please do not expect issues or PRs to be actioned on.**

## Important: Administrator Privileges Required (Windows)

On **Windows**, Velocity requires **Administrator privileges** to access Xbox 360 drives:

- **Right-click** `Velocity.exe` → Select **"Run as administrator"**
- This is necessary for detecting and browsing Xbox 360 hard drives and USB devices
- Without Administrator rights, drives will **not be detected** in the Device Viewer

See [BUILD.md](BUILD.md#running-velocity) for Linux/macOS requirements and more details.


About
-----
Developed mainly by Stevie Hetelekides(Hetelek) and Adam Spindler(Experiment5X). For a full list of the contributors of Velocity, view the [contributors graph](https://github.com/hetelek/Velocity/graphs/contributors).

Velocity is a cross-platform application built using the Qt framework, that allows you to browse and edit Xbox 360 files. Velocity's backend is handled by the XboxInternals library, which currently supports the following Xbox file formats:

- Xbox-specific cryptography
- XDBF
- YTGR
- FATX
- STFS
- Xbox disc formats
  - SVOD
  - GDFX

Other individual files specific to the Xbox 360 are supported, including the Account file and other minor ones.

Licensing Information
---------------------
Velocity is licensed and distributed under the GNU General Public License (v3).

Velocity is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

If no copy of the GNU General Public License was received with this program (FILE: COPYING), it is available at <http://www.gnu.org/licenses/>.
