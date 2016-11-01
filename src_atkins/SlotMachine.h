#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H
class SlotMachine{
	public:
		SlotMachine(int,int,int,int);
	   ~SlotMachine();
		void set_win_lines_check(int );
		void setreels(int*);
		void loadlines();
		void spinreels();
		void printlines(bool);
		void setcol(int,int*);
		void setpaytable(std::string,int*,int);
		void setwinline(int,int*);
		void printwinlines();
		void add_to_balance(int);
		bool set_bet(int);
		void set_coins(int);
		void loadreelstrip(int,std::string* strip); 
		void print_paytable();
		void getwinline(int,int*);
		void printreelstrips();
		int  getbalance();
		int  getcoins();
		int  get_win_lines_check();
	protected:
		std::map<std::string, int> mapSymbol;
		std::map<int,std::string> mapString;
		int   Nreels;
		int   Nsymbols;
		int   Nstops;
		int   Nwinlines;
		int **lines;
		int **winlines;
		int   win_lines_check; // default game checks one line
		int **paytable;
		int   balance; // total amount of coins
		int   wager;
		int   coins;   // coins to be played 
		// array of pointers to Reel ( no default constructor for Reel)
		Reel **reels;
		// array of symbols
};

#endif
