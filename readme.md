# rpihal-system-test

> This repo uses submodules, so clone with `git clone --recurse-submodules https://github.com/oblaser/rpihal-system-test.git`

This is (well, will be) the complete system test of [rpihal](https://github.com/oblaser/rpihal).
It is also an example/template for projects using rpihal.


## Build and Run
### Build
```sh
cd build
./build.sh cmake make
```
> Do not build the debug version (`cmaked`), it may have some options enabled by default.

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
| `app`  | Run the demo application after the tests have succeeded. |


## Hardware

The test hardware is a custom board. [Schematic](https://static.oblaser.ch/rpihal/rpihal-test-board-rev00_SCM.pdf)

Add to `/boot/firmware/config.txt`:
```ini
dtoverlay=spi0-1cs
dtoverlay=i2c1
```
