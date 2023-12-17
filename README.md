## Overview

LED effect generator for small mobile blinky projects.

## Getting started

This project can be built for two targets, either an RP4020 microcontroller, or on windows for testing and development.

### RP2040

#### Prerequisites

- [`Arduino`](https://www.arduino.cc/en/software) (or some other way of programming your hardware)
- [`Python 3.11`](https://www.python.org/downloads/release/python-3110/)

#### Instructions

1. Compile and upload `wings3.ino` to the microcontroller

### Windows

#### Prerequisites

- [`make`](https://gnuwin32.sourceforge.net/packages/make.htm)
- [`gcc`](https://community.chocolatey.org/packages/mingw)
- [`Python 3.11`](https://www.python.org/downloads/release/python-3110/)

#### Instructions

1. Open a terminal and open this repository's root directory
1. Run `python .\run.py` to build and run the test program from the 2D grid simulator.
