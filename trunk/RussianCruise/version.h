#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char RC_DATE[] = "15";
	static const char RC_MONTH[] = "04";
	static const char RC_YEAR[] = "2013";
	static const char RC_UBUNTU_VERSION_STYLE[] = "13.04";
	
	//Software Status
	static const char RC_STATUS[] = "Beta";
	static const char RC_STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long RC_MAJOR = 1;
	static const long RC_MINOR = 2;
	static const long RC_BUILD = 18;
	static const long RC_REVISION = 531;
	
	//Miscellaneous Version Types
	static const long RC_BUILDS_COUNT = 629;
	#define RC_RC_FILEVERSION 1,2,18,531
	#define RC_RC_FILEVERSION_STRING "1, 2, 18, 531\0"
	static const char RC_FULLVERSION_STRING[] = "1.2.18.531";
	
	//SVN Version
	static const char RC_SVN_REVISION[] = "135";
	static const char RC_SVN_DATE[] = "2013-04-14T18:46:40.137378Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long RC_BUILD_HISTORY = 1;
	

}
#endif //VERSION_H
