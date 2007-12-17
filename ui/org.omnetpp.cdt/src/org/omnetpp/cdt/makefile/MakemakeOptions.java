package org.omnetpp.cdt.makefile;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;

/**
 * Value object to represent opp_makemake command-line options in a parsed form.
 * @author Andras
 */
public class MakemakeOptions implements Cloneable {
    public enum Type {EXE, SHAREDLIB, STATICLIB, NOLINK};
    
    // opp_makemake options
    public boolean isNMake = false;
    public String projectDir = null;  // not supported
    public Type type = Type.EXE;
    public String target = null;
    public String outRoot = null;
    public boolean isDeep = true;
    public boolean isRecursive = false;
    public boolean force = false;
    public boolean linkWithObjects = false;
    public String mode = "";
    public String userInterface = "ALL";
    public String ccext = null;
    public String dllExportMacro = "";
    public boolean compileForDll;
    public boolean ignoreNedFiles = true; // note: no option for this
    public List<String> fragmentFiles = new ArrayList<String>();
    public List<String> subdirs = new ArrayList<String>();
    public List<String> exceptSubdirs = new ArrayList<String>();
    public List<String> includeDirs = new ArrayList<String>();
    public List<String> libDirs = new ArrayList<String>();
    public List<String> libs = new ArrayList<String>();
    public List<String> defines = new ArrayList<String>();
    public List<String> extraArgs = new ArrayList<String>();

    // "meta" options (--meta:xxx): they get interpreted by MetaMakemake, 
    // and translated to normal makemake options.
    public boolean metaFoo;  //XXX

    /**
     * Create makemake options with the default settings.
     */
    public MakemakeOptions() {
    }

    public MakemakeOptions(String args) {
        this(StringUtils.isEmpty(args) ? new String[0] : args.trim().split(" +"));  //XXX doesn't honor quotes! use StrTokenizer?
    }

    /**
     * Create makemake options by parsing the given argument list.
     */
    public MakemakeOptions(String[] argv) {
        parseArgs(argv);
    }

    /**
     * Parse the argument list into the member variables
     */
    public void parseArgs(String[] argv) {
        // process arguments
        int i;
        for (i = 0; i < argv.length; i++) {
            String arg = argv[i];
            if (arg.equals("-h") || arg.equals("--help")) {
                // TODO
            }
            else if (arg.equals("-f") || arg.equals("--force")) {
                force = true;
            }
            else if (arg.equals("--nmake")) {
                isNMake = true;
            }
            else if (arg.equals("-e") || arg.equals("--ext")) {
                checkArg(argv, i);
                ccext = argv[++i];
            }
            else if (arg.equals("-o")) {
                checkArg(argv, i);
                target = argv[++i];
            }
            else if (arg.startsWith("-o")) {
                target = arg.substring(2);
            }
            else if (arg.equals("-O") || arg.equals("--out")) {
                checkArg(argv, i);
                outRoot = argv[++i];
            }
            else if (arg.startsWith("-O")) {
                outRoot = arg.substring(2);
            }
            else if (arg.equals("--deep")) {
                isDeep = true;
            }
            else if (arg.equals("--nodeep")) {
                isDeep = false;
            }
            else if (arg.equals("-N") || arg.equals("--ignore-ned")) {
                throw new IllegalArgumentException(arg + ": obsolete option, please remove (dynamic NED loading is now the default)");
            }
            else if (arg.equals("-r") || arg.equals("--recurse")) {
                isRecursive = true;
            }
            else if (arg.equals("-X") || arg.equals("--except")) {
                checkArg(argv, i);
                String dir = argv[++i];
                exceptSubdirs.add(dir);
            }
            else if (arg.startsWith("-X")) {
                String dir = arg.substring(2);
                exceptSubdirs.add(dir);
            }
            else if (arg.equals("-D") || arg.equals("--define")) {
                checkArg(argv, i);
                defines.add(argv[++i]);
            }
            else if (arg.startsWith("-D")) {
                defines.add(arg.substring(2));
            }
            else if (arg.equals("-P") || arg.equals("--projectdir")) {
                checkArg(argv, i);
                projectDir = argv[++i];
            }
            else if (arg.startsWith("-P")) {
                projectDir = arg.substring(2);
            }
            else if (arg.equals("-M") || arg.equals("--mode")) {
                checkArg(argv, i);
                mode = argv[++i];
            }
            else if (arg.startsWith("-M")) {
                mode = arg.substring(2);
            }
            else if (arg.equals("-c") || arg.equals("--configfile")) {
                throw new IllegalArgumentException("option "+arg+" is no longer supported, config file is located using variables (OMNETPP_CONFIGFILE or OMNETPP_ROOT), or by invoking opp_configfilepath");
            }
            else if (arg.equals("-d") || arg.equals("--subdir")) {
                checkArg(argv, i);
                subdirs.add(argv[++i]);
            }
            else if (arg.startsWith("-d")) {
                subdirs.add(arg.substring(2));
            }
            else if (arg.equals("-n") || arg.equals("--nolink")) {
                type = Type.NOLINK;
            }
            else if (arg.equals("-s") || arg.equals("--make-so")) {
                compileForDll = true;
                type = Type.SHAREDLIB;
            }
            else if (arg.equals("-a") || arg.equals("--make-lib")) {
                type = Type.STATICLIB;
            }
            else if (arg.equals("-S") || arg.equals("--fordll")) {
                compileForDll = true;
            }
            else if (arg.equals("-w") || arg.equals("--withobjects")) {
                linkWithObjects = true;
            }
            else if (arg.equals("-x") || arg.equals("--notstamp")) {
                throw new IllegalArgumentException(arg + ": obsolete option, please remove");
            }
            else if (arg.equals("-u") || arg.equals("--userinterface")) {
                checkArg(argv, i);
                userInterface = argv[++i];
                userInterface = userInterface.toUpperCase();
                if (!userInterface.equals("ALL") && !userInterface.equals("CMDENV") && !userInterface.equals("TKENV"))
                    throw new IllegalArgumentException("-u option: specify All, Cmdenv or Tkenv");
            }
            else if (arg.equals("-i") || arg.equals("--includefragment")) {
                checkArg(argv, i);
                String frag = argv[++i];
                fragmentFiles.add(frag);
            }
            else if (arg.equals("-I")) {
                checkArg(argv, i);
                String dir = argv[++i];
                includeDirs.add(dir);
            }
            else if (arg.startsWith("-I")) {
                String dir = arg.substring(2);
                includeDirs.add(dir);
            }
            else if (arg.equals("-L")) {
                checkArg(argv, i);
                String dir = argv[++i];
                libDirs.add(dir);
            }
            else if (arg.startsWith("-L")) {
                String dir = arg.substring(2);
                libDirs.add(dir);
            }
            else if (arg.startsWith("-l")) {
                String lib = arg.substring(2);
                libs.add(lib);
            }
            else if (arg.equals("-p")) {
                checkArg(argv, i);
                dllExportMacro = argv[++i];
            }
            else if (arg.startsWith("-p")) {
                dllExportMacro = arg.substring(2);
            }
            else if (arg.equals("--")) {
                break;
            }
            else {
                Assert.isTrue(!StringUtils.isEmpty(arg), "empty makemake argument found");
                if (arg.startsWith("-"))
                    throw new IllegalArgumentException("unrecognized option: " + arg);
                extraArgs.add(arg);
            }
        }

        // process args after "--"
        ++i;
        for (; i < argv.length; i++) {
            extraArgs.add(argv[i]);
        }
    }

    protected void checkArg(String[] argv, int i) {
        if (i+1 >= argv.length)
            throw new IllegalArgumentException("option " + argv[i] +  " requires an argument");
    }

    /**
     * Re-generate the argument list from the member variables
     */
    public String[] toArgs() {
        List<String> result = new ArrayList<String>();
        if (!StringUtils.isEmpty(projectDir))
            add(result, "-P", projectDir);
        if (!StringUtils.isEmpty(target))
            add(result, "-o", target);
        if (!StringUtils.isEmpty(outRoot))
            add(result, "-O", outRoot);
        add(result, isDeep ? "--deep" : "--nodeep");
        if (force)
            add(result, "-f");
        if (isNMake)
            add(result, "--nmake");
        if (linkWithObjects)
            add(result, "-w");
        if (isRecursive)
            add(result, "-r");
        if (!StringUtils.isEmpty(mode))
            add(result, "-M", mode);
        if (!"ALL".equalsIgnoreCase(userInterface))
            add(result, "-u", userInterface);
        if (type == Type.NOLINK)
            add(result, "-n");
        else if (type == Type.SHAREDLIB)
            add(result, "-s");
        else if (type == Type.STATICLIB)
            add(result, "-a");
        if (!StringUtils.isEmpty(ccext))
            add(result, "-e", ccext); 
        if (!StringUtils.isEmpty(dllExportMacro))
            add(result, "-p"+dllExportMacro);
        if (compileForDll && type != Type.SHAREDLIB)
            add(result, "-S");
        if (!ignoreNedFiles)
            add(result, "-N");
        addOpts2(result, fragmentFiles, "-i");
        addOpts2(result, subdirs, "-d");
        addOpts2(result, exceptSubdirs, "-X");
        addOpts1(result, includeDirs, "-I");
        addOpts1(result, libDirs, "-L");
        addOpts1(result, libs, "-l");
        addOpts2(result, defines, "-D");
        if (!extraArgs.isEmpty())
            result.add("--");
        result.addAll(extraArgs);

        return result.toArray(new String[]{});
    }

    private void add(List<String> argList, String...args) {
        argList.addAll(Arrays.asList(args));
    }

    private void addOpts1(List<String> argList, List<String> args, String option) {
        for (String arg : args)
            argList.add(option + arg);
    }

    private void addOpts2(List<String> argList, List<String> args, String option) {
        for (String arg : args) {
            argList.add(option);
            argList.add(arg);
        }
    }

    @Override
    public String toString() {
        String[] tmp = toArgs();
        return StringUtils.join(tmp, " ");  //FIXME quote args that contain whitespace
    }

    ///FIXME make sure it's up to date!!!!!
    @Override
    public MakemakeOptions clone() {
        MakemakeOptions result = new MakemakeOptions();
        result.isNMake = isNMake;
        result.projectDir = projectDir;
        result.type = type;
        result.target = target;
        result.outRoot = outRoot;
        result.isDeep = isDeep;
        result.force = force;
        result.linkWithObjects = linkWithObjects;
        result.isRecursive = isRecursive;
        result.mode = mode;
        result.userInterface = userInterface;
        result.ccext = ccext;
        result.dllExportMacro = dllExportMacro;
        result.compileForDll = compileForDll;
        result.ignoreNedFiles = ignoreNedFiles;
        result.fragmentFiles.addAll(fragmentFiles);
        result.subdirs.addAll(subdirs);
        result.exceptSubdirs.addAll(exceptSubdirs);
        result.includeDirs.addAll(includeDirs);
        result.libDirs.addAll(libDirs);
        result.libs.addAll(libs);
        result.defines.addAll(defines);
        result.extraArgs.addAll(extraArgs);
        return result;
    }
}

