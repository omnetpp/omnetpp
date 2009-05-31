#include "JMessage.h"
#include "JUtil.h"

using namespace JUtil;  // for jenv, checkExceptions(), findMethod(), etc


//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)


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
    if (jenv && javaPeer)
        jenv->DeleteGlobalRef(javaPeer);
}

std::string JMessage::info() const
{
    // return the first line of toString()
    std::string str = toString();
    const char *s = str.c_str();
    const char *p = strchr(s, '\n');
    if (p)
        str.erase(str.begin()+(p-s), str.end());
    return str;
}

std::string JMessage::detailedInfo() const
{
    return toString();
}

JMessage& JMessage::operator=(const JMessage& msg)
{
    cMessage::operator=(msg);
    if (!jenv)
        return *this;

    if (javaPeer)
        jenv->DeleteGlobalRef(javaPeer);
    javaPeer = 0;
    cloneMethod = 0;
    if (msg.javaPeer)
    {
        // must clone() the other Java object
        javaPeer = msg.javaPeer;  // then we'll clone it
        cloneMethod = msg.cloneMethod;
        if (!cloneMethod)
        {
            jclass clazz = jenv->GetObjectClass(javaPeer);
            cloneMethod = jenv->GetMethodID(clazz, "clone", "()Ljava/lang/Object;");
            const_cast<JMessage&>(msg).cloneMethod = cloneMethod;
            checkExceptions();
        }
        javaPeer = jenv->CallObjectMethod(javaPeer, cloneMethod);
        checkExceptions();
        javaPeer = jenv->NewGlobalRef(javaPeer);
        checkExceptions();
    }
    JObjectAccess::setObject(javaPeer);
    return *this;
}

void JMessage::swigSetJavaPeer(jobject msgObject)
{
    ASSERT(javaPeer==0);
    javaPeer = jenv->NewGlobalRef(msgObject);
    JObjectAccess::setObject(javaPeer);
}

jobject JMessage::swigJavaPeerOf(cMessage *object)
{
    JMessage *msg = dynamic_cast<JMessage *>(object);
    return msg ? msg->swigJavaPeer() : 0;
}



