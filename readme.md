# rpihal-system-test

> This repo uses submodules, so clone with `git clone --recurse-submodules https://github.com/oblaser/rpihal-system-test.git`

This is the system test of [rpihal](https://github.com/oblaser/rpihal).
It is also an example/template for projects using rpihal.


## Build and Run
### Build
```sh
cd build
./build.sh cmake make [run]
```
> Do not build the debug version (`cmaked`), it may have some options enabled by default.
If the application is run directly (optional), it's run with [no options](#run).

### Run
```sh
cd cmake
./rpihal-system-test [options]
```
If the binary is run without any options, only the detected model is printed, no hardware related code is executed (can be run on any Pi with any hardware configuration).

#### Options
| Option | Description |
|:-------|:------------|
| `test` | system test enable, without further options no hardware related code is executed (can be run on any Pi with any hardware configuration) |
| `gpio` | needs `BTN0`, `BTN1`, `LED0` and `LED1` as in the [test hardware](#hardware) |
| `spi`  | needs a MCP3004 and a shift register as in the [test hardware](#hardware) |
| `i2c`  | needs a TMP1075DR as in the [test hardware](#hardware) |
| `all`  | `gpio`, `spi` and `i2c` |
| `app`  | run the demo application after the tests have succeeded |


## Demo Application

Button 0 cycles through the modes. Press and hold button 0 to exit the application. Button 1 has different functions depending on the active mode. Button 1 has no long press function.


## Hardware

The test hardware is a custom board. [Schematic](https://static.oblaser.ch/rpihal/rpihal-test-board-rev00_SCM.pdf)

Add to `/boot/firmware/config.txt`:
```
dtoverlay=spi0-1cs
dtoverlay=i2c1,i2c1_baudrate=400000
```
