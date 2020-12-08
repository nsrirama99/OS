#include <iostream>
#include <vector>
#include "Process.h"


Process::Process(int name, int period, int runTime) {
    this->name = name;
    this->period = period;
    this->runTime = runTime;

    this->runNumber = 1;
    this->timeLeft = runTime;
}

void Process::finishRun() {
  this->timeLeft = runTime;
  this->runNumber++;
}

bool Process::operator<(const Process &rhs) {

}

bool Process::operator>(const Process &rhs) {

}

friend ostream& operator<<(ostream&) {
    os << this->name << this->period >> this->runTime;
    return os;
}
