#include "Instance.h"

Instance::Instance( GeomType* geom )
	: Orientation( m_modelMatrix )
{
	this->visible = true;
	this->distance = 0;
	this->lastFrame = 0;
	
	this->geom = geom;
}

Instance::~Instance()
{
}

float
Instance::recalculateDistance()
{
	// TODO: implement

	//Vector wTranfCenter( this->geom->getCenter() );
	//this->transform.matrix.transform( wTranfCenter );
	
	//this->distance = wTranfCenter.length();
	this->distance = 0;
	
	return this->distance;
}
