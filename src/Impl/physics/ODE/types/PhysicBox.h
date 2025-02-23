#ifndef _PHYSICBOX_H_
#define _PHYSICBOX_H_

#include "PhysicType.h"

class PhysicBox : public PhysicType
{
	public:
		PhysicBox( bool, float, float, float, float );
		virtual ~PhysicBox();
	
		virtual bool create( dWorldID, dSpaceID );
	
	private:
		float m_x;
		float m_y;
		float m_z;
};

#endif /* _PHYSICBOX_H_ */
