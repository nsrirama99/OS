#include <iostream>

using namespace std;

class Process {
public:
  Process(int name, int period, int runTime);

  void finishRun();

  bool operator<(const Process &rhs);
  bool operator>(const Process &rhs);
  friend ostream& operator<<(ostream& os, Process& p);

  int name;
  int period;
  int runTime;
  int runNumber;
  int timeLeft;
};
