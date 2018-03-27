cmake_minimum_required(VERSION 3.9)
project(phi_miner)

set(CMAKE_CXX_STANDARD 11)

add_executable(phi_miner
        forms/miningdialog.ui
        forms/settingsdialog.ui
        forms/ui_miningdialog.h
        forms/ui_settingsdialog.h
        scripts/build-linux.sh
        scripts/start.sh
        scripts/start00.sh
        CMakeLists.txt
        customlistmodel.cpp
        customlistmodel.h
        device_info.cu
        device_info.h
        guiutil.cpp
        guiutil.h
        LICENSE
        main.cpp
        mainwindow.cpp
        mainwindow.h
        miningdialog.cpp
        miningdialog.h
        particle.cu
        particle.h
        randomize.cpp
        randomize.h
        README.md
        settingsdialog.cpp
        settingsdialog.h
        test.cu
        v3.cu
        v3.h)
