#ifndef ATKINS_H
#define ATKINS_H
class AtkinsMachine:public SlotMachine{
	public:
		AtkinsMachine();
	   ~AtkinsMachine();
		void run_statistics(int,int,std::ostream*);
		void run_simulation(int,int,int,std::ostream*);
		void print_statistics(std::ostream *);
		void calc_print_return(std::ostream *, double &);
		void calc_scatter_prob();
		void print_scatter_statistics(std::ostream*);
		void calc_prob();
		int  read_and_set(const char[]);
		bool play(int);
	private:
		enum Symbols{
			Atkins=0,	
			Steak,
			Ham,
			BuffaloWings,	
			Sausage,	
			Eggs,	
			Butter,	
			Cheese,	
			Bacon,	
			Mayonnaise,
			Scale,
			None
		};
		long int **Hits; //hits per symbol Hits[0][5]
		double   **Prob; //probebility matrix
		long int stat_games_count;
		long int stat_lines_count;
		int game_wins_line,game_wins_scatter,bonus_win_times;
		int ncomb; // total number of combinations
		void  mapInitialize();
		void  checkline(int*,int,int&);
		void  checkline(int *,int ,int&,int&,int &);
		bool  check_scatter(int&);
};
#endif
