#ifndef NDEBUG
	//#include <iostream>
	#include <SFML/System.hpp>
#endif

//homemade
#include "data.h" //must be loaded first
#include "tools.h"
#include "objMaster.h"
#include "baseGameObj.h"
#include "gameObjPlayer.h"
#include "nonLivingObj.h"
#include "npc.h"

//use SFML libs
#include "io.h"
#include "graphics.h"
//#include "audio.h"

#ifndef NDEBUG
	//outside game tools (still uses SFML libs)
	#include "imgToMap.h"
#endif

int main( int argc, char* argv[] )
{
	//get exe location to find resource files on windows
	//must be ran before loading resources
	#ifdef _WIN32
		std::string argv_str( argv[0] );
		DATA::startingLocation = argv_str.substr( 0, argv_str.find_last_of( "\\" ) ) + "\\";
	#endif

	//test code to test the image to map tool
	#ifndef NDEBUG
		//TOMAP::startConvert( DATA::startingLocation );
		//return 0;
	#endif


	//test data setup
	Graphics::loadGraphics(); //load starting textures

	objMaster::update( nyanZones::zone01 ); //set starting zone
	objMaster::updateZone(); //do the work to set the zone

	objMaster::addFromFile( DATA::getDataFileName() ); //loading level objects

	objMaster::add( new ObjEnemies( nyanSpTag::spMonstBrear, nyanZones::zone01, 450, 100 ) );
	
	//varables used in main loop
	unsigned int i, size;
	size = objMaster::inZone->size();

	//main game loop, space quits the game
	while ( !IO::keyExit() )
	{
		//update game timer
		TOOLS::timerUpdate();

		if ( TOOLS::inPause() )
		{
			//
			// main game loop
			//

			//step
			for ( i = 0; i < size; ++i )
				(*objMaster::inZone)[i]->stepEvent();

			objMaster::removeMidEvent();      //ran after an event to clean up items that remove themselves during the event
			size = objMaster::inZone->size(); //check size for removed elements

			objMaster::updateZone();          //check if a trigger zone switch


			//attack
			for ( i = 0; i < size; ++i )
				(*objMaster::inZone)[i]->attackEvent();


			//dead check, what should you do if you got hit
			for ( i = 0; i < size; ++i )
				(*objMaster::inZone)[i]->deadCheck();
		
			//clean up the dead
			objMaster::removeMidEvent();
			size = objMaster::inZone->size();


			//physics
			for ( i = 0; i < size; ++i )
				(*objMaster::inZone)[i]->doPhysics();


			//get draw requests
			for ( i = 0; i < size; ++i )
				(*objMaster::inZone)[i]->draw();

		//draw
		Graphics::pullWindowsEvents();
		Graphics::updateCamara();
		Graphics::draw();

		}
		else
		{
			//
			//paused
			//

			Graphics::pullWindowsEvents();   //keep pulling events to make sure window does not freeze
			sf::sleep(sf::milliseconds(16)); //hold for ~ 60fps
		}

		
		#ifndef NDEBUG
			//slow down game for debug
			//sf::sleep(sf::milliseconds(100));
		#endif
	};

	return 0;
}
