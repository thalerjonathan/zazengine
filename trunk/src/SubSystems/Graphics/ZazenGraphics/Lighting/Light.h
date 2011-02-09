/*
 * Light.h
 *
 *  Created on: Feb 4, 2011
 *      Author: jonathan
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "../Scene/Viewer.h"

class Light : public Viewer
{
 public:
	Light( float, int, int );
	~Light();

 private:

};

#endif /* LIGHT_H_ */
