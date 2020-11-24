#include <iostream>
#include <stdlib.h>
#include <map>
#include <iterator>
#include <ctime>

using namespace std;


/*
The following class is used in order to have complex keys with which
to create a page table in the form of a standard library map. It stores
both the address of the process it belongs to, as well as the page number
within that process it is referring to
*/
class pseudokey {
public:
  pseudokey(Process &p, int n) {this.processAddress = p; this.pNum = n;};

  bool operator<(const pseudokey& rhs) const {
    if(this.processAddress < rhs.processAddress) {
      return true;
    } else if (this.processAddress > rhs.processAddress) {
      return false;
    } else {
      return (this.pNum < rhs.pNum);
    }
  };

  Process& getP() {return processAddress;};
  int getN() {return pNum;};

  Process &processAddress;
  int pNum;
};

class pseudoval {
public:
  pseudoval(int f) {this.lastUse = time(NULL); this.frameNum = f;};
  time_t getT() {return lastUse;};
  int getF() {return frameNum;};

  time_t lastUse;
  int frameNum;
}

class MemoryMap {

  friend Process;
  friend ostream& operator<<(ostream& os, MemoryMap& mm);

public:

  MemoryMap(int ptsize, int policy) {x = 0;};
  int   GetFrame(Process &p, int pid, int m, Memory* frame);
  void  P2F(Process &p, int m, Memory* frame, int f);
  void  F2P(Memory* frame, int f, Process &p, int m);
  int   P2FCalled();
  int   F2PCalled();

private:

   int psize;
   int policy;

   //my page table, created as a map
   map<pseudokey, pseudoval> pageTable; //int refers to frame number within main memory

};



// You need to implement this function
ostream& operator<<(ostream& os, MemoryMap& mm)
{
   return os;
}

MemoryMap::MemoryMap(int ptsize, int policy) {
  this.psize = ptsize;
  this.policy = policy;


}



/*return the frame in main memory for the page m for process p.*/
int MemoryMap::GetFrame(Process &p, int pid, int m, Memory* frame) {
  //try to find that particular page within main memory
  auto itr = pageTable.find(pseudokey(p, m));


  if(itr != pageTable.end()) {
    return itr->second.getF();
  } else { //will run if the page is not loaded in main memory
    if(pageTable.size() < this.psize) { //if there is a free frame
      /*The following code identifies a free frame by excluding any
      non-free frames. I had to do it this way becaus the constructor
      doesn't have access to the Memory object to create an initial set
      of free frames*/
      vector<int> v;
      for(auto j = pageTable.begin(); j != pageTable.end(); j++) {
        v.push_back(j->second.getF());
      }

      int ind = -1;
      do {
        ind = rand() % frame->Size();
      } while(find(v.begin(), v.end(), ind) != v.end());

      Entry temp = p.GetPage(m);
      frame->set(ind, temp);
      pageTable.insert({ pseudokey(p, m), pseudoval(ind) });

    } else { //if the code reaches this point the pageTable is full and a victim page must be selected
      int victim;
      switch (this.policy) {
        case 0: //utilizing LRU to identify victim page
          //victim = LRUVictim();
          int ind = 0; //used to store "index" within the page table map
          time_t lowestTime = time(NULL); //initialize this to current time
          int k = 0;
          for(auto j = pageTable.begin(); j != pageTable.end(); j++) {
            if(j->second.getT() < lowestTime) {
              ind = k;
              lowestTime = j->second.getT();
            }
            k++;
          } //end forloop

          map<pseudokey, pseudoval>::iterator it = pageTable.begin();
          for(int j = 0; j < ind; j++)
            it++;

          //save frame back to page
          //delete entry in frame
          //delete that iterator/item from the page table
          break;

        case 1:
          break;

        case 2:
          break;
      }
    }


  }//end outer else

}//end getframe function
