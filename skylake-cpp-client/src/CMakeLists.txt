ADD_EXECUTABLE(skylake-mapping
        core-common/BaseDriver.h
        core-common/CarControl.cpp core-common/CarControl.h
        core-common/WrapperBaseDriver.cpp core-common/WrapperBaseDriver.h
        core-common/CarState.cpp core-common/CarState.h
        core-common/SimpleParser.cpp core-common/SimpleParser.h
        core/SkylakeConsts.cpp
        core/SkylakeBaseDriver.cpp core/SkylakeBaseDriver.h
        mapping-driver/SkylakeDriver.cpp mapping-driver/SkylakeDriver.h
        mapping-driver/client.cpp
        mapping-driver/Kinematics.cpp mapping-driver/Kinematics.h
        mapping-driver/Map.cpp mapping-driver/Map.h
        mapping-driver/MapSDL2.cpp mapping-driver/MapSDL2.h)
TARGET_LINK_LIBRARIES(skylake-mapping ${SDL2_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(skylake-mapping PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")

ADD_EXECUTABLE(skylake-vfh-fuzzy
        core-common/BaseDriver.h
        core-common/CarControl.cpp core-common/CarControl.h
        core-common/WrapperBaseDriver.cpp core-common/WrapperBaseDriver.h
        core-common/CarState.cpp core-common/CarState.h
        core-common/SimpleParser.cpp core-common/SimpleParser.h
        core/SkylakeConsts.cpp
        core/SkylakeBaseDriver.cpp core/SkylakeBaseDriver.h
        vfh-fuzzy-driver/client.cpp
        vfh-fuzzy-driver/VFHFuzzyDriver.cpp vfh-fuzzy-driver/VFHFuzzyDriver.h)

TARGET_LINK_LIBRARIES(skylake-vfh-fuzzy ${FUZZY_LIBRARIES} ${SDL2_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(skylake-vfh-fuzzy PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")