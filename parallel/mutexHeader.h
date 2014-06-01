#ifndef _MUTEX_HEAD
#define _MUTEX_HEAD

#include <omp.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <typeinfo>

class criticalSection abstract {
public:
	criticalSection() :criticalSection(0){}
	criticalSection(const int t) : threadCount(t){}
	
	virtual ~criticalSection() {}

	virtual void lock() = 0;
	virtual void unlock() = 0;

	int threadCount;
};

class Peterson_Lock : public criticalSection {
public:
	Peterson_Lock(){}
	~Peterson_Lock(){ printf("P_L"); }
	void lock();
	void unlock();

private:
	volatile unsigned long flag[2];
	unsigned long vict;

};
class Filter_Lock : public criticalSection {
public:
	Filter_Lock(int);
	~Filter_Lock();
	void lock();
	void unlock();

private:
	volatile long* victim;
	volatile long* levelId;

};
class Lamport : public criticalSection {
public:
	Lamport(int);
	~Lamport();
	void lock();
	void unlock();

private:
	volatile unsigned long* choosen;
	volatile unsigned long* number;

};
class TAS : public criticalSection { 
public:
	TAS();
	void lock();
	void unlock();

private:
	unsigned flag;

};
class TTAS : public criticalSection {
public:
	TTAS();
	void lock();
	void unlock();	
	void printName();

private:
	unsigned flag;
};
class Query_Lock : public criticalSection {
public:
	Query_Lock(int);
	~Query_Lock();
	void lock();
	void unlock();

private:
	volatile unsigned idx;
	volatile long* available;
	unsigned* my_idx;
	unsigned N;

};
class SysLock : public criticalSection {
public:
	SysLock();
	~SysLock();
	void lock();
	void unlock();

private:
	CRITICAL_SECTION c_s;
};
class SysLock_SpinCount : public criticalSection {
public:
	SysLock_SpinCount(DWORD);
	~SysLock_SpinCount();
	void lock();
	void unlock();

private:
	CRITICAL_SECTION c_s;

};

void initializeCSTest(std::vector<criticalSection*>&, int);

#endif _MUTEX_HEAD