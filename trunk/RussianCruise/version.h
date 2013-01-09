#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char RC_DATE[] = "07";
	static const char RC_MONTH[] = "01";
	static const char RC_YEAR[] = "2013";
	static const char RC_UBUNTU_VERSION_STYLE[] = "13.01";
	
	//Software Status
	static const char RC_STATUS[] = "Beta";
	static const char RC_STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long RC_MAJOR = 1;
	static const long RC_MINOR = 2;
	static const long RC_BUILD = 15;
	static const long RC_REVISION = 528;
	
	//Miscellaneous Version Types
	static const long RC_BUILDS_COUNT = 530;
	#define RC_RC_FILEVERSION 1,2,15,528
	#define RC_RC_FILEVERSION_STRING "1, 2, 15, 528\0"
	static const char RC_FULLVERSION_STRING[] = "1.2.15.528";
	
	//SVN Version
	static const char RC_SVN_REVISION[] = "100";
	static const char RC_SVN_DATE[] = "2012-12-29T14:36:32.057539Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long RC_BUILD_HISTORY = 1;
	

}
#endif //VERSION_H
