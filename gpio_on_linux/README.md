# Readme

This example was tested on 

- Raspberry Pi OS 32-bit and 64-bit Raspberry Zero W, Raspberry Zero W 2, Raspberry Pi 3A/B, Raspberry Pi 4B.
- [Seeed Studio BeagleBone Green Wireless](https://wiki.seeedstudio.com/BeagleBone_Green_Wireless/) with the Grove Base Shield v2.0](https://wiki.seeedstudio.com/Grove_Base_Cape_for_BeagleBone_v2/) and Grove LED and Button modules.

## Setup

| Grove Module   | Location   |
|---|---|
| Red LED | GPIO_51  |
| Green LED | GPIO_50 |
| Blue LED | GPIO_117 |
| Button | GPIO_115 |

## References

- [gpiod documentation](https://libgpiod-dlang.dpldocs.info/gpiod.html)
- [GPIO Programming: Exploring the libgpiod Library](https://www.ics.com/blog/gpio-programming-exploring-libgpiod-library)
- [Control GPIO using the new Linux user space GPIO API](https://blog.lxsang.me/post/id/33)
- [How To Install libgpiod-dev on Ubuntu 20.04](https://installati.one/ubuntu/20.04/libgpiod-dev/)

## Library support

```bash
sudo apt -y install libgpiod-dev gpiod
```
   
## Raspberry Pi Notes:

- All GPIOs are on GPIO Chip 0

## Beaglebone GPIO notes.

- [Beaglebone 101 Information on GPIO pins](https://beagleboard.org/support/bone101)
- 4 x gpio chips with 32 lines.
- To calculate the GPIO chip number and line number
  - Example, take the GPIO Number of 49.
  - Chip number = (49 / 32) = chip number = 1 , line number = 49 % 32 = 17

## EdgeDevX

- Refer to the [EdgeDevX and Azure Sphere RemoteX wiki](https://github.com/Azure-Sphere-DevX/EdgeDevX.Examples/wiki) for documentation and introduction videos.

- Refer to the [Getting started with Azure Sphere DevX and RemoteX Examples](https://github.com/Azure-Sphere-DevX/EdgeDevX.Examples/wiki/05-Getting-started-examples) wiki for documentation and introduction videos.

## Demo

The code example shows how to work with GPIO on Linux.
