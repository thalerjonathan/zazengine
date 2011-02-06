#include "Instance.h"

Instance::Instance( GeomType* geom )
{
	this->visible = true;
	this->distance = 0;
	this->lastFrame = 0;
	
	this->geom = geom;

	this->m_modelMatrix = 0;
}

Instance::~Instance()
{
	delete this->m_modelMatrix;
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
