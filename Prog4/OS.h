#include <iostream>
#include <climits>
#include <vector>

using namespace std;

class Entry {

   friend ostream& operator<<(ostream& os, Entry& m);

public:

  Entry() {a1 = a2 = a3 = 0;};
  Entry(const Entry& e1) {a1 = e1.a1; a2 = e1.a2; a3 = e1.a3;};

  Entry& operator=(const Entry &e1) {a1 = e1.a1; a2 = e1.a2; a3 = e1.a3; return *this;};

  void set(int x, int y, int z) {a1 = x; a2 = y; a3 = z;};
  int get(int pos) {if (pos == 1) return a1; if (pos == 2) return a2; if (pos == 3) return a3; return INT_MAX;};

private:

  int a1, a2, a3;
};

class Memory {


   friend ostream& operator<<(ostream& os, Memory& m);

public:

   Memory(int s) {size = s; for (int i = 0; i < s; i++) {Entry e1; e.push_back(e1);}};

   Entry get(int pos) {return e[pos];}
   void set(int pos, Entry& entry) {cout << pos << " - " << entry << endl;e[pos] = entry;}
   int Size() {return size;}


private:
   int size;
   vector<Entry> e;
};

class MemoryMap;

class Process {

   friend ostream& operator<<(ostream& os, Process& p);
   friend MemoryMap;

public:
   Process() {program_counter = 0;};
   void AppendEntry(Entry& e) {page.push_back(e);};
   void IncrementPC() {program_counter++;};
   void SetPC(int v) {program_counter = v;};
   int  GetPC() {return program_counter;};

private:

   Entry GetPage(int i) {return page[i];};
   void SetPage(int i, Entry& e) {page[i] = e;};
   vector<Entry> page;
   int program_counter;
};

#include "memorymap.h"

class OS {

   friend ostream& operator<<(ostream& os, OS& p);

public:

   OS(int num_frames, int sopt, int pt_size, int policy) { frame = new Memory(num_frames); schedule_option = sopt; mmap = new MemoryMap(pt_size, policy);};
   void AddProcess(Process& p) {process.push_back(p);};
   void Run();


   // ALU operations

   void Assign(int f, int value) {Entry e; e.set(value, 0, 0); frame->set(f, e);};
   void Add(int f, int value) {Entry e = frame->get(f);  int a = e.get(1) + value; e.set(a, 0, 0), frame->set(f, e);};
   void Subtract(int f, int value) {Entry e = frame->get(f);  int a = e.get(1) - value; if (a < 0) a = 0; e.set(a, 0, 0), frame->set(f, e);};
   void Multiply(int f, int value) {Entry e = frame->get(f);  int a = e.get(1) * value; e.set(a, 0, 0), frame->set(f, e);};
   void Divide(int f, int value) {Entry e = frame->get(f);  int a = e.get(1) / value; e.set(a, 0, 0), frame->set(f, e);};
   void Copy(int fs, int fd) {Entry e = frame->get(fs); frame->set(fd, e);};

   // Program Counter Function

   void Jump(int p, int newpos) {process[p].SetPC(newpos);};
   void CondJump(int p, int m, int newpos) ;



private:

   int schedule_option;

   Memory *frame;
   MemoryMap *mmap;
   vector<Process> process;

};


#include "OS1.cpp"




ostream& operator<<(ostream& os, Entry& e)
{
   os <<  e.get(1) << " " << e.get(2) << " " << e.get(3);

    return os;
}

ostream& operator<<(ostream& os, Memory& m)
{
    for (int i = 0; i < m.size; i++)
	{
	   os << i << " : " << m.e[i].get(1) << " " << m.e[i].get(2) << " " << m.e[i].get(3) << "\n";
	}

    return os;
}

ostream& operator<<(ostream& os, Process& p)
{
    os << "PC: " << p.program_counter << "\n";

    for (unsigned int i = 0; i < p.page.size(); i++)
	{
	   os << i << " : " << p.page[i] << endl;
	}

    return os;
}

ostream& operator<<(ostream& os, OS& o)
{
    os << "Schedule Option: " << o.schedule_option << "\n";

    os << "Memory " << endl << *(o.frame) << "\n";

    os << "Memory Map " << endl << *(o.mmap) << "\n";

    for (unsigned int i = 0; i < o.process.size(); i++)
	{
	   os << "Process " << i << endl;
	   os << o.process[i] << endl;
	}

    return os;
}
