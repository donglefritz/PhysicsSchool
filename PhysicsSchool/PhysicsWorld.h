#ifndef PhysicsWorld_h
#define PhysicsWorld_h

#include "PhysicsBody.h"
#include <vector>

class PhysicsWorld {
public:
	PhysicsWorld(btVector3& gravity);
	~PhysicsWorld(void);

	btDiscreteDynamicsWorld* getDynamicsWorld(void);

	virtual btCollisionShape* createInfinitePlane(btVector3& normal);
	virtual btCollisionShape* createSphere(btScalar radius);
	virtual PhysicsBody* createBody(btCollisionShape* collisionShape, btScalar mass, btVector3& startingPos);
	virtual void tick(float timeSinceLast);

protected:
	btBroadphaseInterface*               mBroadphase;
	btDefaultCollisionConfiguration*     mCollisionConfig;
	btCollisionDispatcher*               mDispatcher;
	btSequentialImpulseConstraintSolver* mSolver;
	btDiscreteDynamicsWorld*             mDynamicsWorld;
	std::vector<PhysicsBody*>            mBodies;
	std::vector<btCollisionShape*>       mCollisionShapes;

};


#endif