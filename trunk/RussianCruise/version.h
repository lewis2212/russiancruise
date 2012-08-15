#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char RC_DATE[] = "13";
	static const char RC_MONTH[] = "08";
	static const char RC_YEAR[] = "2012";
	static const char RC_UBUNTU_VERSION_STYLE[] = "12.08";
	
	//Software Status
	static const char RC_STATUS[] = "Beta";
	static const char RC_STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long RC_MAJOR = 1;
	static const long RC_MINOR = 2;
	static const long RC_BUILD = 5;
	static const long RC_REVISION = 46;
	
	//Miscellaneous Version Types
	static const long RC_BUILDS_COUNT = 298;
	#define RC_RC_FILEVERSION 1,2,5,46
	#define RC_RC_FILEVERSION_STRING "1, 2, 5, 46\0"
	static const char RC_FULLVERSION_STRING[] = "1.2.5.46";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long RC_BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
