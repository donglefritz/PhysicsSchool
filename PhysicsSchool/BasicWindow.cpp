#include "BasicWindow.h"

// CONSTRUCTOR / DESTRUCTOR:

BasicWindow::BasicWindow(void) :
	mRoot(0),
	mPluginsFileName(Ogre::StringUtil::BLANK),
	mResourcesFileName(Ogre::StringUtil::BLANK),
	mShutDown(false),
	mSendMouseToGUI(true),
	mSendKeyboardToGUI(false),
	mCamera(0),
	mCameraMan(0),
	mKeyboard(0),
	mMouse(0),
	mInputManager(0),
	mPhysicsWorld(0) {
#ifdef _DEBUG
	mPluginsFileName   = "plugins_d.cfg";
	mResourcesFileName = "resources_d.cfg"; 
#else
	mPluginsFileName   = "plugins.cfg";
	mResourcesFileName = "resources.cfg"; 
#endif
}

BasicWindow::~BasicWindow(void) {
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot; mRoot=0;
	delete mCameraMan; mCameraMan=0;

	// bodies:
	for (unsigned int i=0; i<mBodies.size(); ++i) {
		if (mBodies[i] != NULL) {
			delete mBodies[i];
		}
	}
	mBodies.clear();

	if (mPhysicsWorld != NULL) {
		delete mPhysicsWorld; mPhysicsWorld=0;
	}
}

// INITIALISATION:

bool BasicWindow::go(void) {

	// 1. define the root object:
	
	mRoot = new Ogre::Root(mPluginsFileName);

	// 2. define the resources that ogre will use:

	Ogre::ConfigFile configFile;
	configFile.load(mResourcesFileName);
	Ogre::ConfigFile::SectionIterator sectionsIter = configFile.getSectionIterator();
	Ogre::ConfigFile::SettingsMultiMap::iterator settingsIter;
	Ogre::ConfigFile::SettingsMultiMap* settings;
	Ogre::String secName, typeName, archName;

	while(sectionsIter.hasMoreElements()) {
		secName  = sectionsIter.peekNextKey();
		settings = sectionsIter.getNext();
		for (settingsIter=settings->begin(); settingsIter!=settings->end(); ++settingsIter) {
			typeName = settingsIter->first;
			archName = settingsIter->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	// 3. choose and setup the render system:

	if (!(mRoot->restoreConfig() || mRoot->showConfigDialog())) {
		return false;
	}

	// 4. create the render window:

	mWindow = mRoot->initialise(true, "Testing");

	// 5. initialise the required resources:

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// 6. setup scene:

	mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");
	
	mCamera   = mSceneMgr->createCamera("mCamera");
	mCamera->setNearClipDistance(1);
	//mCamera->setFarClipDistance(10000);

	mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cameraNode");
	mCameraNode->attachObject(mCamera);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);

	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	//vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1));
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));



	createGUI();
	createScene();

	// 7. setup 3rd party libraries and plugins:

	Utils::log("*** Initialising OIS ***");
	OIS::ParamList paramList;
	size_t windowHandle = 0;
	std::ostringstream windowHandleString;
	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleString << windowHandle;
	paramList.insert(std::make_pair(std::string("WINDOW"), windowHandleString.str()));

	mInputManager = OIS::InputManager::createInputSystem(paramList);
	mKeyboard     = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse        = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

	windowResized(mWindow);  //<- sets mouse clipping size

	// 8. add listeners and callbacks:

	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
	mRoot->addFrameListener(this);
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	// 9. start the render loop

	mRoot->startRendering();

	return true;
}

void BasicWindow::createGUI(void) {
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
	CEGUI::System* guiSystem     = CEGUI::System::getSingletonPtr();
	CEGUI::Window* rootWindow    = winMgr->createWindow("DefaultWindow", "root");
	guiSystem->setGUISheet(rootWindow);

	/* not using the gui quite yet:
	try {
		rootWindow->addChildWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("Console.layout"));
		rootWindow->addChildWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("RightPanel.layout"));
	} catch (CEGUI::Exception& e) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, std::string(e.getMessage().c_str()), "Error parsing gui layout");
	}
	*/

}

Body* BasicWindow::createSphereBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos) {
	Ogre::Entity* entity  = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
	Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);
	entity->setMaterialName("Rock");
	Body* body = new Body(node, entity, shape, mass, startingPos);
	mBodies.push_back(body);
	return body;
}

Body* BasicWindow::createCubeBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos) {
	Ogre::Entity* entity  = mSceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
	Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);
	entity->setMaterialName("Grass");
	Body* body = new Body(node, entity, shape, mass, startingPos);
	mBodies.push_back(body);
	return body;
}

void BasicWindow::createScene(void) {
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f,1.0f,1.0f));
	addAxesLines(50);

	// create a physics world:
	mPhysicsWorld = new PhysicsWorld(btVector3(0,-10,0));

	// create some shapes to share among bodies:
	btCollisionShape* planeShape  = mPhysicsWorld->createInfinitePlane(btVector3(0,1,0));
	btCollisionShape* sphereShape = mPhysicsWorld->createSphere(50);
	btCollisionShape* boxShape    = mPhysicsWorld->createBox(btVector3(100,100,100));
	btCollisionShape* groundShape = mPhysicsWorld->createBox(btVector3(1500,1,1500));

	mProjectileShape = sphereShape;

	createCubeBody(boxShape, btScalar(1), btVector3(0, 0, 0));

	// new way:
	for (int i=1; i<51; ++i) {
		btScalar mass(1.0f);
		btScalar value((float)i);
		if (Utils::randomBool()) {
			createSphereBody(sphereShape, mass, btVector3(value, value*150, value));
		} else {
			createCubeBody(boxShape, mass, btVector3(value, value*150, value));
		}
	}


	

	// old way (ground):
	PhysicsBody* ground   = mPhysicsWorld->createBody(groundShape, btScalar(0),  btVector3(0, 0, 0));
	Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("groundPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                   groundPlane, 1500, 1500, 20, 20, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* groundEntity  = mSceneMgr->createEntity("groundEntity", "groundPlane");
	Ogre::SceneNode* groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	groundEntity->setMaterialName("BlackBorder");
	groundEntity->setCastShadows(false);
	groundNode->attachObject(groundEntity);
	ground->setSceneNode(groundNode);


	// TODO: stop using the sdkcameraman because it apparently doesn't use scenenodes. This means it's impossible to attach
	//       entities to the camera (guns, players, etc...)






}

// CALLBACKS:

void BasicWindow::drawLine(void) {
	Ogre::ManualObject* myManualObject =  mSceneMgr->createManualObject("manual1"); 
	Ogre::SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node"); 
 
	Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("manual1Material","MyMats"); 
	myManualObjectMaterial->setReceiveShadows(false); 
	myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1); 

 
 
	myManualObject->begin("manual1Material", Ogre::RenderOperation::OT_LINE_LIST); 
	myManualObject->position(3, 2, 1); 
	myManualObject->position(4, 1, 0); 
	// etc 
	myManualObject->end(); 
 
	myManualObjectNode->attachObject(myManualObject);
}

void BasicWindow::shootProjectile(void) {
	float speed = 100.0f;
	Ogre::Vector3 camPos(mCamera->getPosition());
	Ogre::Vector3 direction = mCamera->getDirection();

	btVector3 pos(camPos.x, camPos.y, camPos.z);
	btVector3 gravity(direction.x, direction.y, direction.z);
	gravity *= speed;
	Body* body = createSphereBody(mProjectileShape, btScalar(1), pos);
	body->getPhysicsBody()->getRigidBody()->setGravity(gravity);
}



bool BasicWindow::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	if (mWindow->isClosed() || mShutDown) { return false; }
	mKeyboard->capture();
	mMouse->capture();
	mCameraMan->frameRenderingQueued(evt);
	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	int physicsFactor = 10; //<- 10 seems to be about right
	mPhysicsWorld->tick(evt.timeSinceLastFrame * physicsFactor);


	float yThreshold = -10000.0f;

	for (unsigned int i=0; i<mBodies.size(); ++i) {
		if (mBodies[i]->getSceneNode()->getPosition().y < yThreshold) {
			mSceneMgr->destroyEntity(mBodies[i]->getEntity());
			mSceneMgr->destroySceneNode(mBodies[i]->getSceneNode());
			delete mBodies[i];
			mBodies.erase(mBodies.begin()+i);
		}
	}



	return true;
}

void BasicWindow::windowResized(Ogre::RenderWindow* rw) {
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);
	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width  = width;
	ms.height = height;
}

void BasicWindow::windowClosed(Ogre::RenderWindow* rw) {
	if (rw == mWindow) {
		if (mInputManager) {
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

bool BasicWindow::keyPressed(const OIS::KeyEvent& evt) {

	float moveAmount = 5;

	switch(evt.key) {
	case OIS::KC_ESCAPE:
		mShutDown = true;
		break;
	case OIS::KC_SYSRQ:
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
		break;
	case OIS::KC_HOME:
		mCamera->setPosition(Ogre::Vector3(0,0,5));
		break;
	case OIS::KC_NUMPAD8:	//<- in
		mCamera->setPosition(mCamera->getPosition()+Ogre::Vector3(0,0,-moveAmount));
		break;
	case OIS::KC_NUMPAD2:	//<- out
		mCamera->setPosition(mCamera->getPosition()+Ogre::Vector3(0,0,moveAmount));
		break;
	case OIS::KC_NUMPAD4:	//<- left
		mCamera->setPosition(mCamera->getPosition()+Ogre::Vector3(-moveAmount,0,0));
		break;
	case OIS::KC_NUMPAD6:	//<- right
		mCamera->setPosition(mCamera->getPosition()+Ogre::Vector3(moveAmount,0,0));
		break;
	case OIS::KC_PGUP:		//<- up
		mCamera->setPosition(mCamera->getPosition()+Ogre::Vector3(0,moveAmount,0));
		break;
	case OIS::KC_PGDOWN:	//<- down
		mCamera->setPosition(mCamera->getPosition()+Ogre::Vector3(0,-moveAmount,0));
		break;
	case OIS::KC_SPACE:
		shootProjectile();
		break;
	default:
		if (mSendKeyboardToGUI) {
			CEGUI::System::getSingleton().injectKeyDown(evt.key);
			CEGUI::System::getSingleton().injectChar(evt.text);
		} else {
			mCameraMan->injectKeyDown(evt);
		}
		break;
	}
	return true;
}

bool BasicWindow::keyReleased(const OIS::KeyEvent& evt) {
	if (mSendKeyboardToGUI) {
		CEGUI::System::getSingleton().injectKeyUp(evt.key);
	} else {
		mCameraMan->injectKeyUp(evt);
	}
	return true;
}

bool BasicWindow::mouseMoved(const OIS::MouseEvent& evt) {
	if (mSendMouseToGUI) {
		CEGUI::System& gui = CEGUI::System::getSingleton();
		gui.injectMouseMove((float)evt.state.X.rel, (float)evt.state.Y.rel);
		if (evt.state.Z.rel) { gui.injectMouseWheelChange(evt.state.Z.rel / 120.0f); }
	} else {
		mCameraMan->injectMouseMove(evt);
	}
	return true;
}

bool BasicWindow::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id) {
	if (id == OIS::MB_Right) {
		mSendMouseToGUI = false;
	} else {
		CEGUI::System::getSingleton().injectMouseButtonDown(convertMouseButton(id));
		if (isMouseOverGUI()) {
			mSendKeyboardToGUI = true;
		} else {
			mSendKeyboardToGUI = false;
		}
	}
	return true;
}

bool BasicWindow::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id) {
	if (id == OIS::MB_Right) {
		mSendMouseToGUI = true;
	} else {
		CEGUI::System::getSingleton().injectMouseButtonUp(convertMouseButton(id));
	}
	return true;
}

bool BasicWindow::quit(const CEGUI::EventArgs& evt) {
	mShutDown = true;
	return true;
}

// HELPERS:

CEGUI::MouseButton BasicWindow::convertMouseButton(OIS::MouseButtonID id) {
	switch(id) {
	case OIS::MB_Left:
		return CEGUI::LeftButton;
	case OIS::MB_Right:
		return CEGUI::RightButton;
	case OIS::MB_Middle:
		return CEGUI::MiddleButton;
	default:
		return CEGUI::LeftButton;
	}
}

bool BasicWindow::isMouseOverGUI(void) {
	CEGUI::Window* win  = CEGUI::System::getSingleton().getWindowContainingMouse();
	CEGUI::Window* root = CEGUI::WindowManager::getSingleton().getWindow("root");
	if (win == NULL || win == root)  {
		return false;
	} else {
		return true;
	}
}

void BasicWindow::addAxesLines(float length) {	
	Ogre::ManualObject* mo = mSceneMgr->createManualObject("axesLines");
	mo->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	mo->position(-length,0,0);				   //<- 0
	mo->colour(Ogre::ColourValue::Red);
	mo->position(length,0,0);                  //<- 1
	mo->colour(Ogre::ColourValue::Red);
	mo->position(0,-length,0);                 //<- 2
	mo->colour(Ogre::ColourValue::Green);
	mo->position(0,length,0);                  //<- 3
	mo->colour(Ogre::ColourValue::Green);
	mo->position(0,0,-length);                 //<- 4
	mo->colour(Ogre::ColourValue::Blue);   
	mo->position(0,0,length);                  //<- 5
	mo->colour(Ogre::ColourValue::Blue);
	mo->index(0);
	mo->index(1);
	mo->index(2);
	mo->index(3);
	mo->index(4);
	mo->index(5);
	mo->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mo);
}
