#include "MaterialDoom3.h"

#include "../Program/UniformManagement.h"

using namespace std;

bool
MaterialDoom3::activate( Program* currentProgramm )
{
	if ( this->m_diffuseTexture )
	{
		this->m_diffuseTexture->bind( 0 );
	}

	if ( this->m_specularTexture )
	{
		this->m_specularTexture->bind( 1 );
	}

	if ( this->m_normalMap )
	{
		this->m_normalMap->bind( 2 );
	}

	currentProgramm->activateSubroutine( "doom3Material", Shader::FRAGMENT_SHADER );

	return true;
}

MaterialDoom3::MaterialDoom3( const std::string& name )
	: Material( name, Material::MATERIAL_DOOM3 )
{
	this->m_diffuseTexture = NULL;
	this->m_specularTexture = NULL;
	this->m_normalMap = NULL;
}

MaterialDoom3::~MaterialDoom3()
{
}
