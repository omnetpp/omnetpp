// file: token.h

class TokenRingMAC : public cSimpleModule
{
       Module_Class_Members(TokenRingMAC,cSimpleModule,16384)
       virtual void activity();
};

class Generator : public cSimpleModule
{
       Module_Class_Members(Generator,cSimpleModule,16384)
       virtual void activity();
};

class Sink : public cSimpleModule
{
       Module_Class_Members(Sink,cSimpleModule,16384)
       virtual void activity();
};
