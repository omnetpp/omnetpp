package org.omnetpp.cdt.makefile;

import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;

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

    // computed
    private IPath projectLocation;
    private IPath folderLocation;

    public Makemake() {
    }

    /**
     * Generates Makefile in the given folder.
     */
    public void generateMakefile(IContainer folder, String args, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws CoreException, MakemakeException {
        MakemakeOptions options = new MakemakeOptions(args);
        if (!options.getParseErrors().isEmpty())
            throw new MakemakeException(options.getParseErrors().get(0));
        generateMakefile(folder, options, perFileDeps);
    }

    /**
     * Generates Makefile in the given folder.
     */
    public void generateMakefile(IContainer folder, String[] argv, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws CoreException, MakemakeException {
        MakemakeOptions options = new MakemakeOptions(argv);
        generateMakefile(folder, options, perFileDeps);
        if (!options.getParseErrors().isEmpty())
            throw new MakemakeException(options.getParseErrors().get(0));
    }

    public static boolean isGeneratedMakefile(IFile file) {
        try {
            byte[] buffer = new byte[1024];
            file.getContents().read(buffer, 0, buffer.length);
            String text = new String(buffer);
            return text.contains(GENERATED_MAKEFILE_MAGIC_STRING);
        }
        catch (IOException e) {
            return false;
        }
        catch (CoreException e) {
            return false;
        }
    }

    /**
     * Generates Makefile in the given folder.
     */
    public void generateMakefile(IContainer folder, final MakemakeOptions options, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws CoreException, MakemakeException {
        this.folder = folder;

        File directory = folder.getLocation().toFile();
        projectLocation = folder.getProject().getLocation();
        folderLocation = folder.getLocation();

        boolean isNMake = options.isNMake;
        boolean isDeep = options.isDeep;
        boolean isRecursive = false; // not supported from the IDE, because we have metaRecurse

        String makefileName = isNMake ? "Makefile.vc" : "Makefile";
        IFile makefile = folder.getFile(new Path(makefileName));
        if (makefile.exists() && !options.force)
            throw new MakemakeException("use -f to force overwriting existing " + makefile.getFullPath().toString());
        if (makefile.exists() && !isGeneratedMakefile(makefile))
            throw new MakemakeException(makefile.getFullPath().toString() + " seems to be a hand-written makefile, refusing to overwrite it. Please delete it or move it away.");

        String target = options.target == null ? folder.getProject().getName() : options.target;
        List<String> objs = new ArrayList<String>();
        List<String> extraObjs = new ArrayList<String>();
        List<String> ccfiles = new ArrayList<String>();
        List<String> cppfiles = new ArrayList<String>();
        List<String> nedfiles = new ArrayList<String>();
        List<String> msgfiles = new ArrayList<String>();
        List<String> msgccfiles = new ArrayList<String>();
        List<String> msghfiles = new ArrayList<String>();
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

        String makecommand = isNMake ? "nmake /nologo /f Makefile.vc" : "make";

        if (options.projectDir != null)
            throw new MakemakeException("-P (--projectdir) option not supported, it is always the Eclipse project directory");

        String omnetppRoot = OmnetppMainPlugin.getDefault().getPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT);
        if (StringUtils.isEmpty(omnetppRoot))
            throw new MakemakeException("OMNeT++ root must be set in Window|Preferences");
        String configFile = omnetppRoot + (isNMake ? "\\configuser.vc" : "/Makefile.inc");

        // determine outDir (defaults to "out")
        String outDir;
        IPath outRootPath = new Path(StringUtils.isEmpty(options.outRoot) ? "out" : options.outRoot);
        IPath outRootAbs = outRootPath.isAbsolute() ? outRootPath : folder.getProject().getLocation().append(outRootPath);
        IPath outRootRel = abs2rel(outRootAbs);  // "<project>/out"
        outDir = outRootRel.toString();

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
            sourceDirs.add(".");
        }

        for (String i : sourceDirs) {
            ccfiles.addAll(glob(i, "*.cc"));
            cppfiles.addAll(glob(i, "*.cpp"));
            msgfiles.addAll(glob(i, "*.msg"));
            nedfiles.addAll(glob(i, "*.ned"));
        }

        for (String dir : sourceDirs)
            backslashedSourceDirs.add(dir.replace('/', '\\'));

        // include dirs and lib dirs
        for (String i : options.includeDirs)
            includeDirs.add(abs2rel(i));
        if (!options.noDeepIncludes)
            for (String i : sourceDirs)
                includeDirs.add(i);

        for (String i : options.libDirs)
            libDirs.add(abs2rel(i));


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
                System.out.println("warning: you specified -e cc but you have only .cpp files in this directory!"); //XXX
            if (ccExt.equals("cpp") && !ccfiles.isEmpty() && cppfiles.isEmpty())
                System.out.println("warning: you specified -e cpp but you have only .cc files in this directory!");  //XXX
        }

        String objExt = isNMake ? "obj" : "o";
        String targetPrefix = "";
        String targetSuffix = "";
        if (options.type == MakemakeOptions.Type.EXE)
            targetSuffix = isNMake ? ".exe" : "$(EXE_SUFFIX)";
        else if (options.type == MakemakeOptions.Type.SHAREDLIB) {
            targetSuffix = isNMake ? ".dll" : "$(SHARED_LIB_SUFFIX)";
            targetPrefix = isNMake ? "" : "lib";
        }
        else if (options.type == MakemakeOptions.Type.STATICLIB) {
            targetSuffix = isNMake ? ".lib" : "$(A_LIB_SUFFIX)";
            targetPrefix = isNMake ? "" : "lib";
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
        if (!options.ignoreNedFiles)
            sources.addAll(nedfiles);

        for (String i : sources) {
            i = i.replaceAll("\\*[^ ]*", "");
            i = i.replaceAll("[^ ]*_n\\." + ccExt + "$", "");
            i = i.replaceAll("[^ ]*_m\\." + ccExt + "$", "");
            i = i.replaceAll("\\.ned$", "_n." + objExt);
            i = i.replaceAll("\\.msg$", "_m." + objExt);
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

        String makefrags = "";
        try {
            if (!options.fragmentFiles.isEmpty()) {
                for (String frag : options.fragmentFiles) {
                    makefrags += "# inserted from file '" + frag + "':\n";
                    makefrags += FileUtils.readTextFile(file(frag)) + "\n";
                }
            }
            else {
                String makefragFilename = isNMake ? "makefrag.vc" : "makefrag";
                if (file(makefragFilename).isFile()) {
                    makefrags += "# inserted from file '" + makefragFilename + "':\n";
                    makefrags += FileUtils.readTextFile(file(makefragFilename)) + "\n";
                }
            }
        } catch (IOException e) {
            throw Activator.wrapIntoCoreException(e);
        }

        // defines
        defines.addAll(options.defines);
        if ((options.compileForDll || options.type == Type.SHAREDLIB) && !StringUtils.isEmpty(options.dllSymbol))
            defines.add(options.dllSymbol+"_EXPORT");

        // XP has 8K limit on line length, so we may have to use the inline file feature of nmake
        int approximateLinkerLineLength = 500 + quoteJoin(objs).length() + quoteJoin(extraObjs).length() + 2*quoteJoin(options.libs).length() + 2*quoteJoin(libDirs).length();
        boolean isLongLinkerLine = approximateLinkerLineLength > 8000;

        // write dependencies
        StringBuilder deps = new StringBuilder();
        //String sep = " ";
        String sep = " \\\n\t";
        for (String srcDir : sourceDirs) {
            IContainer srcFolder = srcDir.equals(".") ? folder : folder.getFolder(new Path(srcDir));
            Map<IFile,Set<IFile>> fileDepsMap = perFileDeps.get(srcFolder);
            if (fileDepsMap != null) {
            	for (IFile srcFile : fileDepsMap.keySet()) {
            		if (srcFile.getFileExtension().equals(ccExt)) {
            			String objFileName = "$O/" + abs2rel(srcFile.getLocation()).toString().replaceFirst("\\.[^.]+$", "." + objExt);
            			deps.append(objFileName + ": ");
            			deps.append(abs2rel(srcFile.getLocation()).toString());
            			for (IFile includeFile : fileDepsMap.get(srcFile))
            				deps.append(sep + abs2rel(includeFile.getLocation()).toString());
            			deps.append("\n");
            		}
            	}
            }
        }

        // collect data for the template
        Map<String, Object> m = new HashMap<String, Object>();
        m.put("rem", "");  // allows putting comments into the template
        m.put("lbrace", "{");
        m.put("rbrace", "}");
        m.put("nmake", isNMake);
        m.put("targetprefix", targetPrefix);
        m.put("target", target);
        m.put("targetsuffix", targetSuffix);
        m.put("outdir", outDir);
        m.put("subpath", subpath);
        m.put("isdeep", isDeep);
        m.put("progname", "opp_makemake");  // isNMake ? "opp_nmakemake" : "opp_makemake"
        m.put("args", options.toString());
        m.put("configfile", configFile);
        m.put("-L", isNMake ? "/libpath:" : "-L");
        m.put("-l", isNMake ? "" : "-l");
        m.put(".lib", isNMake ? ".lib" : "");
        m.put("-u", isNMake ? "/include:" : "-u");
        m.put("-out", isNMake ? "/out:" : "-o ");  // note space after "-o" -- OS/X needs it
        m.put("cc", ccExt);
        m.put("obj", objExt);
        m.put("deps", deps.toString());
        m.put("exe", options.type == MakemakeOptions.Type.EXE);
        m.put("sharedlib", options.type == MakemakeOptions.Type.SHAREDLIB);
        m.put("staticlib", options.type == MakemakeOptions.Type.STATICLIB);
        m.put("nolink", options.type == MakemakeOptions.Type.NOLINK);
        m.put("defaultmode", StringUtils.nullToEmpty(options.defaultMode));
        m.put("allenv", options.userInterface.startsWith("A"));
        m.put("cmdenv", options.userInterface.startsWith("C"));
        m.put("tkenv", options.userInterface.startsWith("T"));
        m.put("extraobjs", quoteJoin(extraObjs));
        m.put("includepath", prefixQuoteJoin(includeDirs, "-I"));
        m.put("libpathdirs", libDirs);
        m.put("libs", options.libs);
        m.put("defines", prefixQuoteJoin(defines, "-D"));
        m.put("makefilevariables", options.makefileVariables);
        m.put("makecommand", makecommand);
        m.put("makefile", isNMake ? "Makefile.vc" : "Makefile");
        m.put("makefrags", makefrags);
        m.put("msgccfiles", msgccfiles);
        m.put("msghfiles", msghfiles);
        m.put("msgfiles", msgfiles);
        m.put("objs", quoteJoin(objs));
        m.put("submakedirs", submakeDirs);
        m.put("submakenames", submakeNames);
        m.put("dllsymbol", StringUtils.nullToEmpty(options.dllSymbol));
        m.put("sourcedirs", sourceDirs);
        m.put("backslashedsourcedirs", backslashedSourceDirs);
        m.put("nmake_inlinefile", (isNMake && isLongLinkerLine) ? "@<<\n" : "");
        m.put("nmake_inlineend", (isNMake && isLongLinkerLine) ? "\n<<" : "");

        // now generate the makefile
        System.out.println("generating makefile for " + folder.toString());
        if (template == null) {
            try {
                template = FileUtils.readTextFile(Makemake.class.getResourceAsStream(MAKEFILE_TEMPLATE_NAME));
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
        return result;
    }

    /**
     * If path is absolute, converts the path "path" to relative path (relative to
     * "folderLocation"). All "\" are converted to "/".
     */
    protected String abs2rel(String location) throws CoreException {
        return abs2rel(new Path(location)).toString();
    }

    protected IPath abs2rel(IPath location) throws CoreException {
        if (!location.isAbsolute()) {
            // leave relative paths untouched
            return location;
        }
        else if (projectLocation.isPrefixOf(location)) {
            // location is within the project, make it relative
            return MakefileTools.makeRelativePath(location, folderLocation);
        }
        else {
            IProject containingProject = null;
            for (IProject project : folder.getProject().getReferencedProjects()) { //XXX should use transitive closure of referenced projects!
                if (project.getLocation().isPrefixOf(location)) {
                    containingProject = project; break;
                }
            }
            if (containingProject != null) {
                // generate something like $(OTHER_PROJECT_DIR)/some/file
                IPath projectRelativePath = location.removeFirstSegments(containingProject.getLocation().segmentCount());
                String symbolicProjectName = makeSymbolicProjectName(containingProject);
                return new Path("$(" + symbolicProjectName + ")").append(projectRelativePath);
            }
            else {
                // points outside the project -- leave it as it is
                return location;
            }
        }
    }

    public static String makeSymbolicProjectName(IProject project) {
        //FIXME must not begin with number! must not collide with symbolic name of another project!
        // ==> why not use MakefileTools.generateTargetNames() ??
        return project.getName().replaceAll("[^0-9a-zA-Z_]", "_").toUpperCase()+"_PROJ";
    }
}
