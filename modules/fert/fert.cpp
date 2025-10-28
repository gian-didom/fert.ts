// C++ Standard Library
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <utility>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <map>
#include <memory>
#include <tuple>
#include <cmath>
#include <numeric>
// System libraries
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <utility>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <algorithm>
// System libraries
#include <fcntl.h>
#include <sys/stat.h>
// External libraries
#include <yaml-cpp/yaml.h>
// Internal libraries
#include "fert.hpp"
#include "MemoryMap.hpp"
#include "spk2.hpp"
#include "spk3.hpp"
#include "spk9.hpp"
#include "spk21.hpp"
#include "math.hpp"
#include "spiceIDs.hpp"

namespace fert
{
    /// @brief Length of SPICE kernels line
    int const RECLEN = 1024;
    /// @brief Speed of light [km/s]
    double const c = 2.99792458E5;
};

// Main functions
fert::Cfert::Cfert(const char *metaKernelPath)
{
    SpiceIDs = fert::SpiceIDs;

    YAML::Node metaKernel = YAML::LoadFile(metaKernelPath);

    auto spkKernels = metaKernel["spk"];
    for (std::size_t i = 0; i < spkKernels.size(); i++)
    {
        std::string tempKernel = spkKernels[i].as<std::string>();
        fert::Cfert::loadSpk(tempKernel.c_str());
    }

    auto pckKernels = metaKernel["pck"];
    for (std::size_t i = 0; i < pckKernels.size(); i++)
    {
        std::string tempKernel = pckKernels[i].as<std::string>();
        fert::Cfert::loadPck(tempKernel.c_str());
    }
};

fert::Cfert::~Cfert(){
    // Now empty, memory manegement is handled by smart pointers
};

// SPK related functions

void fert::Cfert::loadSpk(const char *kernelPath)
{
    geco::CMemoryMap mm(kernelPath);
    char *buffer = mm.data();

    // Check that it is an ephemeris type
    std::string ephtag = (char *)&buffer[0];
    if (ephtag.compare("DAF/SPK ") == 0)
    {
        throw std::invalid_argument("Not an SPK file.");
    }

    // Change endianness
    std::string fmt = (char *)&buffer[88];
    if (fmt != std::string("LTL-IEEE"))
    {
        throw std::logic_error("Kernel not compatible with machine endianess");
    }

    // Construct ephemeris
    int nd = *(int *)&buffer[8];
    int ni = *(int *)&buffer[12];
    int fward = *(int *)&buffer[76];
    int bward = *(int *)&buffer[80];

    // Find the summary
    int summary_offset = (fward - 1) * RECLEN;
    int summary_size = nd + (ni + 1) / 2;

    double nxt;
    double nsum;
    do
    {
        nxt = *(double *)&buffer[summary_offset];
        nsum = *(double *)&buffer[summary_offset + 16];
        summary_offset += 24;
        for (int n = 0; n < nsum; n++)
        {
            double t0 = *(double *)&buffer[summary_offset];
            double tf = *(double *)&buffer[summary_offset + 8];
            int targetID = *(int *)&buffer[summary_offset + 16];
            int centerID = *(int *)&buffer[summary_offset + 20];
            int rfID = *(int *)&buffer[summary_offset + 24];
            int type = *(int *)&buffer[summary_offset + 28];
            int dbeg = *(int *)&buffer[summary_offset + 32];
            int dend = *(int *)&buffer[summary_offset + 36];

            spkSummary kk(t0, tf, targetID, centerID, rfID, type);
            if (type == 2)
            {
                sMap.insert({kk, std::make_unique<fert::Cspk2>(&buffer[0], dbeg, dend)});
            }
            else if (type == 3)
            {
                sMap.insert({kk, std::make_unique<fert::Cspk3>(&buffer[0], dbeg, dend)});
            }
            else if (type == 21)
            {
                sMap.insert({kk, std::make_unique<fert::Cspk21>(&buffer[0], dbeg, dend)});
            }
            else if (type == 9)
            {
                sMap.insert({kk, std::make_unique<fert::Cspk9>(&buffer[0], dbeg, dend)});
            }
            else
            {
                throw std::runtime_error("Ephemeris type not supported.");
            }

            summary_offset += 40;
        }
        summary_offset = (int)(RECLEN * (nxt - 1));

    } while (nxt > 0.0);

    mm.~CMemoryMap();
};

void fert::Cfert::printSpkSummary()
{
    auto fnc = fert::MyHashFunction();
    int N = sMap.size();
    double ruru[N];
    int n = 0;
    for (auto const &pair : sMap)
    {
        ruru[n] = fnc(pair.first);
        ++n;
    }

    std::vector<int> V(N);

    std::iota(V.begin(), V.end(), 0);
    std::sort(V.begin(), V.end(), [&](int i, int j)
              { return ruru[i] < ruru[j]; });

    std::cout.precision(4);
    std::cout << "t0 \t\ttf  \t\ttargetID \tcenterID  \tRefFrame \tType\n";
    for (int const i : V)
    {
        auto a = sMap.begin();
        std::advance(a, i);
        auto &pair = *a;
        std::cout << pair.first.t << "\t" << pair.first.tf << "\t" << pair.first.targetID << "\t\t" << pair.first.centerID << "\t\t" << pair.first.rfID << "\t\t" << pair.first.type << "\n";
    }
};

fert::spkMap::iterator fert::Cfert::findSPKdata(const double et, const int tID, const int cID, const int rID)
{
    spkSummary k0(et, tID, cID, rID);
    std::cout << "Calling sMap.find(k0)" << std::endl;
    spkMap::iterator it = sMap.find(k0);
    std::cout << "Called sMap.find(k0)" << std::endl;
    if (it == sMap.end())
    {
        throw std::invalid_argument("No kernel data for the given inputs.");
    }
    return it;
};

void fert::Cfert::getState(const double et, const int tID, const int cID, const int rID, double *r)
{
    spkMap::iterator it = findSPKdata(et, tID, cID, rID);
    it->second->getState(et, &r[0]);
};

void fert::Cfert::getState(const double et, const int tID, const int cID, const int rID, double *r, double *v)
{
    spkMap::iterator it = findSPKdata(et, tID, cID, rID);
    it->second->getState(et, &r[0], &v[0]);
};

void fert::Cfert::getState(const double et, const int tID, const int cID, const int rID, double *r, double *v, double *a)
{
    spkMap::iterator it = findSPKdata(et, tID, cID, rID);
    it->second->getState(et, &r[0], &v[0], &a[0]);
};

fert::simd::simd_vec4d fert::Cfert::mmgetStateR(const double et, const int tID, const int cID, const int rID)
{
    spkMap::iterator it = findSPKdata(et, tID, cID, rID);
    return it->second->mmgetStateR(et);
};

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cfert::mmgetStateRV(const double et, const int tID, const int cID, const int rID)
{
    spkMap::iterator it = findSPKdata(et, tID, cID, rID);
    return it->second->mmgetStateRV(et);
};

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cfert::mmgetStateRVA(const double et, const int tID, const int cID, const int rID)
{
    spkMap::iterator it = findSPKdata(et, tID, cID, rID);
    return it->second->mmgetStateRVA(et);
};

void fert::Cfert::getStateLs(const double et, const int tID, const int cID, const int rID, double *rls, double *lt)
{
    double rc[3];
    double vc[3];
    Cfert::getState(et, cID, 0, 1, &rc[0], &vc[0]);

    Cfert::getStateLsX(et, tID, &rc[0], &vc[0], &rls[0], lt);
};

void fert::Cfert::getStateLs(const double et, const int tID, const int cID, const int rID, double *rls, double *vls, double *lt)
{
    double rc[3];
    double vc[3];
    double ac[3];
    Cfert::getState(et, cID, 0, 1, &rc[0], &vc[0], &ac[0]);

    Cfert::getStateLsX(et, tID, &rc[0], &vc[0], &ac[0], &rls[0], &vls[0], lt);
};

void fert::Cfert::getStateLsX(const double et, const int tID, const double *rc, const double *vc, double *rls, double *lt)
{
    fert::simd::simd_vec4d rcmm = fert::simd::simd_vec4d::load_unaligned(rc);
    fert::simd::simd_vec4d vcmm = fert::simd::simd_vec4d::load_unaligned(vc);

    double rt[3];
    *lt = 0;
    for (int i = 0; i < 2; ++i)
    {
        Cfert::getState(et - *lt, tID, 0, 1, &rt[0]);
        fert::simd::simd_vec4d rtmm = fert::simd::simd_vec4d::load_unaligned(rt);
        fert::simd::simd_vec4d remm = rtmm - rcmm;
        *lt = (ymmnormd(remm) / c);
    };

    double *rl;
    Cfert::getState(et - *lt, tID, 0, 1, &rt[0]);
    fert::simd::simd_vec4d rtmm = fert::simd::simd_vec4d::load_unaligned(rt);
    fert::simd::simd_vec4d rlmm = rtmm - rcmm;

    fert::simd::simd_vec4d rhat = ymmuvec(rlmm);

    fert::simd::simd_vec4d vp = vcmm - rhat * ymmdotd(vcmm, rhat);
    fert::simd::simd_vec4d vphat = ymmuvec(vp);

    double Sd = ymmnormd(vp) / c;
    double C = sqrt(1 - Sd * Sd);
    fert::simd::simd_vec4d S = fert::simd::broadcast(Sd);
    fert::simd::simd_vec4d Cm1 = fert::simd::broadcast(C - 1);

    fert::simd::simd_vec4d scorr = ymmnorm(rlmm) * (rhat * Cm1 + vphat * S);
    (rlmm + scorr).store_unaligned(rls);
};

void fert::Cfert::getStateLsX(const double et, const int tID, const double *rc, const double *vc, const double *ac, double *rls, double *vls, double *lt)
{
    fert::simd::simd_vec4d rcmm = fert::simd::simd_vec4d::load_unaligned(rc);
    fert::simd::simd_vec4d vcmm = fert::simd::simd_vec4d::load_unaligned(vc);
    fert::simd::simd_vec4d acmm = fert::simd::simd_vec4d::load_unaligned(ac);

    double rt[3];
    double vt[3];
    *lt = 0;
    for (int i = 0; i < 2; ++i)
    {
        Cfert::getState(et - *lt, tID, 0, 1, &rt[0]);
        fert::simd::simd_vec4d rtmm = fert::simd::simd_vec4d::load_unaligned(rt);
        fert::simd::simd_vec4d remm = rtmm - rcmm;
        *lt = (ymmnormd(remm) / c);
    };

    double *rl;
    Cfert::getState(et - *lt, tID, 0, 1, &rt[0], &vt[0]);
    fert::simd::simd_vec4d rtmm = fert::simd::simd_vec4d::load_unaligned(rt);
    fert::simd::simd_vec4d vtmm = fert::simd::simd_vec4d::load_unaligned(vt);
    fert::simd::simd_vec4d rlmm = rtmm - rcmm;

    double A_ = 1 / (c * ymmnormd(rlmm));
    double C_ = ymmdotd(rlmm, vtmm);
    fert::simd::simd_vec4d B_ = ymmdot(rlmm, vtmm - vcmm);
    fert::simd::simd_vec4d dlt = fert::simd::broadcast(A_) * B_ * fert::simd::broadcast(1 / (1 - C_ * A_));
    fert::simd::simd_vec4d vlmm = (vtmm - vtmm * dlt) - vcmm;

    fert::simd::simd_vec4d rhat = ymmuvec(rlmm);

    fert::simd::simd_vec4d vc_rh = ymmdot(vcmm, rhat);
    fert::simd::simd_vec4d vp = vcmm - rhat * vc_rh;
    fert::simd::simd_vec4d vphat = ymmuvec(vp);

    double Sd = ymmnormd(vp) / c;
    double Cd = sqrt(1 - Sd * Sd);
    fert::simd::simd_vec4d S = fert::simd::broadcast(Sd);
    fert::simd::simd_vec4d C = fert::simd::broadcast(Cd);

    fert::simd::simd_vec4d scorrb = vphat * S + rhat * (C - fert::simd::broadcast(1.0));
    fert::simd::simd_vec4d rlmmn = ymmnorm(rlmm);
    (rlmm + scorrb * rlmmn).store_unaligned(rls);

    //
    fert::simd::simd_vec4d drtmag = ymmdot(vlmm, rhat);
    fert::simd::simd_vec4d drhat = (vlmm - rhat * drtmag) / ymmnorm(rlmm);
    fert::simd::simd_vec4d dvp = acmm - rhat * (ymmdot(vcmm, drhat) + ymmdot(acmm, rhat)) - vc_rh * drhat;
    fert::simd::simd_vec4d vphat_dvp = ymmdot(vphat, dvp);
    fert::simd::simd_vec4d dvphat = (dvp - vphat * vphat_dvp) / ymmnorm(vp);
    fert::simd::simd_vec4d dphi = vphat_dvp * fert::simd::broadcast(1 / (Cd * c));

    fert::simd::simd_vec4d vcorrb = S * (dvphat - rhat * dphi) + C * (drhat + vphat * dphi) - drhat;
    (vlmm + scorrb * drtmag + vcorrb * rlmmn).store_unaligned(vls);
};

fert::spkSummary::spkSummary(double t_0, double t_f, int tID, int cID, int rID, int tp)
{
    t = t_0;
    tf = t_f;
    targetID = tID;
    centerID = cID;
    rfID = rID;
    type = tp;
    fake = false;
};

fert::spkSummary::spkSummary(double t_0, int tID, int cID, int rID)
{
    t = t_0;
    targetID = tID;
    centerID = cID;
    rfID = rID;

    tf = 0;
    type = 0;
    fake = true;
};

// PCK related functions
void fert::Cfert::loadPck(const char *kernelPath)
{
    std::ifstream pckFile;

    pckFile.open(kernelPath);
    if (!pckFile)
    {
        throw std::system_error(errno, std::system_category(), kernelPath);
    }

    // Check that it is a PCK file type
    std::string ephtag;
    std::getline(pckFile, ephtag);
    if (ephtag != std::string("KPL/PCK"))
    {
        throw std::invalid_argument("Not a PCK file.");
    }

    // Save data
    std::string line;
    std::stringstream pckData;
    bool write = false;

    while (getline(pckFile, line))
    {
        // Check if the current line contains the start string
        if (line.find("\\begindata") != std::string::npos)
        {
            size_t poss = line.find("\\begindata");
            size_t post = line.length();
            if ((post - poss) == 10) // begindata is at the end of a string
            {
                write = true;
            }
            continue;
        }

        // Check if the current line contains the end string
        if (line.find("\\begintext") != std::string::npos)
        {
            size_t poss = line.find("\\begintext");
            size_t post = line.length();
            if ((post - poss) == 10) // begintext is at the end of a string
            {
                write = false;
            }
            continue;
        }

        // Execute
        if (write)
        {
            pckData << line << "\n";
        }
    }
    pckFile.close();
    std::string pckDatastr = pckData.str();

    // Use regex to find the data
    // std::regex wordRegex("BODY([0-9]+)_([A-Z]+)\\b\\s+=\\s+\\(\\s+([^\\s]+)\\s+\\)");
    std::regex wordRegex("BODY([0-9]+)_(\\w+)\\b\\s+=\\s+\\(((?:.*?|\\n)*?)\\)");

    std::sregex_iterator it(pckDatastr.begin(), pckDatastr.end(), wordRegex);

    // Output all matching words
    for (std::sregex_iterator i = it; i != std::sregex_iterator(); ++i)
    {
        std::smatch match = *i;
        int bodyNumber = stoi(match[1]);
        std::string bodyLetters = match[2];

        std::string allDatai = match[3];
        std::regex numRegex("([^\\s]+)");
        std::sregex_iterator it2(allDatai.begin(), allDatai.end(), numRegex);
        int N = std::distance(it2, std::sregex_iterator());
        std::vector<double> value;

        for (std::sregex_iterator j = it2; j != std::sregex_iterator(); ++j)
        {
            std::smatch matchData = *j;
            value.push_back(std::stod(matchData[1]));
        }

        fert::pckPair tempPair(bodyNumber, bodyLetters);
        pMap.insert(std::pair<fert::pckPair, std::vector<double>>(tempPair, value));
    }
}

void fert::Cfert::printPckSummary()
{
    std::cout.precision(4);
    std::cout << std::left << std::setw(25) << "\nBODY";
    std::cout << std::left << std::setw(25) << " Type";
    std::cout << std::left << std::setw(25) << " Value";
    std::cout << "\n";
    for (const auto &[key, value] : pMap)
    {
        std::cout << std::left << std::setw(25) << std::get<0>(key);
        std::cout << std::left << std::setw(25) << std::get<1>(key);
        std::cout << "(";
        for (auto &i : value)
        {
            std::cout << i << "   ";
        }
        std::cout << ")\n";
    }
}

void fert::Cfert::getConstant(const std::string body, const std::string type, double *value)
{
    auto it = SpiceIDs.find(body);
    if (it == SpiceIDs.end())
    {
        throw std::invalid_argument("No SpiceID for the given inputs.");
    }
    int bodyID = it->second;
    getConstant(bodyID, type, value);
}

void fert::Cfert::getConstant(const int body, const std::string type, double *value)
{
    fert::pckPair tempPair(body, type);
    auto it = pMap.find(tempPair);
    if (it == pMap.end())
    {
        throw std::invalid_argument("No constant data for the given inputs.");
    }
    std::copy(it->second.begin(), it->second.end(), value);
}
