#include "xString.h"
using namespace std;
// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<string>&
xString::split( const char delim, int rep)
{
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    unsigned int i = 0;
    while (i < work.length())
    {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1)
        {
            flds.push_back(buf);
            buf = "";
        }
        else if (buf.length() > 0)
        {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}

string
xString::join(vector<string> v, string glue)
{
    string result;
    for (size_t i = 0, i_end = v.size(); i < i_end; ++i) {
        result += (i ? glue : "");  // main line in this blog-post
        result += v[i];
    }
    return result;
}
