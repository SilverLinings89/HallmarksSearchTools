//
// Created by kraft on 07.08.19.
//

#include "Timer.h"
#include <iostream>
#include <string>
#include <boost/date_time/time_clock.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;

void Timer::start_session() {
    this->n_sessions++;
    this->reference = boost::posix_time::second_clock::local_time();
    running = true;
}

void Timer::end_session() {
    boost::posix_time::ptime t2 = boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration diff = t2 - reference;
    elapsed += diff.total_milliseconds();
    running = false;
}

void Timer::print_timing(ostream in_stream) {
    if(running) {
        in_stream << "Timer is still running. Cannot write output." << endl;
    } else {
        if (this->n_sessions == 1) {
            in_stream << "The timer " << name << " measured a total of " << elapsed << "ms." << endl;
        } else {
            in_stream << "The timer " << name << " measured a total of " << elapsed << "ms across " << n_sessions
                      << " calls for an average of " << elapsed / n_sessions << "ms per call." << endl;
        }
    }
}

Timer::Timer(string in_name) {
    this->elapsed = 0;
    this->n_sessions = 0;
    this->name = in_name;
    running = false;
}
