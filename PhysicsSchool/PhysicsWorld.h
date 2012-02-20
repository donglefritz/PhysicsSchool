#ifndef PhysicsWorld_h
#define PhysicsWorld_h

#include "PhysicsBody.h"

class PhysicsWorld {
public:
	PhysicsWorld(btVector3& gravity);
	~PhysicsWorld(void);

	btDiscreteDynamicsWorld* getDynamicsWorld(void);

	void tick(void);

protected:
	btBroadphaseInterface*               mBroadphase;
	btDefaultCollisionConfiguration*     mCollisionConfig;
	btCollisionDispatcher*               mDispatcher;
	btSequentialImpulseConstraintSolver* mSolver;
	btDiscreteDynamicsWorld*             mDynamicsWorld;

};


#endif