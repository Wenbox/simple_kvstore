//
// Created by wxu on 11.05.20.
//

#ifndef SIMPLE_KVSTORE_STOPWATCH_H
#define SIMPLE_KVSTORE_STOPWATCH_H
#include <sys/time.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <iostream>




inline float diff_time (struct timeval &t0, struct timeval &t1) {
    return (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec)/1e6;
}

class stopwatch {
public:
    stopwatch () {reset();}
    inline void reset() {
        m_running = false;
        m_once_stopped = false;
        m_accumulated = 0.0;
    }

    inline void start() {
        if (!m_running && !m_once_stopped) {
            m_running = true;
            gettimeofday(&t0, 0);
        }
    }

    inline void stop() {
        m_once_stopped = true;
        if (m_running) {
            m_running = false;
            gettimeofday(&t1, 0);
            m_accumulated += diff_time(t0, t1);
        }
    }

    inline float elapsed() {
        if(m_running){
            gettimeofday(&t1, 0);
            float runtime = diff_time(t0, t1);
            return m_accumulated + runtime;
        } else {
            return m_accumulated;
        }
    }

private:
    struct timeval t0, t1;
    float m_accumulated;
    bool m_running;
    bool m_once_stopped;
};
#endif //SIMPLE_KVSTORE_STOPWATCH_H
