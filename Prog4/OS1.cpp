#include <iostream>
#include <stdlib.h>

  /*Change the program counter of process p to newpos if entry m for
  process p is equal to 0(the first number of the entry only)*/
  void OS::CondJump(int p, int m, int newpos) {
    //get the main mem location of the page we're interested in
    int mmLoc = mmap->GetFrame(process[p], p, m, frame);

    if(frame->get(mmLoc).get(1) == 0) //check if the value at that page == 0
        process[p].SetPC(newpos); //set new PC if true
  } //end CondJump Function


  void OS::Run() {

  int robin = 0; //used for the round robin scheduling

  /*
  the number of commands to run depends on the schedule option,
  if it's odd we run 5 commands, otherwise we just run 1 command.
  This is done independently of the choice of what process to run
  at each step because it could be generalized with the ternary operator.
  Odd schedule_option values correspond to running 5 steps in a row, even
  correspond to running 1 step
  */
	int steps = (schedule_option % 2 == 1) ? 5 : 1;

  //keeps track of the process "id" (vector index) for which process to mess with
  int id = 0;

  Process* toRun;
	while(!process.empty()) {
      /*
      This if statement tells the program how to choose the next process to
      execute commands from based on the schedule_option. If it's 0 or 1, we
      are using random processes every time. If it's 2 or 3, we are going in a
      round robin fashion.
      */
	    if(schedule_option < 2) {
        id = rand() % process.size();
        toRun = &process[id];
	    } else {
        id = robin;
        toRun = &process[robin];
        robin = (robin+1) % process.size();
	    }

      bool processEnded = false; //flag to check if a process is done at the end of command execution
      /*
      Execute a number of commands for this step equal to the steps var
      calculated earlier
      */
      for(int j = 0; j < steps; j++) {
        //Process &p, int pid, int m, Memory* frame
          //load the instruction we're executing
          int mainMemLoc = mmap->GetFrame(*toRun, id, toRun->GetPC(), frame);

          //split the loaded command into its 3 parts
          //instruction/operator
          int instruction = frame->get(mainMemLoc).get(1);
          //"a" from the program handout
          int opA = frame->get(mainMemLoc).get(2);
          //"b" from the program handout
          int opB = frame->get(mainMemLoc).get(3);


          //load the operand that will be used (assuming the process hasn't been completed)
          int operand = mmap->GetFrame(*toRun, id, opA, frame);

          //execute commands based on the 1st value within the entry currently loaded into memory
          switch (instruction) {
            case -1: //assigning a variable
                Assign(opA, opB);
                break;

            case -2: //incrementing a value
                Add(opA, 1);
                break;

            case -3: //decrementing a value
                Subtract(opA, 1);
                break;

            case -4: //adding a value to a page
                Add(opA, opB);
                break;

            case -5: //subtracting a value from a page
                Subtract(opA, opB);
                break;

            case -6: //multiplying the value in a page by another value
                Multiply(opA, opB);
                break;

            case -7: //copying a page to a different page
                Copy(opA, opB);
                break;

            case -8: //changing the program counter to some other command
                Jump(id, opA);
                break;

            case -9: //only changes the program counter if a page's value is 0
                CondJump(id, opB, opA);
                break;

            default: //end of process
                //save all of the memory belonging to the process back to its pages
                Process *pToDel = &process[id]; //get address of process
                //iterate through page table to find any with the same process address
                //if found, the contents will be saved back to the pages for that process
                for(auto j = mmap->pageTable.begin(); j != mmap->pageTable.end(); j++) {
                  if(j->first.getP() == pToDel) {
                    //if the process addresses match, save the frame back to the right page
                    mmap->F2P(frame, j->second.getF(), *pToDel, j->first.getN());
                  }
                }
                processEnded = true; //set the flag = true to end the for-loop early if need be
                process.erase(process.begin()+id); //get rid of the process from the OS's vector
                break;
          }
          if(processEnded)
              break;
      } //end for loop of command execution
	} //end while(!process.empty())
  } //end Run Function
