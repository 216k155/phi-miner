#!/usr/bin/env bash
#App is in directory /build (./particle_test)
#cuda (ver 9.1) + Qt should be installed in the system for compilation as well as ccminer should be installed for running App and written in the PATH
cd ../build
cmake ..
make -j4
cd ../scripts