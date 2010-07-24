/*
 * PlaygroundAudio.cpp
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#include "PlayGroundAudio.h"

#include <iostream>
#include <stdio.h>

using namespace std;

PlayGroundAudio::PlayGroundAudio()
	: id ("PlayGroundAudio"),
	  type ("audio")
{
	this->system = 0;
	this->bgMusic = 0;
	this->bgMusicCh = 0;
}

PlayGroundAudio::~PlayGroundAudio()
{

}

bool
PlayGroundAudio::initialize()
{
	cout << endl << "=============== PlayGroundAudio initializing... ===============" << endl;

	FMOD_RESULT result;
	unsigned int version;

	result = FMOD::System_Create( &this->system );
    if (result != FMOD_OK)
    {
        printf( "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result) );
       	return false;
    }

    result = this->system->getVersion( &version );
    if (result != FMOD_OK)
    {
        printf( "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result) );
       	return false;
    }

    if (version < FMOD_VERSION)
    {
        printf( "Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION );
        return false;
    }

    result = this->system->init( 10, FMOD_INIT_NORMAL, 0 );
    if (result != FMOD_OK)
    {
        printf( "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result) );
       	return false;
    }

    result = system->createSound("media/audio/getout.ogg", FMOD_SOFTWARE | FMOD_2D, 0, &this->bgMusic);
    if (result != FMOD_OK)
    {
    	printf( "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result) );
    }

	cout << "================ PlayGroundAudio initialized =================" << endl;

	return true;
}

bool
PlayGroundAudio::shutdown()
{
	cout << endl << "=============== PlayGroundAudio shutting down... ===============" << endl;

	FMOD_RESULT result;

	if ( this->bgMusic )
	{
		result = this->bgMusic->release();
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		}
	}

	if ( this->system )
	{
		result = this->system->close();
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		}

		result = this->system->release();
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		}
	}

	cout << "================ PlayGroundAudio shutdown =================" << endl;

	return true;
}

bool
PlayGroundAudio::start()
{
	FMOD_RESULT result;

    result = system->playSound(FMOD_CHANNEL_FREE, this->bgMusic, false, &this->bgMusicCh);
    if (result != FMOD_OK)
	{
    	printf( "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result) );
    	return false;
    }

	return true;
}

bool
PlayGroundAudio::stop()
{
	return true;
}

bool
PlayGroundAudio::pause()
{
	return true;
}

bool
PlayGroundAudio::process(double factor)
{
	cout << "PlayGroundAudio::process" << endl;

	return true;
}

bool
PlayGroundAudio::finalizeProcess()
{
	cout << "PlayGroundAudio::finalizeProcess" << endl;

	return true;
}

bool
PlayGroundAudio::sendEvent(const Event& e)
{
	return true;
}

PlayGroundAudioEntity*
PlayGroundAudio::createEntity()
{
	return new PlayGroundAudioEntity();
}
