#pragma once

#include <chrono>
#include <iostream>

//used this along with links in document pdf
//https://www.youtube.com/watch?v=QYaQStudgnE

class timer{
    std::chrono::high_resolution_clock::time_point start; //start time
    std::chrono::high_resolution_clock::time_point pausedStart; //when pause started
    int extraTime = 0.0; //extra time during a pause
    bool paused = false;

public:
    void startTimer() {
        start = std::chrono::high_resolution_clock::now();
        extraTime = 0;
        paused = false;
    }

    void pause() {
        if (paused){ return; }
        pausedStart = std::chrono::high_resolution_clock::now();
        paused = true;
    }

    void unpause() {
        if (!paused){ return; }
        //calculate extra time now - pausedstart
        extraTime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - pausedStart).count();
        paused = false;
    }

    int timeElapsed() {
        int retVal = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count() - extraTime;
        return retVal;
    }

    int nano() {
        return timeElapsed();
    }

    void printTime() {
        std::cout << nano() << " nanoseconds" << std::endl;
    }
};