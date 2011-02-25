/*
 * Material.h
 *
 *  Created on: Feb 25, 2011
 *      Author: jonathan
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Texture.h"

#include <glm/glm.hpp>

#include <string>
#include <map>
#include <vector>

#define MAX_DIFFUSE_TEXTURES 4

class Material
{
 public:
	static bool loadAll();
	static Material* get( const std::string& );

	~Material();

	const std::string& getName() const { return this->m_name; };
	const std::string& getType() const { return this->m_type; };

	bool activate();

 private:
	static std::map<std::string, Material*> allMaterials;

	Material( const std::string&, const std::string& );

	const std::string m_name;
	const std::string m_type;

	glm::vec3 m_color;
	Texture* m_normalMap;
	std::vector<Texture*> m_diffuseTextures;
};

#endif /* MATERIAL_H_ */
