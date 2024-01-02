## Overview

LED effect generator for small mobile blinky projects.

## Getting started

This project can be built for two targets, either a RP4020 microcontroller, or on windows for testing and development.

### Windows

#### Prerequisites

- [`make`](https://gnuwin32.sourceforge.net/packages/make.htm) (Easily installed to windows by installing [chocolatey](https://chocolatey.org/install) and running `choco install make`)
- [`gcc`](https://community.chocolatey.org/packages/mingw) (Easily installed to windows by installing [chocolatey](https://chocolatey.org/install) and running `choco install mingw`)
- [`Python 3.12.1`](https://www.python.org/downloads/)

#### Instructions

1. Start with prerequisites installed and run all commands from the repository root.
1. Install pipx which will manage an isolated poetry installation with `pip install pipx`.
1. Install poetry to manage project dependencies with `pipx install poetry`.
    - If you get a warning about your path, run `pipx ensurepath` and restart your terminal.
1. Run `poetry install`
1. Run `poetry run sim` to build and run the test program from the 2D grid simulator.

### RP2040

#### Prerequisites

- [`Arduino`](https://www.arduino.cc/en/software) (or some other way of programming your hardware)
- [`Python 3.12.1`](https://www.python.org/downloads/)

#### Instructions

1. Compile and upload `wings3.ino` to the microcontroller

