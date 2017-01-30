ADD_EXECUTABLE(skylake
        core/BaseDriver.h
        core/CarControl.cpp core/CarControl.h
        core/WrapperBaseDriver.cpp core/WrapperBaseDriver.h
        core/CarState.cpp core/CarState.h
        core/SimpleParser.cpp core/SimpleParser.h
        driver/SkylakeDriver.cpp driver/SkylakeDriver.h
        driver/client.cpp
        driver/SkylakeConsts.h
        driver/Kinematics.cpp driver/Kinematics.h
        driver/Map.cpp driver/Map.h
        driver/MapSDL2.cpp driver/MapSDL2.h)
TARGET_LINK_LIBRARIES(skylake ${SDL2_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(skylake PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")

ADD_EXECUTABLE(fuzzy
        core/BaseDriver.h
        core/CarControl.cpp core/CarControl.h
        core/WrapperBaseDriver.cpp core/WrapperBaseDriver.h
        core/CarState.cpp core/CarState.h
        core/SimpleParser.cpp core/SimpleParser.h
        fuzzy-driver/client.cpp
        fuzzy-driver/FuzzyDriver.cpp fuzzy-driver/FuzzyDriver.h)

SET_TARGET_PROPERTIES(fuzzy PROPERTIES COMPILE_FLAGS "-std=c++14 -O3")
TARGET_LINK_LIBRARIES(fuzzy ${FUZZY_LIBRARIES})