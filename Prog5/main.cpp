#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <thread>

using namespace std;

int numProcesses;
int timeToRun;

class Process {
public:
  Process(int name, int period, int runTime) {
      this->name = name;
      this->period = period;
      this->runTime = runTime;

      this->runNumber = 1;
      this->timeLeft = runTime;
  }

  void finishRun() {
    this->timeLeft = runTime;
    this->runNumber++;
  }

  bool operator<(const Process &rhs) {

  }

  bool operator>(const Process &rhs) {

  }

  friend ostream& operator<<(ostream& os, Process p) {
      os << p.name << p.period << p.runTime;
      return os;
  }


  int name;
  int period;
  int runTime;
  int runNumber;
  int timeLeft;
};




int main(int argc, char *argv[]) {
    if(argc > 1) { //if an input argument was supplied at runtime
        vector<Process> processes;

        cout << argv[1] << endl;

        fstream readFromFile;
        readFromFile.open(argv[1]);
        if(readFromFile.is_open()) { //if file was opened successfully

          readFromFile >> numProcesses;
          readFromFile >> timeToRun;

          int pBuffer;
          int pBuffer2;
          for(int j = 0; j < numProcesses; j++) {
              readFromFile >> pBuffer;
              readFromFile >> pBuffer2;

              processes.push_back(Process(j, pBuffer, pBuffer2));
          } //end for loop

          for(int j = 0; j < processes.size(); j++) {
            cout << processes[j] << endl;
          }

        } else { //if the file could not be opened
          cout << "Input file could not be opened!" << endl;
        }
        readFromFile.close();


    } else { //if an input argument was not supplied
        printf("There was a problem with your command line argument \n");
    } //end if argc>1
    return 0;
} //end Main()
