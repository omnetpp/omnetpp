
#ifndef __EVENTLOGENTRIES_H_
#define __EVENTLOGENTRIES_H_

#include "defs.h"
#include "eventlogentry.h"

class BubbleEntry : public EventLogEntry
{
   public:
      BubbleEntry();

   protected:
      int moduleId;
      const char * text;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ModuleMethodCalledEntry : public EventLogEntry
{
   public:
      ModuleMethodCalledEntry();

   protected:
      int fromModuleId;
      int toModuleId;
      const char * method;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ModuleCreatedEntry : public EventLogEntry
{
   public:
      ModuleCreatedEntry();

   protected:
      int moduleId;
      const char * moduleClassName;
      int parentModuleId;
      const char * fullName;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ModuleDeletedEntry : public EventLogEntry
{
   public:
      ModuleDeletedEntry();

   protected:
      int moduleId;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ModuleReparentedEntry : public EventLogEntry
{
   public:
      ModuleReparentedEntry();

   protected:
      int moduleId;
      int newParentModuleId;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ConnectionCreatedEntry : public EventLogEntry
{
   public:
      ConnectionCreatedEntry();

   protected:
      int sourceModuleId;
      int sourceGateId;
      int sourceGateFullName;
      int destModuleId;
      int destGateId;
      int destGateFullName;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ConnectionRemovedEntry : public EventLogEntry
{
   public:
      ConnectionRemovedEntry();

   protected:
      int sourceModuleId;
      int sourceGateId;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ConnectionDisplayStringChangedEntry : public EventLogEntry
{
   public:
      ConnectionDisplayStringChangedEntry();

   protected:
      int sourceModuleId;
      int sourceGateId;
      const char * displayString;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class ModuleDisplayStringChangedEntry : public EventLogEntry
{
   public:
      ModuleDisplayStringChangedEntry();

   protected:
      int moduleId;
      const char * displayString;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class BackgroundDisplayStringChangedEntry : public EventLogEntry
{
   public:
      BackgroundDisplayStringChangedEntry();

   protected:
      int moduleId;
      const char * displayString;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class EventEntry : public EventLogEntry
{
   public:
      EventEntry();

   protected:
      long eventNumber;
      simtime_t simulationTime;
      int moduleId;
      long messageId;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class MessageScheduledEntry : public EventLogEntry
{
   public:
      MessageScheduledEntry();

   protected:
      long messageId;
      long messageTreeId;
      const char * messageClassName;
      const char * messageFullName;
      int senderModuleId;
      simtime_t arrivalTime;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class MessageCancelledEntry : public EventLogEntry
{
   public:
      MessageCancelledEntry();

   protected:
      long messageId;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class BeginSendEntry : public EventLogEntry
{
   public:
      BeginSendEntry();

   protected:
      long messageId;
      long messageTreeId;
      long messageEncapsulationId;
      long messageEncapsulationTreeId;
      const char * messageClassName;
      const char * messageFullName;
      int messageKind;
      long messageLength;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class MessageSendDirectEntry : public EventLogEntry
{
   public:
      MessageSendDirectEntry();

   protected:
      long messageId;
      int senderModuleId;
      int destModuleId;
      int destGateId;
      simtime_t propagationDelay;
      simtime_t transmissionDelay;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

class MessageSendHopEntry : public EventLogEntry
{
   public:
      MessageSendHopEntry();

   protected:
      long messageId;
      int senderModuleId;
      int senderGateId;
      simtime_t transmissionStartId;
      simtime_t propagationDelay;
      simtime_t transmissionDelay;

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};

#endif
