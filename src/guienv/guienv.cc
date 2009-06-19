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
#include <jni.h>
#include <locale.h>
#include <sys/stat.h>

#include "appreg.h"
#include "guienv.h"
#include "jutil.h"
#include "platdep/platmisc.h"

using namespace JUtil;

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


JNIEnv *GUIEnv::jenv;
jobject GUIEnv::javaApp;
WrapperTable GUIEnv::wrapperTable;

#ifdef _WIN32
#define PATH_SEP ";"
#else
#define PATH_SEP ":"
#endif

#define DEBUGPRINTF ::printf
//#define DEBUGPRINTF (void)

Register_GlobalConfigOptionU(CFGID_GUIENV_EXTRA_STACK, "guienv-extra-stack", "B", "48KB", "Specifies the extra amount of stack that is reserved for each activity() simple module when the simulation is run under GUIEnv.");
Register_GlobalConfigOption(CFGID_GUIENV_DEFAULT_CONFIG, "guienv-default-config", CFG_STRING, NULL, "Specifies which config Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigOption(CFGID_GUIENV_DEFAULT_RUN, "guienv-default-run", CFG_INT, "0", "Specifies which run (of the default config, see tkenv-default-config) Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigOption(CFGID_GUIENV_IMAGE_PATH, "guienv-image-path", CFG_PATH, "", "Specifies the path for loading module icons.");

GUIEnv::GUIEnv()
{
    jcallback = NULL;
}

GUIEnv::~GUIEnv()
{
    delete jcallback;
}

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
    options[n++].optionString = "-Domnetpp.guienv.standalone=1";  // tells RCP app that it runs standalone (i.e. not launched from PDT)

    //XXX for debugging:
    options[n++].optionString = "-Djava.compiler=NONE";    // disable JIT
    //options[n++].optionString = "-verbose:jni";            // print JNI-related messages
    //options[n++].optionString = "-verbose:class";          // print class loading messages

    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = n;
    vm_args.ignoreUnrecognized = true;

    JavaVM *jvm;
    int res = JNI_CreateJavaVM(&jvm, (void **)&jenv, &vm_args);
    if (res<0)
        opp_error("Could not create Java VM: JNI_CreateJavaVM returned %d", res); //XXX

    // Here, we'd normally just call SimkernelJNI_registerNatives(jenv);
    // however, that fails because the SimkernelJNI class is not yet loaded
    // (it will be loaded by Equinox, with its own classloaded and from the
    // location determined by Equinox). So we postpone it to a static block
    // within the SimkernelJNI class, and we just provide a way (the GUIEnvHelper
    // class) so that SimkernelJNI can call it from Java.
    //
    DEBUGPRINTF("Registering native methods...\n");
    jbyte bytes[] = {
        // Bytecode for the following Java class:
        //   public class GUIEnvHelper { native static void registerNatives(Class simkernelJNIClass, Class cobjectClazz); }
        // To compile: javac -g:none -source 1.4 -target 1.4 GUIEnvHelper.java
        // To convert to hex: FAR>View>Hex>Mark>Copy/Paste>Replace space with ",0x"
        0xCA, 0xFE, 0xBA, 0xBE, 0x00, 0x00, 0x00, 0x30, 0x00, 0x0C, 0x0A, 0x00, 0x03, 0x00, 0x09, 0x07,
        0x00, 0x0A, 0x07, 0x00, 0x0B, 0x01, 0x00, 0x06, 0x3C, 0x69, 0x6E, 0x69, 0x74, 0x3E, 0x01, 0x00,
        0x03, 0x28, 0x29, 0x56, 0x01, 0x00, 0x04, 0x43, 0x6F, 0x64, 0x65, 0x01, 0x00, 0x0F, 0x72, 0x65,
        0x67, 0x69, 0x73, 0x74, 0x65, 0x72, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x73, 0x01, 0x00, 0x25,
        0x28, 0x4C, 0x6A, 0x61, 0x76, 0x61, 0x2F, 0x6C, 0x61, 0x6E, 0x67, 0x2F, 0x43, 0x6C, 0x61, 0x73,
        0x73, 0x3B, 0x4C, 0x6A, 0x61, 0x76, 0x61, 0x2F, 0x6C, 0x61, 0x6E, 0x67, 0x2F, 0x43, 0x6C, 0x61,
        0x73, 0x73, 0x3B, 0x29, 0x56, 0x0C, 0x00, 0x04, 0x00, 0x05, 0x01, 0x00, 0x0C, 0x47, 0x55, 0x49,
        0x45, 0x6E, 0x76, 0x48, 0x65, 0x6C, 0x70, 0x65, 0x72, 0x01, 0x00, 0x10, 0x6A, 0x61, 0x76, 0x61,
        0x2F, 0x6C, 0x61, 0x6E, 0x67, 0x2F, 0x4F, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x00, 0x21, 0x00, 0x02,
        0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x04, 0x00, 0x05, 0x00, 0x01,
        0x00, 0x06, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x2A, 0xB7,
        0x00, 0x01, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x07, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x00
    };

    // to define GUIEnvHelper, we need a class loader; we use the system class loader
    jclass classLoaderClazz = findClass(jenv, "java/lang/ClassLoader");
    jmethodID getSystemClassLoaderMethodId = getStaticMethodID(jenv, classLoaderClazz, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject classLoader = jenv->CallStaticObjectMethod(classLoaderClazz, getSystemClassLoaderMethodId);
    ASSERT(classLoader!=NULL);

    jclass helperClazz = jenv->DefineClass("GUIEnvHelper", classLoader, bytes, sizeof(bytes));
    checkExceptions(jenv);
    ASSERT(helperClazz!=NULL);

    JNINativeMethod methods[] = {
        { (char *)"registerNatives", (char *)"(Ljava/lang/Class;Ljava/lang/Class;)V", (void *)registerNatives }
    };
    int ret = jenv->RegisterNatives(helperClazz, methods, 1);
    ASSERT(ret==0);
}

void SimkernelJNI_registerNatives(JNIEnv *jenv, jclass clazz); // generated method in registernatives.cc

void GUIEnv::registerNatives(JNIEnv *jenv, jclass /*guienvHelperClazz*/, jclass simkernelJNIClazz, jclass cobjectClazz)
{
    try {
        GUIEnv::jenv = jenv;
        SimkernelJNI_registerNatives(jenv, simkernelJNIClazz);
        wrapperTable.init(jenv, cobjectClazz);  // can only be done when cObject was already loaded
    }
    catch (std::exception& e) {
        fprintf(stderr, "ERROR DURING REGISTERNATIVES: %s\n", e.what());
        exit(1);
    }
}

// This method is the native part of LaunchHelper Java class;
// it is used to bootstrap when program is launched using java.exe
// from Eclipse.
extern "C" JNIEXPORT void JNICALL Java_org_omnetpp_runtime_nativelibs_LaunchHelper_registerNatives(JNIEnv *jenv, jclass)
{
    GUIEnv::registerNatives(jenv, NULL,
        findClass(jenv, "org/omnetpp/runtime/nativelibs/simkernel/SimkernelJNI"),
        findClass(jenv, "org/omnetpp/runtime/nativelibs/simkernel/cObject"));
}

static std::string join(const char *sep, const std::vector<std::string>& v)
{
    std::string result;
    for (int i=0; i<(int)v.size(); i++) {
        if (i!=0) result += sep;
        result += v[i].c_str();
    }
    return result;
}

inline bool exists(const char *fname)
{
    struct opp_stat_t tmp;
    return opp_stat(fname, &tmp)==0;
}

void GUIEnv::run()
{
    if (!jenv)
    {
        // Normal startup: create JVM, and launch RCP application.
        // Note: we'd want to call wrapperTable.init(jenv) here as well,
        // but it has to wait until cObject.class has been loaded
        initJVM();

        DEBUGPRINTF("Launching the RCP app...\n");

        // look for org.eclipse.equinox.launcher.Main
        jclass mainClazz = findClass(jenv, "org/eclipse/equinox/launcher/Main");

        // determine the location of our Equinox config.ini
        const char *equinoxConfigDir = getenv("OMNETPP_GUIENV_CONFIG_DIR");
        if (!equinoxConfigDir)
            equinoxConfigDir = OMNETPP_GUIENV_CONFIG_DIR;
        if (!exists(equinoxConfigDir))
            throw cRuntimeError("Cannot launch GUIEnv: RCP application configuration not found at %s; "
                                "please set or adjust OMNETPP_GUIENV_CONFIG_DIR", equinoxConfigDir);
        std::string devpropertiesFile = std::string(equinoxConfigDir) + "/dev.properties";

        // prepare args array (note: options -os, -ws, -arch, -nl are not needed, as they are usually autodetected)
        std::vector<std::string> args;
        args.push_back("-application");
        args.push_back("org.omnetpp.runtimeenv.application");
        args.push_back("-data");
        args.push_back(".guienv"); //FIXME rather: $HOME/.guienv!
        args.push_back("-configuration");
        args.push_back(std::string("file:") + equinoxConfigDir);
        if (exists(devpropertiesFile.c_str()))
        {
            args.push_back("-dev");
            args.push_back(std::string("file:") + devpropertiesFile);
        }

        //TODO take additional args from command line and/or omnetpp.ini

        DEBUGPRINTF("Launcher args: %s\n", join(" ", args).c_str());

        // run the app: new Main().run(args)
        jobject mainObject = newObject(jenv, mainClazz);
        jmethodID runMethodID = getMethodID(jenv, mainClazz, "run", "([Ljava/lang/String;)I");
        jenv->CallIntMethod(mainObject, runMethodID, toJavaArray(jenv, args));
    }
    else
    {
        // Developer mode: program was launched as an RCP project (via the standard
        // eclipse launcher), and this class was dynamically created from Java code,
        // org.omnetpp.runtimeenv.Application.start(). We need to call back
        // Application.doStart().
        //
        jclass clazz = jenv->GetObjectClass(javaApp);
        jmethodID doStartMethodID = getMethodID(jenv, clazz, "doStart", "()V");
        jenv->CallVoidMethod(javaApp, doStartMethodID);  // this goes back Java and runs the appliation. it only returns when the application exits
        jenv->DeleteGlobalRef(javaApp);
    }
}

void GUIEnv::setJCallback(jobject jcallbackobj)
{
    if (jcallbackobj)
    {
        ASSERT(jenv!=NULL);
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

