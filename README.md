# UCLE
## _Ultimate Computer Learning Environment_

- Ever wanted to design **your own processor architecture** or **play around** with an **existing one**?
- Ever wondered how the computer **really thinks** and **does** it's **job**?
- Did you perhaps wish one day to make **your own operating system** or a **programming language**?
- Maybe you even tried to understand how does the **compiler actually work**, but couldn't because it was way too complex?

*Don't loose hope and look no further!* - **UCLE is meant to answer all of that.**

## Idea
The idea of **UCLE** is precisely this - to allow you, whoever you might be (a student, a programmer, a computer engineer, a professor, or just someone interested in computers), to **understand** and **experience first-hand** the depths of **how it all works together** inside of a computer:

- the **processor**, **IO devices** and other **hardware components**
- the **operating system** with all it's features and modules
- **programming languages**, **algorithms** and **data structures**
- the **compiler**, **assembler**, **linker** and **loader**
- **communication protocols**
- **disk filesystems**
- **multimedia algorithms**

and many, many more.

It is meant to **show** you all these things **in action** and with lots of details, but also to allow you to **experiment** with them, to change and extend some/all of the components, or even **design whole new systems** in a way you see most appropriate for your purposes, be it **learning**, **teaching**, **prototyping**, or just plain old **fun**.


## Features
List all the features you'd like to have one day

## Building the project

UCLE project uses [SCons](http://scons.org/) build tool for building all it's C++-based components. SCons requires a Python runtime, and can be installed via Python-pip, using the following command:
```
pip install scons --wheel
```
(wheel flag is currently neccessary because installation without it encounters some errors).

Once SCons is installed, this repository should be cloned with
```
git clone https://github.com/zjurelinac/UCLE.git
```
(or it can be downloaded it as zip and then extracted).

Once you have the repository, go to `ucle` folder and run `scons`, and wait for the project to build.

## Using UCLE CLI tools

UCLE project offers you several CLI tools for various use-cases.

### FNSIM

**Fnsim**, a functional processor simulator, is one of them. It can be used for debugging and/or assembly code correctness testing, for any of the supported processor architectures (so far only FRISC). It can be started with
```
./build/core/debug/fnsim-cli PROC_ARCH PFILE_PATH [-ijrx] [check CHECKS...]
```
and it supports various modes of simulation execution.

#### Basic mode
#### Interactive mode
#### JSON-controlled mode
#### Execution-tracking mode
#### Checker 
#### Generic option flags

### ASM

### LNK

## Components
Explain which components make up the UCLE

## Status
Say how far the implementation has come

## Support
None yet.
