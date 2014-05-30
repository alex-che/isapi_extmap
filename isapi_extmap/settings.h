#pragma once
#include <map>
#include <string>

class Settings
{
public:
   static Settings & get();
   void init(); // may throw
   const std::map<std::string, std::string> & extensionMap();
private:
   Settings() {} // Singleton
   std::map<std::string, std::string> _extensionMap;
};