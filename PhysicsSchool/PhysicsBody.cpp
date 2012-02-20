#include "PhysicsBody.h"

// static declarations:
btDynamicsWorld* PhysicsBody::dynamicsWorld = NULL;

// static methods:
void PhysicsBody::RegisterDynamicsWorld(btDynamicsWorld* world) {
	PhysicsBody::dynamicsWorld = world;
}

// constructor:
PhysicsBody::PhysicsBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos) : mCollisionShape(0), mMotionState(0), mRigidBody(0) {
	mCollisionShape = shape;
	mMass           = mass;
	mStartingPos    = startingPos;

	if (mMass > 0) { mCollisionShape->calculateLocalInertia(mMass, mInertia); }

	mMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), mStartingPos));
	mRigidBody   = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mMass, mMotionState, mCollisionShape, mInertia));

	if (PhysicsBody::dynamicsWorld != NULL) { PhysicsBody::dynamicsWorld->addRigidBody(mRigidBody); }
}

// destructor:
PhysicsBody::~PhysicsBody(void) {
	if (PhysicsBody::dynamicsWorld != NULL) { PhysicsBody::dynamicsWorld->removeRigidBody(mRigidBody); }
	if (mMotionState != NULL) { delete mMotionState; }
	if (mRigidBody   != NULL) { delete mRigidBody;   }
}

// other methods:


btRigidBody* PhysicsBody::getRigidBody(void) {
	return mRigidBody;
}