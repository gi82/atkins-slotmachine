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
#include "AtkinsMachine.h"

using namespace std;
void printhelp();
char payfilename[200];
char outfilename[200];
bool help=false;
int  Nwin=1;
int  print_every=10000000;
bool out_to_file = false;
int  stat = false;
int  sim = false;
int  game = false;
int  balance = 1000;
int  N=1000000;

int main(int argc,char *argv[]){
	// Variable declarations
	strcpy(payfilename,"paytable.dat");
	strcpy(outfilename,"outfile.dat");
	
	
	for (int i=1;i<argc;i++) {
		if (!strcmp(argv[i],"-payfile"))      strcpy(payfilename, argv[++i]);
		else if (!strcmp(argv[i],"-help"))    help = true;
		else if (!strcmp(argv[i],"-game"))    game = true;
		else if (!strcmp(argv[i],"-balance")) balance = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-stat"))    stat = true;
		else if (!strcmp(argv[i],"-sim"))     sim = true;
		else if (!strcmp(argv[i],"-Nwin"))    Nwin = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-N"))       N = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-out")){
			strcpy(outfilename, argv[++i]);
			out_to_file = true;
		}
		else if (!strcmp(argv[i],"-print")) print_every = atoi(argv[++i]);   
		else {
		  fprintf(stderr,"Error.  Argument '%s' is not recognized.\n", argv[i]);
		  exit(-1);
		}
	}
	if (help){
		printhelp();
		exit(0);
	}
	if (!stat && !game && !sim){stat=true;}
	if (stat) {game = false; sim = false;} // default
	else if (game && sim) {sim = true ; game = false;} 
	ostream *fout;
	if (out_to_file){
		fout = new ofstream(outfilename,std::ofstream::app);
	}else{
		fout = &cout;
	}
	double total_return;	
	AtkinsMachine *myMachine = new AtkinsMachine();
	myMachine->read_and_set(payfilename);
	myMachine->print_paytable();
	myMachine->printwinlines();
	if (sim){
		myMachine->run_simulation(N, Nwin, print_every,fout);
	}
	if (stat){
		myMachine->run_statistics(Nwin,print_every,fout);
	}
	if (sim || stat){
		myMachine->calc_prob();
		myMachine->print_statistics(fout);
		myMachine->calc_print_return(fout,total_return);
		myMachine->calc_scatter_prob();
		myMachine->print_scatter_statistics(fout);
	}
	if (game){
		cout<<std::string(38,'*')<<endl;
		cout<<"*   Atkins slot machine game is ON   *"<<endl;
		cout<<std::string(38,'*')<<endl;
		int coins   = 5;
		myMachine->set_win_lines_check(Nwin);
		myMachine->add_to_balance(balance);
		myMachine->set_coins(coins);
		cout<<"You have: "<<myMachine->getbalance()<<" coins on your balance"<<endl;
		cout<<"You default bet is: "<< myMachine->getcoins()<<" coins"<<endl;
		cout<<"Playing with: "<<myMachine->get_win_lines_check()<<" win lines"<<endl;
		cout<<std::string(38,'*')<<endl;
		bool play = true;
		bool game_over = false;
		std::string again;
		while (play){
			game_over = myMachine->play(Nwin);
			if (game_over) break;
			cout<<"Play again??? ( y (yes) or n(no) ) "<<endl;
			getline(cin,again);
			if (again.compare("Y")==0 || again.compare("y")==0) play = true;
			else if (again.compare("N")==0 || again.compare("n")==0) play = false;
			else if (again.empty()) play=true;
			else play=false;
		}
	}
	
	delete(myMachine);
}

void printhelp(){
	cout<<"******Atkins Slot Machine Help*********"<<endl;
	cout<<"By default the program run game evaluation by looping over"<<endl;
	cout<<"all possible combinations using \"paytable.dat\" as an input"<<endl;
	cout<<"One win line is considered"<<endl<<endl;
	cout<<"-help: prints this help"<<endl;
	cout<<"-payfile [filename]: specify input file (default paytable.dat)"<<endl;
	cout<<"Options"<<endl;
	cout<<"Type:(default -stat) provide options one at a time"<<endl;
	cout<<"  -sim: simulation"<<endl;
	cout<<"  -game: play game"<<endl;
	cout<<"  -stat: run game statistics"<<endl;
	cout<<"-Nwin [integer] : number of win lines to consider"<<endl;
	cout<<"                  (all) types"<<endl;
	cout<<"-N [integer]: number of simulation steps(only for sim)"<<endl;
	cout<<"              -sim , -stat types"<<endl;
	cout<<"-print [integer] : print every (print) games"<<endl;
	cout<<"                   -sim , -stat types"<<endl;
	cout<<"-balance [integer]: initial balance"<<endl;
	cout<<"                   -game"<<endl;
}
