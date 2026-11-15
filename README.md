# 3DS System Info — source project

This is a small Nintendo 3DS homebrew app source project.

## What it displays

- Nintendo 3DS model
- System version string
- FIRM version field
- SD card total, used, and free space
- Battery indicator level and charging/adapter state
- Wi-Fi signal strength
- 3D slider percentage

Controls:
- **A** refreshes the information.
- **B** exits to the Homebrew Launcher.

## Important: this is source code, not an installable CIA

I have not compiled this project in this environment. It does **not** contain a `.cia`, `.3dsx`, or FBI QR code yet.

To build it, a device or cloud build environment needs the official devkitPro `devkitARM` toolchain and libctru. The included Makefile is based on the official devkitPro 3DS application template and builds a `.3dsx` for Homebrew Launcher. Making a CIA requires additional packaging tools (such as `bannertool` and `makerom`) and CIA metadata.

A QR code for FBI Remote Install must point to a real, publicly reachable CIA download. A QR code cannot turn source code into a CIA.

## Build on a computer with devkitPro installed

From this folder, run:

```sh
make
```

The expected Homebrew Launcher output is `3DS_System_Info.3dsx`.

Official setup information:
- https://devkitpro.org/
- https://github.com/devkitPro/3ds-examples
- https://github.com/devkitPro/libctru

## Notes

- Battery information is shown using the 3DS system's battery indicator level, not a precise percentage.
- Luma3DS version is not displayed by this first version.
- No system settings are changed and no files are written to the SD card.
