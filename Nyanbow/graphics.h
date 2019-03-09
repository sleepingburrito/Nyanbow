#ifndef NYAN_GRAPHICS
#define NYAN_GRAPHICS

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <list>
#include <string>

#include "tools.h"

//handles all graphics, makes the window and takes in a list of sprites
//and draws them to that window. This uses SFML library
//must manually add textures in constructor

namespace Graphics
{
	//master list of items to draw
	extern std::list< Draw* > drawList;

	//carmara settings
	//set these values to use the camara
	extern int offsetX, offsetY; //set these to move the camara
	extern int CamX, CamY;	     //offset of the camara from the view offset, i will default this to middle of the view 
	extern hitbox* follow;       //hitbox the camara will follow


	//loading graphics
	void loadGraphics();
	void swapTexture( std::string filePath, int textureId ); //swaps that texture with a new file
	int  getTextureWidth( int textureId );

	//used to add draw objects to be drawn
	//this list is cleared after draw is called
	//camara should be updated first as to not leave objects out when checking if they are in view
	void addDraw( Draw * drawObj );

	//gets the events of the window
	//needs to be called once in a while to make sure window is alive, mostly used around draw event
	//These events are not used in my game
	void pullWindowsEvents();

	//this will update the camara location, should be called before the draw() but after the hitbix
	//your following has moved
	//this centers the hitbox it follows on screen
	//Note: Only call this once you have a hitbox set
	void updateCamara();

	//takes a vector of draw objects and draws
	//call this once perframe at the end
	void draw();
}

#endif
