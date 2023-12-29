# ScriptPad Firmware:

## Firmware Build instructions:

This instructions are focus on Ubuntu or Windows WSl enviorement, but you can build on any OS, due to it's a PICO-SDK project.

### Dependencies:

```
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib build-essentials
```

### Build:

Download submodules:
```
git submodule update --recursive --init
```

Execute build script
```
./buildProject.sh

```