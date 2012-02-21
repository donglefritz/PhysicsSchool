#include "OgreMotionState.h"


OgreMotionState::OgreMotionState(const btTransform& startPos, Ogre::SceneNode* sceneNode) {
	mPosition  = startPos;
	mSceneNode = sceneNode; 
}

OgreMotionState::~OgreMotionState(void) {
}

void OgreMotionState::setSceneNode(Ogre::SceneNode* sceneNode) {
	mSceneNode = sceneNode;
}

void OgreMotionState::getWorldTransform(btTransform& worldTransform) const {
	worldTransform = mPosition;
}

void OgreMotionState::setWorldTransform(const btTransform& worldTransform) {
	if (mSceneNode == NULL) { return; }
	btQuaternion rotation = worldTransform.getRotation();
	mSceneNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
	btVector3 position = worldTransform.getOrigin();
	mSceneNode->setPosition(position.x(), position.y(), position.z());
}
	