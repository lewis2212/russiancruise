#include "tools.h"

namespace tools
{
	void log(const char *text)
	{
		FILE *file;

		file = fopen("cruise.log", "a");
		fputs(text, file);
		fputs("\n", file);
		fclose(file);
	}

	bool read_mysql(const char *path , mysqlConf& conf)
	{
		memset(&conf, 0, sizeof( mysqlConf ) );

		ifstream readf;
		readf.open(path, ios::in);

		if(readf.is_open() == false)
		{
			cout << "Can't open " << path << endl;
			return false;
		}

		char  str[128];
		while ( readf.good() )
		{
			readf.getline(str, 128);

			if ( strlen(str) == 0 )
				continue;

			if ( strncmp(str, "Host=", 5 ) == 0 )
				strcpy( conf.host , str+5 );

			if ( strncmp(str, "Port=", 5 ) == 0 )
				conf.port = atoi( str+5 );

			if ( strncmp(str, "User=", 5 ) == 0 )
				strcpy( conf.user , str+5 );

			if ( strncmp(str, "Pass=", 5 ) == 0 )
				strcpy( conf.password , str+5 );

			if ( strncmp(str, "DBName=", 7 ) == 0 )
				strcpy( conf.database , str+7 );
		}
		readf.close();

		if( strlen(conf.host) == 0 ||
		   conf.port == 0 ||
		   strlen(conf.user) == 0 ||
		   strlen(conf.password) == 0 ||
		   strlen(conf.database) == 0)
		{
			return false;
		}

		return true;
	}
#ifdef __linux__
	string convert_encoding(const string& data, const string& from, const string& to)
	{
		 if (data.empty())
		 {
			  return string();
		 }
		 iconv_t convert_hnd = iconv_open(to.c_str(), from.c_str());
		 if (convert_hnd == (iconv_t)(-1))
		 {
			  throw logic_error("unable to create convertion descriptor");
		 }

		 char* in_ptr = const_cast<char*>(data.c_str());
		 size_t in_size = data.size();
		 vector<char> outbuf(6 * data.size());
		 char* out_ptr = &outbuf[0];
		 size_t reverse_size = outbuf.size();

		 size_t result = iconv(convert_hnd, &in_ptr, &in_size, &out_ptr, &reverse_size);
		 iconv_close(convert_hnd);
		 if (result == (size_t)(-1))
		 {
			  throw logic_error("unable to convert");
		 }
		 return string(outbuf.data(), outbuf.size() - reverse_size);
	}
#endif // __linux__

	struct tm * GetLocalTime()
	{
		time_t rawtime;
		tm *timeStruct;

		time (&rawtime);

		timeStruct = localtime (&rawtime);
		timeStruct->tm_year += 1900;
		timeStruct->tm_mon += 1;
		timeStruct->tm_wday += 1;
		timeStruct->tm_yday += 1;

		return timeStruct;
	}

	bool isAdmin(const char *path, string username)
	{
		char file[255];
		sprintf(file,"%s/misc/admins.txt",path);


		ifstream readf;
		readf.open(file, ios::in);

		if(readf.is_open() == false)
		{
			cout << "Can't open " << file << endl;
			return false;
		}

		char  str[128];
		while ( readf.good() )
		{
			readf.getline(str, 128);

			if ( strlen(str) == 0 )
				continue;

			if ( strncmp(str, username.c_str(), username.length() ) == 0 )
			{
				cout << username << " connected as admin." << endl;
				readf.close();
				return true;
			}
				
		}
		readf.close();
		return false;

	}

}
