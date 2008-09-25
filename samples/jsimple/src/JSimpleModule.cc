#include <assert.h>
#include "JSimpleModule.h"
#include "JUtil.h"

using namespace JUtil;  // for jenv, checkExceptions(), findMethod(), etc


//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

Define_Module(JSimpleModule);


JSimpleModule::JSimpleModule()
{
    javaObject = 0;
}

JSimpleModule::~JSimpleModule()
{
    if (javaObject && finishMethod)
        jenv->DeleteGlobalRef(javaObject);
}

int JSimpleModule::numInitStages() const
{
    if (javaObject==0)
        return 1; // at the beginning, we can only say "at least one stage"

    int n = jenv->CallIntMethod(javaObject, numInitStagesMethod);
    checkExceptions();
    return n;
}

void JSimpleModule::initialize(int stage)
{
    if (stage==0)
        createJavaModuleObject();

    DEBUGPRINTF("Invoking initialize(%d) on new instance...\n", stage);
    jenv->CallVoidMethod(javaObject, initializeStageMethod, stage);
    checkExceptions();
}

void JSimpleModule::createJavaModuleObject()
{
    // create VM if needed
    if (!jenv)
         initJVM();

    // find class and method IDs (note: initialize() and finish() are optional)
    const char *clazzName = getNedTypeName();
    DEBUGPRINTF("Finding class %s...\n", clazzName);
    jclass clazz = jenv->FindClass(clazzName);
    checkExceptions();

    DEBUGPRINTF("Finding initialize()/handleMessage()/finish() methods for class %s...\n", clazzName);
    numInitStagesMethod = findMethod(clazz, clazzName, "numInitStages", "()I");
    initializeStageMethod = findMethod(clazz, clazzName, "initialize", "(I)V");
    doHandleMessageMethod = findMethod(clazz, clazzName, "doHandleMessage", "()V");
    finishMethod = findMethod(clazz, clazzName, "finish", "()V");

    // create Java module object
    DEBUGPRINTF("Instantiating class %s...\n", clazzName);

    jmethodID setCPtrMethod = jenv->GetMethodID(clazz, "setCPtr", "(J)V");
    jenv->ExceptionClear();
    if (setCPtrMethod)
    {
        jmethodID ctor = findMethod(clazz, clazzName, "<init>", "()V");
        javaObject = jenv->NewObject(clazz, ctor);
        checkExceptions();
        jenv->CallVoidMethod(javaObject, setCPtrMethod, (jlong)this);
    }
    else
    {
        jmethodID ctor = findMethod(clazz, clazzName, "<init>", "(J)V");
        javaObject = jenv->NewObject(clazz, ctor, (jlong)this);
        checkExceptions();
    }
    javaObject = jenv->NewGlobalRef(javaObject);
    checkExceptions();
    JObjectAccess::setObject(javaObject);
}

void JSimpleModule::handleMessage(cMessage *msg)
{
    msgToBeHandled = msg;
    jenv->CallVoidMethod(javaObject, doHandleMessageMethod);
    checkExceptions();
}

void JSimpleModule::finish()
{
    jenv->CallVoidMethod(javaObject, finishMethod);
    checkExceptions();
}


