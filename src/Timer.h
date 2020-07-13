//
// Created by kraft on 07.08.19.
//

#include <iostream>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"


#ifndef CANCERDATAANALYSIS_TIMER_H
#define CANCERDATAANALYSIS_TIMER_H

using namespace std;

class Timer {
private:
    double elapsed;
    int n_sessions;
    string name;
    boost::posix_time::ptime reference;
    bool running;
public:
    void start_session();
    void end_session();
    void print_timing(ostream);
    Timer(string name);

};


#endif //CANCERDATAANALYSIS_TIMER_H
