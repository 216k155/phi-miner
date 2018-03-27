//
// Created by 216k155 on 2/20/18.
//

#ifndef CMAKE_AND_CUDA_DEVICE_INFO_H
#define CMAKE_AND_CUDA_DEVICE_INFO_H

#include <iostream>
//#include<bits/stdc++.h>
#include <vector>
#include <string>

void printDeviceProps();
int getDeviceCount();
std::vector <std::string> getPropsOfIDevice(int i);
std::vector<int> getMinableDevices();

#endif //CMAKE_AND_CUDA_DEVICE_INFO_H
