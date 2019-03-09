#ifndef NYAN_NPC
#define NYAN_NPC

#include "baseGameObj.h"

//all the monsters you can fight
class ObjEnemies : public GameObj
{

	//for saving functions
	int startingX, startingY;

	//how the enemy will move, ground or sky
	int moveType, speed;

	//used to set if active if player is close or something else like that
	bool active;

	//what to do when inactive and active
	void inactivate();
	void activate();


	public:

		ObjEnemies( nyanSpTag type, nyanZones zone, int x, int y );

		std::vector<int>* saveFile();

		void deadCheck();

		void doPhysics();

		void draw();

};

#endif