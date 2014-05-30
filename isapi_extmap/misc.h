#pragma once
#include <string>
#include <windows.h>
#include <winreg.h>

namespace Misc
{

const char * findEndOfFilename(const char * path);
inline char * findEndOfFilename(char * path)
{
   return const_cast<char*>(findEndOfFilename(const_cast<const char*>(path)));
}

std::string wideCharToMultiByte(const std::wstring & wstr);

class AutoRegKey
{
public:
   AutoRegKey(HKEY key = NULL) : _key(key) {}
   ~AutoRegKey() { if (_key != NULL) RegCloseKey(_key); }
   HKEY & operator*() { return _key; }
   HKEY * operator&() { return & _key; }
private:
   HKEY _key;
};

bool enumRegStringValues(HKEY key, 
                         size_t & index, 
                         std::wstring & name, 
                         std::wstring & value);

} // namespace Misc