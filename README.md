# tfmicro-on-litex-vexriscv
PoC running [tfmicro](https://www.tensorflow.org/lite/microcontrollers) on [LiteX](https://github.com/enjoy-digital/litex)/[VexRiscv](https://github.com/SpinalHDL/VexRiscv)

## Build and load LiteX SoC
Tested on [ULX3S](https://radiona.org/ulx3s/) with IS42S16160 RAM module

```
$> $(LITEX_DIR)/litex-boards/litex_boards/targets/ulx3s.py --device LFE5U-85F --cpu-variant imac --sdram-module IS42S16160 --build
```

The load the sythesized bitstream with your prefered tool. I use [openFPGALoader](https://github.com/trabucayre/openFPGALoader):
```
$> openFPGALoader -f build/ulx3s/gateware/ulx3s.bit
```

## Build and run the firmware
```
$> cd firmware/micro_mnist
$> make
```

Then load the built firmware onto the SoC using lxterm (provided with LiteX):

```
$> lxterm /dev/serial/by-id/usb-FER-RADIONA-EMARD_ULX3S_FPGA_85K_v3.0.8_K00303-if00-port0 --kernel=micro_mnist.bin


        __   _ __      _  __
       / /  (_) /____ | |/_/
      / /__/ / __/ -_)>  <
     /____/_/\__/\__/_/|_|
   Build your hardware, easily!

 (c) Copyright 2012-2020 Enjoy-Digital
 (c) Copyright 2007-2015 M-Labs

 BIOS built on Mar 31 2021 20:04:39
 BIOS CRC passed (c5e53a0f)

 Migen git sha1: 3ffd64c
 LiteX git sha1: cc02055b

--=============== SoC ==================--
CPU:		VexRiscv_IMAC @ 50MHz
BUS:		WISHBONE 32-bit @ 4GiB
CSR:		32-bit data
ROM:		64KiB
SRAM:		8KiB
L2:		8KiB
SDRAM:		32768KiB 16-bit @ 50MT/s (CL-2 CWL-2)

--========== Initialization ============--
Initializing SDRAM @0x40000000...
Switching SDRAM to software control.
Switching SDRAM to hardware control.
Memtest at 0x40000000 (2MiB)...
  Write: 0x40000000-0x40200000 2MiB     
   Read: 0x40000000-0x40200000 2MiB     
Memtest OK
Memspeed at 0x40000000 (2MiB)...
  Write speed: 10MiB/s
   Read speed: 10MiB/s

--============== Boot ==================--
Booting from serial...
Press Q or ESC to abort boot completely.
sL5DdSMmkekro
[LXTERM] Received firmware download request from the device.
[LXTERM] Uploading micro_mnist.bin to 0x40000000 (265500 bytes)...
[LXTERM] Upload complete (9.9KB/s).
[LXTERM] Booting the device.
[LXTERM] Done.
Executing booted program at 0x40000000

--============= Liftoff! ===============--
Micro MNIST: it's a 1, should be 1
CONV_2D took 6425608 ticks (128 ms).
MAX_POOL_2D took 826383 ticks (16 ms).
RESHAPE took 45501 ticks (0 ms).
FULLY_CONNECTED took 345974 ticks (6 ms).
Micro MNIST: it's a 0, should be 0
CONV_2D took 6451595 ticks (129 ms).
MAX_POOL_2D took 825314 ticks (16 ms).
RESHAPE took 45116 ticks (0 ms).
FULLY_CONNECTED took 345784 ticks (6 ms).
......
......
......
Micro MNIST: it's a 0, should be 0
CONV_2D took 6451298 ticks (129 ms).
MAX_POOL_2D took 825493 ticks (16 ms).
RESHAPE took 45139 ticks (0 ms).
FULLY_CONNECTED took 345893 ticks (6 ms).
Micro MNIST: it's a 6, should be 6
CONV_2D took 6451207 ticks (129 ms).
MAX_POOL_2D took 824769 ticks (16 ms).
RESHAPE took 45119 ticks (0 ms).
FULLY_CONNECTED took 345826 ticks (6 ms).
Micro MNIST: it's a 8, should be 8
CONV_2D took 6451389 ticks (129 ms).
MAX_POOL_2D took 825264 ticks (16 ms).
RESHAPE took 45105 ticks (0 ms).
FULLY_CONNECTED took 345853 ticks (6 ms).
Micro MNIST: matched=98
```

## Build test data
firmware/micro_mnist/scripts contains several scripts to train and evaluate the CNN model. There's also a script to gather 100 random images from the mnist test dataset and to write a binary representation of images and labels. This test data is used on the firmware to test the inference.

```
$> cd firmware/micro_mnist/scripts
$> python build_test_data.py
```
