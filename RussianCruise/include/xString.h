#ifndef __X_STRING
#define __X_STRING

using namespace std;
#include <vector>
#include <string>

class xString : public string
{
    vector<string> flds;
public:
    xString(char *s) : string(s) { };
    xString(const char *s) : string(s) { };
    xString(string s) : string(s) { };
    xString() : string() { };
    vector<string>& split( const char delim, int rep=0);
    static string join(vector<string> v, string glue);
};

#endif
