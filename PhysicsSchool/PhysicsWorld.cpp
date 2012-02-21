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
	// bodies:
	for (unsigned int i=0; i<mBodies.size(); ++i) {
		delete mBodies[i];
	}
	mBodies.clear();
	// shapes:
	for (unsigned int i=0; i<mCollisionShapes.size(); ++i) {
		delete mCollisionShapes[i];
	}
	mCollisionShapes.clear();
	// world:
	delete mDynamicsWorld;
	delete mSolver;
	delete mDispatcher;
	delete mCollisionConfig;
	delete mBroadphase;
}


btDiscreteDynamicsWorld* PhysicsWorld::getDynamicsWorld(void) {
	return mDynamicsWorld;
}

PhysicsBody* PhysicsWorld::createBody(btCollisionShape* collisionShape, btScalar mass, btVector3& startingPos) {
	PhysicsBody* body = new PhysicsBody(collisionShape, mass, startingPos);
	mBodies.push_back(body);
	return body;
}

btCollisionShape* PhysicsWorld::createInfinitePlane(btVector3& normal) {
	btCollisionShape* plane = new btStaticPlaneShape(normal, 0);
	mCollisionShapes.push_back(plane);
	return plane;
}

btCollisionShape* PhysicsWorld::createSphere(btScalar radius) {
	btCollisionShape* sphere = new btSphereShape(radius);
	mCollisionShapes.push_back(sphere);
	return sphere;
}

btCollisionShape* PhysicsWorld::createBox(btVector3& size) {
	btCollisionShape* box = new btBoxShape(size*0.5f);
	mCollisionShapes.push_back(box);
	return box;
}


void PhysicsWorld::tick(float timeSinceLast) {
	mDynamicsWorld->stepSimulation(timeSinceLast, 10);
}