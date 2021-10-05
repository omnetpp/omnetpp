//==========================================================================
//  CLCG32.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
// Contents:
//   class cLCG32
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/clcg32.h"
#include "omnetpp/clog.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cconfigoption.h"

namespace omnetpp {

Register_Class(cLCG32);

Register_PerRunConfigOption(CFGID_SEED_N_LCG32, "seed-%-lcg32", CFG_INT, nullptr, "When cLCG32 is selected as random number generator: seed for the kth RNG. (Substitute k for '%' in the key.)");

void cLCG32::initialize(int seedSet, int rngId, int numRngs,
        int  /*parsimProcId*/, int parsimNumPartitions,
        cConfiguration *cfg)
{
    if (parsimNumPartitions > 1)
        throw cRuntimeError("The cLCG32 RNG is not suitable for parallel simulation runs "
                            "because of its short cycle -- please select cMersenneTwister "
                            "in the configuration instead");

    char key[32];
    sprintf(key, "seed-%d-lcg32", rngId);
    const char *value = cfg->getConfigValue(key);
    if (value == nullptr) {
        int autoSeedIndex = seedSet * numRngs + rngId;
        if (autoSeedIndex >= 256)
            EV << "Warning: LCG32: out of the 256 auto seed values, wrapping around "
                  "-- decrease num-rngs=" << numRngs << " value or run numbers, "
                                                        "or use a different RNG class like Mersenne Twister\n";
        autoSeedIndex = autoSeedIndex % 256;
        seed = autoSeeds[autoSeedIndex];
    }
    else {
        seed = cConfiguration::parseLong(value, nullptr);
        if (seed == 0)
            throw cRuntimeError("cLCG32: Zero is not allowed as seed in %s config file entry", key);
    }
}

void cLCG32::selfTest()
{
    seed = 1;
    for (int i = 0; i < 10000; i++)
        intRand();
    if (seed != 1043618065L)
        throw cRuntimeError("cLCG32: selfTest() failed, please report this problem!");
}

uint32_t cLCG32::intRand()
{
    numDrawn++;
    const long int a = 16807, q = 127773, r = 2836;
    seed = a * (seed % q) - r * (seed / q);
    if (seed <= 0)
        seed += LCG32_MAX + 1;
    return seed - 1;  // shift range [1..2^31-2] to [0..2^31-3]
}

uint32_t cLCG32::intRandMax()
{
    return LCG32_MAX - 1;  // 2^31-3
}

uint32_t cLCG32::intRand(uint32_t n)
{
    if (n > LCG32_MAX)
        throw cRuntimeError("cLCG32: intRand(%u): Argument out of range 1..2^31-2", (unsigned)n);

    // code from MersenneTwister.h, Richard J. Wagner rjwagner@writeme.com
    // Find which bits are used in n
    uint32_t used = n - 1;
    used |= used >> 1;
    used |= used >> 2;
    used |= used >> 4;
    used |= used >> 8;
    used |= used >> 16;

    // Draw numbers until one is found in [0,n]
    uint32_t i;
    do
        i = intRand() & used;  // toss unused bits to shorten search
    while (i >= n);
    return i;
}

double cLCG32::doubleRand()
{
    return (double)intRand() * (1.0 / LCG32_MAX);
}

double cLCG32::doubleRandNonz()
{
    return (double)(intRand() + 1) * (1.0 / (LCG32_MAX + 1));
}

double cLCG32::doubleRandIncl1()
{
    return (double)intRand() * (1.0 / (LCG32_MAX - 1));
}

uint32_t cLCG32::autoSeeds[] = {
    1L, 1331238991L, 1550655590L, 930627303L, 766698560L, 372156336L,
    1645116277L, 1635860990L, 1154667137L, 692982627L, 1961833381L,
    713190994L, 460575272L, 1298018763L, 1497719440L, 2030952567L,
    901595110L, 631930165L, 354421844L, 1213737427L, 1800697079L,
    795809157L, 821079954L, 1624537871L, 1918430133L, 861482464L,
    1736896562L, 1220028201L, 634729389L, 456922549L, 23246132L,
    979545543L, 1008653149L, 1156436224L, 1689665648L, 1604778030L,
    1628735393L, 1949577498L, 550023088L, 1726571906L, 1267216792L,
    1750609806L, 314116942L, 736299588L, 2095003524L, 1281569003L,
    356484144L, 1423591576L, 2140958617L, 1577658393L, 1116852912L,
    1865734753L, 1701937813L, 301264580L, 171817360L, 1809513683L,
    360646631L, 546534802L, 1652205397L, 136501886L, 605448579L,
    1857604347L, 1223969344L, 668104522L, 1821072732L, 738721927L,
    1237280745L, 1753702432L, 2125855022L, 1259255700L, 935058038L,
    1325807218L, 1151620124L, 585222105L, 1970906347L, 1267057970L,
    66562942L, 1959090863L, 1503754591L, 114059398L, 2007872839L,
    1886985293L, 1870986444L, 2110445215L, 1375265396L, 1512926254L,
    470646700L, 1951555990L, 500432100L, 1843528576L, 347147950L,
    1431668171L, 929595364L, 1507452793L, 800443847L, 1428656866L,
    5715478L, 1607979231L, 2032092669L, 37809413L, 996425830L,
    1010869813L, 1884232020L, 312192738L, 1821061493L, 462270727L,
    248900140L, 678804905L, 905130946L, 1892339752L, 1307421505L,
    491642575L, 1091346202L, 1076664921L, 1140141037L, 122447008L,
    1244153851L, 1382627577L, 611793617L, 1989326495L, 808278095L,
    1352281487L, 2106046913L, 1731628906L, 1226683843L, 1683200486L,
    90274853L, 1676390615L, 2147466840L, 498396356L, 2140522509L,
    1217803227L, 1146667727L, 788324559L, 1530171233L, 317473611L,
    319146380L, 992289339L, 2077765218L, 652681396L, 789950931L,
    485020132L, 632682054L, 32775496L, 1683083109L, 603834907L,
    351704670L, 1809710911L, 171230418L, 1511135464L, 1986612391L,
    1646573708L, 1411714374L, 1546459273L, 872179784L, 1307370996L,
    801917373L, 2051724276L, 144283230L, 1535180348L, 1949917822L,
    650872229L, 113201992L, 890256110L, 1965781805L, 1903960687L,
    679060319L, 452497769L, 630187802L, 174438105L, 1298843779L,
    961082145L, 1565131991L, 2078229636L, 50366922L, 959177042L,
    144513213L, 1423462005L, 207666443L, 152219823L, 13354949L,
    412643566L, 631135695L, 166938633L, 958408264L, 1324624652L,
    494931978L, 1472820641L, 1150735880L, 1508483704L, 1640573652L,
    359288909L, 1315013967L, 1051019865L, 1254156333L, 1883764098L,
    587564032L, 1288274932L, 1912367727L, 1595891993L, 2138169990L,
    1794668172L, 2059764593L, 1152025509L, 115613893L, 926625010L,
    131630606L, 706594585L, 1386707532L, 1624163092L, 2081362360L,
    1882152641L, 1428465736L, 602313149L, 1170668648L, 863700348L,
    931140599L, 1856765731L, 197473249L, 507314638L, 1381732824L,
    252975355L, 925311926L, 1726193892L, 576725369L, 774762078L,
    198434005L, 192355221L, 1296038143L, 1201667973L, 653782169L,
    1426685702L, 1503907840L, 211726157L, 33491376L, 906578176L,
    238345926L, 1826083853L, 1366925216L, 480315631L, 1549695660L,
    1337366022L, 1793656969L, 1469954017L, 1701980729L, 98857548L,
    1883864564L, 1709982325L, 251608257L, 1171967839L, 642486710L,
    1358844649L, 1115145546L, 1398997376L, 1021484058L, 2035865982L,
};

}  // namespace omnetpp

