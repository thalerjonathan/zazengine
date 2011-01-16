/*
 *  material.cpp
 *  ZENgine
 *
 *  Created by Jonathan Thaler on 23.04.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Material.h"

#include "../../../../Core/Utils/XML/tinyxml.h"

#include <iostream>

using namespace std;

map<std::string, Material*> Material::allMaterials;

bool
Material::loadMaterials()
{
	string fullFileName = "media/graphics/materials/materials.xml";
	
	TiXmlDocument doc(fullFileName.c_str());
		
	if (doc.LoadFile() == false) {
		cout << "ERROR ... could not load file " << fullFileName << " - reason = " << doc.ErrorDesc() << endl;
		return false;
	}

	TiXmlElement* materialsNode = doc.FirstChildElement("materials");
	if (materialsNode == 0) {
		cout << "ERROR ... root-node \"materials\" in " << fullFileName << " not found" << endl;
		return false;
	}

	for (TiXmlElement* materialNode = materialsNode->FirstChildElement(); materialNode != 0; materialNode = materialNode->NextSiblingElement()) {
		const char* str = materialNode->Value();
		if (str == 0)
			continue;
		
		if (strcmp(str, "material") == 0) {
			string materialName;
			bool transparent = false;

			const char* str = materialNode->Attribute("name");
			if (str == 0) {
				cout << "No name defined for Material - will be ignored" << endl;
				continue;
			} else {
				materialName = str;
			}
			
			str = materialNode->Attribute("transparent");
			if (str != 0) {
				if (strcasecmp("true", str) == 0)
					transparent = true;
			}

			cout << endl << "LOADING ... Material \"" << materialName << "\"" << endl;
			
			Material* newMaterial = new Material(materialName);
			newMaterial->transparent = transparent;
			
			TiXmlElement* texturesNode = materialNode->FirstChildElement("textures");
			if (texturesNode != 0) {
				for (TiXmlElement* textureNode = texturesNode->FirstChildElement(); textureNode != 0; textureNode = textureNode->NextSiblingElement()) {
					const char* str = textureNode->Value();
					if (str == 0)
						continue;

					if (strcmp(str, "texture") == 0) {
						string texFile;
						int targetID = 0;

						str = textureNode->Attribute("file");
						if (str == 0) {
							cout << "file of texture not set - ignoring texture" << endl;
							continue;
						} else {
							texFile = str;


						str = textureNode->Attribute("targetID");
						if (str == 0) {
							cout << "targetID of texture \"" << texFile << "\" not set - ignoring texture" << endl;
							continue;
						} else
							targetID = atoi(str);
						}

						cout << "New Texture \"" << texFile << "\" with texID " << targetID << " in Material \"" << materialName << "\"" << endl;

						Texture* tex = Texture::load(texFile);
						newMaterial->textures[tex] = targetID;
					}
				}
			}

			TiXmlElement* shaderNode = materialNode->FirstChildElement("shader");
			if (shaderNode != 0) {
				string vertextSourceFile;
				string fragmentSourceFile;

				str = shaderNode->Attribute("vertex");
				if (str != 0) {
					vertextSourceFile = str;
				} else {
					cout << "Shader doesn't define a Vertex-File - ignoring Material \"" << materialName << "\"" << endl;
					delete newMaterial;
					continue;
				}

				str = shaderNode->Attribute("fragment");
				if (str != 0) {
					fragmentSourceFile = str;
				} else {
					cout << "Shader doesn't define a Fragment-File - ignoring Material \"" << materialName << "\"" << endl;
					delete newMaterial;
					continue;
				}

				fragmentSourceFile = "media/graphics/materials/shaders/" + fragmentSourceFile;
				vertextSourceFile = "media/graphics/materials/shaders/" + vertextSourceFile;

				newMaterial->m_program = Program::createProgram(vertextSourceFile, fragmentSourceFile);
				if (newMaterial->m_program == 0) {
					cout << "	ERROR ... couldn't create Program for Material "  << materialName << " - ignoring Material" << endl;
					delete newMaterial;
					continue;
				}

				TiXmlElement* uniformsNode = shaderNode->FirstChildElement("uniforms");
				if (uniformsNode != 0) {
					for (TiXmlElement* uniformNode = uniformsNode->FirstChildElement(); uniformNode != 0; uniformNode = uniformNode->NextSiblingElement()) {
						const char* str = uniformNode->Value();
						if (str == 0)
							continue;

						if (strcmp(str, "uniform") == 0) {
							string id;
							int value = 0;
							str = uniformNode->Attribute("id");
							if (str == 0) {
								cout << "id of uniform not set - ignoring uniform" << endl;
								continue;
							} else {
								id = str;
							}

							str = uniformNode->Attribute("value");
							if (str == 0) {
								cout << "value of uniform \"" << id << "\" not set - ignoring uniform" << endl;
								continue;
							} else {
								value = atoi(str);
							}

							cout << "New Uniform with id \"" << id << "\" and value " << value << " in Material \"" << materialName << "\"" << endl;

							newMaterial->m_program->putUniform( id, value );
						}
					}
				}
			}
			
			Material::allMaterials[materialName] = newMaterial;
			
			cout << "LOADED ... Material \"" << materialName << "\"" << endl;
		}
	}
	
	cout << endl;
	
	return true;
}

void
Material::freeAll()
{
	map<string, Material*>::iterator iter = Material::allMaterials.begin();
	while(iter != Material::allMaterials.end()) {
		delete iter->second;
		
		iter++;
	}
	
	Material::allMaterials.clear();
}

Material*
Material::get(const string& matName)
{
	map<string, Material*>::iterator findIter = Material::allMaterials.find(matName);
	if (findIter != Material::allMaterials.end())
		return findIter->second;
	
	return 0;
}

Material::Material(const std::string& name)
	: name(name)
{
	this->transparent = false;

	this->activated = false;

	this->m_program = 0;
}

Material::~Material()
{
	if (this->m_program)
		delete this->m_program;
}

void Material::activate()
{
	if (this->activated)
		return;

	map<Texture*, int>::iterator texIter = this->textures.begin();
	while (texIter != this->textures.end()) {
		Texture* tex = texIter->first;
		int texID = texIter->second;

		tex->activate(texID);

		texIter++;
	}

	if (this->m_program)
		this->m_program->activate();

	this->activated = true;
}

void Material::deactivate()
{
	if (this->activated == false)
		return;

	if (this->m_program)
		this->m_program->deactivate();

	map<Texture*, int>::iterator texIter = this->textures.begin();
	while (texIter != this->textures.end()) {
		Texture* tex = texIter->first;
		tex->deactivate();
		texIter++;
	}

	this->activated = false;
}
