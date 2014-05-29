#include "mutexHeader.h"

void Peterson_Lock::lock(){
	int TID = omp_get_thread_num();
	InterlockedExchange(flag+TID, 1);
	InterlockedExchange(&vict, TID);

	while (flag[1 - TID] && vict == TID);
}

void Peterson_Lock::unlock(){
	flag[omp_get_thread_num()] = false;
}

//-----------------------

Filter_Lock::Filter_Lock(int threadNumb) : criticalSection(threadNumb){
	victim = new long[threadNumb];
	levelId = new long[threadNumb];
	for (size_t i = 0; i<threadNumb-1; ++i){
		levelId[i] = -1;
		victim[i] = -1;
	}
	levelId[threadNumb - 1] = -1;
}
Filter_Lock::~Filter_Lock(){
	delete[] levelId;
	delete[] victim;
}
void Filter_Lock::lock(){
	int threadId = omp_get_thread_num();
	for (int i = 0; i<threadCount-1; ++i){
		InterlockedExchange(levelId + threadId, i);
		InterlockedExchange(victim + i, threadId);
		volatile long flag = true;
		while (flag){
			InterlockedExchange(&flag, false);
			for (int k = 0; k < threadCount; ++k){
				if (k != threadId && levelId[k] >= i && victim[i] == threadId){
					InterlockedExchange(&flag, true); break;
				}
			}
		}
	}
}
void Filter_Lock::unlock(){
	InterlockedExchange(levelId + omp_get_thread_num(), -1);
}

//-----------------------

Lamport::Lamport(const int threadNumb) : criticalSection(threadNumb) {
	number = new volatile unsigned long[threadNumb];
	choosen = new volatile unsigned long[threadNumb];
	for (int i = 0; i < threadNumb; ++i){
		choosen[i] = number[i] = 0;
	}
}

Lamport::~Lamport(){
	delete[] choosen;
	delete[] number;
}

void Lamport::lock(){
	int i = omp_get_thread_num();
	InterlockedExchange(choosen + i, 1);	
	for (int j = 0; j < threadCount; ++j){
		if (number[j] >= number[i]){
			InterlockedExchange(number + i, number[j]); 
		}
	}
	InterlockedIncrement(number + i); 
	InterlockedExchange(choosen + i, 0);

	for (int j = 0; j < threadCount; ++j){
		while (choosen[j]);
		while ((number[j] != 0) && ((number[j] < number[i]) || ((number[j] == number[i]) && (j < i))));
	}
}

void Lamport::unlock(){
	InterlockedExchange(number + omp_get_thread_num(), 0);
}

//-----------------------

TAS::TAS(){ flag = 0;}

void TAS::lock(){
	while(InterlockedExchange(&flag, 1));
}

void TAS::unlock(){
	flag = 0;
}

//-----------------------

TTAS::TTAS(){ flag = 0;}

void TTAS::lock(){
	for (;;){
		if (!flag && !InterlockedExchange(&flag, 1)){ break; }
	}
}

void TTAS::unlock(){ 
	flag = 0; 
}

//-----------------------

Query_Lock::Query_Lock(int threadNumb) : criticalSection(threadNumb){
	idx = 0;
	available = new long[threadNumb];
	available[idx] = true;
	for (unsigned i = 1; i < threadNumb; ++i){
		available[i] = false;
	}
	my_idx = new unsigned[threadNumb];
}

Query_Lock::~Query_Lock(){
	delete[] available;
	delete[] my_idx;
}

void Query_Lock::lock(){
	int TID = omp_get_thread_num();
	int i = my_idx[TID] = (InterlockedIncrement(&idx) - 1) % threadCount;
	while (!available[i]);
}

void Query_Lock::unlock(){
	int TID = omp_get_thread_num();
	InterlockedExchange(available + my_idx[TID], 0);
	InterlockedExchange(available + ((my_idx[TID] + 1) % threadCount), 1);
}

//-----------------------

SysLock::SysLock(){
	InitializeCriticalSection(&c_s);
}

SysLock::~SysLock(){
	DeleteCriticalSection(&c_s);
}

void SysLock::lock(){
	EnterCriticalSection(&c_s);
}

void SysLock::unlock(){
	LeaveCriticalSection(&c_s);
}

//-----------------------

SysLock_SpinCount::SysLock_SpinCount(DWORD spinCounter){
	InitializeCriticalSectionAndSpinCount(&c_s, spinCounter);
}

SysLock_SpinCount::~SysLock_SpinCount(){
	DeleteCriticalSection(&c_s);
}

void SysLock_SpinCount::lock(){
	EnterCriticalSection(&c_s);
}

void SysLock_SpinCount::unlock(){
	LeaveCriticalSection(&c_s);
}


void initializeCSTest(std::vector<criticalSection*>& obj, int threadCount){
	if (threadCount == 2){
		obj.push_back(new Peterson_Lock);
	}
	obj.push_back(new Lamport(threadCount));
	obj.push_back(new Filter_Lock(threadCount));
	obj.push_back(new Query_Lock(threadCount));
	obj.push_back(new TAS);
	obj.push_back(new TTAS);
	obj.push_back(new SysLock);
	obj.push_back(new SysLock_SpinCount(100));
	obj.push_back(new SysLock_SpinCount(5000));
}