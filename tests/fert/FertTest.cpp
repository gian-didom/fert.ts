#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "fert/fert.hpp"

namespace test
{
    class FertTest;
    CPPUNIT_TEST_SUITE_REGISTRATION(FertTest);

    class FertTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(FertTest);
        CPPUNIT_TEST(ReadTest);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp()
        {
        }

        void tearDown()
        {
            // delete[] frt;
        }

    protected:
        void ReadTest()
        {
            // Test YAML reading and kernel loading
            CPPUNIT_ASSERT_NO_THROW(fert::Cfert("./data/metaKernel.yml"));
        }
    };
}
