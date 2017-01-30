ADD_EXECUTABLE(skylake
        driver/client.cpp
        core/BaseDriver.h
        core/CarControl.cpp core/CarControl.h
        core/CarState.cpp core/CarState.h
        driver/SkylakeDriver.cpp driver/SkylakeDriver.h
        core/SimpleParser.cpp core/SimpleParser.h
        driver/SkylakeConsts.h
        driver/Kinematics.cpp driver/Kinematics.h
        driver/Map.cpp driver/Map.h
        driver/MapSDL2.cpp driver/MapSDL2.h
        core/WrapperBaseDriver.cpp core/WrapperBaseDriver.h)
TARGET_LINK_LIBRARIES(skylake ${SDL2_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(skylake PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")