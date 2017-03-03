//==========================================================================
//   CHASHER.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
   Copyright (C) 1992-2017 Andras Varga
   Copyright (C) 2006-2017 OpenSim Ltd.

   This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/chasher.h"

namespace omnetpp {

void cHasher::add(const char *p, size_t length)
{
    // add the bulk in 4-byte chunks
    size_t lengthmod4 = length & ~3U;
    size_t i;
    for (i = 0; i < lengthmod4; i += 4)
        merge((uint32_t)(p[i] | (p[i+1] << 8) | (p[i+2] << 16) | (p[i+3] << 24)));

    // add the 1, 2 or 3 bytes left
    switch (length - i) {
        case 0: break;
        case 1: merge((uint32_t)(p[i])); break;
        case 2: merge((uint32_t)(p[i] | (p[i+1] << 8))); break;
        case 3: merge((uint32_t)(p[i] | (p[i+1] << 8) | (p[i+2] << 16))); break;
        default: ASSERT(false);
    }
}

uint32_t cHasher::parse(const char *hash) const
{
    // remove spaces, hyphens and colons before parsing
    std::string s;
    for (const char *p = hash; *p; p++)
        if (*p != ' ' && *p != '-' && *p != ':')
            s += *p;

    // parse
    char *e;
    unsigned long d = strtoul(s.c_str(), &e, 16);
    uint32_t value = (uint32_t)d;
    if (*e || value != d)
        throw cRuntimeError("Cannot verify hash: Invalid hash text \"%s\"", hash);
    return value;
}

bool cHasher::equals(const char *hash) const
{
    uint32_t value = parse(hash);
    return getHash() == value;
}

std::string cHasher::str() const
{
    char buf[32];
    sprintf(buf, "%08x", getHash());
    std::string str = buf;
    str.insert(str.length() - 4, "-");
    return str;
}

/* XXX to test case

int main(int argc, char **argv)
{
#define PRINT printf("%x\n", h.getHash())
    cHasher h;
    h.add(1); PRINT;
    h.add(4); PRINT;
    h.add(1); PRINT;
    h.add(0xffffffff); PRINT;
    h.add(0); h.add(0); h.add(0); h.add(0); PRINT;

    h.reset();
    h.add("alma"); PRINT;

    h.reset();
    h.add("almaalma"); PRINT;

    h.reset();
    h.add("almaalmaalma"); PRINT;
    return 0;
}
*/

}  // namespace omnetpp

