//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

// Model NED sources for direct embedding into the executable

const char *ALOHA_NED =
	"@namespace(aloha);\n"
	"network Aloha\n"
	"{\n"
	"    parameters:\n"
	"        int numHosts;  // number of hosts\n"
	"        double txRate @unit(bps) = default(9.6kbps); \n"
	"        double slotTime @unit(ms) = default(100ms);\n"
	"    submodules:\n"
	"        server: Server;\n"
	"        host[numHosts]: Host {\n"
	"            txRate = txRate;\n"
	"            slotTime = slotTime;\n"
	"        }\n"
	"}\n";

const char *SERVER_NED =
	"@namespace(aloha);\n"
	"simple Server\n"
	"{\n"
	"    gates:\n"
	"        input in @directIn;\n"
	"}\n";

const char *HOST_NED =
	"@namespace(aloha);\n"
	"simple Host\n"
	"{\n"
	"    parameters:\n"
	"        double txRate @unit(bps);\n"
	"        double radioDelay @unit(s) = 10ms;\n"
	"        volatile int pkLenBits @unit(b) = default(952b);\n"
	"        volatile double iaTime @unit(s);\n"
	"        double slotTime @unit(s); \n"
	"}\n";
