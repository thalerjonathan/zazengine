/*
 * IGraphicsEntity.h
 *
 *  Created on: 15.07.2010
 *      Author: Jonathan Thaler
 */

#ifndef IGRAPHICSENTITY_H_
#define IGRAPHICSENTITY_H_

#include <core/ISubSystemEntity.h>

class IGraphicsEntity : public ISubSystemEntity
{
	public:
		IGraphicsEntity( IGameObject* p ) : ISubSystemEntity( p ) {};
		virtual ~IGraphicsEntity() {};

		virtual void setOrientation( const float* pos, const float* rot) = 0;
};

#endif /* IGRAPHICSENTITY_H_ */
