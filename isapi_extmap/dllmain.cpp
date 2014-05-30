#include <windows.h>
#include "settings.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD reason, LPVOID lpReserved)
{
   if (reason != DLL_PROCESS_ATTACH)
      return TRUE;

   try
   {
      Settings::get().init();
   }
   catch (...)
   {
      return FALSE;   	
   }

   return Settings::get().extensionMap().empty() ? FALSE : TRUE;
}