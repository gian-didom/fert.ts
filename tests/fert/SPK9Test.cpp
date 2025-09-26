#include <tuple>
#include <immintrin.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <SpiceUsr.h>
#include "fert/fert.hpp"

namespace test
{
    class SPK9Test;
    CPPUNIT_TEST_SUITE_REGISTRATION(SPK9Test);

    class SPK9Test : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(SPK9Test);
        CPPUNIT_TEST(SPK9Tests);
        CPPUNIT_TEST_SUITE_END();

    public:
        double x[6];
        double xp[6];
        double lt;
        __m256d rmm;
        __m256d vmm;
        __m256d amm;
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
        void SPK9Tests()
        {
            spkezr_c("-100009", 865468869.184809, "J2000", "None", "399", x, &lt);
            spkezr_c("-100009", 865468869.184809 + 1.0, "J2000", "None", "399", xp, &lt);
            double acc[6];
            for (int i = 0; i < 6; i++)
            {
                acc[i] = (xp[i] - x[i]);
            }

            rmm = frt->mmgetStateR(865468869.184809, -100009, 399, 1);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);

            std::tuple rv = frt->mmgetStateRV(865468869.184809, -100009, 399, 1);
            rmm = std::get<0>(rv);
            vmm = std::get<1>(rv);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[3], vmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[4], vmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[5], vmm[2], 1e-6);

            std::tuple rva = frt->mmgetStateRVA(865468869.184809, -100009, 399, 1);
            rmm = std::get<0>(rva);
            vmm = std::get<1>(rva);
            amm = std::get<2>(rva);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[3], vmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[4], vmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[5], vmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(acc[3], amm[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(acc[4], amm[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(acc[5], amm[2], 1e-8);
        }
    };
}
