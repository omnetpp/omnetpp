//==========================================================================
//  GUIENV.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "appreg.h"
#include "guienv.h"


//
// Register the user interface
//
Register_OmnetApp("GUIEnv", GUIEnv, 30, "RCP-based graphical user interface");

//
// The following function can be used to force linking with GUIEnv; specify
// -u _guienv_lib (gcc) or /include:_guienv_lib (vc++) in the link command.
//
extern "C" GUIENV_API void guienv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" GUIENV_API void _guienv_lib() {}


JavaVM *GUIEnv::jvm;
JNIEnv *GUIEnv::jenv;
jobject GUIEnv::javaApp;

#ifdef _WIN32
#define PATH_SEP ";"
#else
#define PATH_SEP ":"
#endif

#define DEBUGPRINTF printf
//#define DEBUGPRINTF (void)

// comes from the generated registernatives.cc file
void SimkernelJNI_registerNatives(JNIEnv *jenv);

void GUIEnv::initJVM()
{
    DEBUGPRINTF("Starting JVM...\n");
    JavaVMInitArgs vm_args;
    JavaVMOption options[10];

    int n = 0;
/*
    const char *classpath = getenv("CLASSPATH");
    if (!classpath)
        opp_error("CLASSPATH environment variable is not set");
    std::string classpathOption = std::string("-Djava.class.path=")+(classpath ? classpath : "");
    options[n++].optionString = (char *)classpathOption.c_str(); // user classes
    options[n++].optionString = (char *)"-Djava.library.path=."; // set native library path
*/

    options[n++].optionString = "-Dfile.encoding=Cp1252";  //XXX Eclipse did it so, but it this necessary & same on all platforms?
    options[n++].optionString = "-Djava.class.path=C:\\eclipse\\plugins\\org.eclipse.equinox.launcher_1.0.100.v20080509-1800.jar"; //XXX hardcoded!!!!

    //XXX for debugging:
    options[n++].optionString = "-Djava.compiler=NONE";    // disable JIT
    options[n++].optionString = "-verbose:jni";            // print JNI-related messages
    options[n++].optionString = "-verbose:class";          // print class loading messages

    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = n;
    vm_args.ignoreUnrecognized = true;

    int res = JNI_CreateJavaVM(&jvm, (void **)&jenv, &vm_args);
    if (res<0)
        opp_error("Could not create Java VM: JNI_CreateJavaVM returned %d", res); //XXX

    DEBUGPRINTF("Registering native methods...\n");
//FIXME can only be done when class has been loaded:
//SimkernelJNI_registerNatives(jenv);
    DEBUGPRINTF("Done.\n");
}

static jobjectArray toJava(JNIEnv *jenv, const std::vector<std::string>& v)
{
    jclass stringClazz = jenv->FindClass("java/lang/String");
    jobjectArray array = jenv->NewObjectArray(v.size(), stringClazz, NULL);
    for (int i=0; i<(int)v.size(); i++) {
        jstring string = jenv->NewStringUTF(v[i].c_str());
        jenv->SetObjectArrayElement(array, i, string);
    }
    return array;
}

void GUIEnv::run()
{
    if (!jvm) {
        // Normal startup: create JVM, and launch RCP application
        initJVM();
//FIXME can only be done when Java cObject class has been loaded:
// wrapperTable.init(jenv);
        DEBUGPRINTF("Launching the RCP app...\n");

        // look for org.eclipse.equinox.launcher.Main
        jclass mainClazz = jenv->FindClass("org/eclipse/equinox/launcher/Main");
        ASSERT(mainClazz!=NULL);

        // prepare args array
        std::vector<std::string> args;
        args.push_back("-application");
        args.push_back("org.omnetpp.runtimeenv.application");
        args.push_back("-data");
        args.push_back("C:/home/omnetpp40/omnetpp/runtime-org.omnetpp.runtimeenv.application"); //XXX hardcoded!!!
        args.push_back("-configuration");
        args.push_back("file:C:/home/omnetpp40/omnetpp/ui/.metadata/.plugins/org.eclipse.pde.core/org.omnetpp.runtimeenv.application/"); //XXX
        args.push_back("-dev");
        args.push_back("file:C:/home/omnetpp40/omnetpp/ui/.metadata/.plugins/org.eclipse.pde.core/org.omnetpp.runtimeenv.application/dev.properties"); //XXX
        args.push_back("-os");
        args.push_back("win32"); //XXX
        args.push_back("-ws");
        args.push_back("win32"); //XXX
        args.push_back("-arch");
        args.push_back("x86"); //XXX
        args.push_back("-nl");
        args.push_back("en_US");

        // run the app: new Main().run(args)
        jmethodID ctorMethodId = jenv->GetMethodID(mainClazz, "<init>", "()V");
        ASSERT(ctorMethodId!=NULL);
        jobject mainObject = jenv->NewObject(mainClazz, ctorMethodId);
        ASSERT(mainObject!=NULL);
        jmethodID runMethodID = jenv->GetMethodID(mainClazz, "run", "([Ljava/lang/String;)I");
        ASSERT(runMethodID!=NULL);
        jenv->CallVoidMethod(mainObject, runMethodID, toJava(jenv, args));
        //XXX check...
    }
    else {
        // Developer mode: program was launched as an RCP project (via the standard
        // eclipse launcher), and this class was dynamically created from Java code,
        // org.omnetpp.runtimeenv.Application.start(). We need to call back
        // Application.doStart().
        //
        jclass clazz = jenv->GetObjectClass(javaApp);
        jmethodID doStartMethodID = jenv->GetMethodID(clazz, "doStart", "()V");
        if (doStartMethodID==NULL) {
            fprintf(stderr, "GUIEnv initialization failed: application object has no method doStart()\n");
            exit(1);
        }
        jenv->CallVoidMethod(javaApp, doStartMethodID);  // this goes back Java and runs the appliation. it only returns when the application exits
        jenv->DeleteGlobalRef(javaApp);
    }
}

void GUIEnv::setJCallback(JNIEnv *jenv, jobject jcallbackobj)
{
    if (jcallbackobj)
    {
        jcallback = new JCallback(jenv, jcallbackobj);
    }
    else
    {
        delete jcallback;
        jcallback = NULL;
    }
}

void GUIEnv::objectDeleted(cObject *object)
{
    //cEnvirBase::objectDeleted(object);
    if (jenv) wrapperTable.objectDeleted(object);
    //if (jcallback) jcallback->objectDeleted(object);
}

/*XXX already in header
void GUIEnv::simulationEvent(cMessage *msg)
{
    //cEnvirBase::simulationEvent(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->simulationEvent(msg);
}
*/

void GUIEnv::messageScheduled(cMessage *msg)
{
    //cEnvirBase::messageScheduled(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageScheduled(msg);
}

void GUIEnv::messageCancelled(cMessage *msg)
{
    //cEnvirBase::messageCancelled(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageCancelled(msg);
}

void GUIEnv::beginSend(cMessage *msg)
{
    //cEnvirBase::beginSend(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->beginSend(msg);
}

void GUIEnv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    //cEnvirBase::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void GUIEnv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    //cEnvirBase::messageSendHop(msg, srcGate);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendHop(msg, srcGate);
}

void GUIEnv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    //cEnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay, transmissionStartTime);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void GUIEnv::endSend(cMessage *msg)
{
    //cEnvirBase::endSend(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->endSend(msg);
}

void GUIEnv::messageDeleted(cMessage *msg)
{
    //cEnvirBase::messageDeleted(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageDeleted(msg);
}

void GUIEnv::moduleReparented(cModule *module, cModule *oldparent)
{
    //cEnvirBase::moduleReparented(module, oldparent);
    if (jcallback) jcallback->moduleReparented(module, oldparent);
}

void GUIEnv::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va)
{
const char *method = methodFmt; //FIXME vsprintf()
    //cEnvirBase::componentMethodBegin(from, to, method);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->componentMethodBegin(from, to, method);
}

void GUIEnv::componentMethodEnd()
{
    //cEnvirBase::componentMethodEnd();
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->componentMethodEnd();
}

void GUIEnv::moduleCreated(cModule *newmodule)
{
    //cEnvirBase::moduleCreated(newmodule);
    if (jcallback) jcallback->moduleCreated(newmodule);
}

void GUIEnv::moduleDeleted(cModule *module)
{
    //cEnvirBase::moduleDeleted(module);
    if (jcallback) jcallback->moduleDeleted(module);
}

void GUIEnv::gateCreated(cGate *srcgate)
{
    //cEnvirBase::gateCreated(srcgate);
    if (jcallback) jcallback->gateCreated(srcgate);
}

void GUIEnv::gateDeleted(cGate *srcgate)
{
    //cEnvirBase::gateDeleted(srcgate);
    if (jcallback) jcallback->gateDeleted(srcgate);
}

void GUIEnv::connectionCreated(cGate *srcgate)
{
    //cEnvirBase::connectionCreated(srcgate);
    if (jcallback) jcallback->connectionCreated(srcgate);
}

void GUIEnv::connectionDeleted(cGate *srcgate)
{
    //cEnvirBase::connectionDeleted(srcgate);
    if (jcallback) jcallback->connectionDeleted(srcgate);
}

void GUIEnv::displayStringChanged(cComponent *component)
{
    //cEnvirBase::displayStringChanged(component);
    if (jcallback) jcallback->displayStringChanged(component);
}

void GUIEnv::undisposedObject(cObject *obj)
{
    //cEnvirBase::undisposedObject(component);
    if (jcallback) jcallback->undisposedObject(obj);
}

void GUIEnv::bubble(cModule *mod, const char *text)
{
    //cEnvirBase::bubble(mod, text);
    if (jcallback) jcallback->bubble(mod, text);
}

std::string GUIEnv::gets(const char *prompt, const char *defaultreply)
{
    ASSERT(jcallback);
    return jcallback->gets(prompt, defaultreply);
}

bool GUIEnv::idle()
{
    bool stop = false;
    if (jcallback)
        stop = jcallback->idle();
    return stop;
}

