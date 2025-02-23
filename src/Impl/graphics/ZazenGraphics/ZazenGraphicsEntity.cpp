#include "ZazenGraphicsEntity.h"
#include "ZazenGraphics.h"

#include <core/ICore.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace std;
using namespace boost;

ZazenGraphicsEntity::ZazenGraphicsEntity( IGameObject* p )
	: IGraphicsEntity( p ),
	m_type( "graphics" ),
	Orientation( m_modelMatrix )
{
	this->m_activeAnimation = NULL;

	this->m_visible = true;
	this->m_distance = 0;
	this->m_lastFrame = 0;
	
	this->m_rootMeshNode = NULL;
	this->m_material = NULL;

	this->m_light = NULL;
	this->m_camera = NULL;

	this->m_isAnimated = false;
	this->m_animRoll = 0;
	this->m_animPitch = 0;
	this->m_heading = 0;
}

ZazenGraphicsEntity::~ZazenGraphicsEntity()
{
	// m_allAnimations IS OWNED by this entity and NOT by AnimationFactory
	// AnimationFactory hands out clones because animations are unique per instance
	// unlike meshes which are "static" and can be shared for all instances (for now...)
	for ( unsigned int i = 0; i < this->m_allAnimations.size(); i++ )
	{
		delete this->m_allAnimations[ i ];
	}
}

void
ZazenGraphicsEntity::matrixChanged()
{
	Event e( "POSITION_CHANGED" );
	e.addValue( "matrix", glm::value_ptr( this->getModelMatrix() ) );
	e.setTarget( this->getParent() );

	ZazenGraphics::getInstance().getCore().getEventManager().postEvent( e );

	if ( this->m_camera )
	{
		this->m_camera->setModelMatrix( this->getModelMatrix() );
	}

	if ( this->m_light )
	{
		this->m_light->setModelMatrix( this->getModelMatrix() );
	}
}

void
ZazenGraphicsEntity::update()
{
	if ( this->m_isAnimated )
	{
		if ( 0.0f != this->m_heading )
		{
			this->changeHeading( this->m_heading * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}

		if ( 0.0f != this->m_animRoll )
		{
			this->changeRoll( this->m_animRoll * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
	
		if ( 0.0f != this->m_animPitch )
		{
			this->changePitch( this->m_animPitch * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
	}

	list<int>::iterator pressedKeysIter = this->m_keyDown.begin();
	while ( pressedKeysIter != this->m_keyDown.end() )
	{
		int keyCode = *pressedKeysIter++;
		
		// Q
		if ( 16 == keyCode )
		{
			this->changeRoll( 50.0f * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
		// E
		else if ( 18 == keyCode )
		{
			this->changeRoll( -50.0f * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
		// W
		else if ( 17 == keyCode )
		{
			this->strafeForward( -100.0f * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
		// S
		else if ( 31 == keyCode )
		{
			this->strafeForward( 100.0f * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
		// A
		else if ( 30 == keyCode )
		{
			this->strafeRight( -100.0f * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
		// D
		else if ( 32 == keyCode )
		{
			this->strafeRight( 100.0f * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		}
	}

	if ( this->m_activeAnimation )
	{
		this->m_activeAnimation->update();
	}
}

bool
ZazenGraphicsEntity::sendEvent( Event& e )
{
	// process immediately because no call to graphics api, just coping of data (yet)
	if ( e == "UPDATE_PHYSICS" )
	{
		boost::any& pos = e.getValue( "pos" );
		boost::any& rot = e.getValue( "rot" );

		this->setOrientation( any_cast<const float*>( pos ), any_cast<const float*>( rot ) );

		return true;
	}
	else if ( e == "KEY_PRESSED" )
	{
		int keyCode = any_cast<int>( e.getValue( "key" ) );

		this->m_keyDown.push_back( keyCode );
	}
	else if ( e == "KEY_RELEASED" )
	{
		int keyCode = any_cast<int>( e.getValue( "key" ) );

		this->m_keyDown.remove( keyCode );
	}
	else if ( e == "MOUSE_MOVED" )
	{
		int x = any_cast<int>( e.getValue( "x" ) );
		int y = any_cast<int>( e.getValue( "y" ) );

		this->changeHeading( -50.0f * x * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
		this->changePitch( 50.0f * y * ZazenGraphics::getInstance().getCore().getProcessingFactor() );
	}

	return false;
}

void
ZazenGraphicsEntity::setOrientation( const float* pos, const float* rot )
{
	if ( this->m_camera )
	{
		this->m_camera->setRaw( rot, pos );
	}
	else
	{
		this->setRaw( rot, pos );
	}
}

void
ZazenGraphicsEntity::setAnimation( float heading, float roll, float pitch )
{
	this->m_isAnimated = true;
	this->m_heading = heading;
	this->m_animRoll = roll;
	this->m_animPitch = pitch;
}

void
ZazenGraphicsEntity::recalculateDistance( const glm::mat4& viewMatrix )
{
	// calculate model-view for this instance
	glm::mat4 modelView = viewMatrix * this->m_modelMatrix;
	// calculate center of instance in view-space
	glm::vec4 vsCenter = modelView * glm::vec4( this->m_rootMeshNode->getCenter(), 1.0 );

	// distance from the viewer is the z-component of the center
	this->m_distance = vsCenter.z;
}
