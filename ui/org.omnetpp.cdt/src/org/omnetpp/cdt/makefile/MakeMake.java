package org.omnetpp.cdt.makefile;

import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * An opp_nmakemake implementation. May be invoked as a command-line tool
 * as well as via a Java API. Note: we work with filesystem dirs and files
 * not workspace resources, because "make" will know about the filesystem only.
 *
 * @author Andras
 */
//FIXME adjust perl/bash versions:
//- commandline add support for "--" after which everything is extraArg
//- commandline: rename old "-d" to "-S"
//- commandline: add "-d" option
//- here we store -P option (p.exportDefOpt) without the "-P"
//- here we understand "-I" and "-L" both with and without space
//- commandline: "--notstamp" should take effect with linkDirs as well!
//- todo: must add support for "-DSYMBOL=value"
//FIXME support Linux too
public class MakeMake {
    private static final String MAKEFILE_TEMPLATE_NAME = "Makefile.TEMPLATE";

    private String template;

    // parameters for the makemake function
    private IContainer folder;
    private MakemakeOptions p;

    // computed
    private IPath projectLocation;
    private IPath folderLocation;

    public MakeMake() {
    }

    public static void main(String[] args) throws Throwable {
        new MakeMake().generateMakefile(null /*FIXME use current working dir*/, args, null);
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public boolean generateMakefile(IContainer folder, String args, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws IOException, CoreException {
        return generateMakefile(folder, new MakemakeOptions(args), perFileDeps);
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public boolean generateMakefile(IContainer folder, String[] argv, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws IOException, CoreException {
        return generateMakefile(folder, new MakemakeOptions(argv), perFileDeps);
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public boolean generateMakefile(IContainer folder, MakemakeOptions options, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws IOException, CoreException {
        this.folder = folder;
        this.p = options;
        
        File directory = folder.getLocation().toFile();
        projectLocation = folder.getProject().getLocation();
        folderLocation = folder.getLocation();

        boolean isNMake = p.isNMake;

        String makefile = isNMake ? "Makefile.vc" : "Makefile";
        if (file(makefile).isFile() && !p.force)
            throw new IllegalStateException("use -f to force overwriting existing " + makefile);

        String target = p.target == null ? folder.getName() : p.target;
        List<String> externaldirobjs = new ArrayList<String>();
        List<String> externaldirtstamps = new ArrayList<String>();
        List<String> objs = new ArrayList<String>();
        List<String> generatedHeaders = new ArrayList<String>();
        List<String> linkDirs = new ArrayList<String>();
        List<String> externalObjects = new ArrayList<String>();
        List<String> tstampDirs = new ArrayList<String>();
        List<String> msgfiles = new ArrayList<String>();
        List<String> msgccandhfiles = new ArrayList<String>();

        target = abs2rel(target);

        List<String> includeDirs = new ArrayList<String>();
        List<String> libDirs = new ArrayList<String>();

        for (String i : p.includeDirs)
            includeDirs.add(abs2rel(i));
        for (String i : p.libDirs)
            libDirs.add(abs2rel(i));

        String makecommand = isNMake ? "make" : "nmake /nologo /f Makefile.vc";

        if (p.baseDir != null)
            throw new IllegalStateException("specifying the base directory (-b option) is not supported, it is always the project directory");

        String configFile = null;
        if (isNMake) {
            configFile = p.configFile;
            if (configFile == null) {
                // try to find it in obvious places
                for (String f : new String[] {"configuser.vc", "../configuser.vc", "../../configuser.vc", "../../../configuser.vc", "../../../../configuser.vc"}) {
                    if (new File(directory.getPath() + File.separator + f).exists()) {
                        configFile = f;
                        break;
                    }
                }
                if (configFile == null)
                    throw new RuntimeException("warning: configuser.vc file not found -- specify its location with the -c option");
            }
            else {
                if (!new File(configFile).exists())
                    throw new RuntimeException("error: file " + configFile + " not found");
            }
            configFile = abs2rel(configFile);
        }

        // try to determine if .cc or .cpp files are used
        String ccExt = p.ccext;
        List<String> ccfiles = glob("*.cc");
        List<String> cppfiles = glob("*.cpp");
        if (ccExt == null) {
            if (ccfiles.isEmpty() && !cppfiles.isEmpty())
                ccExt = "cpp";
            else if (!ccfiles.isEmpty() && cppfiles.isEmpty())
                ccExt = "cc";
            else if (!ccfiles.isEmpty() && !cppfiles.isEmpty())
                throw new RuntimeException("you have both .cc and .cpp files -- specify -e cc or -e cpp option to select which set of files to use");
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

        // prepare subdirs. First, check that all specified subdirs exist
        List<String> subdirs = new ArrayList<String>();
        subdirs.addAll(p.subdirs);

        for (String subdir : subdirs)
            if (!file(subdir).isDirectory())
                throw new IllegalArgumentException("subdirectory '" + subdir + "' does not exist");

        if (p.recursive) {
            File[] list = directory.listFiles(new FileFilter() {
                public boolean accept(File file) {
                    if (file.isDirectory()) {
                        String name = file.getName();
                        if (!name.startsWith(".") && !ArrayUtils.contains(MakefileTools.IGNORABLE_DIRS, name) && !p.exceptSubdirs.contains(name))
                            return true;
                    }
                    return false;
                }});
            for (File i : list) {
                subdirs.add(i.getName());
            }
        }

        List<String> subdirTargets = new ArrayList<String>();
        for (String subdir : subdirs)
            subdirTargets.add(subdir + (isNMake ? "_dir" : ""));  //XXX make sure none contains "_dir" as substring

        for (String arg : p.extraArgs) {
            Assert.isTrue(!StringUtils.isEmpty(arg), "empty makemake argument found");
            if (file(arg).isDirectory()) {
                arg = abs2rel(arg);
                linkDirs.add(arg);
            }
            else if (file(arg).isFile()) {
                arg = abs2rel(arg);
                externalObjects.add(arg);
            }
            else {
                throw new IllegalArgumentException("'" + arg + "' is neither an existing file/dir nor a valid option");
            }
        }

        if (p.linkWithObjects)
            for (String i : includeDirs)
                if (!i.equals("."))
                    externaldirobjs.add(i + "/*." + objExt);

        for (String i : linkDirs)
            externaldirobjs.add(i + "/*." + objExt);

        for (String i : includeDirs)
            if (p.tstamp && !i.equals("."))
                tstampDirs.add(i);
        for (String i : linkDirs)
            if (p.tstamp)
                tstampDirs.add(i);

        for (String i : tstampDirs)
            externaldirtstamps.add(quote(i + "/.tstamp"));

        String[] objpatts = p.ignoreNedFiles ? new String[] {"*.msg", "*." + ccExt} : new String[] {"*.ned", "*.msg", "*." + ccExt};
        for (String objpatt : objpatts) {
            for (String i : glob(objpatt)) {
                i = i.replaceAll("\\*[^ ]*", "");
                i = i.replaceAll("[^ ]*_n\\." + ccExt + "$", "");
                i = i.replaceAll("[^ ]*_m\\." + ccExt + "$", "");
                i = i.replaceAll("\\.ned$", "_n." + objExt);
                i = i.replaceAll("\\.msg$", "_m." + objExt);
                i = i.replaceAll("\\." + ccExt + "$", "." + objExt);
                if (!i.equals(""))
                    objs.add(i);
            }
        }

        msgfiles = glob("*.msg");
        for (String i : msgfiles) {
            String h = i.replaceAll("\\.msg$", "_m.h");
            String cc = i.replaceAll("\\.msg$", "_m." + ccExt);
            generatedHeaders.add(h);
            msgccandhfiles.add(cc);
            msgccandhfiles.add(h);
        }

        String makefrags = "";
        if (!p.fragmentFiles.isEmpty()) {
            for (String frag : p.fragmentFiles) {
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

        // write dependencies
        Map<IFile,Set<IFile>> fileDepsMap = perFileDeps == null ? null : perFileDeps.get(folder);

        StringBuilder deps = new StringBuilder();
        if (fileDepsMap != null) {
            for (IFile sourceFile : fileDepsMap.keySet()) {
                deps.append(sourceFile.getName() + ":");
                for (IFile includeFile : fileDepsMap.get(sourceFile))
                    deps.append(" " + abs2rel(includeFile.getLocation().toString()));
                deps.append("\n");
            }
            deps.append("\n");
        }
        //TODO: into deps:  prefixQuoteJoin(generatedHeaders);

        Map<String, Object> m = new HashMap<String, Object>();
        m.put("nmake", isNMake);
        m.put("target", target);
        m.put("progname", isNMake ? "opp_nmakemake" : "opp_makemake");
        m.put("args", quoteJoin(p.args));
        m.put("configfile", configFile);
        m.put("-L", isNMake ? "/libpath:" : "-L");
        m.put("-l", isNMake ? "" : "-l");
        m.put(".lib", isNMake ? ".lib" : "");
        m.put("-u", isNMake ? "/include:" : "-u");
        m.put("_dir", "_dir");
        m.put("cc", ccExt);
        m.put("deps", deps.toString());
        m.put("exe", p.type == MakemakeOptions.Type.EXE);
        m.put("so", p.type == MakemakeOptions.Type.SO);
        m.put("nolink", p.type == MakemakeOptions.Type.NOLINK);
        m.put("allenv", p.userInterface.startsWith("A"));
        m.put("cmdenv", p.userInterface.startsWith("C"));
        m.put("tkenv", p.userInterface.startsWith("T"));
        m.put("extdirobjs", quoteJoin(externaldirobjs));
        m.put("extdirtstamps", quoteJoin(externaldirtstamps));
        m.put("extraobjs", quoteJoin(externalObjects));
        m.put("includepath", prefixQuoteJoin(includeDirs, "-I"));
        m.put("libpath", prefixQuoteJoin(libDirs, (isNMake ? "/libpath:" : "-L")));
        m.put("libs", quoteJoin(p.libs));
        m.put("importlibs", quoteJoin(p.importLibs));
        m.put("link-o", isNMake ? "/out:" : "-o");
        m.put("makecommand", makecommand);
        m.put("makefile", isNMake ? "Makefile.vc" : "Makefile");
        m.put("makefrags", makefrags);
        m.put("msgccandhfiles", quoteJoin(msgccandhfiles));
        m.put("msgfiles", quoteJoin(msgfiles));
        m.put("objs", quoteJoin(objs));
        m.put("hassubdir", !subdirs.isEmpty());
        m.put("subdirs", quoteJoin(subdirs));
        m.put("subdirtargets", quoteJoin(subdirTargets));
        m.put("fordllopt", p.compileForDll ? "/DWIN32_DLL" : "");
        m.put("dllexportmacro", p.exportDefOpt==null ? "" : ("-P" + p.exportDefOpt));

        // now generate the makefile

//TODO into the template
//        if (!externaldirtstamps.isEmpty()) {
//            out.println("$(EXT_DIR_TSTAMPS):");
//            out.println("\t@echo Error: $(@:/=\\) does not exist.");
//            out.println("\t@echo This means that at least the above dependency directory has not been built.");
//            out.println("\t@echo Maybe you need to do a top-level make?");
//            out.println("\t@echo.");
//            out.println("\t@exit /b 1");
//            out.println();
//        }
//
// simplified because nmake doesn't support phony targets

        System.out.println("generating makefile for " + folder.toString());
        if (template == null) {
            template = FileUtils.readTextFile(MakeMake.class.getResourceAsStream(MAKEFILE_TEMPLATE_NAME));
            template = template.replace("\r\n", "\n");
        }

        String content = StringUtils.substituteIntoTemplate(template, m);
        byte[] bytes = content.getBytes();

        // only overwrite file if it does not already exist with the same content,
        // to avoid excessive Eclipse workspace refreshes and infinite builder invocations
        File file = new File(directory.getPath() + File.separator + makefile);
        if (!file.exists() || !Arrays.equals(FileUtils.readBinaryFile(file), bytes)) { // NOTE: byte[].equals does NOT compare the bytes, only the two object references!!!
            FileUtils.writeBinaryFile(bytes, file);
            return true;  // no change
        }
        return false;  // it was already OK
    }

    protected String quoteJoin(List<String> args) {
        return prefixQuoteJoin(args, "");
    }

    protected String prefixQuoteJoin(List<String> args, String prefix) {
        StringBuilder result = new StringBuilder(args.size() * 32);
        for (String i : args)
            result.append(" ").append(prefix).append(quote(i));
        //XXX split to several backslashed lines if too long?
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

    protected List<String> glob(String pattern) {
        final String regex = pattern.replace(".", "\\.").replace("*", ".*").replace("?", ".?"); // good enough for what we need here
        String[] files = folder.getLocation().toFile().list(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.matches(regex);
            }});
        return Arrays.asList(files==null ? new String[0] : files);
    }

    /**
     * If path is absolute, converts the path "path" to relative path (relative to the "base" directory),
     * All "\" are converted to "/".
     */
    protected String abs2rel(String location) throws CoreException {
        IPath path = new Path(location);
        if (!path.isAbsolute()) {
            // leave relative paths untouched
            return location;
        }
        else if (projectLocation.isPrefixOf(path)) {
            // location is within the project, make it relative
            return MakefileTools.makeRelativePath(folderLocation, path).toString();
        }
        else {
            IProject containingProject = null;
            for (IProject project : folder.getProject().getReferencedProjects()) { //XXX transitive closure of referenced projects
                if (project.getLocation().isPrefixOf(path)) {
                    containingProject = project; break;
                }
            }
            if (containingProject != null) {
                // generate something like $(OTHER_PROJECT_DIR)/some/file
                IPath projectRelativePath = path.removeFirstSegments(projectLocation.segmentCount());
                String symbolicProjectName = containingProject.getName().replaceAll("[^0-9a-zA-Z_]", "_").toUpperCase()+"_DIR"; //FIXME must not begin with number! must not collide with symbolic name of another project!
                return "$(" + symbolicProjectName + ")/" + projectRelativePath.toString();
            }
            else {
                // points outside the project -- leave it as it is
                return location;
            }
        }
    }
}
