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
#include "SlotMachine.h"
using namespace std;
SlotMachine::SlotMachine(int N_reels, int N_symbols,int N_stops,int N_winlines):Nreels(N_reels),Nsymbols(N_symbols),Nstops(N_stops),Nwinlines(N_winlines){
	// creates slot machine with N_reels each one having N_stops
	//Nreels = N_reels;
	//Nwinlines = N_winlines;
	//Nstops=N_stops;
	//Nsymbols = N_symbols;
	coins = 0;
	balance = 0;

	reels = new Reel*[N_reels];
	for (int i=0;i<N_reels;i++){
		reels[i] = new Reel(Nstops);
	}
	// three vissible lines
	lines = new int*[3];
	for (int i=0;i<3;i++){
		lines[i] = new int[Nreels];
	}
	winlines = new int*[Nwinlines];
	for (int i=0;i<N_winlines;i++){
		winlines[i] = new int[Nreels];	
	}
	paytable = new int*[Nsymbols];
	for (int i=0;i<Nsymbols;i++){
		paytable[i] = new int[N_reels];
	}

}	
void SlotMachine::setreels(int *line){
	// set middle (1) values of all reels
	for (int i=0;i < Nreels;i++){
		reels[i]->setstop(line[i]);
	}
	//loadlines();
}			
void SlotMachine::loadreelstrip(int nreel, std::string *strip){
	// load strips for reel (nreel) 
	int loc_nstops= reels[nreel]->getnstops();
	int loc_intstrip[loc_nstops];
	for (int i=0;i<loc_nstops;i++){
		loc_intstrip[i] = mapSymbol[strip[i]];
	}
	reels[nreel]->loadstops(loc_intstrip);
	
}
void SlotMachine::setpaytable(std::string symbol,int* paytable,int paytablesize){
	//copy(paytable,paytable+paytablesize,this->paytable[symbolid-1]);	
	
	int j=Nreels-1;
	int symbolid = mapSymbol[symbol];
	for (int i=0;i<paytablesize;i++){
		this->paytable[symbolid][j--] = paytable[i];
	}	
}
void SlotMachine::print_paytable(){
	cout<<"Paytable"<<endl;
	for (int i=0;i<Nsymbols;i++){
		cout<<mapString[i]<<" pays:";
		for (int j=Nreels-1;j>=0;j--){
			cout<<paytable[i][j]<<" ";
		}
		cout<<endl;
		
	}
}
void SlotMachine::getwinline(int win_line_num, int* win_line){
	for (int i=0;i<Nreels;i++){
		win_line[i]=lines[winlines[win_line_num][i]][i];
	}
}

void SlotMachine::loadlines(){
	int *tmpreel = new int[3];
	for (int i=0;i<Nreels;i++){
		reels[i]->getlines(tmpreel);
		setcol(i,tmpreel);	
	}
	delete(tmpreel);
}
void SlotMachine::spinreels(){
	int *col = new int[Nreels];
	for (int i=0;i<Nreels;i++){
		reels[i]->spin();
	}
	loadlines();
	delete(col);
}
void SlotMachine::printreelstrips(){
	int **loc_strips, loc_stops;
	loc_strips = new int*[Nreels];
	// load all strips
	cout<<"Slot Machine strips"<<endl;
	for (int i=0;i<Nreels;i++){
		loc_stops = reels[i]->getnstops();
		loc_strips[i] = new int[loc_stops] ;
		loc_strips[i] = reels[i]->getstops();
	}
	for (int i=0;i<loc_stops;i++){
		for (int j=0;j<Nreels;j++){
			cout<<setw(16)<<mapString[loc_strips[j][i]]<<"\t";
		}
		cout<<endl;
	}
	delete(loc_strips);
}

void SlotMachine::setcol(int nreel, int* reelcol){
	for (int i=0;i<3;i++){
		lines[i][nreel] = reelcol[i];
	}
}
void SlotMachine::setwinline(int nline, int *winline){
	if (nline < Nwinlines){
		for (int i=0;i<Nreels;i++){
			winlines[nline][i] = winline[i];
		}
	}else{
		cout<<"exceeded max number of winlines"<<endl;
	}
}
void SlotMachine::printwinlines(){
	for (int i=0;i<Nwinlines;i++){
		for (int j=0;j<Nreels;j++){	
			cout<<setw(2)<<this->winlines[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}
void SlotMachine::add_to_balance(int amount){
	if (amount > 0 ){
		balance += amount;
		cout<<"Added: "<<amount<<"  Balance:"<<balance<<endl;
	}
}
void SlotMachine::set_coins(int numcoins){
	if (numcoins > 0){
		coins = numcoins;
	}
}
bool SlotMachine::set_bet(int win_lines_in){
	bool ret_value ;
	if ( win_lines_in < 1 || win_lines_in > Nwinlines){
		cout<<"Wrong number of winlines to check while betting"<<endl;
		ret_value = false;
	}
	else{
		if ( ( balance - win_lines_in*coins ) < 0 ){
			cout << "Not enough coins to play this game\n\twin lines to check="<<winlines<<"\n\tcoins on bet="<<coins<<"\n\tWager="<<win_lines_in*coins<<endl;
			ret_value = false;
		}
		else{
			wager = win_lines_in * coins;
			win_lines_check = win_lines_in;
			balance -= wager;
			ret_value = true;
		}
	}
	return ret_value;
}
SlotMachine::~SlotMachine(){
	cout<<"Bye bye Slot Machine"<<endl;
	for (int i=0;i<this->Nreels;i++){
		delete(reels[i]);
	}
	for (int i=0;i<3;i++){
		delete(lines[i]);
	}
	delete lines;
	for (int i=0;i<Nwinlines;i++){
		delete(winlines[i]);
	}
	delete(winlines);
	for (int i=0;i<Nsymbols;i++){
		delete(paytable[i]);
	}
	delete(paytable);
}

void SlotMachine::set_win_lines_check(int win_lines_check){
	if (win_lines_check <= Nwinlines && win_lines_check > 0 ){
		this->win_lines_check = win_lines_check;
	}
	else{
		cout<<"Wrong numner of win lines to check"<<endl;
	}
}
void SlotMachine::printlines(bool is_string = true){
	// print lines:
	// [0][0] [0][1] [0][2] [0][3] [0][4] [0][2]: Top
	// [1][0] [1][1] [1][2] [1][3] [1][4] [1][2]: Middle
	// [2][0] [2][1] [2][2] [2][3] [2][4] [2][2]: Bottom
	std::cout<<endl;
	for (int i=0;i<3;i++){
		for (int j=0;j<Nreels;j++){
			if (is_string == true){
				cout<<setw(12)<<mapString[lines[i][j]]<<"("<<setw(2)<<lines[i][j]<<")\t";
			}
			else{
				cout<<setw(3)<<lines[i][j]<<"\t";
			}
		}
		cout<<endl;
	}	
	std::cout<<endl;
}
int SlotMachine::getbalance(){
	return balance;
}
int SlotMachine::getcoins(){
	return coins;
}
int SlotMachine::get_win_lines_check(){
	return win_lines_check;
}
