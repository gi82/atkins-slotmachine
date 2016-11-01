#ifndef REEL_H
#define REEL_H
class Reel {
	public :
		Reel();
		Reel(int); //construct real with (Nstops) stops
	   ~Reel();
		void  setstop(int); // takes what you see as an argument
		void  loadstops(int*);
		void  spin();
		int   getnstops();
		void  getlines(int*); 
		int   getstop(int);
		int*  getstops();
	private:
		int   Nstops;
		int   *stops;
		int   position;
		int   astop; // unique ID (0, Nstops]
};
#endif
