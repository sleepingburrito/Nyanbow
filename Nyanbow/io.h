#ifndef NYAN_IO
#define NYAN_IO

//Functions return true when key is down, false if not
//these functions use the SFML library

namespace IO
{
	//main action
	bool keyJump();
	bool keyShoot();
	bool keyThrust();

	//directional
	bool keyUp();
	bool keyDown();
	bool keyLeft();
	bool keyRight();

	//services
	bool keyStart();
	bool keyExit();

	//range is zero to 100. Zero is center.
	extern int deadZone;
}

#endif
