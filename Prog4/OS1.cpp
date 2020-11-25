// This is where you implement the not yet implemented method for OS class
class OS {

  /*Change the program counter of process p to newpos if entry m for
  process p is equal to 0(the first number of the entry only)*/

  void CondJump(int p, int m, int newpos) {

  } //end CondJump Function




  void Run() {
//initialize variables if necessary???

        int robin = 0;
	int steps = (schedule_option == 1 || schedule_option == 3) ? 5 : 1;

	while(!process.empty()) {
	    if(schedule_option < 2) {


	    } else {


	    }
	}
  } //end Run Function
} //end of OS class




//   int schedule_option;

//   Memory *frame;
//   MemoryMap *mmap;
//   vector<Process> process;
