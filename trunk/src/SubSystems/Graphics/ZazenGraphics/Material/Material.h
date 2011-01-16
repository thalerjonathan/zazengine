/*
 *  material.h
 *  ZENgine
 *
 *  Created by Jonathan Thaler on 23.04.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <GL/glew.h>

#include "Program.h"
#include "Texture.h"

#include <string>
#include <map>
#include <vector>

// TOOD: implement nvidias gfx

class Material
{
 public:
	static bool loadMaterials();
	static void freeAll();
	 
	static Material* get(const std::string&);
	 
	const std::string& getName() const { return this->name; };
	bool isTransparent() { return this->transparent; };
	
	void activate();
	void deactivate();
	
 private:
	static std::map<std::string, Material*> allMaterials;

	Material(const std::string&);
	~Material();
	
	const std::string name;

	bool activated;
	bool transparent;

	std::map<Texture*, int> textures;

	Program* m_program;
};

#endif
