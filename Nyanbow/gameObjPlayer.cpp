#include <vector>
#include <algorithm>

#include "gameObjPlayer.h"
#include "data.h"
#include "tools.h"
#include "graphics.h"
#include "objMaster.h"
#include "io.h"
#include "nonLivingObj.h"

//sprites names
#define SPIWALK 0
#define SPIHEAD 1
#define SPISTAND 2
#define SPIJUMP 3
#define SPIFALL 4
#define SPIARMTOP 5
#define SPIARMBACK 6
#define SPIBOW 7
#define SPITRUSTFWR 8
#define SPITRUSTBK 9
#define SPIWALLHOLD 10

#define OFFSET_SPR_X 20

//#define SPIDEBUG  0

//dimenmsions
#define HEIGHT 62
#define WIDTH  26

#define SPRITEHEIGHT 64
#define SPRITEWIDTH  64

//hspeed
#define RUNNING    0.5f
#define RUNNINGMAX 2
#define SLOPE_TP_DOWN 5

//vspeed
#define JUMPHEIGHT 10
#define MAXJUMPS   2

//wall jumping
#define WALLJUMPHEIGHT 8
#define WALLJUMPLENGTH 5
#define WALL_JUMP_FREEZE 5

//thrusting
#define THRUSTSPEED 7.0f
#define SIN45 0.707f

#define THRUSTTIMERPICK  10
#define THRUSTTIMERTHRUST 7
#define THRUSTTIMERCOOLDOWN 30

#define THRUSTSTATENON  0
#define THRUSTSTATEPICK 1
#define THRUSTSTATETHRUSTING 2
#define THRUSTSTATERESET 3

//shooting
#define ATTACK_COOLDOWN 5

//facing
#define FACING_FROWARD 0
#define FACING_UP   1
#define FACING_DOWN 2

//reused code to make doPhysics() code more readable
#define SPFLOOR_ONEWAY  dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i] )->oneWay
#define SPFLOOR_CANFALL dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i] )->canFall
#define SPFLOOR_SLOPE   dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i] )->slope

ObjPlayer * ObjPlayer::player = NULL;


ObjPlayer::ObjPlayer( int x, int y )
{
	setDefaults();

	//save who you are for all to see
	player = this;

	//values to save to file
	saveX = x;
	saveY = y;

	//setup
	zone  = nyanZones::zoneAll;
	tag   = nyanTag::player;
	spTag = nyanSpTag::spPlayer;

	meta = false;
	dirFacing = nyanDirection::left;

	box.setX( x );
	box.setY( y );	
	box.setHeight( HEIGHT );
	box.setWidth ( WIDTH  );
	Graphics::follow = &box; //make the camara follow us

	wallTop   = false;
	wallRight = false;
	wallLeft  = false;
	onGround  = false;
	onSlopeLastFrame = false;

	thrustingAngleSpeed = (int)( THRUSTSPEED * SIN45 );
	
	//one way platforms values
	ignoreOneWay     = false;
	downKeyLastFrame = false;

	//physics
	hspeed = 0;

	vspeed = 0;
	maxHSpeed = RUNNINGMAX;

	physics  = true;
	gravity  = true;
	friction = true;

	onGround = false;
	solid    = false;

	//players abilities
	canTrust      = true;
	canDoubleJump = true;
	canWallJump   = true;

	//jump
	jumps = 0; //how many jumps you have done
	jumpedLastFrame = false;


	//thrusting
	thrustState = THRUSTSTATENON;
	thrustTimer = 0;

	//freezing
	froze = false;
	freezeTimer = 0;

	canIO = true; //turn off user IO

	//shooting values
	shootCoolDown = 0;
	
	#ifndef NDEBUG
	//debug
	debugOnce = true; //safe to del, used for debug
	#endif

	//graphics
	visible = true;

	//make a test sprite
	sprites = new std::vector<Draw*>;

	headFacing = FACING_FROWARD;

	bowGraphic = false;
	
	//real sprites
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );

	//setting the range on the sprite sheet of where things are
	sprites->at( SPIWALK ) -> drawSetFrames( 11, 14, 15, 1, false );
	sprites->at( SPIHEAD ) -> drawSetFrames( 7, 10,  0, 1, false ); //needs adjusting
	sprites->at( SPISTAND ) -> drawSetFrames( 15, 16,  15, 1, false );
	sprites->at( SPIJUMP ) -> drawSetFrames( 18, 19,  6, 1, false );
	sprites->at( SPIFALL ) -> drawSetFrames( 20, 21,  6, 1, false );
	sprites->at( SPIARMTOP ) -> drawSetFrames( 30, 32,  0, 1, false ); //needs adjusting
	sprites->at( SPIARMBACK ) -> drawSetFrames( 37, 39,  0, 1, false ); //needs adjusting 
	
	sprites->at( SPIBOW ) -> drawSetFrames( 4, 5, 18, 1, false ); 
	sprites->at( SPITRUSTFWR ) -> drawSetFrames( 22, 23, 10, 1, false );
	sprites->at( SPITRUSTBK ) -> drawSetFrames( 24, 25, 10, 1, false );
	sprites->at( SPIWALLHOLD ) -> drawSetFrames( 17, 17, 0, 0, false );
	//debug square
	//#ifndef NDEBUG
	//sprites->push_back(new Draw(2, x, y, HEIGHT, WIDTH, true));
	//#endif
}

void ObjPlayer::stepEvent()
{

	#ifndef NDEBUG
	//reset debug
	if ( IO::keyStart() )
	{
		box.setX( saveX );
		box.setY( saveY );
	}
	#endif

	//
	//key attack
	//
	if ( IO::keyShoot() && TOOLS::timerPassed( shootCoolDown ) )
	{
		//TOOLS::timedPause(60);

		if ( IO::keyDown() )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::down,  nyanSpTag::spDmgFishMelee, false ) );
		else 
		if ( IO::keyUp() )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::up,    nyanSpTag::spDmgFishMelee, false ) );
		else
		if ( dirFacing == nyanDirection::left )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::left,  nyanSpTag::spDmgFishMelee, false ) );
		else
		if ( dirFacing == nyanDirection::right )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::right, nyanSpTag::spDmgFishMelee, false ) );

		shootCoolDown = TOOLS::timerSet( ATTACK_COOLDOWN );
	}

	//looking direction
	if ( IO::keyDown() ) headFacing = FACING_DOWN;
	else
	if ( IO::keyUp() ) headFacing = FACING_UP;
	else
		headFacing = FACING_FROWARD;

	//
	//jumping
	//
	//disable double jump
	if ( !canDoubleJump && jumps == 0 )
		jumps = 1;

	//main jumping code
	if ( 
			( IO::keyJump() && canIO && !froze ) && 
			( onGround || jumps < MAXJUMPS )     && 
			!jumpedLastFrame 
	   )
	{
		vspeed = -JUMPHEIGHT;
		++jumps;
		jumpedLastFrame = true;
	}
	
	//reset jump key to allow you to have to tap jump twice
	if ( !IO::keyJump() )
		jumpedLastFrame = false;

	//reset jumps when you land
	if ( onGround )
	{
		jumps = 0;
		jumpedLastFrame = false;
	}
	else //only get one jump if you fall (without jumping) then jump
		if ( jumps == 0 )
			++jumps;


	//moving left/right
	if ( canIO && !froze )
	{
		//left
		if ( IO::keyLeft() && !IO::keyRight() )
		{
			//wall jumping
			if( wallRight && !onGround && canWallJump )
			{
				vspeed = -WALLJUMPHEIGHT;
				hspeed = -WALLJUMPLENGTH;

				freeze( WALL_JUMP_FREEZE, true, true, true, hspeed, true, vspeed );
				dirFacing = nyanDirection::right;
			}
			else
			{
				//running
				dirFacing = nyanDirection::left;
				hspeedAdderWithLimter( -RUNNING );
			}
		}

		//right
		if ( IO::keyRight() && !IO::keyLeft() )
		{
			//wall jumping
			if( wallLeft && !onGround && canWallJump )
			{
				vspeed = -WALLJUMPHEIGHT;
				hspeed =  WALLJUMPLENGTH;

				freeze( WALL_JUMP_FREEZE, true, true, true, hspeed, true, vspeed );
				dirFacing = nyanDirection::left;
			}
			else
			{
				//running
				dirFacing = nyanDirection::right;
				hspeedAdderWithLimter( RUNNING );
			}
		}

	}
	//end of left/right
	

	//down falling through platforms
	if ( ignoreOneWay ) ignoreOneWay = false;

	if ( IO::keyDown() && !downKeyLastFrame && !froze && canIO )
	{
		downKeyLastFrame = true;
		ignoreOneWay     = true;
	}

	if ( !IO::keyDown() )
		downKeyLastFrame = false;


	//thrusting
	//resetting thrust
	if ( thrustState == THRUSTSTATERESET && onGround ) 
		thrustState = THRUSTSTATENON;

	//thrusting start
	if ( IO::keyThrust() && !froze && canIO && canTrust &&
		thrustState == THRUSTSTATENON && TOOLS::timerPassed( thrustTimer ) )
	{
		//freeze player in place and set timer
		canIO   = false;
		physics = false;
		hspeed  = 0;
		vspeed  = 0;
		thrustState = THRUSTSTATEPICK;
		thrustTimer = TOOLS::timerSet( THRUSTTIMERPICK );
	}
	
	//let player choose thrust direction
	if ( thrustState == THRUSTSTATEPICK )
	{
		//no angle x axis
		if ( IO::keyDown() )
			vspeed =  THRUSTSPEED;

		if ( IO::keyUp()  ) 
			vspeed = -THRUSTSPEED;

		if ( IO::keyRight() )
			hspeed =  THRUSTSPEED;

		if ( IO::keyLeft()  ) 
			hspeed = -THRUSTSPEED;

		//angled
		if ( IO::keyDown() )
		{
			if( IO::keyRight() )
			{
				hspeed = (float)thrustingAngleSpeed;
				vspeed = (float)thrustingAngleSpeed;
			}

			if( IO::keyLeft() )
			{
				hspeed = (float)-thrustingAngleSpeed;
				vspeed = (float) thrustingAngleSpeed;
			}
		}

		if ( IO::keyUp() )
		{
			if( IO::keyRight() )
			{
				hspeed = (float) thrustingAngleSpeed;
				vspeed = (float)-thrustingAngleSpeed;
			}

			if( IO::keyLeft() )
			{
				hspeed = (float)-thrustingAngleSpeed;
				vspeed = (float)-thrustingAngleSpeed;
			}

		}

		//thrust player when timer goes out
		if ( TOOLS::timerPassed( thrustTimer ) )
		{
			//if no direction was picked, thrust backwards
			if ( vspeed == 0 && hspeed == 0 )
				hspeed = ( dirFacing == nyanDirection::left ) ? THRUSTSPEED : -THRUSTSPEED;

			//set state up for the thrusting
			thrustState = THRUSTSTATETHRUSTING;
			thrustTimer = TOOLS::timerSet( THRUSTTIMERTHRUST );

			canIO    = true;
			physics  = true;
			gravity  = directionMoving( nyanDirection::down ); //if going down let gravity help
			friction = false;
		}
				
	}
	
	//thrusting
	if ( thrustState == THRUSTSTATETHRUSTING && TOOLS::timerPassed( thrustTimer ) )
	{
		gravity     = true;
		friction    = true;
		thrustState = THRUSTSTATERESET;
		thrustTimer = TOOLS::timerSet( THRUSTTIMERCOOLDOWN );
	}


	//im not sure but I think this needs to be at the end
	unFreeze( false );
}

std::vector<int>* ObjPlayer::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( spTag );
	items->push_back( saveX );
	items->push_back( saveY );

	return items;
}

void ObjPlayer::doPhysics()
{
	if ( !physics ) return;

	//collision detection and movement
	int size, i, tempHspeed;
	float tempMaxFallSpeed;
	GameObj tempObj;
	bool    tempDir;	   //used to hold which direction you are going even after speed reset (fix bug in teleporting to wrong side of object when you hit a wall)
	hitbox  tempLeftRight; //used for left right wall detection

	//apply gravity if not over max fall speed, and only add up to max speed
	if ( gravity && ( vspeed < DATA::maxFall ) ) 
	{
		vspeed += DATA::gravity;

		//"hang" onto walls
		tempMaxFallSpeed = ( wallRight || wallLeft ) ? DATA::maxFallWall : DATA::maxFall;

		if ( vspeed > tempMaxFallSpeed ) vspeed = tempMaxFallSpeed;
	}

	//
	//check if you have moved from last frame for vertical
	//
	if ( boxLastFramePhysics.y != box.y ) 
	{
		wallTop  = false;
		onGround = false;
	}

	boxLastFramePhysics.copyBox( box );

	//
	//1 way wall reset
	//
	if ( box.x >= oneWayOn.right || box.right <= oneWayOn.x ) 
		oneWayOn.reset();

	//
	//slop animation fix
	//
	if ( vspeed <= 0 )
		onSlopeLastFrame = false;


	//
	//find vspeed cd first
	//
	box.setYRel( (int)ceil( vspeed ) ); //moves the player based on vpseed

	size = objMaster::solidList->size();
	tempDir = GameObj::directionMoving( nyanDirection::down );

	for( i = 0; i < size; ++i )
	{
		tempObj = *(*objMaster::solidList)[i];

		//
		//check collision on solid objects
		//
		if ( box.intercepts( tempObj.box ) && this != &tempObj )
		{

			//if its a slope jump to the top and skip the rest
			//this is really simple, it will push the player though solid objects going up
			if ( tempObj.spTag == nyanSpTag::spFloor && dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i] )->slope )
			{
				box.topOf( tempObj.box );
				onGround = true;
				vspeed   = 0;
				onSlopeLastFrame = true;
				continue;
			}
			else
				onSlopeLastFrame = false; //fix slop animation

			//wall bottom
			if ( tempDir ) //going down
			{

				//if you find a oneway plat while going down
				if ( tempObj.spTag == nyanSpTag::spFloor && SPFLOOR_ONEWAY )
				{
					//ignore plat if set to ignore
					if ( SPFLOOR_CANFALL && ignoreOneWay )
					{
						oneWayOn.reset();
						continue;
					}

					//check if we have see this plat before
					if( !tempObj.box.checkSameBox( oneWayOn ) )
					{
						//if not see if we approached it while falling from the top
						if ( ( box.bottom - ceil(vspeed) <= tempObj.box.y ) )
							oneWayOn.copyBox( tempObj.box ); //add this to the see plat and move onto normal movement code below
						else //if not skip this box and keep falling
							continue;
					}
					else //if we have see this box before treat it like a nomral box if we only moved at much as gravity is pushing us, if not fall
						if ( !( vspeed = DATA::gravity ) )
							continue;
				}
				else
					oneWayOn.reset(); //if we dont see it anymore froget it



				//move obj ontop of obj you fell on and set that you are on ground
				box.topOf( tempObj.box );
				onGround = true;
			}
			else //wall top
			{
				oneWayOn.reset(); //reset oneWay plat

				//ignore 1 way platforms 
				if ( tempObj.spTag == nyanSpTag::spFloor && SPFLOOR_ONEWAY ) 
					continue;

				box.bottomOf(tempObj.box);
				wallTop = true;
			}

			vspeed = 0;
		} //end of checking walls

	}//end of for loop


	//
	//hspeed cd
	//
	tempHspeed = (int)ceil( hspeed );

	box.setXRel( tempHspeed );
	tempDir = GameObj::directionMoving( nyanDirection::right );

	//assume no walls till you see one
	wallRight = false;
	wallLeft  = false;

	for( i = 0; i < size; ++i )
	{
		tempObj = *(*objMaster::solidList)[i];

		//ignore 1 way platforms and slopes
		if ( !( tempObj.spTag == nyanSpTag::spFloor && ( SPFLOOR_ONEWAY || SPFLOOR_SLOPE ) ) )
		{
			if
			(	//check collision on solid objects
				tempHspeed != 0  && //only do these checks if moving
				this != &tempObj && 
				box.intercepts( tempObj.box )
			)
			{
				if( tempDir ) //if going right
					box.leftOf( tempObj.box );  //wall right
				else
					box.rightOf( tempObj.box ); //wall left

				hspeed = 0;
			}
		
			//
			//check if there is a box to the left or right of you.
			//
			tempLeftRight.copyBox( box );

			tempLeftRight.setXRel(  1 );  //check right
			if ( tempLeftRight.intercepts( tempObj.box ) ) wallRight = true;

			tempLeftRight.setXRel( -2 ); //check left
			if ( tempLeftRight.intercepts( tempObj.box ) ) wallLeft  = true;
		}

	}


	//hspeed friction
	if ( friction )
	{
		float tempHspeed = fabs( hspeed );

		//make friction a % of your speed
		float tempFriction = std::max( 
			tempHspeed * ( onGround ? DATA::frictionGround : DATA::frictionAir ),  
			DATA::frictionMin );

		if ( tempHspeed - tempFriction < 0 )
			hspeed = 0;
		else
			hspeed = copysign( tempHspeed - tempFriction, hspeed );
	}
}

void ObjPlayer::draw()
{
	//if standing
	//if ( ceil( hspeed ) == 0 && ceil( vspeed ) == 0 )
	//	mainSprite = sprites->at( SPISTAND );
	//else
	//	if ( ( ceil( vspeed ) != 0 || !onGround ) && !onSlopeLastFrame ) //drawing in air sprites
	//		if ( directionMoving( nyanDirection::up ) )
	//			mainSprite = sprites->at( SPIJUMP );
	//		else
	//			mainSprite = sprites->at( SPIFALL );
	//	else //draw walking sprite
	//		mainSprite = sprites->at( SPIWALK );


	//note on the sprites that have no speed i should have set the increment amount to zero, so the offsets are weird

	//setting up body (base item things are based on)
	Draw * body;

	if ( abs( ceil( hspeed ) ) <= RUNNINGMAX )
	{
		if ( onGround )
		{
			if ( ceil( hspeed ) == 0 ) // waalking or standing
				body = sprites->at( SPISTAND );
			else
				body = sprites->at( SPIWALK );
		}
		else
		if ( wallRight || wallLeft ) //if holding onto wall
			body = sprites->at( SPIWALLHOLD );
		else
		{
			if ( directionMoving( nyanDirection::down ) ) //falling or jumping
				body = sprites->at( SPIFALL );
			else
				body = sprites->at( SPIJUMP );
		}
	}
	else //thrusting
	{
		if ( directionMoving( (nyanDirection)dirFacing ) )
			body = sprites->at( SPITRUSTFWR );
		else
			body = sprites->at( SPITRUSTBK );
	}

	
	body->drawUpdate();
	body->x = box.x - OFFSET_SPR_X;
	body->y = box.y;
	body->flip = ( dirFacing == nyanDirection::left ) ? true : false; //flip the sprite to dirFacing


	//setting up head
	Draw * head;
	head = sprites->at( SPIHEAD );

	if ( headFacing == FACING_UP ) head->frame = 7;
	if ( headFacing == FACING_FROWARD ) head->frame = 8;
	if ( headFacing == FACING_DOWN ) head->frame = 9;

	head->drawUpdate();
	head->x = box.x - OFFSET_SPR_X;

	if (onGround)
		head->y = box.y - ( body->frame % 2 == 0 ? 2 : 0 ) + ( body == sprites->at( SPISTAND ) ? 1 : 0 ) + 2; //move head with body
	else
		head->y = box.y;

	head->flip = ( dirFacing == nyanDirection::left ) ? true : false;


	//arm top
	Draw * arm;

	arm = sprites->at( SPIARMTOP );

	if ( headFacing == FACING_UP ) arm->frame = 32;
	if ( headFacing == FACING_FROWARD ) arm->frame = 30;
	if ( headFacing == FACING_DOWN ) arm->frame = 31;

	arm->drawUpdate();
	arm->x = box.x - OFFSET_SPR_X + ( ( dirFacing == nyanDirection::left ) ? -1 : 1 ) * ( ( ceil( hspeed ) != 0 ) ? 2 : 1 );

	if (onGround)
		arm->y = box.y - ( body->frame % 2 == 0 ? 2 : 0 ) + ( body == sprites->at( SPISTAND ) ? 2 : 0 ) + 2; //move head with body
	else
		arm->y = box.y;

	arm->flip = ( dirFacing == nyanDirection::left ) ? true : false;



	//arm back
	Draw * armBack;

	armBack = sprites->at( SPIARMBACK );

	if ( headFacing == FACING_UP ) armBack->frame = 39;
	if ( headFacing == FACING_FROWARD ) armBack->frame = 37;
	if ( headFacing == FACING_DOWN ) armBack->frame = 38;

	armBack->drawUpdate();
	armBack->x = box.x - OFFSET_SPR_X + ( ( dirFacing == nyanDirection::left ) ? -1 : 1 ) * ( ( ceil( hspeed ) != 0 ) ? 2 : 1 );

	if (onGround)
		armBack->y = box.y - ( body->frame % 2 == 0 ? 2 : 0 ) + ( body == sprites->at( SPISTAND ) ? 2 : 0 ) + 2; //move head with body
	else
		armBack->y = box.y;

	armBack->flip = ( dirFacing == nyanDirection::left ) ? true : false;
	

	//bow
	Draw * bow;

	bow = sprites->at( SPIBOW );

	bow->drawUpdate();
	bow->x = box.x - 18;

	if (onGround)
		bow->y = box.y - ( body->frame % 2 == 0 ? 2 : 0 ) + ( body == sprites->at( SPISTAND ) ? 2 : 0 ) + 2; //move head with body
	else
		bow->y = box.y;

	bow->flip = ( dirFacing == nyanDirection::left ) ? true : false;




	//send all the sprites off to be drawn, order matters
	if ( dirFacing == nyanDirection::left ) // bow only on one side
		Graphics::addDraw( bow );

	if ( dirFacing == nyanDirection::left ) //flip arms when switching directions
		Graphics::addDraw( arm );
	else
		Graphics::addDraw( armBack );

	Graphics::addDraw( head );
	Graphics::addDraw( body );
	
	if ( dirFacing == nyanDirection::left )
		Graphics::addDraw( armBack );
	else
		Graphics::addDraw( arm );
}


void ObjPlayer::freeze( int freezeTimer, bool setGravityFalse, bool setFrictionFalse, bool setHspeed, float newHspeed, bool setVspeed, float newVspeed )
{
	//saving settings for the unFreeze
	setGravity  = setGravityFalse;
	setFriction = setFrictionFalse;
	
	this->setHspeed = setHspeed;
	this->newHspeed = newHspeed;

	//freeze the obj
	if ( setHspeed ) hspeed = 0;
	if ( setVspeed ) vspeed = 0;

    this->setVspeed = setVspeed;
	this->newVspeed = newVspeed;
	
	this->freezeTimer = TOOLS::timerSet( freezeTimer );
	
	//Change these vaules if requested 
	if ( setGravity  ) this->gravity  = false;
	if ( setFriction ) this->friction = false;

	froze = true; 
}

void ObjPlayer::unFreeze( bool now )
{
	if ( !( now || TOOLS::timerPassed( freezeTimer ) ) ) return;
	if ( !froze ) return;

	if ( setGravity  ) this->gravity  = true;
	if ( setFriction ) this->friction = true;

	if ( setHspeed ) this->hspeed = newHspeed;
	if ( setVspeed ) this->vspeed = newVspeed;

	froze = false; 
}

ObjPlayer::~ObjPlayer()
{
	deleteDraw();
}

