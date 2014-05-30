#include <string>
#include <boost/test/unit_test.hpp>
#include "misc.h"

BOOST_AUTO_TEST_SUITE(misc_tests)

using std::string;
using Misc::findEndOfFilename;

BOOST_AUTO_TEST_SUITE(findEndOfFilename_tests)

BOOST_AUTO_TEST_CASE(GivenNull_ReturnsNull)
{
   char * result = findEndOfFilename((char*)NULL);

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenAnEmptyString_ReturnsNull)
{
   const char * result = findEndOfFilename("");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenAForwardSlash_ReturnsNull)
{
   const char * result = findEndOfFilename("/");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenABackwardSlash_ReturnsNull)
{
   const char * result = findEndOfFilename("\\");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenForwardSlashOnEnd_ReturnsNull)
{
   const char * result = findEndOfFilename("somestring/");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenBackwardSlashOnEnd_ReturnsNull)
{
   const char * result = findEndOfFilename("somestring\\");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenForwardSlashOnEndOfStringWithADot_ReturnsNull)
{
   const char * result = findEndOfFilename("somestring.ext/");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenBackwardSlashOnEndOfStringWithADot_ReturnsNull)
{
   const char * result = findEndOfFilename("somestring.ext\\");

   BOOST_CHECK_EQUAL(result, (char*)NULL);
}

BOOST_AUTO_TEST_CASE(GivenADot_ReturnsIt)
{
   const char * dot = ".";
   const char * result = findEndOfFilename(dot);

   BOOST_CHECK_EQUAL(result, dot);
}

BOOST_AUTO_TEST_CASE(GivenAStringWithoutADot_ReturnsEndOfString)
{
   const string somestring = "somestring";
   const char * pSomestring = somestring.c_str();
   const char * result = findEndOfFilename(pSomestring);

   BOOST_CHECK_EQUAL(result, pSomestring + somestring.length());
}

BOOST_AUTO_TEST_CASE(GivenADotOnEnd_ReturnsItsPos)
{
   const string somestring = "somestring.";
   const char * pSomestring = somestring.c_str();
   const char * result = findEndOfFilename(pSomestring);

   BOOST_CHECK_EQUAL(result, pSomestring + somestring.length() - 1);
}

BOOST_AUTO_TEST_CASE(GivenADotNotOnTheEnd_ReturnsItsPos)
{
   const string somestring = "somestring.ext";
   const char * pSomestring = somestring.c_str();
   const char * result = findEndOfFilename(pSomestring);

   BOOST_CHECK_EQUAL(result, pSomestring + somestring.length() - 4);
}

BOOST_AUTO_TEST_CASE(GivenADotBeforeAForwardSlash_ReturnsEndOfString)
{
   const string somestring = "somestring.ext/end";
   const char * pSomestring = somestring.c_str();
   const char * result = findEndOfFilename(pSomestring);

   BOOST_CHECK_EQUAL(result, pSomestring + somestring.length());
}

BOOST_AUTO_TEST_CASE(GivenADotBeforeABackwardSlash_ReturnsEndOfString)
{
   const string somestring = "somestring.ext\\end";
   const char * pSomestring = somestring.c_str();
   const char * result = findEndOfFilename(pSomestring);

   BOOST_CHECK_EQUAL(result, pSomestring + somestring.length());
}

BOOST_AUTO_TEST_CASE(GivenSeveralDots_ReturnsTheLast)
{
   const string somestring = "somestring.ext.end";
   const char * pSomestring = somestring.c_str();
   const char * result = findEndOfFilename(pSomestring);

   BOOST_CHECK_EQUAL(result, pSomestring + somestring.length() - 4);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()