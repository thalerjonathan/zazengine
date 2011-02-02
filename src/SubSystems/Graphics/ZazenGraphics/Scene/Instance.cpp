#include "Instance.h"

Instance::Instance( GeomType* geom )
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
	Vector wTranfCenter( this->geom->getCenter() );
	this->transform.matrix.transform( wTranfCenter );
	
	this->distance = wTranfCenter.length();
	
	return this->distance;
}
