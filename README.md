# Introduction
This project is a STM32 microcontroller Commodore 64 SID file format player. The microcontroller application emulates the MOS Technology SID 6581/8580 chip, which is responsible for audio generation on the Commodore 64 computers. Nostalgia, fueled by the distinct sound style produced by this chip contributed greatly to the rise of the [chiptune](https://en.wikipedia.org/wiki/Chiptune) music scene.

A great source of SID music files is the High Voltage SID Collection at: http://www.hvsc.c64.org/

For more info about the MOS SID chip and SID files in general, visit the following websites:
* https://en.wikipedia.org/wiki/MOS_Technology_SID
* http://www.hvsc.c64.org/download/C64Music/DOCUMENTS/SID_file_format.txt

# More info
The SID chip emulation engine (TinySID) has been borrowed from the open-source [Rockbox](http://www.rockbox.org/) codec library. Many thanks and kudos to the Rockbox team for making the code quite reusable.

GUI engine is based on the SEGGER STemWin library and it is included as a GCC *.a lib in the repository.  

# Board support
Currently, only the STM32F746G-DISCO evaluation board is supported. However, since the code relies heavily on the STM32Cube drivers and BSP, porting to other boards (with the appropriate hardware) should not be a problem.

# Prerequisites
* a GNU-compatible ARM toolchain (`arm-none-eabi-*`),
* Python 2.7 or newer,
* CMake 3.3 or newer,
* make.

Before building, make sure that all of the tools mentioned above are accessible globally in your system (i.e. through the PATH variable).

# Building
```
python create_proj.py
cd build
make all
```

# Usage
Simply put your favorite SID files on a microSD card, run the app and you are ready to go!

# Known problems and limitations
* Only the PlaySID (PSID) file format is supported. Attempts to play Real C64 SID (RSID) files usually cause the app to lock up.
* A nasty bug in early Cortex-M7 core revisions makes it almost impossible to single-step during debugging when interrupts are enabled. For more info, visit: http://www.keil.com/support/docs/3778.htm
