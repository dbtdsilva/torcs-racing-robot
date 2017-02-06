ADD_EXECUTABLE(skylake
        core-common/BaseDriver.h
        core-common/CarControl.cpp core-common/CarControl.h
        core-common/WrapperBaseDriver.cpp core-common/WrapperBaseDriver.h
        core-common/CarState.cpp core-common/CarState.h
        core-common/SimpleParser.cpp core-common/SimpleParser.h
        core/SkylakeConsts.cpp
        core/SkylakeBaseDriver.cpp core/SkylakeBaseDriver.h
        driver/SkylakeDriver.cpp driver/SkylakeDriver.h
        driver/client.cpp
        driver/Kinematics.cpp driver/Kinematics.h
        driver/Map.cpp driver/Map.h
        driver/MapSDL2.cpp driver/MapSDL2.h)
TARGET_LINK_LIBRARIES(skylake ${SDL2_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(skylake PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")

ADD_EXECUTABLE(fuzzy
        core-common/BaseDriver.h
        core-common/CarControl.cpp core-common/CarControl.h
        core-common/WrapperBaseDriver.cpp core-common/WrapperBaseDriver.h
        core-common/CarState.cpp core-common/CarState.h
        core-common/SimpleParser.cpp core-common/SimpleParser.h
        core/SkylakeConsts.cpp
        core/SkylakeBaseDriver.cpp core/SkylakeBaseDriver.h
        fuzzy-driver/client.cpp
        fuzzy-driver/FuzzyDriver.cpp fuzzy-driver/FuzzyDriver.h )

TARGET_LINK_LIBRARIES(fuzzy ${FUZZY_LIBRARIES})
SET_TARGET_PROPERTIES(fuzzy PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")

ADD_EXECUTABLE(vfh-fuzzy
        core-common/BaseDriver.h
        core-common/CarControl.cpp core-common/CarControl.h
        core-common/WrapperBaseDriver.cpp core-common/WrapperBaseDriver.h
        core-common/CarState.cpp core-common/CarState.h
        core-common/SimpleParser.cpp core-common/SimpleParser.h
        core/SkylakeConsts.cpp
        core/SkylakeBaseDriver.cpp core/SkylakeBaseDriver.h
        vfh-fuzzy-driver/client.cpp
        vfh-fuzzy-driver/VFHFuzzyDriver.cpp vfh-fuzzy-driver/VFHFuzzyDriver.h)

TARGET_LINK_LIBRARIES(vfh-fuzzy ${FUZZY_LIBRARIES} ${SDL2_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(vfh-fuzzy PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")