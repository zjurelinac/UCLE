# UCLE
## _Ultimate Computer Learning Environment_

Some short and catchy intro

## Idea
Say a few things about the idea

## Features
List all the features you'd like to have one day

## Manual build & usage

### Building the project

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

### Using UCLE CLI tools

UCLE project offers you several CLI tools for various use-cases.

**Fnsim**, a functional processor simulator, is one of them. It can be used for debugging and/or assembly code correctness testing, for any of the supported processor architectures (so far only FRISC). It can be started with
```
./build/core/debug/fnsim-cli PROC_ARCH PFILE_PATH [-ijrx] [check CHECKS...]
```
and it supports various modes of simulation execution.

#### Basic mode
TODO

#### Interactive mode
TODO

#### JSON-controlled mode
TODO

#### Execution-tracking mode
TODO

#### Checker mode
TODO

#### Generic option flags
TODO

## Components
Explain which components make up the UCLE

## Status
Say how far the implementation has come

## Support
None yet.
