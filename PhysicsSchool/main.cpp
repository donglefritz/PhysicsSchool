
#include <iostream>
#include "PhysicsWorld.h"


void hangConsole(void) {
	std::cout << std::endl << "[hit ENTER to close]" << std::endl;
	std::cin.get();
}


int main(int argc, char* argv[]) {
	std::cout << "ok" << std::endl;

	// 1. SETUP BULLET PHYSICS:

	PhysicsWorld* physicsWorld = new PhysicsWorld(btVector3(0,-10,0));

	// 2. SETUP COLLISION SHAPES (reuse as much as possible):

	btCollisionShape* groundShape  = new btStaticPlaneShape(btVector3(0,1,0),1);
	btCollisionShape* fallingShape = new btSphereShape(1);

	// 3. SETUP RIGID BODIES:

	PhysicsBody* ground  = new PhysicsBody(groundShape,  btScalar(0), btVector3(0, -1, 0));
	PhysicsBody* falling = new PhysicsBody(fallingShape, btScalar(1), btVector3(0, 50, 0)); 

	// 4. RUN SIMULATION:

	int numSteps=300;
	for (int i=0; i<numSteps; ++i) {
		physicsWorld->tick();
		btTransform transform;
		falling->getRigidBody()->getMotionState()->getWorldTransform(transform);
		std::cout << "sphere height: " << transform.getOrigin().getY() << std::endl;
	}

	// 5. TEARDOWN RIGID BODIES:

	delete falling;
	delete ground;

	// 6. TEARDOWN COLLISION SHAPES:

	delete fallingShape;
	delete groundShape;

	// 7. TEARDOWN BULLET PHYSICS:

	delete physicsWorld;

	// all done.
	hangConsole();
	return 0;
}