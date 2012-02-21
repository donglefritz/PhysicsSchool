#ifndef Body_h
#define Body_h

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreVector3.h>
#include "PhysicsBody.h"


class Body {
public:
	Body(Ogre::SceneNode* sceneNode, Ogre::Entity* entity, btCollisionShape* shape, btScalar mass, btVector3& startingPos);
	~Body(void);

protected:
	Ogre::SceneNode* mSceneNode;
	Ogre::Entity*    mEntity;
	PhysicsBody*     mPhysicsBody;
	
};

#endif