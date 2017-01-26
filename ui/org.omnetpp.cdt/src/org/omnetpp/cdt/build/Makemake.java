/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.MakemakeOptions.Type;
import org.omnetpp.common.Debug;
import org.omnetpp.common.OmnetppDirs;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * An opp_nmakemake implementation. May be invoked as a command-line tool
 * as well as via a Java API. Note: we work with filesystem dirs and files
 * not workspace resources, because "make" will know about the filesystem only.
 *
 * This class MUST NOT make any reference to CDT data structures, DependencyCache etc;
 * all input should be passed in via MakemakeOptions.
 *
 * @author Andras
 */
//XXX support ccExt = "cc,cpp" too! (because by default, CDT creates files with .cpp extension)
public class Makemake {
    private static final String MAKEFILE_TEMPLATE_NAME = "Makefile.TEMPLATE";
    private static final String GENERATED_MAKEFILE_MAGIC_STRING = "# This file was generated"; // template must contain this

    private static String template;

    // parameters for the makemake function
    private IContainer folder;

    public Makemake() {
    }

    /**
     * Generates Makefile in the given folder.
     */
    public void generateMakefile(IContainer folder, String args) throws CoreException, MakemakeException {
        MakemakeOptions options = MakemakeOptions.parse(args);
        if (!options.getParseErrors().isEmpty())
            throw new MakemakeException(options.getParseErrors().get(0));
        generateMakefile(folder, options);
    }

    /**
     * Generates Makefile in the given folder.
     */
    public void generateMakefile(IContainer folder, String[] argv) throws CoreException, MakemakeException {
        MakemakeOptions options = MakemakeOptions.parse(argv);
        generateMakefile(folder, options);
        if (!options.getParseErrors().isEmpty())
            throw new MakemakeException(options.getParseErrors().get(0));
    }

    public static boolean isGeneratedMakefile(IFile file) throws CoreException {
        try {
            byte[] buffer = new byte[1024];
            file.getContents().read(buffer, 0, buffer.length);
            String text = new String(buffer);
            return text.contains(GENERATED_MAKEFILE_MAGIC_STRING);
        }
        catch (IOException e) {
            throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "Error accessing " + file.getFullPath(), e));
        }
    }

    /**
     * Generates Makefile in the given folder.
     */
    public void generateMakefile(IContainer folder, final MakemakeOptions options) throws CoreException, MakemakeException {
        this.folder = folder;

        File directory = folder.getLocation().toFile();

        boolean isDeep = options.isDeep;
        boolean isRecursive = false; // not supported from the IDE, because we have metaRecurse

        String makefileName = "Makefile";
        IFile makefile = folder.getFile(new Path(makefileName));
        makefile.refreshLocal(IResource.DEPTH_ZERO, null);
        if (makefile.exists() && !options.force)
            throw new MakemakeException("use -f to force overwriting existing " + makefile.getFullPath().toString(), false);
        if (makefile.exists() && !isGeneratedMakefile(makefile))
            throw new MakemakeException(makefile.getFullPath().toString() + " seems to be a hand-written makefile, refusing to overwrite it. Please delete it or move it away.", false);

        String target = options.target == null ? folder.getProject().getName() : options.target;
        List<String> objs = new ArrayList<String>();
        List<String> extraObjs = new ArrayList<String>();
        List<String> ccfiles = new ArrayList<String>();
        List<String> cppfiles = new ArrayList<String>();
        List<String> nedfiles = new ArrayList<String>();
        List<String> msgfiles = new ArrayList<String>();
        List<String> msgccfiles = new ArrayList<String>();
        List<String> msghfiles = new ArrayList<String>();
        List<String> smfiles = new ArrayList<String>();
        List<String> smccfiles = new ArrayList<String>();
        List<String> smhfiles = new ArrayList<String>();
        List<String> sourceDirs = new ArrayList<String>();
        List<String> backslashedSourceDirs = new ArrayList<String>();
        List<String> includeDirs = new ArrayList<String>();
        List<String> libDirs = new ArrayList<String>();
        List<String> defines = new ArrayList<String>();

        // target should only be a name, cannot contain relative path
        if (target.contains("/") || target.contains("\\"))
            throw new MakemakeException("target (-o option) should only be a name, it cannot contain relative path");

        // isRecursive and deep do not mix
        if (isDeep)
            isRecursive = false;

        String makecommand = "make";

        if (options.projectDir != null)
            throw new MakemakeException("-P (--projectdir) option not supported, it is always the Eclipse project directory");

        String omnetppRoot = OmnetppDirs.getOmnetppRootDir();
        if (StringUtils.isEmpty(omnetppRoot))
            throw new MakemakeException("OMNeT++ root must be set in Window|Preferences");
        String configFile = omnetppRoot + "/Makefile.inc";

        // determine outDir (defaults to "out")
        String outDir = getOutDir(folder, options.outRoot);

        // determine subpath: the project-relative path of this folder
        String subpath = folder.getProjectRelativePath().toString();

        // collect source files
        if (isDeep) {
            List<String> allExcludedDirs = new ArrayList<String>();
            allExcludedDirs.add(outDir);
            allExcludedDirs.addAll(options.exceptSubdirs);
            allExcludedDirs.addAll(options.submakeDirs);
            sourceDirs = collectDirs(folder, allExcludedDirs);
        }
        else {
            if (!options.exceptSubdirs.contains("."))
                sourceDirs.add(".");
        }

        for (String i : sourceDirs) {
            ccfiles.addAll(glob(i, "*.cc"));
            cppfiles.addAll(glob(i, "*.cpp"));
            msgfiles.addAll(glob(i, "*.msg"));
            smfiles.addAll(glob(i, "*.sm"));
            nedfiles.addAll(glob(i, "*.ned"));
        }

        for (String dir : sourceDirs)
            backslashedSourceDirs.add(dir.replace('/', '\\'));

        // include dirs and lib dirs
        includeDirs.addAll(options.includeDirs);
        libDirs.addAll(options.libDirs);

        // try to determine if .cc or .cpp files are used
        String ccExt = options.ccext;
        if (ccExt == null) {
            if (ccfiles.isEmpty() && !cppfiles.isEmpty())
                ccExt = "cpp";
            else if (!ccfiles.isEmpty() && cppfiles.isEmpty())
                ccExt = "cc";
            else if (!ccfiles.isEmpty() && !cppfiles.isEmpty())
                throw new MakemakeException("you have both .cc and .cpp files -- specify -e cc or -e cpp option to select which set of files to use");
            else
                ccExt = "cc";  // if no files, use .cc extension
        }
        else {
            if (ccExt.equals("cc") && ccfiles.isEmpty() && !cppfiles.isEmpty())
                Debug.println("warning: you specified -e cc but you have only .cpp files in this directory!"); //XXX
            if (ccExt.equals("cpp") && !ccfiles.isEmpty() && cppfiles.isEmpty())
                Debug.println("warning: you specified -e cpp but you have only .cc files in this directory!");  //XXX
        }

        String objExt = "o";
        String targetPrefix = "";
        String targetSuffix = "";
        if (options.type == MakemakeOptions.Type.EXE)
            targetSuffix = "$(EXE_SUFFIX)";
        else if (options.type == MakemakeOptions.Type.SHAREDLIB) {
            targetSuffix = "$(SHARED_LIB_SUFFIX)";
            targetPrefix = "$(LIB_PREFIX)";
        }
        else if (options.type == MakemakeOptions.Type.STATICLIB) {
            targetSuffix = "$(A_LIB_SUFFIX)";
            targetPrefix = "$(LIB_PREFIX)";
        }

        // prepare submakeDirs. First, check that all specified submakeDirs exist
        List<String> submakeDirs = new ArrayList<String>();
        submakeDirs.addAll(options.submakeDirs);

        for (String subdir : submakeDirs)
            if (!file(subdir).isDirectory())
                throw new MakemakeException("subdirectory '" + subdir + "' does not exist");

        if (isRecursive) {
            File[] list = directory.listFiles(new FileFilter() {
                public boolean accept(File file) {
                    if (file.isDirectory()) {
                        String name = file.getName();
                        if (!name.startsWith(".") && !ArrayUtils.contains(MakefileTools.IGNORABLE_DIRS, name) && !options.exceptSubdirs.contains(name))
                            return true;
                    }
                    return false;
                }});
            for (File i : list) {
                submakeDirs.add(i.getName());
            }
        }

        // create names for subdir targets
        List<String> submakeNames = new ArrayList<String>();
        for (String i : submakeDirs)
            submakeNames.add(i.replaceAll("[^a-zA-Z0-9_]", "__"));

        // process extraArgs (do ".o" <--> ".obj" translation; NOTE: we don't try to
        // translate library names, because libs are supposed to be given via -L/-l)
        for (String arg : options.extraArgs) {
            Assert.isTrue(!StringUtils.isEmpty(arg), "empty makemake argument found");
            arg = arg.replaceFirst("^'(.*)'$", "$1"); // remove possible quotes (cmd.exe does not do it)
            arg = arg.replaceFirst(".o(bj)?$", "."+objExt);
            extraObjs.add(arg);
        }

        List<String> sources = new ArrayList<String>();
        if (ccExt.equals("cc"))
            sources.addAll(ccfiles);
        if (ccExt.equals("cpp"))
            sources.addAll(cppfiles);

        sources.addAll(msgfiles);
        sources.addAll(smfiles);
        if (!options.ignoreNedFiles)
            sources.addAll(nedfiles);

        for (String i : sources) {
            i = i.replaceAll("\\*[^ ]*", "");
            i = i.replaceAll("[^ ]*_n\\." + ccExt + "$", "");
            i = i.replaceAll("[^ ]*_m\\." + ccExt + "$", "");
            i = i.replaceAll("[^ ]*_sm\\." + ccExt + "$", "");
            i = i.replaceAll("\\.ned$", "_n." + objExt);
            i = i.replaceAll("\\.msg$", "_m." + objExt);
            i = i.replaceAll("\\.sm$", "_sm." + objExt);
            i = i.replaceAll("\\." + ccExt + "$", "." + objExt);
            if (!i.equals(""))
                objs.add("$O/" + i);
        }

        for (String i : msgfiles) {
            String h = i.replaceAll("\\.msg$", "_m.h");
            String cc = i.replaceAll("\\.msg$", "_m." + ccExt);
            msgccfiles.add(cc);
            msghfiles.add(h);
        }

        for (String i : smfiles) {
            String h = i.replaceAll("\\.sm$", "_sm.h");
            String cc = i.replaceAll("\\.sm$", "_sm." + ccExt);
            smccfiles.add(cc);
            smhfiles.add(h);
        }

        String makefrags = "";
        try {
            if (!options.fragmentFiles.isEmpty()) {
                for (String frag : options.fragmentFiles) {
                    makefrags += "# inserted from file '" + frag + "':\n";
                    makefrags += FileUtils.readTextFile(file(frag), null) + "\n";
                }
            }
            else {
                String makefragFilename = "makefrag";
                if (file(makefragFilename).isFile()) {
                    makefrags += "# inserted from file '" + makefragFilename + "':\n";
                    makefrags += FileUtils.readTextFile(file(makefragFilename), null) + "\n";
                }
            }
        } catch (IOException e) {
            throw Activator.wrapIntoCoreException(e);
        }

        // defines
        defines.addAll(options.defines);
        if ((options.type == Type.SHAREDLIB || options.forceCompileForDll) && !StringUtils.isEmpty(options.dllSymbol))
            defines.add(options.dllSymbol+"_EXPORT");

        // Windows (cmd.exe) has 8K limit on line length, so we may have to use the inline file feature of nmake
        int approximateLinkerLineLength = 500 + estimateLength(objs) + estimateLength(extraObjs) + estimateLength(options.libs) + estimateLength(libDirs);
        boolean isLongLinkerLine = approximateLinkerLineLength > 8000;

        // collect data for the template
        Map<String, Object> m = new HashMap<String, Object>();
        m.put("lbrace", "{");
        m.put("rbrace", "}");
        m.put("targetprefix", targetPrefix);
        m.put("target", target);
        m.put("targetsuffix", targetSuffix);
        m.put("outdir", outDir);
        m.put("subpath", subpath);
        m.put("isdeep", isDeep);
        m.put("progname", "opp_makemake");
        m.put("args", StringUtils.escapeBash(options.toString()));
        m.put("configfile", configFile);
        m.put("cc", ccExt);
        m.put("exe", options.type == MakemakeOptions.Type.EXE);
        m.put("sharedlib", options.type == MakemakeOptions.Type.SHAREDLIB);
        m.put("staticlib", options.type == MakemakeOptions.Type.STATICLIB);
        m.put("nolink", options.type == MakemakeOptions.Type.NOLINK);
        m.put("defaultmode", StringUtils.nullToEmpty(options.defaultMode));
        m.put("allenv", options.userInterface.startsWith("A"));
        m.put("cmdenv", options.userInterface.startsWith("C"));
        m.put("tkenv", options.userInterface.startsWith("T"));
        m.put("qtenv", options.userInterface.startsWith("Q"));
        m.put("extraobjs", quoteJoin(extraObjs));
        m.put("includepath", prefixQuoteJoin(includeDirs, "-I"));
        m.put("libpathdirs", libDirs);
        m.put("libs", options.libs);
        m.put("defines", prefixQuoteJoin(defines, "-D"));
        m.put("makefilevariables", options.makefileVariables);
        m.put("makecommand", makecommand);
        m.put("makefile", "Makefile");
        m.put("makefrags", makefrags);
        m.put("msgccfiles", msgccfiles);
        m.put("msghfiles", msghfiles);
        m.put("msgfiles", msgfiles);
        m.put("smccfiles", smccfiles);
        m.put("smhfiles", smhfiles);
        m.put("smfiles", smfiles);
        m.put("objs", quoteJoin(objs));
        m.put("submakedirs", submakeDirs);
        m.put("submakenames", submakeNames);
        m.put("dllsymbol", StringUtils.nullToEmpty(options.dllSymbol));
        m.put("sourcedirs", sourceDirs);
        m.put("backslashedsourcedirs", backslashedSourceDirs);
        m.put("gcclongline", isLongLinkerLine && Platform.getOS().equals(Platform.OS_WIN32));

        // now generate the makefile
        Debug.println("generating makefile for " + folder.toString());
        if (template == null) {
            try {
                template = FileUtils.readTextFile(Makemake.class.getResourceAsStream(MAKEFILE_TEMPLATE_NAME), null);
                template = template.replace("\r\n", "\n");
            } catch (IOException e) {
                throw Activator.wrapIntoCoreException(e);
            }
        }
        String content = StringUtils.substituteIntoTemplate(template, m);
        content = content.replace("\r\n", "\n");  // make line endings consistent

        Assert.isTrue(content.contains(GENERATED_MAKEFILE_MAGIC_STRING)); // so that we recognize our own generated file

        // only overwrite file if it does not already exist with the same content,
        // to avoid excessive Eclipse workspace refreshes and infinite builder invocations
        byte[] bytes = content.getBytes();
        MakefileTools.ensureFileContent(makefile, bytes, null);
    }

    protected String getOutDir(IContainer folder, String outRoot) {
        IPath outRootPath = new Path(StringUtils.defaultIfEmpty(outRoot, "out"));
        IPath outRootAbs = outRootPath.isAbsolute() ? outRootPath : folder.getProject().getLocation().append(outRootPath);
        IPath outRootRel = MakefileTools.makeRelativePath(outRootAbs, folder.getLocation());  // "<project>/out"
        return outRootRel.toString();
    }

    public List<String> getSourceDirs(IContainer folder, MakemakeOptions options) throws CoreException {
        //FIXME factor out similar code from makemake body

        // collect source files
        if (options.isDeep) {
            String outDir = getOutDir(folder, options.outRoot);

            List<String> allExcludedDirs = new ArrayList<String>();
            allExcludedDirs.add(outDir);
            allExcludedDirs.addAll(options.exceptSubdirs);
            allExcludedDirs.addAll(options.submakeDirs);
            return collectDirs(folder, allExcludedDirs);
        }
        else {
            List<String> sourceDirs = new ArrayList<String>();
            sourceDirs.add("."); //FIXME and if excluded?
            return sourceDirs;
        }
    }

    protected List<String> collectDirs(IContainer dir, List<String> exceptSubdirs) throws CoreException {
        List<String> result = new ArrayList<String>();
        collectDirs(dir, ".", exceptSubdirs, result);
        return result;
    }

    protected void collectDirs(IContainer dir, String dirPath, List<String> exceptSubdirs, final List<String> result) throws CoreException {
        if (!exceptSubdirs.contains(dirPath)) {  //FIXME like in Perl! ie. -Xtmp should mean -X./tmp not -X**/tmp!!! also wildcards should be supported!!!
            result.add(dirPath);
            for (IResource member : dir.members())
                if (MakefileTools.isGoodFolder(member))
                    collectDirs((IContainer)member, dirPath.equals(".") ? member.getName() : dirPath + "/" + member.getName(), exceptSubdirs, result);
        }
    }

    protected int estimateLength(List<String> list) {
        int sum = 0;
        for (String i : list) {
            sum += i.length() + 10; // assume 10 chars for item separator and possible quoting
            if (i.contains("$"))
                sum += 80;  // assume expanded variables are at most 80 chars
        }
        return sum;
    }

    protected String quoteJoin(List<String> list) {
        return prefixQuoteJoin(list, "");
    }

    protected String prefixQuoteJoin(List<String> list, String prefix) {
        StringBuilder result = new StringBuilder(list.size() * 32);
        String sep = list.size() > 5 ? " \\\n    " : " ";
        for (String i : list)
            result.append(sep).append(prefix).append(quote(i));
        return result.length()==0 ? "" : result.substring(1); // chop off leading space
    }

    protected String quote(String string) {
        return string.contains(" ") ? ("\"" + string + "\"") : string;
    }

    protected File file(String path) {
        File file = new File(path);
        if (!file.isAbsolute())
            file = folder.getLocation().append(path).toFile();
        return file;
    }

    // FIXME filename should not contain /
    protected List<String> glob(String pattern) {
        return glob(".", pattern);
    }

    protected List<String> glob(String subFolder, String pattern) {
        final String regex = pattern.replace(".", "\\.").replace("*", ".*").replace("?", ".?"); // good enough for what we need here
        String[] files = folder.getLocation().append(subFolder).toFile().list(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.matches(regex);
            }});
        List<String> result = new ArrayList<String>();
        if (files != null)
            for (String name : files)
                result.add(subFolder.equals(".") ? name : subFolder+"/"+name);

        result.sort(String::compareToIgnoreCase);
        return result;
    }

}
