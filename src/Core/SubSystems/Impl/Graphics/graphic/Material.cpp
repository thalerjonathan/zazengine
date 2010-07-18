/*
 *  material.cpp
 *  ZENgine
 *
 *  Created by Jonathan Thaler on 23.04.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Material.h"

#include "../../../Utils/XML/tinyxml.h"

#include <iostream>

using namespace std;

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, (GLint*) &infologLength);

	if (infologLength > 0) {
		infoLog = (char*) malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, (GLint*) &charsWritten, infoLog);
		if (charsWritten)
			printf("%s\n",infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, (GLint*) &infologLength);
	if (infologLength > 0) {
	        infoLog = (char*) malloc(infologLength);
	        glGetProgramInfoLog(obj, infologLength, (GLint*) &charsWritten, infoLog);
		if (charsWritten)
			printf("%s\n",infoLog);
	        free(infoLog);
	}
}

map<std::string, Material*> Material::allMaterials;

bool Material::loadMaterials()
{
	string fullFileName = "resources/materials/materials.xml";
	
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

				newMaterial->materialProgram = Material::createProgram(vertextSourceFile, fragmentSourceFile);
				if (newMaterial->materialProgram == 0) {
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

							GLint loc = Material::queryUniformLoc(newMaterial->materialProgram, id.c_str());
							if (loc != -1)
								newMaterial->uniforms[loc] = value;
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

void Material::freeAll()
{
	map<string, Material*>::iterator iter = Material::allMaterials.begin();
	while(iter != Material::allMaterials.end()) {
		delete iter->second;
		
		iter++;
	}
	
	Material::allMaterials.clear();
}

Material* Material::get(const string& matName)
{
	map<string, Material*>::iterator findIter = Material::allMaterials.find(matName);
	if (findIter != Material::allMaterials.end())
		return findIter->second;
	
	return 0;
}

Material::Material(const std::string& name)
	: name(name)
{
	this->materialProgram = 0;
	this->vertexShader = 0;
	this->fragmentShader = 0;

	this->transparent = false;

	this->activated = false;
}

Material::~Material()
{
	if (this->materialProgram) {
		// TODO: free
	}
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

	if (this->materialProgram) {
		glUseProgram(this->materialProgram);

		map<GLint, int>::iterator uniIter = this->uniforms.begin();
		while (uniIter != this->uniforms.end()) {
			GLint loc = uniIter->first;
			int value = uniIter->second;

			glUniform1i(loc, value);

			uniIter++;
		}
	}

	this->activated = true;
}

void Material::deactivate()
{
	if (this->activated == false)
		return;

	if (this->materialProgram)
		glUseProgram(0);

	map<Texture*, int>::iterator texIter = this->textures.begin();
	while (texIter != this->textures.end()) {
		Texture* tex = texIter->first;
		tex->deactivate();
		texIter++;
	}

	this->activated = false;
}

GLuint Material::createProgram(const string& vertexSourceFile, const string& fragmentSourceFile)
{
	GLint status;
	GLuint program = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;

	string vertexSourceStr;
	string fragmentSourceStr;

	if (Material::readShaderSource(vertexSourceFile, vertexSourceStr) == false)
		return 0;

	if (Material::readShaderSource(fragmentSourceFile, fragmentSourceStr) == false)
		return 0;

	const char* vertexSource = vertexSourceStr.c_str();
	const char* fragmentSource = fragmentSourceStr.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (vertexShader == 0) {
		cout << "glCreateShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) {
		cout << "glCreateShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	glShaderSource(vertexShader, 1, (const GLchar**) &vertexSource, NULL);
	if (glGetError() != GL_NO_ERROR) {
		cout << "glShaderSource for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}
	glShaderSource(fragmentShader, 1, (const GLchar**) &fragmentSource, NULL);
	if (glGetError() != GL_NO_ERROR) {
		cout << "glShaderSource for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	glCompileShader(vertexShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	printShaderInfoLog(vertexShader);
	if (!status) {
		cout << "Failed compiling GL_VERTEX_SHADER \"" << vertexSourceFile << "\"" << endl;
		return 0;
	}

	glCompileShader(fragmentShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	printShaderInfoLog(fragmentShader);
	if (!status) {
		cout << "Failed compiling GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\"" << endl;
		return 0;
	}

	program = glCreateProgram();
	if (program == 0) {
		if (glGetError() != GL_NO_ERROR)
			cout << "glCreateProgram failed with " << gluErrorString(glGetError()) << endl;

		return 0;
	}

	glAttachShader(program, vertexShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
	glAttachShader(program, fragmentShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;

	glLinkProgram(program);
	if (glGetError() != GL_NO_ERROR)
		cout << "glLinkProgramARB failed with " << gluErrorString(glGetError()) << endl;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	printProgramInfoLog(program);
	if (!status) {
		cout << "ERROR linking of program \"" << vertexSourceFile << "\" and \"" << fragmentSourceFile << "\" failed" << endl;
		return 0;
	}

	return program;
}

bool Material::readShaderSource(const string& file, string& shaderSource)
{
	string fullFileName = "resources/materials/shaders/" + file;

	FILE* shaderSourceFile = fopen(fullFileName.c_str(), "r");
	if (shaderSourceFile == 0) {
		cout << "ERROR ... couldn't open Shadersource-File " << fullFileName << endl;
		return false;
	}

	char c;
	while ((c = fgetc(shaderSourceFile)) != EOF)
		shaderSource += c;

	return true;
}

GLint Material::queryUniformLoc(GLint prog, const GLchar* name)
{
	GLint location = 0;

	glGetError();

	location = glGetUniformLocation(prog, name);
	if (location == -1) {
		cout << "Coulnd't get Uniform Location for name \"" << name << "\". OpenGL-Error: " << gluErrorString(glGetError())  << endl;
		return -1;
	}

	return location;
}
