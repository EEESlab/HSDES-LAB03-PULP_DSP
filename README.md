This is a basic exercise for the Hardware-Software Design of Embedded Systems class (a.y. 2019-2020).

# Instructions
## Prerequisites
These instructions do not make many assumptions on your knowledge of a Linux command line, but you should know your way around a bit.
In particular, you should know how to open a terminal and have a (minimal!) confidence with applications based on a command line interface.
In case the sentence "open a terminal" already brings you to total confusion, here you can find a Linux command line tutorial for absolute beginners:
https://ubuntu.com/tutorials/command-line-for-beginners

## PULPissimo
PULPissimo, which will be presented during the lecture, is a single-core PULP system, with one RISC-V core augmented with digital signal processing extensions to the ISA.
PULPissimo is fully open-source, including all the RTL code (written in SystemVerilog), FPGA synthesis scripts, the high-level simulator we use in this exercise, and the software running on it.
You can get a look at PULPissimo here: https://github.com/pulp-platform/pulpissimo
For this simple exercise, you will not need to download the full platform: what you need is already available in your *pulp-box* virtual machine.

## The SDK
The PULP Software Development Kit (SDK) is the main tool that we will use during this exercise. It will be configured to target PULPissimo, the single-core MCU platform that will be presented during the lecture.
The *pulp-box* virtual platform comes with the SDK preinstalled; you can find more information about the SDK here if you are interested: https://github.com/pulp-platform/pulp-sdk . 
We will use mainly two components of the SDK:
- APIs like `pulp-rt`, which provides a relatively high-level access to the software ecosystem; and `hal`, which gives lower level access to the hardware
- a simulator (virtual platform) called *GVSOC*. GVSOC is an open-source project that enables to simulate PULP-based chips directly on your computer, without the complexity of running a full RTL platform simulation.

Although you can find some documentation in your local PULP SDK (in `/pulp/pkg/sdk/2020.01.01/doc/sdk/index.html` and `/pulp/pkg/sdk/2020.01.01/doc/runtime/pulpissimo/index.html`), one of the best sources for SDK documentation are from the site of GreenWaves Technologies - a company that makes a commercial product based on the PULP platform.
You can find the most useful information here https://greenwaves-technologies.com/manuals/BUILD/PULP-OS/html/index.html 

## The example: a 2D convolution filter
The example is a very basic 2D convolution filter run over a 64x64 image.
Variants of this kernel are a very common basic block of traditional image processing and compression algorithms -- but also of state-of-the-art Artificial Intelligence, in particular deep neural networks.
The code is organized in four files: 
- `convolution.c` includes the `main` function of the tests and various checks to ensure it works correctly
- `conv_kernel.c` is the actual convolution kernel;
- `conv_kernel.h` is the header of `conv_kernel.c`, used to include its function.
- `data.h` includes the input data and filter.


## Running the example
Enough with basic information: let's make this example work.
To use the SDK, you must open a terminal and make the SDK available in that terminal -- an operation called *sourcing* the SDK.
You can do it in this way in your *pulp-box*; just open a terminal and type:
```
source /pulp/sourceme.sh
```
You should see the following output:
```
Configuring PULP SDK at: /pulp/pkg/sdk/2020.01.01
```
Now the SDK can be used in your setup. As a first thing, download this example in your home folder and enter it:
```
git clone https://github.com/hsdes_mcu_conv_naive
cd hsdes_mcu_conv_naive
```
Now you can try to build the example:
```
make clean all
```
A significant amount of text will follow, showing all compilation steps. At the end check if there is an Error or not (there shouldn't be!).
Then, you can run the experiment in GVSOC:
```
make run
```
The expected output is
```
errors=0
E
cycles=265621
```
