# wisblock-lorawan
Enable LoRaWAN communications on a RAK11310 WisBlock Core module. this module is based on a RAspberry Pi RP2040 processor and Semtech SX1262 LoRa transceiver. This software is based on the pico-lorawan project (https://github.com/sandeepmistry/pico-lorawan.git and https://github.com/siuwahzhong/lorawan-library-for-pico) which in turn are based on the Semtech's [LoRaWAN end-device stack implementation and example projects](https://github.com/Lora-net/LoRaMac-node).

The main addition to these libraries is a modification to store the LoRa dev-nonce in EEPROM on the RAK12002 RTC. The LoRaWan specification indicates that a device can only use a dev-nonce once. If the device powers down and needs to re-establish a connection to a LoRa Gateway it either needs to start at 1 and step through each value until one that has not been used is found, or it needs to store the last dev-nonce value that was used and increment that value. It is this latter more efficient approach that is implemented ehre.

## Hardware

 * RAK11310 WisBlock Core module (https://docs.rakwireless.com/Product-Categories/WisBlock/RAK11310/Datasheet/#overview)
 * RAK19001 WisBlock Dual IO Base Board (https://docs.rakwireless.com/Product-Categories/WisBlock/RAK19001/Datasheet/#wisblock-dual-io-base-board-overview)
 * RAK12002 WisBlock RTC Module Micro Crystal RV-3028-C7 (https://docs.rakwireless.com/Product-Categories/WisBlock/RAK12002/Datasheet/)


## Examples

See [examples](examples/) folder.

There is a `config.h` file to your ABP or OTAA node configuration for each example.

## Cloning

```sh
git clone --recurse-submodules https://github.com/m-nahirny/wisblock-lorawan.git 
```

## Building

1. [Set up the Pico C/C++ SDK](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
2. Set `PICO_SDK_PATH`
```sh
export PICO_SDK_PATH=/path/to/pico-sdk
```
3. Create `build` dir, run `cmake` and `make`:
```
mkdir build
cd build
cmake .. -DPICO_BOARD=pico
make
```
4. Copy example `.uf2` to RAK11310 when in BOOT mode.

