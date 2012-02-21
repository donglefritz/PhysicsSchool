#ifndef Utils_h
#define Utils_h

#include <time.h>
#include <OgreString.h>
#include <OgreLogManager.h>


class Utils {
public:
	Utils(void);
	~Utils(void);

	static bool  randomBool(void);
	static float randomFloat(void);
	static int   randomInt(int excludedMax);
	static void  log(Ogre::String msg);

};

#endif