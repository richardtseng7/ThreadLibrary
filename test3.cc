#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "thread.h"
#include <map>
#include <math.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

using namespace std;

struct orders {
	int sandwichNum;
	int cashierNum;
};

int cashiers_alive;
int lastOrder = -1;
int max_orders;

int waiters = 0;

std::vector<orders> corkboard;
std::map<int, std::vector<int> > cashierMap;

unsigned int hasRoom = 2000;
unsigned int isFull = 3000;

int c[5][2] = {{53, 785}, {914, 350}, {827, 567}, {302, 230}, {631, 11}};

// helper method checks if cashier has posted on corkboard
bool ifContains(int cashierID) {
	if (corkboard.empty()) {
		return false;
	}
	for (int i = 0; i < corkboard.size(); i++) {
		orders curOrder = corkboard[i];
		int curID = curOrder.cashierNum;
		if (curID == cashierID) {
			return true;
		}
	}
	return false;
}


void sandwichMaker(void* arg){
	while ((corkboard.size() > 0) || (cashiers_alive > 0)){	
        if (thread_lock(1) == -1){
            
        }
		while(corkboard.size() < min(cashiers_alive, max_orders)){ 
			thread_wait(1, isFull);
		}
		if (cashiers_alive == 0){
			thread_unlock(1);
			return;
		}
		int min = INT_MAX;
		int ind;
		int currCashier;
		int currSandwich;
		for (int i = 0; i < corkboard.size(); i++){
			int diff = abs(lastOrder - corkboard[i].sandwichNum);
			if (diff < min){ 
				min = diff;
				ind = i;
				currCashier = corkboard[i].cashierNum;
				currSandwich = corkboard[i].sandwichNum;
			}
		}
		lastOrder = currSandwich;
		cout << "READY: cashier " << currCashier << " sandwich " << currSandwich << endl;
		corkboard.erase(corkboard.begin() + ind);
		if (cashierMap[currCashier].size() == 0){
			cashiers_alive--;
		}
		thread_broadcast(1, hasRoom);
		thread_unlock(1);
	}	
}


void cashiers(void* arg){
	int cashierID = (intptr_t) arg;
	while (cashierMap[cashierID].size() > 0){
		thread_lock(1);
		// cashier CANNOT post on corkboard, wait on these 2 conditions
		while ((corkboard.size() == max_orders) || (ifContains(cashierID))) {
			thread_wait(1, hasRoom);
		}
		// cashier can post on corkboard, proceed
		if (cashierMap[cashierID].size() > 0){
			orders toAdd;
			toAdd.cashierNum = cashierID;
			toAdd.sandwichNum = cashierMap[cashierID][0];
			corkboard.push_back(toAdd);
			cashierMap[cashierID].erase(cashierMap[cashierID].begin());
			cout << "POSTED: cashier " << toAdd.cashierNum << " sandwich " << toAdd.sandwichNum << endl;
		}
		thread_signal(1, isFull);
		thread_unlock(1);
	}
}


void cashierInit(void* arg){
    start_preemptions(false, true, 1);
	// initialize sandwich maker thread
	thread_create((thread_startfunc_t) sandwichMaker, (void *) 14);
	// initialize all cashiers threads
	for (int i = 0; i < cashiers_alive; i++){ 
		thread_create((thread_startfunc_t) cashiers, (void *) (intptr_t) i);
	}
}


int main(int argc, char* argv[]) {
	max_orders = 3;
	cashiers_alive = 5;
	if (cashiers_alive == 0) {
		return 0;
	}
	//add sandwich orders to map here
	for (int i = 0; i < 5; i++) {
        std::vector<int> arrList;
        for (int j = 0; j < 2; j++){
            arrList.push_back(c[i][j]);
        }
        cashierMap[i] = arrList;
	}
	thread_libinit((thread_startfunc_t) cashierInit, (void *) 7);
}