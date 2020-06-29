//
// Created by Abdulrahman Semrie on 5/26/20.
//

#ifndef OPENCOG_TEST_TIMER_H
#define OPENCOG_TEST_TIMER_H

#include <iostream>
#include <ctime>

class Timer
{
public:
    Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec +
               (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_{}, end_{};
};


#endif //OPENCOG_TEST_TIMER_H
