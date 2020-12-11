#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <climits>
#include <thread>

using namespace std;

int numProcesses;
int timeToRun;

ofstream outputFile;

class Process {
public:
  Process(int name, int period, int runTime) {
      this->name = name;
      this->period = period;
      this->runTime = runTime;

      this->runNumber = 1;
      this->timeLeft = runTime;
      this->done = false;
  }

  void finishRun() {
//    this->timeLeft = runTime;
//    this->runNumber++;
    this->done = true;
  }

  void restartRun() {
    this->timeLeft = runTime;
    this->runNumber++;
    this->done = false;
  }

  void finishEDF() {
    this->done = true;
    this->runNumber++;
  }

  void restartEDF() {
    this->timeLeft = runTime;
    this->done = false;
  }

  friend ostream& operator<<(ostream& os, Process p) {
      os << p.name << " " << p.period << " " << p.runTime;
      return os;
  }


  int name;
  int period;
  int runTime;
  int runNumber;
  int timeLeft;
  bool done;
};

/*
The following functions all refer to
Rate Monotonic Scheduling (RMS)
*/
bool RMSCompare(Process p1, Process p2);
void RMSPrint3(vector<Process> processes, int running);
void RMSPrint4(vector<Process> processes, int running, string failureString);
void RMScheduling(vector<Process> processes);

/*
The following functions all refer to
Earliest Deadline First Scheduling (EDFS)
*/
void EDFPrint3(vector<Process> processes, int running);
void EDFPrint4(vector<Process> processes, int running, string failureString);
void EDFScheduling(vector<Process> processes);

int main(int argc, char *argv[]) {
    if(argc > 1) { //if an input argument was supplied at runtime
        vector<Process> processes;

        fstream readFromFile;
        readFromFile.open(argv[1]);
        outputFile.open("outputFile.txt");
        if(readFromFile.is_open()) { //if file was opened successfully

          readFromFile >> numProcesses;
          readFromFile >> timeToRun;

          int pBuffer;
          int pBuffer2;
          for(int j = 0; j < numProcesses; j++) {
              readFromFile >> pBuffer;
              readFromFile >> pBuffer2;

              processes.push_back(Process(j+1, pBuffer, pBuffer2));
          } //end for loop

          RMScheduling(processes);
          outputFile << endl;
          EDFScheduling(processes);
        } else { //if the file could not be opened
          cout << "Input file could not be opened!" << endl;
        }
        readFromFile.close();


    } else { //if an input argument was not supplied
        cout << "There was a problem with your command line argument \n";
    } //end if argc>1
    return 0;
} //end Main()



/*
Function to take care of Rate Monotonic Scheduling.
*/
void RMScheduling(vector<Process> processes) {
    sort(processes.begin(), processes.end(), RMSCompare);
    int currentlyRunning = 0;
    int timeStarted = 0;

    outputFile << 0 << endl;
    outputFile << "A:";
    for(int j = 0; j < processes.size(); j++) {
        outputFile << " P" << processes[j].name << "-" << processes[j].runNumber;
        //if(j != processes.size()-1)
        //  outputFile << ' ';
    }

    outputFile << endl;
    RMSPrint3(processes, currentlyRunning);
    RMSPrint4(processes, currentlyRunning, "");

    bool failure;
    for(int time = 1; time <= timeToRun; time++) {
      bool eventOccur = false;
      bool isThereSomethingToRun = false;
      //if current process has finished
      if((time-timeStarted) == processes[currentlyRunning].timeLeft) {
        outputFile << time << endl;
        eventOccur = true;

        outputFile << "F: P" << processes[currentlyRunning].name << "-" << \
        processes[currentlyRunning].runNumber;

        processes[currentlyRunning].finishRun();

        for(int j = 0; j < processes.size(); j++) {
          if(!processes[j].done) {
            isThereSomethingToRun = true;
            currentlyRunning = j;
            timeStarted = time;
            break;
          }
        }
      } //end if a process has finished

      //if 1 or more new processes has arrived
      bool printAFlag = false;
      bool printAnything = false;
      failure = false;
      string failureString = "";
      int index = -1;

      for(int j = 0; j < processes.size(); j++) {
        if(time % processes[j].period == 0) {
          if(!eventOccur){
            outputFile << time << endl;
            eventOccur = true;
          }else
            outputFile << " ";

          if(index < 0)
            index = j;

          printAnything = true;
          if(!printAFlag){
            outputFile << "A:";
            printAFlag = true;
          }
          isThereSomethingToRun = true;
          outputFile << " P" << processes[j].name << "-" << \
          processes[j].runNumber+1;

          if(!processes[j].done) {
            outputFile << " FAIL";
            failure = true;
            failureString = "P" + to_string(processes[j].name) + "-" + \
            to_string(processes[j].runNumber) + " (" + \
            to_string(processes[j].period) + ", " + \
            to_string(processes[j].timeLeft) + ") ";
          } //end if failure
          processes[j].restartRun();
        } //end if process has arrived
      } //end forloop through processes
      if(printAnything) {
        outputFile << endl;
        processes[currentlyRunning].timeLeft-= (time-timeStarted);
        timeStarted = time;
        if(index < currentlyRunning)
          currentlyRunning = index;

      }
      if(eventOccur && !printAnything)
        outputFile << endl;
      if(eventOccur) {
        if(isThereSomethingToRun){
          RMSPrint3(processes, currentlyRunning);
        } else {
          outputFile << endl;
        }
        RMSPrint4(processes, currentlyRunning, failureString);
      }
      if(failure)
        break;

    } //end timer for loop
    if(!failure)
      outputFile << endl;
} //end of Rate Monotonic Function

bool RMSCompare(Process p1, Process p2) {
  if(p1.period == p2.period)
      return (p1.name < p2.name);

  return (p1.period < p2.period);
}

void RMSPrint3(vector<Process> processes, int running) {
  if(processes[running].timeLeft > 0) {
    outputFile << "P" << processes[running].name << "-" << \
    processes[running].runNumber << " (" << processes[running].period << ", " << \
    processes[running].timeLeft << ")" << endl;
  }
} //end of RMSPrint for 3rd column

void RMSPrint4(vector<Process> processes, int running, string failureString) {
  string buf = "";
  for(int j = 0; j < processes.size(); j++) {
    if(failureString.length() > 0 && failureString[1]-'0' == processes[j].name)
        buf += failureString;

    if(j!=running && processes[j].done == false) {

      buf += "P" + to_string(processes[j].name) + "-" + \
      to_string(processes[j].runNumber) + " (" + to_string(processes[j].period)\
      + ", " + to_string(processes[j].timeLeft) + ")";

      if(j != processes.size()-1)
        buf+= ' ';
    }
  }
  char last = buf.back();
  if(last == ' ')
    buf.pop_back();

  buf += '\n';

  outputFile << buf;
} //end of RMSPrint for 4th column



void EDFScheduling(vector<Process> processes) {
    sort(processes.begin(), processes.end(), RMSCompare);
    int currentlyRunning = 0;
    int timeStarted = 0;

    outputFile << 0 << endl;
    outputFile << "A:";
    for(int j = 0; j < processes.size(); j++) {
        outputFile << " P" << processes[j].name << "-" << processes[j].runNumber;
    }

    outputFile << endl;
    EDFPrint3(processes, currentlyRunning);
    EDFPrint4(processes, currentlyRunning, "");

    for(int time = 1; time <= timeToRun; time++) {
        bool eventOccur = false;
        bool isThereSomethingToRun = false;

        if((time-timeStarted) == processes[currentlyRunning].timeLeft) {
            outputFile << time << endl;
            eventOccur = true;

            outputFile << "F: P" << processes[currentlyRunning].name << "-" << \
            processes[currentlyRunning].runNumber;

            processes[currentlyRunning].finishEDF();

            int shortestDeadline = INT_MAX;
            int nextIndex = -1;
            for(int j = 0; j < processes.size(); j++) {
              if(!processes[j].done && processes[j].period*processes[j].runNumber < shortestDeadline) {
                  isThereSomethingToRun = true;
                  shortestDeadline = processes[j].period*processes[j].runNumber;
                  nextIndex = j;
              }
            }
            if(nextIndex > -1) {
              currentlyRunning = nextIndex;
              timeStarted = time;
            }
        } //end if a process has finished

        //if 1 or more new processes has arrived
        bool printAFlag = false;
        bool printAnything = false;
        bool failure = false;
        string failureString = "";
        int index = -1;

        for(int j = 0; j < processes.size(); j++) {
          if(time % processes[j].period == 0) {
              if(!eventOccur) {
                outputFile << time << endl;
                eventOccur = true;
              } else
                outputFile << " ";

              if(index < 0)
                  index = j;

              printAnything = true;
              if(!printAFlag){
                  outputFile << "A:";
                  printAFlag = true;
              }
              isThereSomethingToRun = true;

              if(!processes[j].done) {
                outputFile << " P" << processes[j].name << "-" << \
                processes[j].runNumber+1;
                outputFile << " FAIL";
                failure = true;
                failureString = "P" + to_string(processes[j].name) + "-" + \
                to_string(processes[j].runNumber+1) + " (" + \
                to_string(processes[j].period*(processes[j].runNumber+1)) + \
                ", " + to_string(processes[j].runTime) + ") ";
              } else {
                outputFile << " P" << processes[j].name << "-" << \
                processes[j].runNumber;
                processes[j].restartEDF();
              }//end if failure


          } //end if process has arrived
        }//end forloop through processes to check for arrivals

        if(printAnything) {
          outputFile << endl;
          processes[currentlyRunning].timeLeft -= (time-timeStarted);
          timeStarted = time;
          int indexDeadline = processes[index].period*processes[index].runNumber;
          int currDeadline = processes[currentlyRunning].period*processes[currentlyRunning].runNumber;
          if(indexDeadline < currDeadline)
              currentlyRunning = index;

        }
        if(eventOccur && !printAnything)
          outputFile << endl;
        if(eventOccur) {
          if(isThereSomethingToRun){
            EDFPrint3(processes, currentlyRunning);
          } else {
            outputFile << endl;
          }
          EDFPrint4(processes, currentlyRunning, failureString);
        }
        if(failure)
          break;

    }//end of timer for loop
    outputFile << endl;
}//end of Earliest Deadline Function

void EDFPrint3(vector<Process> processes, int running) {
  if(processes[running].timeLeft > 0) {
      outputFile << "P" << processes[running].name << "-" << \
      processes[running].runNumber << " (" << \
      processes[running].period*processes[running].runNumber << ", " << \
      processes[running].timeLeft << ")" << endl;
  }
}

void EDFPrint4(vector<Process> processes, int running, string failureString) {
  string buf = "";
  for(int j = 0; j < processes.size(); j++) {
    if(failureString.length() > 0 && failureString[1]-'0' == processes[j].name)
        buf += failureString;
    if(j!=running && processes[j].done == false) {

      buf += "P" + to_string(processes[j].name) + "-" + \
      to_string(processes[j].runNumber) + " (" + \
      to_string(processes[j].period*processes[j].runNumber)\
      + ", " + to_string(processes[j].timeLeft) + ")";

      if(j != processes.size()-1)
        buf+= ' ';
    }
  }
  char last = buf.back();
  if(last == ' ')
    buf.pop_back();

  buf += '\n';

  outputFile << buf;
}
