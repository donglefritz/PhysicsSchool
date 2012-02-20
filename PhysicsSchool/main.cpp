
#include <iostream>
#include "PhysicsWorld.h"


void hangConsole(void) {
	std::cout << std::endl << "[hit ENTER to close]" << std::endl;
	std::cin.get();
}

int main(int argc, char* argv[]) {

	// create a physics world:
	PhysicsWorld* physicsWorld = new PhysicsWorld(btVector3(0,-10,0));

	// create some collision shapes to share among bodies:
	btCollisionShape* groundShape  = physicsWorld->createInfinitePlane(btVector3(0,1,0));
	btCollisionShape* fallingShape = physicsWorld->createSphere(1);

	// create some bodies:
	PhysicsBody* ground  = physicsWorld->createBody(groundShape,  btScalar(0), btVector3(0, -1, 0));
	PhysicsBody* falling = physicsWorld->createBody(fallingShape, btScalar(1), btVector3(0, 50, 0)); 

	// simulate:
	int numSteps=300;
	for (int i=0; i<numSteps; ++i) {
		physicsWorld->tick();
		btTransform transform;
		falling->getRigidBody()->getMotionState()->getWorldTransform(transform);
		std::cout << "sphere height: " << transform.getOrigin().getY() << std::endl;
	}
	
	// clean up:
	delete physicsWorld;
	hangConsole();
	return 0;
}
