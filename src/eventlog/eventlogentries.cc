
#include "eventlogentries.h"

BubbleEntry::BubbleEntry()
{
   moduleId = -1;
   text = NULL;
}

void BubbleEntry::parse(const char **tokens, int numTokens)
{
   moduleId = getIntToken(tokens, numTokens, "id");
   text = getStringToken(tokens, numTokens, "txt");
}

void BubbleEntry::print(FILE *fout)
{
   ::fprintf(fout, "BU ");
   ::fprintf(fout, "id %d ", moduleId);
   ::fflush(fout);
   ::fprintf(fout, "txt %s ", text);
   ::fflush(fout);
} 

ModuleMethodCalledEntry::ModuleMethodCalledEntry()
{
   fromModuleId = -1;
   toModuleId = -1;
   method = NULL;
}

void ModuleMethodCalledEntry::parse(const char **tokens, int numTokens)
{
   fromModuleId = getIntToken(tokens, numTokens, "sm");
   toModuleId = getIntToken(tokens, numTokens, "tm");
   method = getStringToken(tokens, numTokens, "m");
}

void ModuleMethodCalledEntry::print(FILE *fout)
{
   ::fprintf(fout, "MM ");
   ::fprintf(fout, "sm %d ", fromModuleId);
   ::fflush(fout);
   ::fprintf(fout, "tm %d ", toModuleId);
   ::fflush(fout);
   ::fprintf(fout, "m %s ", method);
   ::fflush(fout);
} 

ModuleCreatedEntry::ModuleCreatedEntry()
{
   moduleId = -1;
   moduleClassName = NULL;
   parentModuleId = -1;
   fullName = NULL;
}

void ModuleCreatedEntry::parse(const char **tokens, int numTokens)
{
   moduleId = getIntToken(tokens, numTokens, "id");
   moduleClassName = getStringToken(tokens, numTokens, "c");
   parentModuleId = getIntToken(tokens, numTokens, "pid");
   fullName = getStringToken(tokens, numTokens, "n");
}

void ModuleCreatedEntry::print(FILE *fout)
{
   ::fprintf(fout, "MC ");
   ::fprintf(fout, "id %d ", moduleId);
   ::fflush(fout);
   ::fprintf(fout, "c %s ", moduleClassName);
   ::fflush(fout);
   ::fprintf(fout, "pid %d ", parentModuleId);
   ::fflush(fout);
   ::fprintf(fout, "n %s ", fullName);
   ::fflush(fout);
} 

ModuleDeletedEntry::ModuleDeletedEntry()
{
   moduleId = -1;
}

void ModuleDeletedEntry::parse(const char **tokens, int numTokens)
{
   moduleId = getIntToken(tokens, numTokens, "id");
}

void ModuleDeletedEntry::print(FILE *fout)
{
   ::fprintf(fout, "MD ");
   ::fprintf(fout, "id %d ", moduleId);
   ::fflush(fout);
} 

ModuleReparentedEntry::ModuleReparentedEntry()
{
   moduleId = -1;
   newParentModuleId = -1;
}

void ModuleReparentedEntry::parse(const char **tokens, int numTokens)
{
   moduleId = getIntToken(tokens, numTokens, "id");
   newParentModuleId = getIntToken(tokens, numTokens, "p");
}

void ModuleReparentedEntry::print(FILE *fout)
{
   ::fprintf(fout, "MR ");
   ::fprintf(fout, "id %d ", moduleId);
   ::fflush(fout);
   ::fprintf(fout, "p %d ", newParentModuleId);
   ::fflush(fout);
} 

ConnectionCreatedEntry::ConnectionCreatedEntry()
{
   sourceModuleId = -1;
   sourceGateId = -1;
   sourceGateFullName = -1;
   destModuleId = -1;
   destGateId = -1;
   destGateFullName = -1;
}

void ConnectionCreatedEntry::parse(const char **tokens, int numTokens)
{
   sourceModuleId = getIntToken(tokens, numTokens, "sm");
   sourceGateId = getIntToken(tokens, numTokens, "sg");
   sourceGateFullName = getIntToken(tokens, numTokens, "sn");
   destModuleId = getIntToken(tokens, numTokens, "dm");
   destGateId = getIntToken(tokens, numTokens, "dg");
   destGateFullName = getIntToken(tokens, numTokens, "dn");
}

void ConnectionCreatedEntry::print(FILE *fout)
{
   ::fprintf(fout, "CC ");
   ::fprintf(fout, "sm %d ", sourceModuleId);
   ::fflush(fout);
   ::fprintf(fout, "sg %d ", sourceGateId);
   ::fflush(fout);
   ::fprintf(fout, "sn %d ", sourceGateFullName);
   ::fflush(fout);
   ::fprintf(fout, "dm %d ", destModuleId);
   ::fflush(fout);
   ::fprintf(fout, "dg %d ", destGateId);
   ::fflush(fout);
   ::fprintf(fout, "dn %d ", destGateFullName);
   ::fflush(fout);
} 

ConnectionRemovedEntry::ConnectionRemovedEntry()
{
   sourceModuleId = -1;
   sourceGateId = -1;
}

void ConnectionRemovedEntry::parse(const char **tokens, int numTokens)
{
   sourceModuleId = getIntToken(tokens, numTokens, "sm");
   sourceGateId = getIntToken(tokens, numTokens, "sg");
}

void ConnectionRemovedEntry::print(FILE *fout)
{
   ::fprintf(fout, "CD ");
   ::fprintf(fout, "sm %d ", sourceModuleId);
   ::fflush(fout);
   ::fprintf(fout, "sg %d ", sourceGateId);
   ::fflush(fout);
} 

ConnectionDisplayStringChangedEntry::ConnectionDisplayStringChangedEntry()
{
   sourceModuleId = -1;
   sourceGateId = -1;
   displayString = NULL;
}

void ConnectionDisplayStringChangedEntry::parse(const char **tokens, int numTokens)
{
   sourceModuleId = getIntToken(tokens, numTokens, "sm");
   sourceGateId = getIntToken(tokens, numTokens, "sg");
   displayString = getStringToken(tokens, numTokens, "d");
}

void ConnectionDisplayStringChangedEntry::print(FILE *fout)
{
   ::fprintf(fout, "CS ");
   ::fprintf(fout, "sm %d ", sourceModuleId);
   ::fflush(fout);
   ::fprintf(fout, "sg %d ", sourceGateId);
   ::fflush(fout);
   ::fprintf(fout, "d %s ", displayString);
   ::fflush(fout);
} 

ModuleDisplayStringChangedEntry::ModuleDisplayStringChangedEntry()
{
   moduleId = -1;
   displayString = NULL;
}

void ModuleDisplayStringChangedEntry::parse(const char **tokens, int numTokens)
{
   moduleId = getIntToken(tokens, numTokens, "id");
   displayString = getStringToken(tokens, numTokens, "d");
}

void ModuleDisplayStringChangedEntry::print(FILE *fout)
{
   ::fprintf(fout, "DS ");
   ::fprintf(fout, "id %d ", moduleId);
   ::fflush(fout);
   ::fprintf(fout, "d %s ", displayString);
   ::fflush(fout);
} 

BackgroundDisplayStringChangedEntry::BackgroundDisplayStringChangedEntry()
{
   moduleId = -1;
   displayString = NULL;
}

void BackgroundDisplayStringChangedEntry::parse(const char **tokens, int numTokens)
{
   moduleId = getIntToken(tokens, numTokens, "id");
   displayString = getStringToken(tokens, numTokens, "d");
}

void BackgroundDisplayStringChangedEntry::print(FILE *fout)
{
   ::fprintf(fout, "DG ");
   ::fprintf(fout, "id %d ", moduleId);
   ::fflush(fout);
   ::fprintf(fout, "d %s ", displayString);
   ::fflush(fout);
} 

EventEntry::EventEntry()
{
   eventNumber = -1;
   simulationTime = -1;
   moduleId = -1;
   messageId = -1;
}

void EventEntry::parse(const char **tokens, int numTokens)
{
   eventNumber = getLongToken(tokens, numTokens, "#");
   simulationTime = getSimtimeToken(tokens, numTokens, "t");
   moduleId = getIntToken(tokens, numTokens, "m");
   messageId = getLongToken(tokens, numTokens, "msg");
}

void EventEntry::print(FILE *fout)
{
   ::fprintf(fout, "E ");
   ::fprintf(fout, "# %ld ", eventNumber);
   ::fflush(fout);
   ::fprintf(fout, "t %x ", simulationTime);
   ::fflush(fout);
   ::fprintf(fout, "m %d ", moduleId);
   ::fflush(fout);
   ::fprintf(fout, "msg %ld ", messageId);
   ::fflush(fout);
} 

MessageScheduledEntry::MessageScheduledEntry()
{
   messageId = -1;
   messageTreeId = -1;
   messageClassName = NULL;
   messageFullName = NULL;
   senderModuleId = -1;
   arrivalTime = -1;
}

void MessageScheduledEntry::parse(const char **tokens, int numTokens)
{
   messageId = getLongToken(tokens, numTokens, "id");
   messageTreeId = getLongToken(tokens, numTokens, "tid");
   messageClassName = getStringToken(tokens, numTokens, "c");
   messageFullName = getStringToken(tokens, numTokens, "n");
   senderModuleId = getIntToken(tokens, numTokens, "sm");
   arrivalTime = getSimtimeToken(tokens, numTokens, "t");
}

void MessageScheduledEntry::print(FILE *fout)
{
   ::fprintf(fout, "SA ");
   ::fprintf(fout, "id %ld ", messageId);
   ::fflush(fout);
   ::fprintf(fout, "tid %ld ", messageTreeId);
   ::fflush(fout);
   ::fprintf(fout, "c %s ", messageClassName);
   ::fflush(fout);
   ::fprintf(fout, "n %s ", messageFullName);
   ::fflush(fout);
   ::fprintf(fout, "sm %d ", senderModuleId);
   ::fflush(fout);
   ::fprintf(fout, "t %x ", arrivalTime);
   ::fflush(fout);
} 

MessageCancelledEntry::MessageCancelledEntry()
{
   messageId = -1;
}

void MessageCancelledEntry::parse(const char **tokens, int numTokens)
{
   messageId = getLongToken(tokens, numTokens, "id");
}

void MessageCancelledEntry::print(FILE *fout)
{
   ::fprintf(fout, "CE ");
   ::fprintf(fout, "id %ld ", messageId);
   ::fflush(fout);
} 

BeginSendEntry::BeginSendEntry()
{
   messageId = -1;
   messageTreeId = -1;
   messageEncapsulationId = -1;
   messageEncapsulationTreeId = -1;
   messageClassName = NULL;
   messageFullName = NULL;
   messageKind = -1;
   messageLength = -1;
}

void BeginSendEntry::parse(const char **tokens, int numTokens)
{
   messageId = getLongToken(tokens, numTokens, "id");
   messageTreeId = getLongToken(tokens, numTokens, "tid");
   messageEncapsulationId = getLongToken(tokens, numTokens, "eid");
   messageEncapsulationTreeId = getLongToken(tokens, numTokens, "etid");
   messageClassName = getStringToken(tokens, numTokens, "c");
   messageFullName = getStringToken(tokens, numTokens, "n");
   messageKind = getIntToken(tokens, numTokens, "k");
   messageLength = getLongToken(tokens, numTokens, "l");
}

void BeginSendEntry::print(FILE *fout)
{
   ::fprintf(fout, "BS ");
   ::fprintf(fout, "id %ld ", messageId);
   ::fflush(fout);
   ::fprintf(fout, "tid %ld ", messageTreeId);
   ::fflush(fout);
   ::fprintf(fout, "eid %ld ", messageEncapsulationId);
   ::fflush(fout);
   ::fprintf(fout, "etid %ld ", messageEncapsulationTreeId);
   ::fflush(fout);
   ::fprintf(fout, "c %s ", messageClassName);
   ::fflush(fout);
   ::fprintf(fout, "n %s ", messageFullName);
   ::fflush(fout);
   ::fprintf(fout, "k %d ", messageKind);
   ::fflush(fout);
   ::fprintf(fout, "l %ld ", messageLength);
   ::fflush(fout);
} 

MessageSendDirectEntry::MessageSendDirectEntry()
{
   messageId = -1;
   senderModuleId = -1;
   destModuleId = -1;
   destGateId = -1;
   propagationDelay = -1;
   transmissionDelay = -1;
}

void MessageSendDirectEntry::parse(const char **tokens, int numTokens)
{
   messageId = getLongToken(tokens, numTokens, "id");
   senderModuleId = getIntToken(tokens, numTokens, "sm");
   destModuleId = getIntToken(tokens, numTokens, "dm");
   destGateId = getIntToken(tokens, numTokens, "dg");
   propagationDelay = getSimtimeToken(tokens, numTokens, "pd");
   transmissionDelay = getSimtimeToken(tokens, numTokens, "td");
}

void MessageSendDirectEntry::print(FILE *fout)
{
   ::fprintf(fout, "SD ");
   ::fprintf(fout, "id %ld ", messageId);
   ::fflush(fout);
   ::fprintf(fout, "sm %d ", senderModuleId);
   ::fflush(fout);
   ::fprintf(fout, "dm %d ", destModuleId);
   ::fflush(fout);
   ::fprintf(fout, "dg %d ", destGateId);
   ::fflush(fout);
   ::fprintf(fout, "pd %x ", propagationDelay);
   ::fflush(fout);
   ::fprintf(fout, "td %x ", transmissionDelay);
   ::fflush(fout);
} 

MessageSendHopEntry::MessageSendHopEntry()
{
   messageId = -1;
   senderModuleId = -1;
   senderGateId = -1;
   transmissionStartId = -1;
   propagationDelay = -1;
   transmissionDelay = -1;
}

void MessageSendHopEntry::parse(const char **tokens, int numTokens)
{
   messageId = getLongToken(tokens, numTokens, "id");
   senderModuleId = getIntToken(tokens, numTokens, "sm");
   senderGateId = getIntToken(tokens, numTokens, "sg");
   transmissionStartId = getSimtimeToken(tokens, numTokens, "ts");
   propagationDelay = getSimtimeToken(tokens, numTokens, "pd");
   transmissionDelay = getSimtimeToken(tokens, numTokens, "td");
}

void MessageSendHopEntry::print(FILE *fout)
{
   ::fprintf(fout, "SH ");
   ::fprintf(fout, "id %ld ", messageId);
   ::fflush(fout);
   ::fprintf(fout, "sm %d ", senderModuleId);
   ::fflush(fout);
   ::fprintf(fout, "sg %d ", senderGateId);
   ::fflush(fout);
   ::fprintf(fout, "ts %x ", transmissionStartId);
   ::fflush(fout);
   ::fprintf(fout, "pd %x ", propagationDelay);
   ::fflush(fout);
   ::fprintf(fout, "td %x ", transmissionDelay);
   ::fflush(fout);
} 
