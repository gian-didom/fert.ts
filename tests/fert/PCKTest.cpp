#include <tuple>
#include <immintrin.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <SpiceUsr.h>
#include "fert/fert.hpp"

namespace test
{
    class PCKTest;
    CPPUNIT_TEST_SUITE_REGISTRATION(PCKTest);

    class PCKTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(PCKTest);
        CPPUNIT_TEST(PCKTests);
        CPPUNIT_TEST_SUITE_END();

    public:
        double kvec[5];
        double kscal;
        double kvec_spice[5];
        double kscal_spice;
        int dim;
        fert::Cfert *frt;

        void setUp()
        {
            furnsh_c("./data/metaKernel.txt");
            this->frt = new fert::Cfert("./data/metaKernel.yml");
        }

        void tearDown()
        {
            // delete[] frt;
        }

    protected:
        void PCKTests()
        {
            // Get scalar constant
            bodvrd_c("3", "GM", 1, &dim, &kscal_spice);

            frt->getConstant(3, "GM", &kscal);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kscal_spice, kscal, 1e-6);

            // Get scalar constant w/ Name
            frt->getConstant("EMB", "GM", &kscal);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kscal_spice, kscal, 1e-6);

            // Get vector constant
            bodvrd_c("199", "NUT_PREC_PM", 5, &dim, kvec_spice);

            frt->getConstant(199, "NUT_PREC_PM", kvec);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kvec_spice[0], kvec[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kvec_spice[1], kvec[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kvec_spice[2], kvec[2], 1e-6);

            // Get scalar constant w/ Name
            frt->getConstant("MERCURY", "NUT_PREC_PM", kvec);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kvec_spice[0], kvec[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kvec_spice[1], kvec[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(kvec_spice[2], kvec[2], 1e-6);
        }
    };
}
