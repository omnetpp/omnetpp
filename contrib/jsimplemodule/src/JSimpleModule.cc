#include <assert.h>
#include "JSimpleModule.h"

//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

Define_Module(JSimpleModule);

// This will come from the generated SimkernelJNI_registerNatives.cc
void SimkernelJNI_registerNatives(JNIEnv *jenv);

JavaVM *JSimpleModule::vm;
JNIEnv *JSimpleModule::jenv;

JSimpleModule::JSimpleModule()
{
    javaObject = 0;
}

JSimpleModule::~JSimpleModule()
{
    if (javaObject && finishMethod)
        jenv->DeleteGlobalRef(javaObject);
}

void JSimpleModule::initJVM()
{
    DEBUGPRINTF("Starting JVM...\n");
    JavaVMInitArgs vm_args;
    JavaVMOption options[10];

    int n = 0;
    const char *classpath = getenv("CLASSPATH");
    if (!classpath)
        opp_error("CLASSPATH environment variable is not set");
    std::string classpathOption = std::string("-Djava.class.path=")+(classpath ? classpath : "");
    options[n++].optionString = (char *)classpathOption.c_str(); /* user classes */
    options[n++].optionString = "-Djava.library.path=.";   /* set native library path */
    //options[n++].optionString = "-Djava.compiler=NONE";    /* disable JIT */
    //options[n++].optionString = "-verbose:jni";            /* print JNI-related messages */
    //options[n++].optionString = "-verbose:class";          /* print class loading messages */

    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = n;
    vm_args.ignoreUnrecognized = true;

    int res = JNI_CreateJavaVM(&vm, (void **)&jenv, &vm_args);
    if (res<0)
        opp_error("Could not create Java VM: JNI_CreateJavaVM returned %d", res);

    DEBUGPRINTF("Registering native methods...\n");
    SimkernelJNI_registerNatives(jenv);
    DEBUGPRINTF("Done.\n");
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
        createJavaObject();

    DEBUGPRINTF("Invoking initialize(%d) on new instance...\n", stage);
    jenv->CallVoidMethod(javaObject, initializeStageMethod, stage);
    checkExceptions();
}

void JSimpleModule::createJavaObject()
{
    // create VM if needed
    if (!jenv)
         initJVM();

    // find class and method IDs (note: initialize() and finish() are optional)
    const char *clazzName = par("javaClass");
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
}

jmethodID JSimpleModule::findMethod(jclass clazz, const char *clazzName, const char *methodName, const char *methodSig)
{
    jmethodID ret = jenv->GetMethodID(clazz, methodName, methodSig);
    jenv->ExceptionClear();
    if (!ret)
        opp_error("No `%s' %s method in Java class `%s'", methodName, methodSig, clazzName);
    return ret;
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

static std::string fromJavaString(jstring stringObject)
{
    if (!stringObject)
        return "<null>";
    jboolean isCopy;
    const char *buf = JSimpleModule::jenv->GetStringUTFChars(stringObject, &isCopy);
    std::string str = buf ? buf : "";
    JSimpleModule::jenv->ReleaseStringUTFChars(stringObject, buf);
    return str;
}

void JSimpleModule::checkExceptions() const
{
    jthrowable exceptionObject = jenv->ExceptionOccurred();
    if (exceptionObject)
    {
        DEBUGPRINTF("JSimpleModule: exception occurred:\n");
        jenv->ExceptionDescribe();
        jenv->ExceptionClear();

        jclass throwableClass = jenv->GetObjectClass(exceptionObject);
        jmethodID getMessageMethod = jenv->GetMethodID(throwableClass, "getMessage", "()Ljava/lang/String;");
        jstring msg = (jstring)jenv->CallObjectMethod(exceptionObject, getMessageMethod);
        opp_error("%s", fromJavaString(msg).c_str());
    }
}


