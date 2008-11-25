package org.omnetpp.cdt.makefile;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacroProvider;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.util.StringUtils;

/**
 * Value object to represent opp_makemake command-line options in a parsed form.
 * @author Andras
 */
//TODO support a "copy to bin/ or lib/ directory" meta-option
public class MakemakeOptions implements Cloneable {
    public enum Type {EXE, SHAREDLIB, STATICLIB, NOLINK};
    
    // opp_makemake options (note: -r deliberately not supported, there's --meta:recurse instead)
    public boolean isNMake = false;
    public String projectDir = null;  // ignored (implicit)
    public Type type = Type.EXE;
    public String target = null;
    public String outRoot = null;
    public boolean isDeep = false;
    public boolean noDeepIncludes = false;
    public boolean force = false;
    public String defaultMode = null;
    public String userInterface = "ALL";
    public String ccext = null;
    public boolean compileForDll;
    public String dllSymbol = "";
    public boolean ignoreNedFiles = true; // note: no option for this
    public List<String> fragmentFiles = new ArrayList<String>();
    public List<String> submakeDirs = new ArrayList<String>();
    public List<String> exceptSubdirs = new ArrayList<String>();
    public List<String> includeDirs = new ArrayList<String>();
    public List<String> libDirs = new ArrayList<String>();
    public List<String> libs = new ArrayList<String>();
    public List<String> defines = new ArrayList<String>();
    public List<String> makefileVariables = new ArrayList<String>();
    public List<String> extraArgs = new ArrayList<String>();

    // "meta" options (--meta:...): they get interpreted by MetaMakemake, 
    // and translated to normal makemake options.
    public boolean metaRecurse = false;
    public boolean metaAutoIncludePath = false;
    public boolean metaExportLibrary = false;
    public boolean metaUseExportedLibs = false;

    private List<String> errors = null; 
    
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
     * Create a new MakemakeOptions for a new source folder. 
     */
    public static MakemakeOptions createInitial() {
        MakemakeOptions result = new MakemakeOptions();
        result.outRoot = "out"; 
        result.isDeep = true;
        result.metaRecurse = true;
        result.metaAutoIncludePath = true;
        result.metaExportLibrary = true;
        result.metaUseExportedLibs = true;
        return result;
    }
    
    /**
     * Parse the argument list into the member variables
     */
    public void parseArgs(String[] argv) {
        errors = null;
        
        // process arguments
        int i;
        for (i = 0; i < argv.length; i++) {
            String arg = argv[i];
            if (arg.equals("-h") || arg.equals("--help")) {
                // ignore
            }
            else if (arg.equals("-f") || arg.equals("--force")) {
                force = true;
            }
            else if (arg.equals("--nmake")) {
                isNMake = true;
            }
            else if (arg.equals("-e") || arg.equals("--ext")) {
                if (checkArg(argv, i))
                    ccext = argv[++i];
            }
            else if (arg.equals("-o")) {
                if (checkArg(argv, i))
                    target = argv[++i];
            }
            else if (arg.startsWith("-o")) {
                target = arg.substring(2);
            }
            else if (arg.equals("-O") || arg.equals("--out")) {
                if (checkArg(argv, i))
                    outRoot = argv[++i];
            }
            else if (arg.startsWith("-O")) {
                outRoot = arg.substring(2);
            }
            else if (arg.equals("--deep")) {
                isDeep = true;
            }
            else if (arg.equals("-r") || arg.equals("--recurse")) {
                addError(arg + ": not supported by the IDE, use --meta:recurse instead");
            }
            else if (arg.equals("-X") || arg.equals("--except")) {
                if (checkArg(argv, i))
                    exceptSubdirs.add(argv[++i]);
            }
            else if (arg.startsWith("-X")) {
                String dir = arg.substring(2);
                exceptSubdirs.add(dir);
            }
            else if (arg.equals("--no-deep-includes")) {
                noDeepIncludes = true;
            }
            else if (arg.equals("-D") || arg.equals("--define")) {
                if (checkArg(argv, i))
                    defines.add(argv[++i]);
            }
            else if (arg.startsWith("-D")) {
                defines.add(arg.substring(2));
            }
            else if (arg.equals("-K") || arg.equals("--makefile-define")) {
                if (checkArg(argv, i))
                    makefileVariables.add(argv[++i]);
            }
            else if (arg.startsWith("-K")) {
                makefileVariables.add(arg.substring(2));
            }
            else if (arg.equals("-N") || arg.equals("--ignore-ned")) {
                addError(arg + ": obsolete option, please remove (dynamic NED loading is now the default)");
            }
            else if (arg.equals("-P") || arg.equals("--projectdir")) {
                if (checkArg(argv, i))
                    projectDir = argv[++i];
            }
            else if (arg.startsWith("-P")) {
                projectDir = arg.substring(2);
            }
            else if (arg.equals("-M") || arg.equals("--mode")) {
                if (checkArg(argv, i))
                    defaultMode = argv[++i];
            }
            else if (arg.startsWith("-M")) {
                defaultMode = arg.substring(2);
            }
            else if (arg.equals("-c") || arg.equals("--configfile")) {
                addError("option "+arg+" is no longer supported, config file is located using variables (OMNETPP_CONFIGFILE or OMNETPP_ROOT), or by invoking opp_configfilepath");
            }
            else if (arg.equals("-d") || arg.equals("--subdir")) {
                if (checkArg(argv, i))
                    submakeDirs.add(argv[++i]);
            }
            else if (arg.startsWith("-d")) {
                submakeDirs.add(arg.substring(2));
            }
            else if (arg.equals("-n") || arg.equals("--nolink")) {
                type = Type.NOLINK;
            }
            else if (arg.equals("-s") || arg.equals("--make-so")) {
                type = Type.SHAREDLIB;
            }
            else if (arg.equals("-a") || arg.equals("--make-lib")) {
                type = Type.STATICLIB;
            }
            else if (arg.equals("-S") || arg.equals("--fordll")) {
                compileForDll = true;
            }
            else if (arg.equals("-w") || arg.equals("--withobjects")) {
                addError(arg + ": obsolete option, please remove");
            }
            else if (arg.equals("-x") || arg.equals("--notstamp")) {
                addError(arg + ": obsolete option, please remove");
            }
            else if (arg.equals("-u") || arg.equals("--userinterface")) {
                if (checkArg(argv, i)) {
                    userInterface = argv[++i].toUpperCase();
                    if (!userInterface.equals("ALL") && !userInterface.equals("CMDENV") && !userInterface.equals("TKENV"))
                        addError("-u option: specify All, Cmdenv or Tkenv");
                }
            }
            else if (arg.equals("-i") || arg.equals("--includefragment")) {
                if (checkArg(argv, i))
                    fragmentFiles.add(argv[++i]);
            }
            else if (arg.equals("-I")) {
                if (checkArg(argv, i))
                    includeDirs.add(argv[++i]);
            }
            else if (arg.startsWith("-I")) {
                String dir = arg.substring(2);
                includeDirs.add(dir);
            }
            else if (arg.equals("-L")) {
                if (checkArg(argv, i))
                    libDirs.add(argv[++i]);
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
                if (checkArg(argv, i))
                    dllSymbol = argv[++i];
            }
            else if (arg.startsWith("-p")) {
                dllSymbol = arg.substring(2);
            }
            else if (arg.equals("--meta:recurse")) {
                metaRecurse = true;
            }
            else if (arg.equals("--meta:auto-include-path")) {
                metaAutoIncludePath = true;
            }
            else if (arg.equals("--meta:export-library")) {
                metaExportLibrary = true;
            }
            else if (arg.equals("--meta:use-exported-libs")) {
                metaUseExportedLibs = true;
            }
            else if (arg.equals("--")) {
                break;
            }
            else {
                Assert.isTrue(!StringUtils.isEmpty(arg), "empty makemake argument found");
                if (arg.startsWith("-"))
                    addError("unrecognized option: " + arg);
                extraArgs.add(arg);
            }
        }

        // process args after "--"
        ++i;
        for (; i < argv.length; i++) {
            extraArgs.add(argv[i]);
        }
    }

    protected boolean checkArg(String[] argv, int i) {
        if (i+1 >= argv.length) {
            addError("option " + argv[i] +  " requires an argument");
            return false;
        }
        return true;
    }
    
    protected void addError(String message) {
        if (errors == null)
            errors = new ArrayList<String>();
        errors.add(message);
    }

    public List<String> getParseErrors() {
        return errors==null ? new ArrayList<String>() : errors;
    }

    public void clearErrors() {
        errors = null;
    }

    /**
     * Re-generate the argument list from the member variables
     */
    public String[] toArgs() {
        List<String> result = new ArrayList<String>();
        if (force)
            add(result, "-f");
        if (isNMake)
            add(result, "--nmake");
        if (isDeep)
            add(result, "--deep");
        if (type == Type.NOLINK)
            add(result, "--nolink");
        else if (type == Type.SHAREDLIB)
            add(result, "--make-so");
        else if (type == Type.STATICLIB)
            add(result, "--make-lib");
        if (!StringUtils.isEmpty(projectDir))
            add(result, "-P", projectDir);
        if (!StringUtils.isEmpty(target))
            add(result, "-o", target);
        if (!StringUtils.isEmpty(outRoot))
            add(result, "-O", outRoot);
        if (!StringUtils.isEmpty(defaultMode))
            add(result, "-M", defaultMode);
        if (!StringUtils.isEmpty(userInterface) && !userInterface.equalsIgnoreCase("All"))
            add(result, "-u", userInterface);
        if (!StringUtils.isEmpty(ccext))
            add(result, "-e", ccext); 
        if (!StringUtils.isEmpty(dllSymbol))
            add(result, "-p" + dllSymbol);
        if (compileForDll && type != Type.SHAREDLIB)
            add(result, "-S");
        if (noDeepIncludes)
            result.add("--no-deep-includes");
        if (!ignoreNedFiles)
            add(result, "-N");
        addOpts2(result, fragmentFiles, "-i");
        addOpts2(result, submakeDirs, "-d");
        addOpts1(result, exceptSubdirs, "-X");
        addOpts1(result, includeDirs, "-I");
        addOpts1(result, libDirs, "-L");
        addOpts1(result, libs, "-l");
        addOpts1(result, defines, "-D");
        addOpts1(result, makefileVariables, "-K");
        if (metaRecurse)
            result.add("--meta:recurse");
        if (metaAutoIncludePath)
            result.add("--meta:auto-include-path");
        if (metaExportLibrary)
            result.add("--meta:export-library");
        if (metaUseExportedLibs)
            result.add("--meta:use-exported-libs");

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

    @Override
    public boolean equals(Object obj) {
        return obj != null && obj.getClass() == getClass() && Arrays.equals(((MakemakeOptions)obj).toArgs(), toArgs());
    }
    
    /**
     * Carry out a eclipse variable substitution on all args in the list
     * @throws CoreException for unresolved variables  
     */
    private static List<String> makeVariableSubstitution(List<String> args, IProject project) throws CoreException {
		if (args == null)
			return null;
		
    	List<String> substedArgs = new ArrayList<String>(); 
    	for(String arg : args) 
			substedArgs.add(makeVariableSubstitution(arg, project));
    	
    	return substedArgs;
    }

    /**
     * Carry out Eclipse variable substitution on the argument.
     * @throws CoreException for unresolved variables  
     */
	private static String makeVariableSubstitution(String arg, IProject project) throws CoreException {
		if (arg == null)
			return null;
		
		// resolve macros for this configuration
		IBuildMacroProvider provider = ManagedBuildManager.getBuildMacroProvider();
		arg = provider.resolveValue(arg, "", " ", IBuildMacroProvider.CONTEXT_CONFIGURATION, ManagedBuildManager.getBuildInfo(project).getDefaultConfiguration());

		// resolve global eclipse variables
		arg = StringUtils.substituteVariables(arg);

		return arg;
	}
    
    /**
     * Substitutes all ${} eclipse variables in all String options.
     * project is used for context information to evaluate BuildMacros
     * @throws CoreException for unresolved variables  
     */
    public void substituteVariables(IProject project) throws CoreException {
        projectDir = makeVariableSubstitution(projectDir, project);
        target = makeVariableSubstitution(target, project);
        outRoot = makeVariableSubstitution(outRoot, project);
        defaultMode = makeVariableSubstitution(defaultMode, project);
        userInterface = makeVariableSubstitution(userInterface, project);
        ccext = makeVariableSubstitution(ccext, project);
        dllSymbol = makeVariableSubstitution(dllSymbol, project);
        fragmentFiles = makeVariableSubstitution(fragmentFiles, project);
        submakeDirs = makeVariableSubstitution(submakeDirs, project);
        exceptSubdirs = makeVariableSubstitution(exceptSubdirs, project);
        includeDirs = makeVariableSubstitution(includeDirs, project);
        libDirs = makeVariableSubstitution(libDirs, project);
        libs = makeVariableSubstitution(libs, project);
        defines = makeVariableSubstitution(defines, project);
        makefileVariables = makeVariableSubstitution(makefileVariables, project);
        extraArgs = makeVariableSubstitution(extraArgs, project);
    }
    
    @Override
    public MakemakeOptions clone() {
        MakemakeOptions result = new MakemakeOptions();
        result.isNMake = isNMake;
        result.projectDir = projectDir;
        result.type = type;
        result.target = target;
        result.outRoot = outRoot;
        result.isDeep = isDeep;
        result.noDeepIncludes = noDeepIncludes;
        result.force = force;
        result.defaultMode = defaultMode;
        result.userInterface = userInterface;
        result.ccext = ccext;
        result.dllSymbol = dllSymbol;
        result.compileForDll = compileForDll;
        result.ignoreNedFiles = ignoreNedFiles;
        result.fragmentFiles.addAll(fragmentFiles);
        result.submakeDirs.addAll(submakeDirs);
        result.exceptSubdirs.addAll(exceptSubdirs);
        result.includeDirs.addAll(includeDirs);
        result.libDirs.addAll(libDirs);
        result.libs.addAll(libs);
        result.defines.addAll(defines);
        result.makefileVariables.addAll(makefileVariables);
        result.extraArgs.addAll(extraArgs);
        result.metaRecurse = metaRecurse;
        result.metaAutoIncludePath = metaAutoIncludePath;
        result.metaExportLibrary = metaExportLibrary;
        result.metaUseExportedLibs = metaUseExportedLibs;
        return result;
    }
}

