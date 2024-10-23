# Vector Chart Object Search  Plugin for OpenCPN

This plugin provides a search interface to the objects on the vector charts in OpenCPN. Note that it indexes the objects on the charts which were loaded at least once with the plugin enabled.

[![Hosted By: Cloudsmith](https://img.shields.io/badge/OSS%20hosting%20by-cloudsmith-blue?logo=cloudsmith&style=flat-square)](https://cloudsmith.com)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/3039/badge.svg)](https://scan.coverity.com/projects/nohal-objsearch_pi)

## Building

This assumes you have already seen the building instructions at http://opencpn.org/ocpn/compiling_source_linux or http://opencpn.org/ocpn/compiling_source_windows and can build OpenCPN from source.

### Linux:

```
git clone --recurse-submodules https://github.com/nohal/objsearch_pi.git
cd objsearch_pi
mkdir build
cd build
cmake ..
make
```
### Windows:

```
git clone --recurse-submodules https://github.com/nohal/objsearch_pi.git
cd objsearch_pi
mkdir build
cd build
cmake ..
cmake --build . --config release
```

## CSV Data Import

The format of the CSV is simple:
```Lat, Lon, Object name, Feature name,"Source" name[[,Scale],PPM Scale]```
Strings containing commas of course have to be quoted, the scale fields are optional

## Contributing

### General

The project is developed in C++, language features up to C++17 can be used.
Please format your code using `clang-format` before submitting pull requests, they are checked for compliance with the `.clang-format` file used by the project during the CI workflow and can't be accepted if the test is not passing.
To make this as simple as possible, a [pre-commit](https://pre-commit.com) configuration is provided in the project, which activates a Git pre-commit hook taking care of the formatting automatically. To use it run the following commands:

```bash
pip3 install pre-commit
cd <plugin source directory>
pre-commit install
```

#### pre-commit on Windows

Use [WSL](https://learn.microsoft.com/en-us/windows/wsl/) for pre-commit and save yourself the headache.
