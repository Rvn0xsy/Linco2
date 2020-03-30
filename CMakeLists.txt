cmake_minimum_required(VERSION 3.15)
PROJECT(beacon)
INCLUDE_DIRECTORIES(src/include)
ADD_EXECUTABLE(${PROJECT_NAME} src/Beacon.cpp src/b64.c)