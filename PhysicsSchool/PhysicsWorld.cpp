#include "PhysicsWorld.h"


PhysicsWorld::PhysicsWorld(btVector3& gravity) : mBroadphase(0), mCollisionConfig(0), mDispatcher(0), mSolver(0), mDynamicsWorld(0) {
	mBroadphase      = new btDbvtBroadphase();
	mCollisionConfig = new btDefaultCollisionConfiguration();
	mDispatcher      = new btCollisionDispatcher(mCollisionConfig);
	mSolver          = new btSequentialImpulseConstraintSolver();
	mDynamicsWorld   = new btDiscreteDynamicsWorld(mDispatcher,mBroadphase,mSolver,mCollisionConfig);
	mDynamicsWorld->setGravity(gravity);
	PhysicsBody::RegisterDynamicsWorld(mDynamicsWorld);
}


PhysicsWorld::~PhysicsWorld(void) {
	for (unsigned int i=0; i<mBodies.size(); ++i) {
		delete mBodies[i];
	}
	mBodies.clear();
	delete mDynamicsWorld;
	delete mSolver;
	delete mDispatcher;
	delete mCollisionConfig;
	delete mBroadphase;
}


btDiscreteDynamicsWorld* PhysicsWorld::getDynamicsWorld(void) {
	return mDynamicsWorld;
}

PhysicsBody* PhysicsWorld::createBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos) {
	PhysicsBody* body = new PhysicsBody(shape, mass, startingPos);
	mBodies.push_back(body);
	return body;
}


void PhysicsWorld::tick(void) {
	mDynamicsWorld->stepSimulation(1/60.0f, 10);
}