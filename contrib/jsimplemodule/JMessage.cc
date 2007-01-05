#include "JMessage.h"
#include "JSimpleModule.h"

//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

#define JENV   JSimpleModule::jenv


JMessage::JMessage(const char *name, int kind, int) : cMessage(name, kind)
{
    javaPeer = 0;
    cloneMethod = 0;
}

JMessage::JMessage(const JMessage& msg)
{
    javaPeer = 0;
    cloneMethod = 0;
    operator=(msg);
}

JMessage::~JMessage()
{
    if (JENV && javaPeer)
        JENV->DeleteGlobalRef(javaPeer);
}

JMessage& JMessage::operator=(const JMessage& msg)
{
    cMessage::operator=(msg);
    if (!JENV)
        return *this;

    if (javaPeer)
        JENV->DeleteGlobalRef(javaPeer);
    javaPeer = 0;
    cloneMethod = 0;
    if (msg.javaPeer)
    {
        // must clone() the other Java object
        javaPeer = msg.javaPeer;  // then we'll clone it
        cloneMethod = msg.cloneMethod;
        if (!cloneMethod)
        {
            jclass clazz = JENV->GetObjectClass(javaPeer);
            cloneMethod = JENV->GetMethodID(clazz, "clone", "()Ljava/lang/Object;");
            const_cast<JMessage&>(msg).cloneMethod = cloneMethod;
            checkExceptions();
        }
        javaPeer = JENV->CallObjectMethod(javaPeer, cloneMethod);
        checkExceptions();
        javaPeer = JENV->NewGlobalRef(javaPeer);
        checkExceptions();
    }
    return *this;
}

void JMessage::swigSetJavaPeer(jobject msgObject)
{
    ASSERT(javaPeer==0);
    this->javaPeer = JENV->NewGlobalRef(msgObject);
}

void JMessage::checkExceptions() const
{
    jthrowable exceptionObject = JENV->ExceptionOccurred();
    if (exceptionObject)
    {
        DEBUGPRINTF("JSimpleModule: exception occurred:\n");
        JENV->ExceptionDescribe();
        JENV->ExceptionClear();

        jclass throwableClass = JENV->FindClass("java/lang/Throwable");
        jmethodID getMessageMethod = JENV->GetMethodID(throwableClass, "getMessage", "()Ljava/lang/String;");
        jstring messageStringObject = (jstring)JENV->CallObjectMethod(exceptionObject, getMessageMethod);
        jboolean isCopy;
        const char *buf = JENV->GetStringUTFChars(messageStringObject, &isCopy);
        std::string msg = buf ? buf : "";
        JENV->ReleaseStringUTFChars(messageStringObject, buf);


        opp_error("%s", msg.c_str());
    }
}

jobject JMessage::swigJavaPeerOf(cObject *object)
{
    JMessage *msg = dynamic_cast<JMessage *>(object);
    return msg ? msg->swigJavaPeer() : 0;
}

