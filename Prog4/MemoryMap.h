#include <iostream>
#include <stdlib.h>
#include <map>
#include <queue>
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

/*
The following class was created in order to allow values within my pageTable
to also include a value indicating the time it was last called, in order to
implement an LRU algorithm
*/
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

   int pfcalls;
   int fpcalls;

   //my page table, created as a map
   map<pseudokey, pseudoval> pageTable;
   //Queue used for the FIFO policy
   queue<map<pseudokey, pseudoval>::iterator> fifo;

};



// You need to implement this function
ostream& operator<<(ostream& os, MemoryMap& mm)
{
   return os;
}

MemoryMap::MemoryMap(int ptsize, int policy) {
  this.psize = ptsize;
  this.policy = policy;
  pfcalls = 0;
  fpcalls = 0;
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
      non-free frames. I did it this way because the constructor
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

      //get the page and insert it into main memory
      //then, add the reference to the page table
      //Entry temp = p.GetPage(m);
      //frame->set(ind, temp);
     P2F(p, m, frame, victim);
     pageTable.insert({ pseudokey(p, m), pseudoval(ind) });

      //if we are using fifo replacement policy, add the page to our queue
      if(this.policy == 1) {
        auto itr = pageTable.find(pseudokey(p,m));
        fifo.push(itr);
      }

      return ind;

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


          //Process &p, int pid, int m, Memory* frame
          //save frame back to page
            F2P(frame, it->second.getF(), it->first.getP(), it->first.getN());

          //save the frame number to insert the new page into main memory
            victim = it->second.getF();

          //delete that iterator/item from the page table
            this.pageTable.erase(it);


            break;

        case 1:
	//FIFO method. Frames are saved to queue as they enter, so just use the queue.front()
	    auto it = fifo.front();

	    //save frame back to its process' page
	    F2P(frame, it->second.getF(), it->first.getP(), it->first.getN());

	    //save frame number to insert new page into main memory
	    victim = it->second.getF();

	    //delete the iterator/item from page table
	    this.pageTable.erase(it);

	    //pop iterator from fifo queue
	    fifo.pop();
            break;

        case 2:
	//The method I chose for the extra credit portion was to randomly choose an element from the page table
	    int ind = rand() % this.pageTable.size();
	    auto it = this.pageTable.begin();
	    for(int j = 0; j < ind; j ++)
		it++;

	    //save frame back to its process' page
	    F2P(frame, it->second.getF(), it->first.getP(), it->first.getN());

	    //save frame number to insert new page into main memory
	    victim = it->second.getF();

	    //delete the iterator/item from page table
	    this.pageTable.erase(it);

            break;
      }

      //get the page and insert it into main memory
      //then, add the reference to the page table
      P2F(p, m, frame, victim);
      //Entry temp = p.GetPage(m);
      //frame->set(ind, temp);
      pageTable.insert({ pseudokey(p, m), pseudoval(victim) });

      //if we are using fifo replacement policy, add the page to our queue
      if(this.policy == 1) {
        auto itr = pageTable.find(pseudokey(p,m));
        fifo.push(itr);
      }

      return victim;

    } //end of if the page is not loaded into memory


  }//end outer else

}//end getframe function


void MemoryMap::P2F(Process &p, int m, Memory* frame, int f) {
  this.pfcalls++;

  Entry e = p.GetPage(m);
  f->set(f, e);
} //end of P2F method

void MemoryMap::F2P(Memory* frame, int f, Process &p, int m) {
  this.fpcalls++;

  Entry e = frame->get(f);
  p.SetPage(m, e);

} //end of F2P method


int MemoryMap::P2FCalled() {return pfcalls;}

int MemoryMap::F2PCalled() {return fpcalls;}
