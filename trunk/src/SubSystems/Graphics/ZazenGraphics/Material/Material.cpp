/*
 * Material.cpp
 *
 *  Created on: Feb 25, 2011
 *      Author: jonathan
 */

#include "Material.h"

#include <iostream>

#include "../../../../Core/Utils/XML/tinyxml.h"

using namespace std;

std::map<std::string, Material*> Material::allMaterials;

bool
Material::loadAll()
{
	string fullFileName = "media/graphics/materials/materials.xml";

	TiXmlDocument doc( fullFileName.c_str() );

	if ( false == doc.LoadFile() )
	{
		cout << "ERROR ... could not load file " << fullFileName << " - reason = " << doc.ErrorDesc() << " at row = " << doc.ErrorRow() << " col = " << doc.ErrorCol() << endl;
		return false;
	}

	TiXmlElement* rootNode = doc.FirstChildElement("materials");
	if ( 0 == rootNode )
	{
		cout << "ERROR ... root-node \"materials\" in " << fullFileName << " not found" << endl;
		return false;
	}

	for (TiXmlElement* materialNode = rootNode->FirstChildElement(); materialNode != 0; materialNode = materialNode->NextSiblingElement())
	{
		const char* str = materialNode->Value();
		if (str == 0)
			continue;

		if ( 0 == strcmp(str, "material") )
		{
			std::string name;
			std::string type;

			str = materialNode->Attribute( "name" );
			if ( 0 == str )
			{
				cout << "No name for material - will be ignored" << endl;
				continue;
			}
			else
			{
				name = str;
			}

			TiXmlElement* materialTypeNode = materialNode->FirstChildElement("type");
			if ( 0 == materialTypeNode )
			{
				cout << "ERROR ... node \"type\" for material " << name << " not found" << endl;
				return false;
			}

			str = materialNode->Attribute( "id" );
			if ( 0 == str )
			{
				cout << "No name for material - will be ignored" << endl;
				continue;
			}
			else
			{
				type = str;
			}

			Material* material = new Material( name, type );

			for (TiXmlElement* materialCfgNode = materialTypeNode->FirstChildElement(); materialCfgNode != 0; materialCfgNode = materialCfgNode->NextSiblingElement())
			{
				str = materialCfgNode->Value();
				if (str == 0)
					continue;

				if ( 0 != strcmp(str, "diffuseColor1") )
				{
					str = materialCfgNode->Attribute( "file" );
					if ( 0 != str )
					{
						Texture* texture = Texture::load( str );
						if ( texture )
						{
							material->m_diffuseTextures[ 0 ] = texture;
						}
					}
				}
				else if ( 0 != strcmp(str, "diffuseColor2") )
				{
					str = materialCfgNode->Attribute( "file" );
					if ( 0 != str )
					{
						Texture* texture = Texture::load( str );
						if ( texture )
						{
							material->m_diffuseTextures[ 1 ] = texture;
						}
					}
				}
				else if ( 0 != strcmp(str, "diffuseColor3") )
				{
					str = materialCfgNode->Attribute( "file" );
					if ( 0 != str )
					{
						Texture* texture = Texture::load( str );
						if ( texture )
						{
							material->m_diffuseTextures[ 2 ] = texture;
						}
					}
				}
				else if ( 0 != strcmp(str, "diffuseColor4") )
				{
					str = materialCfgNode->Attribute( "file" );
					if ( 0 != str )
					{
						Texture* texture = Texture::load( str );
						if ( texture )
						{
							material->m_diffuseTextures[ 3 ] = texture;
						}
					}
				}
				else if ( 0 != strcmp(str, "normalMap") )
				{
					str = materialCfgNode->Attribute( "file" );
					if ( 0 != str )
					{
						Texture* texture = Texture::load( str );
						if ( texture )
						{
							material->m_normalMap = texture;
						}
					}
				}
				else if ( 0 != strcmp(str, "color") )
				{
					glm::vec3 color;

					str = materialCfgNode->Attribute( "r" );
					if ( 0 != str )
					{
						color.r = atof( str );
					}

					str = materialCfgNode->Attribute( "g" );
					if ( 0 != str )
					{
						color.g = atof( str );
					}

					str = materialCfgNode->Attribute( "b" );
					if ( 0 != str )
					{
						color.b = atof( str );
					}

					material->m_color = color;
				}

				Material::allMaterials.insert( make_pair( material->m_name, material ) );
			}
		}
	}

	return true;
}

Material*
Material::get( const std::string& name )
{
	map<string, Material*>::iterator findIter = Material::allMaterials.find( name );
	if (findIter != Material::allMaterials.end())
		return findIter->second;

	return 0;
}

bool
Material::activate()
{
	// bind diffuse textures to tex-units 0 - N

	for ( int i = 0; i < MAX_DIFFUSE_TEXTURES; i++ )
	{
		Texture* texture = this->m_diffuseTextures[ i ];

		if ( texture )
			texture->bind( i );
	}

	if ( this->m_normalMap )
		this->m_normalMap->bind( MAX_DIFFUSE_TEXTURES );

	return true;
}

Material::Material( const std::string& name, const std::string& type )
	: m_name( name ),
	  m_type( type )
{
	this->m_normalMap = 0;

	for ( int i = 0; i < MAX_DIFFUSE_TEXTURES; i++ )
	{
		this->m_diffuseTextures.push_back( 0 );
	}
}

Material::~Material()
{
	// don't delete textures, they are shared
}
