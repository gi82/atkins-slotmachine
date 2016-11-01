#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iterator>
#include <iomanip>
#include <map>
#include <algorithm>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "Reel.h"
#include "ran250.h"

using namespace std;
	
Reel::Reel(){
}
Reel::Reel(int N_stops){
	//srand((int)time(NULL));
	r250_init((int)time(NULL));
	this->Nstops = N_stops;
	this->stops = new int[N_stops];
	//set unique ID for stop each stop
}
Reel::~Reel(){
	delete stops;
}
void Reel::spin() {
	// set number of spins
	int max_spins = 20;
	int num_spins = r250n(max_spins)+1;
	for (int i=0;i < num_spins;i++){
		this->astop = r250n(this->Nstops); // return a number uniformly distributed in the interval [0, Nstops)
		//this->astop = rand()%Nstops;
	}
}
void Reel::setstop(int stop){
	// stop is uniqueID (0,Nstop]
	if (stop > 0 && stop <= this->Nstops ){
		this->astop = stop-1;
	}
	else{
		cout<<"stop="<<stop<<", stop-1="<<stop-1<<endl;
		cout<<"Reel::stop @ setstop(...) out of range"<<endl;
	}
}
void Reel::loadstops(int* stopstrip){
	for (int i=0;i<this->Nstops;i++) stops[i] = stopstrip[i];
}
int Reel::getnstops(){
	return this->Nstops;
}
int Reel::getstop(int nstop){
	return stops[nstop];
}
int* Reel::getstops(){
	return stops;
}
void Reel::getlines(int* r){
	// return the three vissible lines of a column
	int prev = ( astop > 0 ) ? astop - 1 : Nstops-1;
	int next = ( astop < Nstops -1 ) ? astop + 1 : 0;
	
	// return top (0) middle(1) bottom(2)
	r[0] = stops[prev] ;
	r[1] = stops[astop];
	r[2] = stops[next];
	
}
