CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(razernaga)
FIND_PACKAGE(OpenCV REQUIRED)


SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_C_COMPILER "gcc-5")
SET(CMAKE_CXX_COMPILER "g++-5")

#FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
#INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})

ADD_EXECUTABLE(skylake
		Driver/client.cpp
		Base/BaseDriver.h
		Base/CarControl.cpp Base/CarControl.h
		Base/CarState.cpp Base/CarState.h
		Driver/SkylakeDriver.cpp Driver/SkylakeDriver.h
		Base/SimpleParser.cpp Base/SimpleParser.h
		Driver/SkylakeConsts.h
		Driver/Kinematics.cpp Driver/Kinematics.h
		Driver/Map.cpp Driver/Map.h
		Driver/MapSDL2.cpp Driver/MapSDL2.h
		Base/WrapperBaseDriver.cpp Base/WrapperBaseDriver.h)
TARGET_LINK_LIBRARIES(skylake ${SDL2_LIBRARIES} ${OpenCV_LIBS}) #${QT_LIBRARIES}
SET_TARGET_PROPERTIES(skylake PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")

ADD_DEFINITIONS(-DDEBUG)