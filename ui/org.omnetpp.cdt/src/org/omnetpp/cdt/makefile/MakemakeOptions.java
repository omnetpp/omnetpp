/**
 *
 */
package org.omnetpp.cdt.makefile;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang.StringEscapeUtils;
import org.apache.commons.lang.text.StrTokenizer;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.StringUtils;

public class MakemakeOptions implements Cloneable {
    public enum Type {EXE, SO, NOLINK};
    public List<String> args;
    public boolean isNMake = false;
    public String baseDir = null;  // not supported
    public Type type = Type.EXE;
    public String target = null;
    public boolean force = false;
    public boolean linkWithObjects = false;
    public boolean tstamp = true;
    public boolean recursive = false;
    public String userInterface = "ALL";
    public String ccext = null;
    public String configFile = null;
    public String exportDefOpt = "";
    public boolean compileForDll;
    public boolean ignoreNedFiles = true; // note: no option for this
    public List<String> fragmentFiles = new ArrayList<String>();
    public List<String> subdirs = new ArrayList<String>();
    public List<String> exceptSubdirs = new ArrayList<String>();
    public List<String> includeDirs = new ArrayList<String>();
    public List<String> libDirs = new ArrayList<String>();
    public List<String> libs = new ArrayList<String>();
    public List<String> importLibs = new ArrayList<String>();
    public List<String> extraArgs = new ArrayList<String>();

    /**
     * Create makemake options with the default settings.
     */
    public MakemakeOptions() {
        this.args = new ArrayList<String>();
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
        this.args = Arrays.asList(argv);

        // process arguments
        for (int i = 0; i < argv.length; i++) {
            String arg = argv[i];
            if (arg.equals("-h") || arg.equals("--help")) {
                // TODO
            }
            else if (arg.equals("-f") || arg.equals("--force")) {
                force = true;
            }
            else if (arg("--nmake")) {
                isNMake = true;
            }
            else if (arg.equals("-e") || arg.equals("--ext")) {
                checkArg(argv, i);
                ccext = argv[++i];
            }
            else if (arg.equals("-o") || arg.equals("--outputfile")) {
                checkArg(argv, i);
                target = argv[++i];
            }
            else if (arg.equals("-N") || arg.equals("--ignore-ned")) {
                throw new IllegalArgumentException(arg + ": obsolete option, please remove (dynamic NED loading is now the default)");
            }
            else if (arg.equals("-r") || arg.equals("--recurse")) {
                recursive = true;
            }
            else if (arg.equals("-X") || arg.equals("--except")) {
                checkArg(argv, i);
                String dir = argv[++i];
                exceptSubdirs.add(dir);
            }
            else if (arg.startsWith("-X")) {
                String dir = StringUtils.removeStart(arg, "-X");
                exceptSubdirs.add(dir);
            }
            else if (arg.equals("-b") || arg.equals("--basedir")) {
                checkArg(argv, i);
                baseDir = argv[++i];
            }
            else if (arg.equals("-c") || arg.equals("--configfile")) {
                checkArg(argv, i);
                configFile = argv[++i];
            }
            else if (arg.equals("-n") || arg.equals("--nolink")) {
                type = Type.NOLINK;
            }
            else if (arg.equals("-d") || arg.equals("--subdir")) {
                checkArg(argv, i);
                subdirs.add(argv[++i]);
            }
            else if (arg.startsWith("-d")) {
                String dir = StringUtils.removeStart(arg, "-d");
                subdirs.add(dir);
            }
            else if (arg.equals("-s") || arg.equals("--make-so")) {
                compileForDll = true;
                type = Type.SO;
            }
            else if (arg.equals("-t") || arg.equals("--importlib")) {
                checkArg(argv, i);
                importLibs.add(argv[++i]);
            }
            else if (arg.equals("-S") || arg.equals("--fordll")) {
                compileForDll = true;
            }
            else if (arg.equals("-w") || arg.equals("--withobjects")) {
                linkWithObjects = true;
            }
            else if (arg.equals("-x") || arg.equals("--notstamp")) {
                tstamp = false;
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
                String dir = StringUtils.removeStart(arg, "-I");
                includeDirs.add(dir);
            }
            else if (arg.equals("-L")) {
                checkArg(argv, i);
                String dir = argv[++i];
                libDirs.add(dir);
            }
            else if (arg.startsWith("-L")) {
                String dir = StringUtils.removeStart(arg, "-L");
                libDirs.add(dir);
            }
            else if (arg.startsWith("-l")) {
                String lib = StringUtils.removeStart(arg, "-l");
                libs.add(lib);
            }
            else if (arg.equals("-P")) {
                checkArg(argv, i);
                exportDefOpt = argv[++i];
            }
            else if (arg.startsWith("-P")) {
                exportDefOpt = StringUtils.removeStart(arg, "-P");
            }
            else {
                Assert.isTrue(!StringUtils.isEmpty(arg), "empty makemake argument found");
                //FIXME add support for "--" after which everything is extraArg
                if (!arg.equals("--")) {
                    if (arg.startsWith("-"))
                        throw new IllegalArgumentException("unrecognized option: " + arg);
                    extraArgs.add(arg);
                }
            }
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

        if (!StringUtils.isEmpty(baseDir))
            add(result, "-b", baseDir);

        if (target != null)
            add(result, "-o", target);

        if (force)
            add(result, "-f");

        if (isNMake)
            add(result, "--nmake");

        if (linkWithObjects)
            add(result, "-w");

        if (!tstamp)
            add(result, "-x"); // no-tstamp

        if (recursive)
            add(result, "-r");

        if (!"ALL".equals(userInterface))
            add(result, "-u", userInterface);

        if (type == Type.SO)
            add(result, "-s");
        else if (type == Type.NOLINK)
            add(result, "-n");

        if (!StringUtils.isEmpty(ccext))
            add(result, "-e", ccext);  //XXX detect?

        if (!StringUtils.isEmpty(configFile))
            add(result, "-c", configFile); //XXX omit if can be detected?

        if (!StringUtils.isEmpty(exportDefOpt))
            add(result, "-P"+exportDefOpt);

        if (compileForDll && type != Type.SO)
            add(result, "-S");

        if (!ignoreNedFiles)
            add(result, "???"); //XXX

        addOpts2(result, fragmentFiles, "-i");
        addOpts2(result, subdirs, "-d");
        addOpts2(result, exceptSubdirs, "-X");
        addOpts1(result, includeDirs, "-I");
        addOpts1(result, libDirs, "-L");
        addOpts1(result, libs, "-l");
        addOpts2(result, importLibs, "-t");
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

    @Override
    public MakemakeOptions clone() {
        MakemakeOptions result = new MakemakeOptions();
        result.args = args;
        result.isNMake = isNMake;
        result.baseDir = baseDir;
        result.type = type;
        result.target = target;
        result.force = force;
        result.linkWithObjects = linkWithObjects;
        result.tstamp = tstamp;
        result.recursive = recursive;
        result.userInterface = userInterface;
        result.ccext = ccext;
        result.configFile = configFile;
        result.exportDefOpt = exportDefOpt;
        result.compileForDll = compileForDll;
        result.ignoreNedFiles = ignoreNedFiles;
        result.fragmentFiles.addAll(fragmentFiles);
        result.subdirs.addAll(subdirs);
        result.exceptSubdirs.addAll(exceptSubdirs);
        result.includeDirs.addAll(includeDirs);
        result.libDirs.addAll(libDirs);
        result.libs.addAll(libs);
        result.importLibs.addAll(importLibs);
        result.extraArgs.addAll(extraArgs);
        return result;
    }
}

