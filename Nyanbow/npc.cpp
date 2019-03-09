#include <stdlib.h>

#include "npc.h"
#include "gameObjPlayer.h"
#include "objMaster.h"
#include "graphics.h"
#include "nonLivingObj.h"
#include "io.h"

//movement type
#define MOVEGROUND 0
#define MOVEAIR 1

#define SPRITEHEIGHT64 64
#define SPRITEWIDTH64 64

//bear monster sprites
#define BEAR_SPI_WALK 0

#define BEAR_OFFSET_SPR_X 20


//
//ObjEnemies
//

ObjEnemies::ObjEnemies( nyanSpTag type, nyanZones zone, int x, int y )
{
	setDefaults();

	startingX = x;
	startingY = y;

	this->zone = zone;
	this->tag  = nyanTag::enemies;
	activate();
	spTag  = type;
	
	//make enemy face wherever
	dirFacing = ( rand() % 2 == 0 ) ? nyanDirection::right : nyanDirection::left;

	box.setX( x );
	box.setY( y );
	
	sprites = new std::vector<Draw*>;

	switch( spTag )
	{
		case nyanSpTag::spMonstBrear:
			box.setWidth ( 30 );
			box.setHeight( 62 );
			moveType = MOVEGROUND;
			speed = 1;
			hp = 8;

			sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT64, SPRITEWIDTH64, DATA::depthMiddleGround, 0, 0 , true ) );
			sprites->at( BEAR_SPI_WALK ) -> drawSetFrames( (32 * 2) + 16, (32 * 2) + 16 + 3, 15, 1, false );
		break;

		#ifndef NDEBUG
		default:
			//enemy not known
			exit( 1 );
			break;
		#endif
	}

	//#ifndef NDEBUG
	//test box so i can see the enemies
	//sprites = new std::vector<Draw*>;
	//sprites->push_back( new Draw( 2, x, y, box.height, box.width, true ) );

	////used so I can toggle this lator on
	//visible = true;
	//#endif
}

void ObjEnemies::inactivate()
{
	active = false;
}

void ObjEnemies::activate()
{
	active = true;
}

std::vector<int>* ObjEnemies::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( tag );

	items->push_back( spTag );
	items->push_back( zone );
	items->push_back( startingX );
	items->push_back( startingY );

	return items;
}

void ObjEnemies::deadCheck()
{
	if ( hp < 1 ) objMaster::remove( this );
}

void ObjEnemies::doPhysics()
{
	if ( !active ) return;

	//move in the dirFacing
	hspeed = (float)( dirFacing == nyanDirection::left ? -speed : speed );
	box.setXRel( (int)hspeed ); 

	//
	//check collision
	//

	//hspeed
	//if hit a wall move other direction
	int i, size = objMaster::solidList->size();
	GameObj * tempObj;

	for( i = 0; i < size; ++i )
	{
		tempObj = ( *objMaster::solidList )[i];

		if ( this != tempObj && box.intercepts( tempObj->box ) )
		{
			if ( directionMoving( nyanDirection::left ) )
			{
				dirFacing = nyanDirection::right;
				box.rightOf( tempObj->box );
			}
			else //right
			{
				dirFacing = nyanDirection::left;
				box.leftOf( tempObj->box );
			}
		}
	}


	//
	//vspeed
	//
	//gravity
	if ( vspeed <  DATA::maxFall )
		 vspeed += DATA::gravity;

	box.setYRel( ( int )ceil( vspeed ) );

	for( i = 0; i < size; ++i )
	{
		tempObj = ( *objMaster::solidList )[i];

		if ( box.intercepts( tempObj->box ) && this != tempObj )
		{
			if ( directionMoving( nyanDirection::up ) )
			{
				vspeed = 0;
				box.bottomOf( tempObj->box );
			}
			else //down
			{
				vspeed = 0;
				box.topOf   ( tempObj->box );

				//if about to fall off edge then turn around
				if ( box.x      <=  tempObj -> box.x     ) dirFacing = nyanDirection::right;
				else
				if ( box.right  >=  tempObj -> box.right ) dirFacing = nyanDirection::left;
			}
		}
	}



}

void ObjEnemies::draw()
{
	if ( spTag == nyanSpTag::spMonstBrear )
	{
		Draw * bear;

		bear = sprites->at( BEAR_SPI_WALK );

		bear->x = box.x;
		bear->y = box.y;

		bear->flip = ( dirFacing == nyanDirection::left ) ? false : true;
		bear->drawUpdate();
		Graphics::addDraw( bear );
	}

	//#ifndef NDEBUG
	//if ( !visible ) return;

	//sprites->at(0)->width  = box.width;
	//sprites->at(0)->height = box.height;
	//sprites->at(0)->x = box.x;
	//sprites->at(0)->y = box.y;
	//Graphics::addDraw( sprites->at(0) );
	//#endif
}
