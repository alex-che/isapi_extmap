#include <windows.h>
#include <httpfilt.h>

#include "misc.h"
#include "settings.h"

#ifdef UNIT_TEST
#define IMPLEMENTATION_EXPOSED_TO_UNIT_TESTS Implementation
#else
#define IMPLEMENTATION_EXPOSED_TO_UNIT_TESTS
#endif

namespace IMPLEMENTATION_EXPOSED_TO_UNIT_TESTS
{

using std::map;
using std::string;

typedef VOID* (WINAPI *AllocationFunction)(
   HTTP_FILTER_CONTEXT * context, DWORD size, DWORD reserved);
bool replaceExt(HTTP_FILTER_CONTEXT * context,
                HTTP_FILTER_URL_MAP & data, 
                const map<string, string> & replacementMap, 
                AllocationFunction allocate)
{
   char * pExtension = Misc::findEndOfFilename(data.pszPhysicalPath);
   if (pExtension == NULL)
      return true;

   string extension = pExtension;
   map<string, string>::const_iterator it = replacementMap.find(extension);
   if (it == replacementMap.end())
      return true;

   string replacement = it->second;
   int extendedLength = replacement.length() - extension.length();

   if ((extendedLength <= 0) ||
      (pExtension + replacement.length() < data.pszPhysicalPath + data.cbPathBuff))
   {
      // Buffer size is sufficient for in-place modification
      size_t copied = replacement.copy(pExtension, replacement.size());
      pExtension[copied] = '\0';
      return true;
   }

   if (allocate == NULL)
      return false;

   int newBufferSize = data.cbPathBuff + extendedLength;
   char * newBuffer = 
      reinterpret_cast<char*>(allocate(context, newBufferSize, 0));
   if (newBuffer == NULL)
      return false;

   char * end = std::copy(data.pszPhysicalPath, pExtension, newBuffer);
   size_t copied = replacement.copy(end, replacement.size());
   end[copied] = '\0';

   data.pszPhysicalPath = newBuffer;
   data.cbPathBuff = newBufferSize;

   return true;   
}

} // namespace 

#ifdef UNIT_TEST
using IMPLEMENTATION_EXPOSED_TO_UNIT_TESTS::replaceExt;
#endif

BOOL WINAPI GetFilterVersion(HTTP_FILTER_VERSION * filterSettings)
{
   if (filterSettings == NULL)
      return FALSE;

   // HTTPD ignores version info, so don't bother filling it

   filterSettings->dwFlags = SF_NOTIFY_URL_MAP | 
      SF_NOTIFY_ORDER_DEFAULT | 
      SF_NOTIFY_SECURE_PORT | SF_NOTIFY_NONSECURE_PORT;

   return TRUE;
}

DWORD WINAPI HttpFilterProc(HTTP_FILTER_CONTEXT * context, DWORD type, VOID* data)
{
   if (type != SF_NOTIFY_URL_MAP)
      return SF_STATUS_REQ_NEXT_NOTIFICATION;

   HTTP_FILTER_URL_MAP * mapData = static_cast<HTTP_FILTER_URL_MAP*>(data);
   if ((mapData == NULL) || (context == NULL))
   {
      SetLastError(ERROR_INVALID_PARAMETER);
      return SF_STATUS_REQ_ERROR;
   }
   
   return replaceExt(context, 
                     *mapData, 
                     Settings::get().extensionMap(), 
                     context->AllocMem) 
      ? SF_STATUS_REQ_NEXT_NOTIFICATION
      : SF_STATUS_REQ_ERROR;
}

BOOL WINAPI TerminateFilter(DWORD)
{
   return TRUE;
}



