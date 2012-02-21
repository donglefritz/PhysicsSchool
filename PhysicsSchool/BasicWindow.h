#ifndef BasicWindow_h
#define BasicWindow_h

// graphics:
#include <OgreRoot.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreHardwarePixelBuffer.h>
// input:
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISMouse.h>
#include <OISKeyboard.h>
#include <SdkCameraMan.h>    //<- must be after OIS
// gui:
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
// other:
#include "Utils.h"
#include "PhysicsWorld.h"
#include "Body.h"
#include <ostream>
#include <cassert>


class BasicWindow : public Ogre::WindowEventListener, public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener {
public:
	BasicWindow(void);
	virtual ~BasicWindow(void);
	virtual bool go(void);

protected:
	Ogre::Root*              mRoot;
	Ogre::RenderWindow*      mWindow;
	Ogre::SceneManager*	     mSceneMgr;
	Ogre::Camera*            mCamera;
	OgreBites::SdkCameraMan* mCameraMan;
	OIS::InputManager*       mInputManager;
	OIS::Mouse*              mMouse;
	OIS::Keyboard*           mKeyboard;
	Ogre::String             mPluginsFileName;
	Ogre::String             mResourcesFileName;
	Ogre::SceneNode*         mCameraNode;
	bool                     mShutDown;
	CEGUI::OgreRenderer*     mRenderer;          //<- gui
	bool                     mSendKeyboardToGUI; //<- send input to gui?
	bool                     mSendMouseToGUI;    //<- send input to gui?
	PhysicsWorld*            mPhysicsWorld;
	std::vector<Body*>       mBodies;
	
	// WindowEventListener interface:
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	// FrameListener interface:
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// buffered input:
	virtual bool keyPressed(const OIS::KeyEvent& evt);
	virtual bool keyReleased(const OIS::KeyEvent& evt);
	virtual bool mouseMoved(const OIS::MouseEvent& evt);
	virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	// scene:
	virtual void createScene(void);
	virtual void addAxesLines(float length);

	// gui:
	virtual void createGUI(void);
	virtual bool quit(const CEGUI::EventArgs& evt);
	virtual bool isMouseOverGUI(void);
	virtual CEGUI::MouseButton BasicWindow::convertMouseButton(OIS::MouseButtonID id);

	// physics:
	virtual Body* createSphereBody(btCollisionShape* shape, btScalar mass, btVector3& startingPos);

};

#endif // BasicWindow_h

