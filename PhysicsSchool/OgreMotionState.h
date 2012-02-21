#ifndef OgreMotionState_h
#define OgreMotionState_h

#include <btBulletDynamicsCommon.h>
#include <OgreSceneNode.h>

class OgreMotionState :	public btMotionState {
public:
	OgreMotionState(const btTransform& startPos, Ogre::SceneNode* sceneNode);
	virtual ~OgreMotionState(void);

	virtual void getWorldTransform(btTransform& worldTransform) const;
	virtual void setWorldTransform(const btTransform& worldTransform);
	void setSceneNode(Ogre::SceneNode* sceneNode);

protected:
	Ogre::SceneNode* mSceneNode;
	btTransform      mPosition;

};

#endif