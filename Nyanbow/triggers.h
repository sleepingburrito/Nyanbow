#ifndef NYAN_TRIGGERS
#define NYAN_TRIGGERS

#include "baseGameObj.h"

//
//triggers
//when the player is inside this object (or presses a key if set) it does a event
//todo: have a global save state that saves to file (hint use spTags)

//these enums are only used inside the trigger
//if its been activated
enum triggerStates
{
	triggerWaiting,  //waiting to be active
	triggerInEvent,  //doing something
	triggerNonActive //completed
};

//when its activated what state is the event in
enum triggerEvents
{
	//default value
	trigEvNon,

	//spTestTrigger
	//this event shows a test dialog message
	trigEvTestDialogWait
};

class ObjTrigger : public GameObj
{
	triggerStates state;
	triggerEvents eventState;

	bool keyActivated;

	//temp varables for events
	bool dialogDone;

public:
	//when keyActivated the user has to press down to activate the trigger
	ObjTrigger( nyanSpTag type, nyanZones zone, int x, int y, int height, int width, bool keyActivated );

	void stepEvent();

	std::vector<int>* saveFile();

	void draw();
};

#endif