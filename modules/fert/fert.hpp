/// Copyright (C) 2021 DART Lab - Politecnico di Milano.
/// All right reserved.

#pragma once

// C++ Standard Library
#include <unordered_map>
#include <map>
#include <tuple>
#include <vector>
#include <memory>
#include <sstream>

// Internal libraries
#include "spk.hpp"
#include "SIMD.hpp"
/// @brief The Fast Ephemeris Retrieval Tool interface
namespace fert
{
    struct spkSummary;
    class MyHashFunction;
    class Cfert;
    /// @brief A map gathering the loaded kernels
    typedef std::unordered_map<spkSummary, std::unique_ptr<fert::Cspk>, MyHashFunction> spkMap;
    typedef std::tuple<int, std::string> pckPair;
    typedef std::map<fert::pckPair, std::vector<double>> pckMap;
}

/// @brief A structure for the ephemerides summary and comparison

struct fert::spkSummary
{
    double t,     ///< Initial kernel ephemeris time [s] after J2000
        tf;       ///< Final kernel ephemeris time [s] after J2000
    int targetID, ///< Target SPICE ID
        centerID, ///< Center SPICE ID
        rfID,     ///< Reference frame SPICE ID
        type;     ///< SPK 
    bool
        fake;

    /**
     * @brief Construct a new Summary object
     *
     * @param t_0 Initial kernel ephemeris time [s] after J2000
     * @param t_f Final kernel ephemeris time [s] after J2000
     * @param tID Target SPICE ID
     * @param cID Center SPICE ID
     * @param rID Reference frame SPICE ID
     * @param tp SPK type
     */
    spkSummary(double t_0, double t_f, int tID, int cID, int rID, int tp);

    /**
     * @brief Construct a (fake) Summary object for fast ephemeris retrieval
     *
     * @see Summary(double t_0, double t_f, int tID, int cID, int rID, int tp);
     */
    spkSummary(double t_0, int tID, int cID, int rID);

    /**
     * @brief Comparison function between loaded kernel data and the requested data
     * It compares the loaded SPK summary data with the requested ephemeris for fast retrieval.
     * Normally, the parameter other is the requested ephemeris data in the form of a (fake) Summary object.
     *
     * @param other Another Summary object to perform comparison (usually the reuqested ephemeris)
     * @return true If the requested ephemeris in \a other is contained in the Summary object
     * @return false If the requested ephemeris in \a other is \b not contained in the Summary object
     */
    bool operator==(const spkSummary &other) const { 
        // std::cout << "Comparing this " << *this << " to other: " << other << std::endl;
        // std::cout << "Target check: "   << (targetID == other.targetID ? "true" :"false") << std::endl;
        // std::cout << "rfID check: "     << (rfID == other.rfID ? "true" : "false") << std::endl;
        // std::cout << "centerID check: "   << (centerID == other.centerID ? "true" :"false") << std::endl;
        // std::cout << "t check: "   << (t >= other.t ? "true" :"false") << std::endl;
        // std::cout << "tf check: "   << (t <= other.tf ? "true" :"false") << std::endl;

        const spkSummary& lhs =  (this->fake) ? *this : other;
        const spkSummary& rhs = (this->fake) ? other : *this;
        return (lhs.targetID == rhs.targetID && lhs.centerID == rhs.centerID && lhs.rfID == rhs.rfID && (lhs.t >= rhs.t && lhs.t <= rhs.tf)); }

    /**
     * @brief Printing function used for debugging with std::cout. It prints a summary of the 
     * summary data.
     */
    friend std::ostream &operator<<(std::ostream &os, const spkSummary& summary) {
        os << "t: " << summary.t << std::endl \
        << "tf : " << summary.tf << std::endl \
        << "targetID : " << summary.targetID << std::endl \
        << "centerID : " << summary.centerID << std::endl \
        << "rfID : " << summary.rfID << std::endl \
        << "type : " << summary.type << std::endl;
        return os;
    }
};

/// @brief Hash function for fast ephemeris retrieval
class fert::MyHashFunction
{
public:
    /**
     * @brief Hash function for fast ephemeris retrieval
     * The hash function is computed as @f[ID_c+170ID_t+31ID_r@f] where @f$ID@f$ is the SPICE ID
     * and the subscripts correlate with the target, center, and reference frame. Coefficients are
     * selected to avoid multiple keys definition.
     *
     * @param p Pointer to a summary line
     * @return std::size_t Hash value computed as per formula
     */
    std::size_t operator()(const spkSummary &p) const { 
    return p.centerID + p.targetID * 170 + p.rfID * 31; }
};

/// @brief The main FERT class
class fert::Cfert
{
public:
    /**
     * @brief Construct a new Cfert object
     *
     * @param metaKernelPath Path to a metakernel yaml
     */
    Cfert(const char *metaKernelPath);

    /// @brief Print the summary of the loaded SPKs
    void printSpkSummary();

    /// @brief Print the summary of the loaded PCKs
    void printPckSummary();

    /// @overload
    void getState(const double et, const int tID, const int cID, const int rID, double *r);
    /// @overload
    void getState(const double et, const int tID, const int cID, const int rID, double *r, double *v);

    /**
     * @brief Get the state of the object \a tID with respect to \a cID in the frame \a rID at time \a et.
     *
     * @param[in] et Ephemeris time in [s] after J2000
     * @param[in] tID Target SPICE ID
     * @param[in] cID Center SPICE ID
     * @param[in] rID Reference frame SPICE ID
     * @param[out] r Pointer to position vector [km]
     * @param[out] v Pointer to velocity vector [km/s]
     * @param[out] a Pointer to acceleration vector [km/s^2]
     * @throw std::invalid_argument if no data fro the given inputs
     */
    void getState(const double et, const int tID, const int cID, const int rID, double *r, double *v, double *a);

    /// @overload
    void getStateLs(const double et, const int tID, const int cID, const int rID, double *r, double *lt);

    /**
     * @brief Get the light-time and aberration corrected state of the object \a tID with respect to \a cID in the frame \a rID at time \a et.
     *
     * @param[in] et Ephemeris time in [s] after J2000
     * @param[in] tID Target SPICE ID
     * @param[in] cID Center SPICE ID
     * @param[in] rID Reference frame SPICE ID
     * @param[out] r Pointer to corrected position vector [km]
     * @param[out] v Pointer to corrected velocity vector [km/s]
     * @param[out] lt Pointer to the light time [s]
     * @throw std::invalid_argument if no data fro the given inputs
     */
    void getStateLs(const double et, const int tID, const int cID, const int rID, double *r, double *v, double *lt);

    /// @overload
    void getStateLsX(const double et, const int tID, const double *rc, const double *vc, double *r, double *lt);

    /**
     * @brief Get the corrected state of the object \a tID with respect to a center object having position \a rc, velocity \a vc, and acceleration \a ac at time \a et.
     *
     * @param[in] et Ephemeris time in [s] after J2000
     * @param[in] tID Target SPICE ID
     * @param[in] rc Pointer to center position vector [km]
     * @param[in] vc Pointer to center velocity vector [km/s]
     * @param[in] ac Pointer to center acceleration vector [km/s^2]
     * @param[out] r Pointer to corrected position vector [km]
     * @param[out] v Pointer to corrected velocity vector [km/s]
     * @param[out] lt Pointer to the light time [s]
     * @throw std::invalid_argument if no data fro the given inputs
     */
    void getStateLsX(const double et, const int tID, const double *rc, const double *vc, const double *ac, double *r, double *v, double *lt);

    fert::simd::simd_vec4d mmgetStateR(const double et, const int tID, const int cID, const int rID);
    std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRV(const double et, const int tID, const int cID, const int rID);
    std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRVA(const double et, const int tID, const int cID, const int rID);

    void getConstant(const int body, const std::string type, double *value);
    void getConstant(const std::string body, const std::string type, double *value);

    /// @brief Destroy a Cfert object
    ~Cfert();

    /// @brief Map containing the loaded SPK kernels data
    spkMap sMap;
    /// @brief Map containing the loaded PCK kernels data
    pckMap pMap;

    std::map<std::string, int> SpiceIDs;

private:
    /**
     * @brief Load the data from a SPK kernel
     *
     * @param kernelPath Path to a SPK kernel
     * @throw std::invalid_argument if \a kernelPath not an ephemeris file
     * @throw std::logic_error if \a kernelPath having different endianness
     * @throw std::runtime_error if ephemeris type not supported
     */
    void loadSpk(const char *kernelPath);

    void loadPck(const char *kernelPath);

    spkMap::iterator findSPKdata(const double et, const int tID, const int cID, const int rID);
};