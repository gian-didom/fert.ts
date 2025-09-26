#include <tuple>
#include <immintrin.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <SpiceUsr.h>
#include "fert/fert.hpp"

namespace test
{
    class SPKChebTest;
    CPPUNIT_TEST_SUITE_REGISTRATION(SPKChebTest);

    class SPKChebTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(SPKChebTest);
        CPPUNIT_TEST(SPK2Test);
        CPPUNIT_TEST(SPK3Test);
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
        void SPK2Test()
        {
            spkezr_c("3", 739238469.184808, "J2000", "None", "0", x, &lt);
            spkezr_c("3", 739238469.184808 + 1.0, "J2000", "None", "0", xp, &lt);
            double acc[6];
            for (int i = 0; i < 6; i++)
            {
                acc[i] = (xp[i] - x[i]);
            }

            rmm = frt->mmgetStateR(739238469.184808, 3, 0, 1);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);

            std::tuple rv = frt->mmgetStateRV(739238469.184808, 3, 0, 1);
            rmm = std::get<0>(rv);
            vmm = std::get<1>(rv);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[3], vmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[4], vmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[5], vmm[2], 1e-6);

            std::tuple rva = frt->mmgetStateRVA(739238469.184808, 3, 0, 1);
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

        void SPK3Test()
        {
            spkezr_c("401", 644630469.184789, "J2000", "None", "4", x, &lt);
            spkezr_c("401", 644630469.184789 + 1.0, "J2000", "None", "4", xp, &lt);
            double acc[6];
            for (int i = 0; i < 6; i++)
            {
                acc[i] = (xp[i] - x[i]);
            }

            rmm = frt->mmgetStateR(644630469.184789, 401, 4, 1);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);

            std::tuple rv = frt->mmgetStateRV(644630469.184789, 401, 4, 1);
            rmm = std::get<0>(rv);
            vmm = std::get<1>(rv);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[3], vmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[4], vmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[5], vmm[2], 1e-6);

            std::tuple rva = frt->mmgetStateRVA(644630469.184789, 401, 4, 1);
            rmm = std::get<0>(rva);
            vmm = std::get<1>(rva);
            amm = std::get<2>(rva);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[0], rmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[1], rmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[2], rmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[3], vmm[0], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[4], vmm[1], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(x[5], vmm[2], 1e-6);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(acc[3], amm[0], 1e-7);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(acc[4], amm[1], 1e-7);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(acc[5], amm[2], 1e-7);
        }
    };
}
