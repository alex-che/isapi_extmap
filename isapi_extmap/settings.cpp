#include <string>
#include <windows.h>
#include <winreg.h>
#include "misc.h"
#include "settings.h"

Settings & Settings::get()
{
   static Settings globalSettings;
   return globalSettings;
}

namespace 
{
using std::string;
using std::wstring;
   
const wstring ExtMapKeyW = L"Comm\\HTTPD\\ExtMap";

void normalizeExtension(string & extension)
{
   if (extension == ".")
   {
      extension.clear();
   }
   else if ((!extension.empty()) && (extension[0] != '.'))
   {
      extension.insert(extension.begin(), 1, '.');
   }
}

bool isExtension(wstring & extension)
{
   return (!extension.empty()) && (extension[0] == L'.');
}

} // namespace 

void Settings::init()
{
   using Misc::AutoRegKey;
   using Misc::wideCharToMultiByte;
   using Misc::enumRegStringValues;

   _extensionMap.clear();

   AutoRegKey key;
   if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, ExtMapKeyW.c_str(), 0, 0, &key) != 
      ERROR_SUCCESS)
      throw std::exception("Cannot open registry settings key");

   size_t index = 0;
   wstring name, value;
   while (enumRegStringValues(*key, index, name, value))
   {
      if ((!isExtension(name)) || (!isExtension(value)))
         continue;

      string extension = wideCharToMultiByte(name);
      normalizeExtension(extension);

      string replacement = wideCharToMultiByte(value);
      normalizeExtension(replacement);

      _extensionMap[extension] = replacement;
   }
}

const std::map<std::string, std::string> & Settings::extensionMap()
{
   return _extensionMap;
}