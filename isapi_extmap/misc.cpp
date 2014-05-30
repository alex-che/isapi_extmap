#include <vector>
#include "misc.h"

namespace Misc
{

using std::string;
using std::vector;
using std::wstring;

const char * findEndOfFilename(const char * path)
{
   if ((path == 0) || (*path == '\0'))
      return 0;

   const char * lastDot = 0;
   const char * p;
   for (p = path; *p != '\0'; ++p)
   {
      if (*p == '.')
         lastDot = p;
      else if ((*p == '\\') || (*p == '/'))
         lastDot = 0;
   }
   const char * end = p;

   if (lastDot != 0)
      return lastDot;

   return (((*(end - 1)) == '\\') || (*(end - 1) == '/')) ? 0 : end;
}

string wideCharToMultiByte(const wstring & wstr)
{
   if (wstr.empty())
      return string();

   int bytesNeeded = ::WideCharToMultiByte(CP_UTF8, 0, 
      &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
   if (bytesNeeded == 0)
      throw std::exception("Failed converting wide char string to UTF-8");

   string str(bytesNeeded, 0);
   if (::WideCharToMultiByte(CP_UTF8, 0, 
      &wstr[0], (int)wstr.size(), &str[0], bytesNeeded, NULL, NULL) == 0)
      throw std::exception("Failed converting wide char string to UTF-8");

   return str;
}

bool enumRegStringValues(HKEY key, size_t & index, wstring & name, wstring & value)
{
   DWORD nameLengthInChars = 4096;
   vector<wchar_t> nameBuffer(nameLengthInChars);

   DWORD valueLengthInBytes = 4096;
   vector<wchar_t> valueBuffer(valueLengthInBytes / sizeof(wchar_t));

   DWORD type, result;
   while ((result = RegEnumValueW(
      key, static_cast<DWORD>(index), 
      &nameBuffer[0], &nameLengthInChars, 
      NULL, &type, 
      reinterpret_cast<BYTE*>(&valueBuffer[0]), &valueLengthInBytes)) != 
      ERROR_NO_MORE_ITEMS)
   {
      if (result == ERROR_MORE_DATA)
      {
         if (nameLengthInChars > nameBuffer.size())
            nameBuffer.resize(nameLengthInChars);
         else if (valueLengthInBytes > valueBuffer.size() * sizeof(wchar_t))
            valueBuffer.resize(valueLengthInBytes / sizeof(wchar_t) + 1);
         else
            throw std::exception("Failed to enumerate registry values");
         continue;
      }

      if (result != ERROR_SUCCESS)
         throw std::exception("Failed to enumerate registry values");

      ++index;
      if (type == REG_SZ)
      {
         name = &nameBuffer[0];
         value = &valueBuffer[0];
         return true;
      }
   }
   return false;
}

} // namespace Misc