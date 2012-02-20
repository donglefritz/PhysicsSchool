#ifndef PhysicsBody_h
#define PhysicsBody_h

#include <btBulletDynamicsCommon.h>

class PhysicsBody {
public:
	PhysicsBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos);
	~PhysicsBody(void);

	// call this before instantiating objects:
	static void RegisterDynamicsWorld(btDynamicsWorld* world);

	btRigidBody* getRigidBody(void);

protected:
	btScalar              mMass;
	btVector3             mInertia;
	btVector3             mStartingPos;
	btCollisionShape*     mCollisionShape;
	btDefaultMotionState* mMotionState;
	btRigidBody*          mRigidBody;

private:
	static btDynamicsWorld* dynamicsWorld;

};


#endif

