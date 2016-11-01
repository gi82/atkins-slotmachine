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
template<typename T> void splitString(std::string s, std::string delimiter,T* arr,int maxsize){
	size_t pos = 0;
	std::string token;
	int i=0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
	    token = s.substr(0, pos);
		// trim begin and end of token
		token.erase(0, token.find_first_not_of(' ')); //prefix	
		token.erase(token.find_last_not_of(' ')+1);   //suffix
		// end trim
	    //std::cout << token << std::endl;
	    if (token.empty()) cout<<"token empty"<<endl;
	    stringstream ss(token);
		if (i < maxsize+1){
	    	ss >> arr[i++];
		}else{
			cout<<"maxsize exceeded on string split"<<endl; }
	    s.erase(0, pos + delimiter.length());
	}
	//std::cout << s << std::endl;
};
AtkinsMachine::AtkinsMachine():SlotMachine(5,11,32,20){
	cout<<"I am Atkins"<<endl;
	win_lines_check = 1;
	Hits = new long int*[Nsymbols];
	Prob = new double*[Nsymbols];
	ncomb = Nreels * Nreels * Nreels * Nreels *Nreels;
	for (int i=0;i<Nsymbols;i++){
		Hits[i] = new long int[Nreels];	
		Prob[i] = new double[Nreels];
	}
	for (int i=0;i<Nsymbols;i++){
		for (int j=0;j<Nreels;j++){
			Hits[i][j]=0;
			Prob[i][j]=0.;
		}
	}

	mapInitialize();
}
AtkinsMachine::~AtkinsMachine(){}
int AtkinsMachine::read_and_set(const char infilename[]){
	std::string fileline,*symbol_array,*reels_array,*winlines_array,*paytable_array;// temporary to read 
	ifstream in;
	symbol_array = new std::string[Nsymbols];
	reels_array = new std::string[Nstops];
	winlines_array = new std::string[Nwinlines];
	paytable_array = new std::string[Nreels+1];
	int *paytable = new int[Nreels];
	
	in.open(infilename,std::ios::in);

	getline(in,fileline);
	for (int i=0;i<Nsymbols;i++){
		getline(in,fileline);
		symbol_array[i] = fileline;
		//cout<<symbol_array[i]<<endl;
	}
	// Read Reels
	getline(in,fileline);
	for (int i=0;i<Nreels;i++){
		getline(in,fileline);
		reels_array[i] = fileline; 	
		//cout<<reels_array[i]<<endl;
	}
	// Read winlines configurations
	getline(in,fileline);
	for (int i=0;i<Nwinlines;i++){
		getline(in,fileline);
		winlines_array[i] = fileline; 	
		//cout<<winlines_array[i]<<endl;
	}
	in.close();

	for (int i=0;i<Nsymbols;i++){
		splitString(symbol_array[i],";",paytable_array,Nreels+1);
		for (int k=0;k<Nreels;k++) paytable[k] = atoi( paytable_array[k+1].c_str()) ;
			setpaytable(paytable_array[0],paytable,5);
	}
	string arra[Nstops];
	for (int i=0;i<Nreels;i++){
		splitString(reels_array[i],";",arra,Nstops);
		for (int j=0;j<Nstops;j++){
			loadreelstrip(i,arra);
		}
		//cout<<endl;
	}
	int win[Nreels];
	for (int i=0;i<Nwinlines;i++){
		splitString(winlines_array[i],";",win,Nreels);
		setwinline(i,win);
	}
	// Read
	return 0;
}
bool AtkinsMachine::play(int win_lines_play){
	bool game_over = false;
	int *l = new int[Nreels];
	int pay_total = 0;
	int winsymbol,wintimes;
	int freespins = 0;
	if (!set_bet(win_lines_play)){
		game_over = true;
		return game_over;
	}
	if (balance > 0 ){
		spinreels();
		printlines(true);
		int pay_line, pay_scatter;
		pay_total = 0;
		for (int ln =0;ln<win_lines_check;ln++){
			getwinline(ln,l);
			checkline(l,this->Nreels,pay_line,winsymbol,wintimes);
			if (wintimes>0 && winsymbol!=None){
				cout<<"Winline "<<ln<<"(";
				for (int ll=0;ll<Nreels;ll++) cout<<setw(2)<<this->winlines[ln][ll]<<" ";
				cout<<") score: "<<setw(2)<<wintimes+1<<" "<<setw(16)<<mapString[winsymbol]<<"  in a row"<<endl;
				cout<<"Pay:"<<pay_line<<endl;
				winsymbol = None;
				wintimes = -1;
			}
			pay_total  += pay_line;
		}
		if ( check_scatter(pay_scatter) == true ){
			freespins += 10;
			pay_total +=  pay_scatter;
		}
		balance += coins * pay_total;
		cout<<"Total pay="<<pay_total<<" Balance="<<balance<<endl;
		pay_total = 0;
		if ( freespins > 0 ){
			cout<<"You have won "<<freespins<<" freespins\n";
			cout<<"Starting free spin game"<<endl;
			while (freespins > 0){
				wintimes = -1;winsymbol = None;
				spinreels();
				cout<<"Free spins left:"<<freespins<<endl;
				for (int ln =0;ln<win_lines_check;ln++){
					getwinline(ln,l);
					checkline(l,this->Nreels,pay_line,winsymbol,wintimes);
					if (wintimes>0 && winsymbol!=None){
						cout<<"Winline "<<ln<<"(";
						for (int ll=0;ll<Nreels;ll++) cout<<setw(2)<<this->winlines[ln][ll]<<" ";
						cout<<") score: "<<setw(2)<<wintimes+1<<" "<<setw(16)<<mapString[winsymbol]<<"  in a row"<<endl;
						cout<<"Pay:"<<pay_line<<endl;
						winsymbol = None;
						wintimes = -1;
					}
					pay_total  += pay_line;
				}
				if ( check_scatter(pay_scatter) == true ){
					freespins += 10;
					pay_total +=  pay_scatter;
				}
				freespins-=1;;
			}
			balance += 3* coins * pay_total;
			cout<<"End of free spins"<<endl;
			cout<<"Total free spin pay:"<<3*pay_total<<"( 3 x "<< pay_total<<")"<<endl;
			
		}
		game_over = false;
	}
	else{
		game_over = true;
		cout<<" GAME OVER (balance ="<<balance<<")\n";
	}
	delete (l);
	return game_over;
}
void AtkinsMachine::mapInitialize(){
	mapSymbol["Atkins"]=Atkins;	
	mapSymbol["Steak"]=Steak;
	mapSymbol["Ham"]=Ham;
	mapSymbol["BuffaloWings"]=BuffaloWings;
	mapSymbol["Sausage"]=Sausage;	
	mapSymbol["Eggs"]=Eggs;
	mapSymbol["Butter"]=Butter;
	mapSymbol["Cheese"]=Cheese;
	mapSymbol["Bacon"]=Bacon;	
	mapSymbol["Mayonnaise"]=Mayonnaise;
	mapSymbol["Scale"]=Scale;
	mapSymbol["None"]=None;
	
	mapString[Atkins]="Atkins";	
	mapString[Steak]="Steak";
	mapString[Ham]="Ham";
	mapString[BuffaloWings]="BuffaloWings";
	mapString[Sausage]="Sausage";	
	mapString[Eggs]="Eggs";
	mapString[Butter]="Butter";
	mapString[Cheese]="Cheese";
	mapString[Bacon]="Bacon";	
	mapString[Mayonnaise]="Mayonnaise";
	mapString[Scale]="Scale";
	mapString[None]="None";
}
void AtkinsMachine::run_statistics(int win_lines_in,int print_every,std::ostream* fout){
	if (win_lines_in > 0){
		set_win_lines_check(win_lines_in);
		(*fout)<<"\nCalculate Statistics\n"<<endl;
		(*fout)<<"check: "<<win_lines_check<<" win lines"<<endl;
		(*fout)<<"output every:"<<print_every<<" steps"<<endl;
		int winsymbol=None;
		int wintimes=-2;
		int pay_line=0, pay_scatter=0;
		game_wins_line=0;
		game_wins_scatter=0;
		stat_lines_count=0;
		stat_games_count= 0;
		bonus_win_times = 0;
		int *reelstop = new int[Nreels];
		int *l        = new int[Nreels];
		for (int i=0;i<Nsymbols;i++){
			for (int j=0;j<Nreels;j++){
				Hits[i][j]=0;
			}
		}
		for (int s1=0;s1<Nstops;s1++){
			reelstop[0] = s1+1;
			for (int s2=0;s2<Nstops;s2++){
				reelstop[1] = s2+1;
				for (int s3=0;s3<Nstops;s3++){
					reelstop[2] = s3+1;
					for (int s4=0;s4<Nstops;s4++){
						reelstop[3] = s4+1;
						for (int s5=0;s5<Nstops;s5++){
							reelstop[4] = s5+1;
							setreels(reelstop);
							loadlines();
							for (int ln =0;ln<win_lines_check;ln++){
								getwinline(ln,l);
								checkline(l,Nreels,pay_line,winsymbol,wintimes);
								if ( (winsymbol!=None) && (wintimes>=0)){
									Hits[winsymbol][wintimes] += 1;
								}
								if ( pay_line > 0 ) game_wins_line++;
								stat_lines_count++;
							}
							if (check_scatter(pay_scatter) ) bonus_win_times+=1;
							stat_games_count++;
							if (pay_scatter > 0 ) game_wins_scatter++;
							if ( print_every > 0){
								if (stat_games_count % print_every == 0) {
									calc_prob();
									calc_scatter_prob();
									print_statistics(fout);
								}
							}
						}
					}
				}
			}
		}
		(*fout)<<std::string(100,'*')<<endl;
		(*fout)<<std::string(100,'*')<<endl;
		(*fout)<<"Total line combinations:"<<stat_lines_count<<endl;
		(*fout)<<"games_win="<<(double)(game_wins_line )/ (double)stat_lines_count<<endl;
		(*fout)<<"Prob bonus="<<(double)bonus_win_times / (double) stat_games_count<<endl;
		(*fout)<<"game_win_scatter="<<(double)game_wins_scatter / (double) stat_games_count<<endl;
		(*fout)<<"Hit rate: "<<(double) stat_games_count / (double) game_wins_line<<endl;
		(*fout)<<std::string(100,'*')<<endl;
		(*fout)<<std::string(100,'*')<<endl;
		delete(reelstop);
		delete(l);
	}
	else{
		cout<<"Cannot run statistics, number of win lines defined is:"<<win_lines_check<<endl;
	}
}
void AtkinsMachine::run_simulation(int times, int win_lines_in,int print_every,std::ostream* fout){
	if (win_lines_in > 0){
		set_win_lines_check(win_lines_in);
		(*fout)<<"\nRunning Simulations of:"<<setw(10)<<times<<" steps"<<endl;
		(*fout)<<"check: "<<win_lines_check<<" win lines"<<endl;
		(*fout)<<"output every:"<<print_every<<" steps"<<endl;
		int *l = new int[Nreels];
		int winsymbol, wintimes;
		int pay_line=0, pay_scatter=0;
		game_wins_line=0;
		game_wins_scatter=0;
		stat_lines_count=0;
		stat_games_count= 0;
		bonus_win_times = 0;
		// Initialize probabilite and hits tables
		for (int i=0;i<Nsymbols;i++){
			for (int j=0;j<Nreels;j++){
				Hits[i][j]=0;
				Prob[i][j]=0;
			}
		}
		for (int t=0; t < times; t++){
			spinreels();
			for (int ln =0;ln<win_lines_check;ln++){
				getwinline(ln,l);
				checkline(l,Nreels,pay_line,winsymbol,wintimes);
				if ( (winsymbol!=None) && (wintimes>=0)){
					Hits[winsymbol][wintimes] += 1;
				}
				if ( pay_line > 0 ) game_wins_line++;
				stat_lines_count++;
			}
			if (check_scatter(pay_scatter) ) bonus_win_times+=1;
			stat_games_count++;
			if (pay_scatter > 0 ) game_wins_scatter++;
			if (stat_games_count % print_every == 0) {
				calc_prob();
				calc_scatter_prob();
				print_statistics(fout);
			}
		}
		(*fout)<<std::string(100,'*')<<endl;
		(*fout)<<std::string(100,'*')<<endl;
		(*fout)<<"Total line combinations:"<<stat_lines_count<<endl;
		(*fout)<<"games_win="<<(double)(game_wins_line )/ (double)stat_lines_count<<endl;
		(*fout)<<"Prob bonus="<<(double)bonus_win_times / (double) stat_games_count<<endl;
		(*fout)<<"game_win_scatter="<<(double)game_wins_scatter / (double) stat_games_count<<endl;
		(*fout)<<"Hit rate: "<<(double) stat_games_count / (double) game_wins_line<<endl;
		(*fout)<<std::string(100,'*')<<endl;
		(*fout)<<"END OF SIMULATION"<<endl;
		(*fout)<<std::string(100,'*')<<endl;
		delete(l);
		
	}
}
void AtkinsMachine::print_statistics(std::ostream *fout){
	// Not for SCATTER Symbol
	(*fout)<<"Statistics"<<endl;
	(*fout)<<"Combinations for each win"<<endl;
	(*fout)<<"Step:"<<setw(20)<<stat_games_count<<endl;
	int total_col[Nreels];
	int total_row[Nreels];
	int total_row_total=0;
	double total_col_prob[Nsymbols];
	double total_row_prob[Nsymbols];
	double total_prob_row_total=0.;
	
	std::string s;
	for (int i=1; i < Nreels; i++){
		total_col[i] = 0;
		total_col_prob[i] = 0.;
		for (int j=0; j < Nsymbols-1; j++){	
			total_col[i] += Hits[j][i];
			total_col_prob[i] += Prob[j][i];
		}
	}	
	for (int j=0; j < Nsymbols-1; j++){	
		total_row[j] = 0;
		total_row_prob[j] =0.;
		for (int i=1; i < Nreels; i++){
			total_row[j] +=Hits[j][i];
			total_row_prob[j]+= Prob[j][i];
		}
	}	

	(*fout)<<"\t Symbol\t\t"<<"5 in a row     4 in a row   3 in a row   2 in a row"<<endl;
	(*fout)<<string(100,'*')<<endl;
	for (int i=0;i<Nsymbols-1;i++){
		s = mapString[i];
		(*fout)<<setw(16)<<s<<" ";
		for (int j=Nreels-1;j>0 ;j--){
			(*fout)<<setw(13)<<Hits[i][j]<<std::fixed<<"("<<setw(10)<<setprecision(8)<<Prob[i][j]<<") ";
		}
		(*fout)<<setw(13)<<total_row[i]<<std::fixed<<"("<<setw(10)<<setprecision(8)<<total_row_prob[i]<<") "<<endl;
	}
	(*fout)<<string(100,'*')<<endl;
	(*fout)<<setw(16)<<"Total:";
	for (int i=Nreels - 1;i > 0;i--){
		(*fout)<<setw(13)<<total_col[i]<<"("<<std::fixed<<setw(10)<<setprecision(8)<<total_col_prob[i]<<") ";
	}
	for (int i=0;i<Nsymbols-1;i++){
		total_row_total += total_row[i];
		total_prob_row_total += total_row_prob[i];
	}
	(*fout)<<setw(13)<<total_row_total<<"("<<std::fixed<<setw(10)<<setprecision(8)<<total_prob_row_total<<") ";
	(*fout)<<endl;
	(*fout).flush();

}
void AtkinsMachine::calc_prob(){
	if (stat_lines_count >0){
		for (int i=0;i<Nsymbols;i++){
			for (int j=0;j<Nreels;j++){
				Prob[i][j]= (double)Hits[i][j] / (double)stat_lines_count;
			}
		}
	}
}
void AtkinsMachine::calc_print_return(std::ostream* fout,double &total_return_value){
	// 
	(*fout)<<"RETURN TABLE"<<endl;
	double *total_return;
	double total_return_row;
	double total_return_col;
	std::string s;
	total_return = new double[Nreels];
	for (int i=1; i < Nreels; i++){
		total_return[i] = 0.;
		for (int j=0; j < Nsymbols-1; j++){	
			total_return[i] +=Prob[j][i]*paytable[j][i];
		}
	}	
	(*fout)<<"Return of each win"<<endl;
	(*fout)<<"\t Symbol"<<"  "<<"5 in a row 4 in a row 3 in a row 2 in a row"<<endl;
	(*fout)<<string(84,'*')<<endl;
	for (int i=0;i<Nsymbols-1;i++){
		s = mapString[i];
		(*fout)<<setw(16)<<s<<" ";
		total_return_row =0.;
		for (int j=Nreels-1;j>0 ;j--){
			(*fout)<<std::fixed<<setw(7)<<setprecision(5)<<Prob[i][j]*paytable[i][j]<<" ";
			total_return_row += Prob[i][j]*paytable[i][j];
		}
		(*fout)<<std::fixed<<setw(7)<<setprecision(5)<<total_return_row<<endl;;
	}
	(*fout)<<string(84,'*')<<endl;
	(*fout)<<setw(16)<<"Total:";
	total_return_col = 0;
	for (int i=Nreels - 1;i > 0;i--){
		(*fout)<<std::fixed<<setw(7)<<setprecision(5)<<total_return[i]<<" ";
		total_return_col+=total_return[i];
	}
	(*fout)<<std::fixed<<setw(7)<<setprecision(5)<<total_return_col<<endl;
	total_return_value = total_return_col;
	delete(total_return);
}
bool AtkinsMachine::check_scatter(int& pay){
	int ret_value = false;
	// Check all visible lines for scatter symbols
	int count_scatter = 0;
	for (int j=0;j<Nreels;j++){
		for (int i=0;i<3;i++){
			if ( lines[i][j] == Scale ){
				count_scatter ++;
			}
		}
	}	
	pay = paytable[Scale][count_scatter];
	Hits[Scale][count_scatter-1]+=1;
	// triple the prize according to the number of lines played
	pay *= win_lines_check;
	if (count_scatter >=3 ) ret_value = true; 
	return ret_value;	
}
void AtkinsMachine::calc_scatter_prob(){
	for (int i=0;i<Nreels ;i++){
		Prob[Scale][i] = (double) Hits[Scale][i] / (double)stat_games_count;
	}
}
void AtkinsMachine::print_scatter_statistics(std::ostream* fout){
	(*fout)<<"Scatter Pays"<<endl;
	(*fout)<<string(80,'*')<<endl;
	(*fout)<<"Total Scatters   Pays   Combinations  Probability  Return"<<endl;
	for (int i=Nreels -1 ;i >=0;i--){
		(*fout)<<setw(14)<<i+1<<setw(6)<<paytable[Scale][i]<<setw(10)<<Hits[Scale][i]<<"\t"<<std::fixed<<setw(10)<<setprecision(6)<<Prob[Scale][i]<<setw(10)<<setprecision(6)<<Prob[Scale][i]*paytable[Scale][i]<<endl;
	}
	
}
void AtkinsMachine::checkline(int *l,int lsize,int&pay){
	//all winning are left aligned
	int left_sym,first_wild_pos=-1;
	int inrow,inrow2,loop;
	int tmp_sym;
	int wilds[Nreels];
	bool foundwild = false;
	int countwild=0;
	std::string s;
	pay = 0;
	// CHECK IF LIE HAS AT LEAST ON WILD CARD
	int j=0;
	for (int i=0;i<lsize;i++) {
		if (l[i]==Atkins){
			countwild++;
			wilds[j++]=i;
			if (!foundwild){
				first_wild_pos = i;
				foundwild = true;
			}
		}
	}
	// if at least one symbol is present
	if (countwild>0){	
		if (countwild == 5){ // five wild in a row
			pay = paytable[Atkins][4];
			Hits[Atkins][4]+=1;
		}	
		else if (countwild ==1 ||countwild == 2||countwild == 3 || countwild ==4 ){ // place 3 WILD(w) to 5 reels(r) C(r,w) = (P [r,w])/w! ) 
			//check until first wildpos
			if ( l[0]!= Atkins  ){ // if line doesn't start with WILD all cases except all WILD in row
				if ( l[0] != Scale){
					tmp_sym = l[0];
					inrow=1; // calculates how many l[0] are in row
					// check all previous if they are the same
						loop=inrow;
						while (l[loop] == tmp_sym || l[loop] == Atkins){ 
							inrow++;
							loop++;	
							if (loop == Nreels) break;
						}
					pay = paytable[ l[0] ][inrow-1];
					if (pay >0){
						Hits[ l[0] ][inrow-1]+=1;
					}
				}
			}
			else if (l[0] == Atkins){
				// 1 WILD in row :-)
				inrow = 1;
				loop  = inrow; // first card index to check
				while ( ( l[loop] == Atkins ) && ( loop < Nreels ) ) {
					inrow++;
					loop++;
				}
				// at this point the pay should be (inrow) Atkins in row
				pay = paytable[ Atkins ][inrow-1];
				// checkpay with the rest 
				if ( l[inrow] != Scale  ){
					tmp_sym = l[inrow]; // first symbol after (inrow) WILD symbols
					loop   = inrow;
					inrow2 = inrow;
					while (l[loop] == tmp_sym || l[loop] == Atkins){ 
						inrow2++;
						loop++;	
						if (loop == Nreels) break;
					}
					if (  inrow2 > inrow && paytable[ tmp_sym ][inrow2-1] > pay ){
						if (paytable[ tmp_sym ][inrow2-1] >0){
							pay = paytable[ tmp_sym ][inrow2-1];
							Hits[ tmp_sym ][inrow2-1]+=1;
						}
					}
					else{ // hit  WILD in row
						if (pay >0){
							Hits[Atkins][inrow-1]+=1;
						}
					}
				}
				else{ // if SCATTER then pay WILD in row
					if (pay > 0)
						Hits[Atkins][inrow-1]+=1;
				}
			}
			else{
				pay = 0;
			}
			
		}
	}
	// if NO wild symbol is present
	else {
		left_sym = l[0];
		if (left_sym !=Scale){
			if (l[1] == left_sym){
				if (l[2]!=left_sym){// 2 in a row
					pay=paytable[ left_sym ][1];
					if ( pay > 0 ) Hits[ left_sym ][1]+=1;
				}
				else if (l[2]==left_sym){ 
					if ( l[3]!=left_sym ){ // 3 in a row
						pay=paytable[ left_sym ][2];
						if ( pay > 0 ) Hits[ left_sym ][2]+=1;
					}
					else if ( l[3]==left_sym ){
						if ( l[4] != left_sym ){ //4 in a row
							pay=paytable[ left_sym ][3];	
							if ( pay > 0 ) Hits[ left_sym ][3]+=1;
						}
						else if ( l[4] == left_sym ){ // 5 in a row
							pay=paytable[ left_sym ][4];
							if ( pay > 0 )Hits[ left_sym ][4]+=1;
						}
					}
				}
			}	
		}
		else{
			pay =0;
		}
	}
}
void AtkinsMachine::checkline(int *l,int lsize,int&pay,int &winsymbol,int &wintimes){
	//all winning are left aligned
	winsymbol = None;
	wintimes  = -1;
	int left_sym,first_wild_pos=-1;
	int inrow,inrow2,loop;
	int tmp_sym;
	int wilds[Nreels];
	bool foundwild = false;
	int countwild=0;
	std::string s;
	pay = 0;
	// CHECK IF LIE HAS AT LEAST ON WILD CARD
	int j=0;
	for (int i=0;i<lsize;i++) {
		if (l[i]==Atkins){
			countwild++;
			wilds[j++]=i;
			if (!foundwild){
				first_wild_pos = i;
				foundwild = true;
			}
		}
	}
	// if at least one symbol is present
	if (countwild>0){	
		if (countwild == 5){ // five wild in a row
			pay = paytable[Atkins][4];
			winsymbol = Atkins;
			wintimes = 4;
			//Hits[Atkins][4]+=1;
		}	
		else if (countwild ==1 ||countwild == 2||countwild == 3 || countwild ==4 ){ // place 3 WILD(w) to 5 reels(r) C(r,w) = (P [r,w])/w! ) 
			//check until first wildpos
			if ( l[0]!= Atkins  ){ // if line doesn't start with WILD all cases except all WILD in row
				if ( l[0] != Scale){
					tmp_sym = l[0];
					inrow=1; // calculates how many l[0] are in row
					// check all previous if they are the same
						loop=inrow;
						while (l[loop] == tmp_sym || l[loop] == Atkins){ 
							inrow++;
							loop++;	
							if (loop == Nreels) break;
						}
					pay = paytable[ l[0] ][inrow-1];
					if (pay >0){
						winsymbol = l[0];
						wintimes  = inrow - 1;
						//Hits[ l[0] ][inrow-1]+=1;
					}
				}
			}
			else if (l[0] == Atkins){
				// 1 WILD in row :-)
				inrow = 1;
				loop  = inrow; // first card index to check
				while ( ( l[loop] == Atkins ) && ( loop < Nreels ) ) {
					inrow++;
					loop++;
				}
				// at this point the pay should be (inrow) Atkins in row
				pay = paytable[ Atkins ][inrow-1];
				// checkpay with the rest 
				if ( l[inrow] != Scale  ){
					tmp_sym = l[inrow]; // first symbol after (inrow) WILD symbols
					loop   = inrow;
					inrow2 = inrow;
					while (l[loop] == tmp_sym || l[loop] == Atkins){ 
						inrow2++;
						loop++;	
						if (loop == Nreels) break;
					}
					if (  inrow2 > inrow && paytable[ tmp_sym ][inrow2-1] > pay ){
						if (paytable[ tmp_sym ][inrow2-1] >0){
							pay = paytable[ tmp_sym ][inrow2-1];
							winsymbol = tmp_sym;
							wintimes  = inrow2 - 1;
							//Hits[ tmp_sym ][inrow2-1]+=1;
						}
					}
					else{ // hit  WILD in row
						if (pay >0){
							winsymbol = Atkins;
							wintimes  = inrow - 1;
							//Hits[Atkins][inrow-1]+=1;
						}
					}
				}
				else{ // if SCATTER then pay WILD in row
					if (pay > 0)
						winsymbol = Atkins;
						wintimes  = inrow - 1;
						//Hits[Atkins][inrow-1]+=1;
				}
			}
			else{
				winsymbol = None;
				wintimes  = -1;
				pay = 0;
			}
			
		}
	}
	// if NO wild symbol is present
	else {
		left_sym = l[0];
		if (left_sym !=Scale){
			if (l[1] == left_sym){
				if (l[2]!=left_sym){// 2 in a row
					pay=paytable[ left_sym ][1];
					if ( pay > 0 ) {
						winsymbol = left_sym;
						wintimes  = 1;
						//Hits[ left_sym ][1]+=1;
					}
				}
				else if (l[2]==left_sym){ 
					if ( l[3]!=left_sym ){ // 3 in a row
						pay=paytable[ left_sym ][2];
						if ( pay > 0 ){
							winsymbol = left_sym;
							wintimes  = 2;
							//Hits[ left_sym ][2]+=1;
						}
					}
					else if ( l[3]==left_sym ){
						if ( l[4] != left_sym ){ //4 in a row
							pay=paytable[ left_sym ][3];	
							if ( pay > 0 ){
								// Hits[ left_sym ][3]+=1;
								winsymbol = left_sym;
								wintimes  = 3;
							}
						}
						else if ( l[4] == left_sym ){ // 5 in a row
							pay=paytable[ left_sym ][4];
							if ( pay > 0 ){
								winsymbol = left_sym;
								wintimes  = 4;
								//Hits[ left_sym ][4]+=1;
							}
						}
					}
				}
			}	
		}
		else{
			winsymbol = None;
			wintimes = -1;
			pay =0;
		}
	}
}
