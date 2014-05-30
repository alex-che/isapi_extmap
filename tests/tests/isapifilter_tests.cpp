#include <cstring>
#include <map>
#include <string>
#include <windows.h>
#include <httpfilt.h>
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>

namespace std
{
inline std::ostream& operator<<(std::ostream& strm, const std::vector<char>& v) 
{
   strm << L"\"" << std::string(v.begin(), v.end()) << L"\"";
   return strm;
}
}

namespace Implementation
{
typedef VOID* (WINAPI *AllocationFunction)(
   HTTP_FILTER_CONTEXT * context, DWORD size, DWORD reserved);
bool replaceExt(HTTP_FILTER_CONTEXT * context,
   HTTP_FILTER_URL_MAP & data, 
   const std::map<std::string, std::string> & replacementMap, 
   AllocationFunction allocate);
}
using Implementation::replaceExt;

namespace 
{
using std::list;
using std::vector;
using boost::shared_ptr;
list<shared_ptr<vector<char>>> allocatedRegions;
VOID * allocate(HTTP_FILTER_CONTEXT *, DWORD size, DWORD)
{
   BOOST_ASSERT(size > 0);
   shared_ptr<vector<char>> region(new vector<char>(size));
   allocatedRegions.push_back(region);
   return &(*region)[0];
}

vector<char> toVector(const char * pString)
{
   BOOST_ASSERT(pString != NULL);
   return vector<char>(pString, pString + strlen(pString) + 1);
}

} // namespace 


BOOST_AUTO_TEST_SUITE(isapifilter_tests)

using std::map;
using std::string;
using std::vector;

BOOST_AUTO_TEST_SUITE(replaceExt_tests)

HTTP_FILTER_CONTEXT * ctx = 0;
map<string, string> emptyReplacementMap;

BOOST_AUTO_TEST_CASE(GivenNullForPathBuffer_ReturnsTrue)
{
   HTTP_FILTER_URL_MAP mapData = {"url", NULL, 0};
   bool success = replaceExt(ctx, mapData, emptyReplacementMap, NULL);

   BOOST_CHECK(success);
}

BOOST_AUTO_TEST_CASE(GivenEmptyMap_DoesNothingAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath"), original = pathBuffer;
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   bool success = replaceExt(ctx, mapData, emptyReplacementMap, NULL);

   BOOST_CHECK(success);
   BOOST_CHECK_EQUAL(pathBuffer, original);
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, original.size());
}

BOOST_AUTO_TEST_CASE(GivenAnIncompatibleReplacementMap_DoesNothingAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath.ccc"), original = pathBuffer;
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   map<string, string> replacementMap;
   replacementMap[".aaa"] = ".bbb";
   bool success = replaceExt(ctx, mapData, replacementMap, NULL);

   BOOST_CHECK(success);
   BOOST_CHECK_EQUAL(pathBuffer, original);
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, original.size());
}

BOOST_AUTO_TEST_CASE(GivenAnExtensionToReplaceOfTheSameSize_ReplacesItAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath.aaa");
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   map<string, string> replacementMap;
   replacementMap[".aaa"] = ".bbb";
   bool success = replaceExt(ctx, mapData, replacementMap, NULL);

   BOOST_CHECK(success);
   BOOST_CHECK_EQUAL(pathBuffer, toVector("somepath.bbb"));
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, pathBuffer.size());
}

BOOST_AUTO_TEST_CASE(GivenAnExtensionToReplaceOfLesserSize_ReplacesItAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath.aaa");
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   map<string, string> replacementMap;
   replacementMap[".aaa"] = ".bb";
   bool success = replaceExt(ctx, mapData, replacementMap, NULL);

   BOOST_CHECK(success);
   BOOST_CHECK_EQUAL(mapData.pszPhysicalPath, string("somepath.bb"));
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, pathBuffer.size());
}

BOOST_AUTO_TEST_CASE(GivenAnExtensionToReplaceOfBiggerSize_ReplacesItAllocatingNewBufferAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath.aaa");
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   map<string, string> replacementMap;
   replacementMap[".aaa"] = ".bbbb";
   size_t initialAllocatedRegionsCount = allocatedRegions.size();
   bool success = replaceExt(ctx, mapData, replacementMap, allocate);

   BOOST_CHECK(success);
   BOOST_REQUIRE_EQUAL(allocatedRegions.size(), initialAllocatedRegionsCount + 1);
   BOOST_CHECK_EQUAL(mapData.pszPhysicalPath, &(*(allocatedRegions.back()))[0]);
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, (*(allocatedRegions.back())).size());
   BOOST_CHECK_EQUAL(vector<char>(mapData.pszPhysicalPath, 
                                  mapData.pszPhysicalPath + mapData.cbPathBuff), 
                     toVector("somepath.bbbb"));
}

BOOST_AUTO_TEST_CASE(GivenAnExtensionToReplaceToEmpty_ReplacesItAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath.aaa");
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   map<string, string> replacementMap;
   replacementMap[".aaa"] = "";
   bool success = replaceExt(ctx, mapData, replacementMap, NULL);

   BOOST_CHECK(success);
   BOOST_CHECK_EQUAL(mapData.pszPhysicalPath, string("somepath"));
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, pathBuffer.size());
}

BOOST_AUTO_TEST_CASE(GivenAnExtensionToReplaceFromEmpty_ReplacesItAllocatingNewBufferAndReturnsTrue)
{
   vector<char> pathBuffer = toVector("somepath");
   HTTP_FILTER_URL_MAP mapData = {"url", &pathBuffer[0], pathBuffer.size()};
   map<string, string> replacementMap;
   replacementMap[""] = ".bbb";
   size_t initialAllocatedRegionsCount = allocatedRegions.size();
   bool success = replaceExt(ctx, mapData, replacementMap, allocate);

   BOOST_CHECK(success);
   BOOST_REQUIRE_EQUAL(allocatedRegions.size(), initialAllocatedRegionsCount + 1);
   BOOST_CHECK_EQUAL(mapData.pszPhysicalPath, &(*(allocatedRegions.back()))[0]);
   BOOST_CHECK_EQUAL(mapData.cbPathBuff, (*(allocatedRegions.back())).size());
   BOOST_CHECK_EQUAL(vector<char>(mapData.pszPhysicalPath, 
      mapData.pszPhysicalPath + mapData.cbPathBuff), 
      toVector("somepath.bbb"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()