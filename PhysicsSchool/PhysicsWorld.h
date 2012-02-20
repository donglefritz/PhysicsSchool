#ifndef PhysicsWorld_h
#define PhysicsWorld_h

#include "PhysicsBody.h"
#include <vector>

class PhysicsWorld {
public:
	PhysicsWorld(btVector3& gravity);
	~PhysicsWorld(void);

	btDiscreteDynamicsWorld* getDynamicsWorld(void);

	virtual void tick(void);
	virtual PhysicsBody* createBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos);

protected:
	btBroadphaseInterface*               mBroadphase;
	btDefaultCollisionConfiguration*     mCollisionConfig;
	btCollisionDispatcher*               mDispatcher;
	btSequentialImpulseConstraintSolver* mSolver;
	btDiscreteDynamicsWorld*             mDynamicsWorld;
	std::vector<PhysicsBody*>            mBodies;

};


#endif