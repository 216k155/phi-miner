# Miner-ui based on tpruvot's ccminer by 216k155
------------------------------------------------
 
Miner-ui is an UI development based on ccminer. This version is for miners community who want the fastest and the simple interface. Check out the new miner-ui to mine the most profitable cryptocurrency with your GPU.

# Build
-------

1. Install:

а) Microsoft Visual Studio 2015 (do not forget to install C++ in Programming Languages,
because it’s not installed by default)

b) Windows 8.1 SDK

c) CUDA 9.1

d) Qt5 (choose for compiler msvc 2015 х64)

e) Cmake

f) ccminer

2. Add to PATH variable of the environment by the analogy with the following paths:

а) C:\Users\user\Downloads\ccminer-x64-2.2.4-cuda9

b) C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v9.1\bin

3. Change paths of CMAKE_CUDA_COMPILER & CMAKE_PREFIX_PATH in

CMakeLists.txt file to the corresponding for your OS

4. Open cmake.

As a source code choose cmake_cuda project directory.

As a directory for build choose build folder inside the project

Press Configure and choose MSVC 2015, win64. Press OK button

5. Press Generate

6. Choose in CMake @Open Project@

7. Choose Release, x64.

8. Create the project build - ALL_BUILD

9. Copy the following files from an analogous folder - C:\Qt\5.10.1\msvc2015_64\bin to the

folder - cmake_cuda/build/Release

а) Qt5Core.dll

b) Qt5Gui.dll

c) Qt5Widgets.dll

10. Done. Now you can run - build/Release/particle_test.exe
