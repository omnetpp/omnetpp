// file: gensink.h

class HCGenerator : public cSimpleModule
{
       Module_Class_Members(HCGenerator,cSimpleModule,16384)
       virtual void activity();
};

class HCSink : public cSimpleModule
{
       Module_Class_Members(HCSink,cSimpleModule,16384)
       virtual void activity();
};
