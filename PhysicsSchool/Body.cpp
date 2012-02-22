#include "Body.h"

Body::Body(Ogre::SceneNode* sceneNode, Ogre::Entity* entity, btCollisionShape* shape, btScalar mass, btVector3& startingPos) {
	mSceneNode   = sceneNode;
	mEntity      = entity;
	mPhysicsBody = new PhysicsBody(shape, mass, startingPos);
	mPhysicsBody->setSceneNode(mSceneNode);
}

Body::~Body(void) {
	delete mPhysicsBody;
}

Ogre::SceneNode* Body::getSceneNode(void) {
	return mSceneNode;
}

Ogre::Entity* Body::getEntity(void) {
	return mEntity;
}

PhysicsBody* Body::getPhysicsBody(void) {
	return mPhysicsBody;
}