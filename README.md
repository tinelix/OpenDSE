# Open Digital Sound Engine

FOSS cross-platform audio library and API for Windows and Linux, including legacy systems.

## Main Features

* **PCM samples playback** \
  supports local WAV files only yet
  
* **Lightweight in use** \
  depending on the operating system, OpenDSE consumes on average from 400 to 2 MB of RAM

* **Multiple audio frontends** \
  supports WASAPIv2 and WaveOut for Windows and ALSA for Linux

* **Backward compatibility** \
  Windows NT 3.1* or above, Linux 2.4 with ALSA 0.9.1** or above is required
  
* **RMS value for each frame** \
  allows developers to create simple sound visualizations
  
<sub>* if the library is compiled in Microsoft Visual C++ 2.0
<br>** tested in SUSE Linux 8.1</sub>

## Building

### Windows NT and 9x

_Mininal target:_ Windows NT 3.1 with Microsoft Visual C++ 2.0

To use OpenDSE in Microsoft Visual C++, run the BAT script in the VS Command Prompt profile:
    
```bat
cd [OpenDSE root]\build
make
```

Windows NT 4.0 and below do not support interactive variable input. To build with debug symbols on these systems, specify the following arguments to the script:

```bat
make vc2 y
```
...and then use `OpenDSE.lib` from `out/bin` in your projects to import OpenDSE.

### Linux

_Mininal target:_ GCC 3.2 with glibc 2.0 and libasound2 0.9.1+

To use OpenDSE in GCC, you need to follow several steps:
    
1. Install the `libasound2` or `libasound1` development package from your distribution;

```sh
# Ubuntu / Debian
sudo apt-get install libasound2-dev
# openSUSE / SUSE Linux
sudo zypper install alsa-devel # or install from YaST2
# Arch/Artix Linux
sudo pacman -S alsa-lib
# Gentoo Linux
sudo emerge media-libs/alsa-lib
```

2. Run the Shell script in your terminal:

```sh
cd [OpenDSE root]/build
chmod +x ./make.sh                        # if the *.sh script does not run
./make.sh --prefix /usr/local/lib
```

3. Use `libopendse.a` from `out/bin` in your projects to import OpenDSE.

For extended support of legacy Linux distributions, a special flag `--enable-legacy-support` is provided, as well as `--enable-debug` for building the library with debug symbols.

## Using OpenDSE in your own programs

Use the `opendse.h` C/C++ header file from [`include` directory](https://github.com/tinelix/OpenDSE), after copying all the necessary header files to the root of your project.

## License

**OpenDSE licensed under [BSD 3-Clause License](https://github.com/tinelix/OpenDSE?tab=BSD-3-Clause-1-ov-file).**

Absolutely free and accessible to **everyone**: FOSS developers, businesses, and hobbyists, which is something [BASS with an alternative implementation](https://www.un4seen.com/doc/#bass/bass.html) certainly cannot boast.
