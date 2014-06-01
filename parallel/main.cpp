#include "mutexHeader.h"

#define THREADNUMB 2

double difficult(){
	return log(log(log(log(log(log(log(log(log(log(log(log(20480000.0)))) + 3) + 3) + 3) + 3) + 3) + 3) + 3) + 3); // 4607
}

void main(int args, char* argv[]){
	omp_set_num_threads(THREADNUMB);
	std::vector <criticalSection*> test;
	initializeCSTest(test, THREADNUMB);

	// все работает ок, только lamport-работает слишком долго

	for (auto cs : test){
		double loggSumm = 0;
		unsigned long a = GetTickCount();
#pragma omp parallel for
		for (int i = 0; i < 20000000; i++){
			double parralelRez = difficult();

			cs->lock();
			loggSumm += parralelRez;
			cs->unlock();

		}
		unsigned long z = GetTickCount();
		printf("%s workTime:%lu(%ut) + (%f)\n", typeid(*cs).name(), z - a, THREADNUMB, loggSumm);
	}

	test.clear(); // здесь память не освобождается, надо делать освобождение явно
}
