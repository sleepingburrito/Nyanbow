#include "triggers.h"
#include "gameObjPlayer.h"
#include "io.h"
#include "objMaster.h"
#include "graphics.h"
#include "nonLivingObj.h"

//
//ObjTrigger
//
ObjTrigger::ObjTrigger( nyanSpTag type, nyanZones zone, int x, int y, int height, int width, bool keyActivated )
{
	setDefaults();

	box.setX( x );
	box.setY( y );
	box.setWidth ( width  );
	box.setHeight( height );

	this->zone = zone;
	this->keyActivated = keyActivated;
	spTag = type;
	tag   = nyanTag::triggers;

	state      = triggerStates::triggerWaiting;
	eventState = triggerEvents::trigEvNon;


#ifndef NDEBUG
	//test sprite
	sprites = new std::vector<Draw*>;
	sprites->push_back( new Draw( 4, x, y, box.height, box.width, true ) );

	//used so I can toggle this lator on
	visible = true;
#endif

}

void ObjTrigger::stepEvent()
{
	//activate
	if ( state == triggerStates::triggerWaiting && box.intercepts( ObjPlayer::player->box ) )
	{
		if ( keyActivated )
			if ( IO::keyDown() ) 
				state = triggerStates::triggerInEvent;
			else
				state = triggerStates::triggerInEvent;
	}

	if ( keyActivated && IO::keyDown() )

		//dont move on unless you are in an event
		if ( state != triggerStates::triggerInEvent ) return;


	//decode what to do when active
	if ( spTag == nyanSpTag::spTestTrigger )
	{
		//
		//test dialog start
		//
		switch ( eventState )
		{
		case triggerEvents::trigEvNon:

			//make a dialog and then set event to wait on it
			dialogDone = false;

			//make it so the player cant move
			ObjPlayer::player->canIO = false; 
			ObjPlayer::player->freeze( 1, false, false, true, 0, true, 0 );

			objMaster::add( new ObjDialog( "[I00Please read this first line!\n[E[I01[X01This is line 2 test. end bye!", &dialogDone ) );
			eventState = triggerEvents::trigEvTestDialogWait;

			break;

		case triggerEvents::trigEvTestDialogWait:

			//end this trigger when dialog is done
			if ( dialogDone ) 
			{
				ObjPlayer::player->canIO = true;
				state = triggerStates::triggerNonActive; //this might be useless
				objMaster::remove( this );
			}


			break;

		default:
			exit( 1 );
			break;
		}
		//
		//test dialog end
		//

	}
#ifndef NDEBUG
	else
	{
		//Trigger not known
		exit( 1 );
	}
#endif

}

std::vector<int>* ObjTrigger::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( tag );

	items->push_back( spTag );
	items->push_back( zone  );
	items->push_back( box.x );
	items->push_back( box.y );
	items->push_back( box.width  );
	items->push_back( box.height );
	items->push_back( keyActivated );

	return items;
}

void ObjTrigger::draw()
{
#ifndef NDEBUG
	if ( !visible ) return;
	Graphics::addDraw( sprites->at(0) );
#endif
}
