// This will define the correct application main() function for us.
// For this to work, we also changed the entry point linker setting: from
// mainWCRTStartup to mainCRTStartup, to fix the 'unresolved wmain' linker
// error.

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>