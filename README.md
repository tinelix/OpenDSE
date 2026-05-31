# Open Digital Sound Engine

FOSS cross-platform audio library and API for Windows and Linux, including legacy systems.

## Main Features

* PCM samples playback
  supports local WAV files only yet
  
* Lightweight in use
  depending on the operating system, OpenDSE consumes on average from 400 to 2 MB of RAM

* Multiple frontends
  supports WASAPIv2 and WaveOut for Windows and ALSA for Linux

* Backward compatibility
  Windows NT 3.1* or above, Linux 2.4 with ALSA 0.9.1** or above is required
  
* RMS value for each frame
  allows developers to create simple sound visualizations
  
#####  * if the library is compiled in Microsoft Visual C++ 2.0
##### ** tested in SUSE Linux 8.1

## Building

### Windows NT and 9x, including Windows XP and newer

To use OpenDSE in Microsoft Visual C++, run the BAT script in the VS Command Prompt profile:
    
```bat
cd [OpenDSE root]\build
make
```

Windows NT 4.0 and below do not support interactive variable input. To build with debug symbols on these systems, specify the following arguments to the script:

```bat
make vc2 y
```
...and then use OpenDSE.lib in your projects to import OpenDSE.

### Linux

To use OpenDSE in GCC, run the Shell script in your terminal:

```sh
cd [OpenDSE root]/build
chmod +x ./make.sh                        # if the *.sh script does not run
./make.sh --prefix /usr/local/lib
```

...and then use libopendse.a in your projects to import OpenDSE.

For extended support of legacy Linux distributions, a special flag `--enable-legacy-support` is provided, as well as `--enable-debug` for building the library with debug symbols.

## Using OpenDSE in your own programs

Use the `opendse.h` C/C++ header file from [`include` directory](https://github.com/tinelix/OpenDSE), after copying all the necessary header files to the root of your project.

## License

OpenDSE licensed under [BSD 3-Clause License](https://github.com/tinelix/OpenDSE?tab=BSD-3-Clause-1-ov-file). Absolutely free and accessible to **everyone**: FOSS developers, businesses, and hobbyists, which is something [BASS with an alternative implementation](https://www.un4seen.com/doc/#bass/bass.html) certainly cannot boast.
