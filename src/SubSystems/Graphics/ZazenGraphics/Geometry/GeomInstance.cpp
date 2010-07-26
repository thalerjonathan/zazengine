#include "GeomInstance.h"

GeomInstance::GeomInstance( GeomType* geom )
{
	this->visible = true;
	this->distance = 0;
	this->lastFrame = 0;
	
	this->geom = geom;
	this->parent = 0;
}

GeomInstance::~GeomInstance()
{
}

float
GeomInstance::recalculateDistance()
{
	Vector wTranfCenter(this->geom->getCenter());
	this->transform.transform(wTranfCenter);
	
	this->distance = wTranfCenter.length();
	
	return this->distance;
}
