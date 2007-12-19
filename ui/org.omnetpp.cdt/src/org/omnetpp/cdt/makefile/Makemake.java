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
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;

/**
 * An opp_nmakemake implementation. May be invoked as a command-line tool
 * as well as via a Java API. Note: we work with filesystem dirs and files
 * not workspace resources, because "make" will know about the filesystem only.
 *
 * @author Andras
 */
//FIXME in CDT one can exclude files too, but currently makemake can only exclude whole folders
//FIXME makefrag vs makefrag.vc: which one to include??? use only one, but send that through template processing?
public class Makemake {
    private static final String MAKEFILE_TEMPLATE_NAME = "Makefile.TEMPLATE";

    private String template;

    // parameters for the makemake function
    private IContainer folder;
    private MakemakeOptions p;

    // computed
    private IPath projectLocation;
    private IPath folderLocation;

    public Makemake() {
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
        boolean isRecursive = p.isRecursive;
        boolean isDeep = p.isDeep;

        String makefile = isNMake ? "Makefile.vc" : "Makefile";
        if (file(makefile).isFile() && !p.force)
            throw new IllegalStateException("use -f to force overwriting existing " + makefile);
        boolean compileForDll = p.compileForDll || p.type==MakemakeOptions.Type.SHAREDLIB;

        String target = p.target == null ? folder.getName() : p.target;
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

        // FIXME target should not cotain relative path (just a name)
        target = abs2rel(target);

        // isRecursive and deep do not mix
        if (isRecursive) 
            isDeep = false;

        String makecommand = isNMake ? "nmake /nologo /f Makefile.vc" : "make";

        if (p.projectDir != null)
            throw new IllegalStateException("-P (--projectdir) option not supported, it is always the Eclipse project directory");

        String omnetppRoot = OmnetppMainPlugin.getDefault().getPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT);
        if (StringUtils.isEmpty(omnetppRoot))
            throw new IllegalStateException("OMNeT++ root must be set in Window|Preferences");
        String configFile = omnetppRoot + (isNMake ? "\\configuser.vc" : "/Makefile.inc"); 

        // collect source files
        if (!isDeep) {
            ccfiles = glob("*.cc");
            cppfiles = glob("*.cpp");
            msgfiles = glob("*.msg");
            nedfiles = glob("*.ned");
        }
        else {
            sourceDirs = collectDirs(folder, p.exceptSubdirs);
            for (String i : sourceDirs) {
                ccfiles.addAll(glob(i, "*.cc"));
                cppfiles.addAll(glob(i, "*.cpp"));
                msgfiles.addAll(glob(i, "*.msg"));
                nedfiles.addAll(glob(i, "*.ned"));
            }
        }

        for (String dir : sourceDirs)
            backslashedSourceDirs.add(dir.replace('/', '\\'));

        // include dirs and lib dirs
        for (String i : p.includeDirs)
            includeDirs.add(abs2rel(i));
        if (!p.noDeepIncludes)
            for (String i : sourceDirs)
                includeDirs.add(i);

        for (String i : p.libDirs)
            libDirs.add(abs2rel(i));

        
        // try to determine if .cc or .cpp files are used
        String ccExt = p.ccext;
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
        String targetPrefix = "";  // FIXME add to perl too
        String targetSuffix = "";
        if (p.type == MakemakeOptions.Type.EXE)
            targetSuffix = isNMake ? ".exe" : "";
        else if (p.type == MakemakeOptions.Type.SHAREDLIB) {
            targetSuffix = isNMake ? ".dll" : ".so";
            targetPrefix = isNMake ? "" : "lib";
        }
        else if (p.type == MakemakeOptions.Type.STATICLIB) {
            targetSuffix = isNMake ? ".lib" : ".a";
            targetPrefix = isNMake ? "" : "lib";
        }

        // prepare subdirs. First, check that all specified subdirs exist
        List<String> subdirs = new ArrayList<String>();
        subdirs.addAll(p.subdirs);

        for (String subdir : subdirs)
            if (!file(subdir).isDirectory())
                throw new IllegalArgumentException("subdirectory '" + subdir + "' does not exist");

        if (isRecursive) {
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
            extraObjs.add(arg);
        }

        if (p.linkWithObjects)
            for (String i : includeDirs)
                if (!i.equals("."))
                    extraObjs.add(i + "/*." + objExt);

        List<String> sources = new ArrayList<String>();
        if (ccExt.equals("cc"))
            sources.addAll(ccfiles);
        if (ccExt.equals("cpp"))
            sources.addAll(cppfiles);
        sources.addAll(msgfiles);
        if (!p.ignoreNedFiles) 
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

        // determine outDir
        String outdir;
        if (StringUtils.isEmpty(p.outRoot)) {
            outdir = ".";
        }
        else {
            IPath outRootPath = new Path(p.outRoot);
            IPath outRootAbs = outRootPath.isAbsolute() ? outRootPath : folder.getProject().getLocation().append(outRootPath);
            IPath outRootRel = abs2rel(outRootAbs);  // "<project>/out"
            outdir = outRootRel.toString();
        }

        // determine subpath: the project-relative path of this folder
        String subpath = folder.getProjectRelativePath().toString(); 
        
        // write dependencies
        // FIXME factor out common parts
        StringBuilder deps = new StringBuilder();
        if (!isDeep) {
            Map<IFile,Set<IFile>> fileDepsMap = perFileDeps == null ? null : perFileDeps.get(folder);
            if (fileDepsMap != null) {
                for (IFile sourceFile : fileDepsMap.keySet()) {
                    String objFileName;
                    if (sourceFile.getFileExtension().equals("msg"))
                        objFileName = sourceFile.getName().replaceFirst("\\.[^.]+$", "_m." + objExt);
                    else
                        objFileName = sourceFile.getName().replaceFirst("\\.[^.]+$", "." + objExt);
                    deps.append(objFileName + ":");
                    for (IFile includeFile : fileDepsMap.get(sourceFile))
                        deps.append(" " + abs2rel(includeFile.getLocation()).toString());
                    deps.append("\n");
                }
                deps.append("\n");
            }
        }
        else {
            for (IContainer depfolder : perFileDeps.keySet()) {
                // FIXME only within the make folder
                Map<IFile,Set<IFile>> fileDepsMap = perFileDeps.get(depfolder);
                String makefolderRelPath = abs2rel(depfolder.getLocation()).toString();
                if (StringUtils.isNotEmpty(makefolderRelPath))
                    makefolderRelPath += "/";
                for (IFile sourceFile : fileDepsMap.keySet()) {
                    String objFileName;
                    if (sourceFile.getFileExtension().equals("msg"))
                        objFileName = makefolderRelPath+sourceFile.getName().replaceFirst("\\.[^.]+$", "_m." + objExt);
                    else
                        objFileName = makefolderRelPath+sourceFile.getName().replaceFirst("\\.[^.]+$", "." + objExt);
                    
                    deps.append(objFileName + ": "+makefolderRelPath+sourceFile.getName());
                    for (IFile includeFile : fileDepsMap.get(sourceFile))
                        deps.append(" " + abs2rel(includeFile.getLocation()).toString());
                    deps.append("\n");
                }
            }
            
        }

        Map<String, Object> m = new HashMap<String, Object>();
        m.put("rem", "");  // allows putting comments into the template
        m.put("lbrace", "{");
        m.put("rbrace", "}");
        m.put("nmake", isNMake);
        m.put("target", targetPrefix+ target + targetSuffix);
        m.put("outdir", outdir); 
        m.put("subpath", subpath); 
        m.put("isdeep", isDeep);
        m.put("progname", "opp_makemake");  // isNMake ? "opp_nmakemake" : "opp_makemake"
        m.put("args", p.toString());
        m.put("configfile", configFile);
        m.put("-L", isNMake ? "/libpath:" : "-L");
        m.put("-l", isNMake ? "" : "-l");
        m.put(".lib", isNMake ? ".lib" : "");
        m.put("-u", isNMake ? "/include:" : "-u");
        m.put("-out", isNMake ? "/out:" : "-o ");
        m.put("_dir", "_dir");
        m.put("cc", ccExt);
        m.put("obj", objExt);
        m.put("deps", deps.toString());
        m.put("exe", p.type == MakemakeOptions.Type.EXE);
        m.put("sharedlib", p.type == MakemakeOptions.Type.SHAREDLIB);
        m.put("staticlib", p.type == MakemakeOptions.Type.STATICLIB);
        m.put("nolink", p.type == MakemakeOptions.Type.NOLINK);
        m.put("defaultmode", p.defaultMode);
        m.put("allenv", p.userInterface.startsWith("A"));
        m.put("cmdenv", p.userInterface.startsWith("C"));
        m.put("tkenv", p.userInterface.startsWith("T"));
        m.put("extraobjs", quoteJoin(extraObjs));
        m.put("includepath", prefixQuoteJoin(includeDirs, "-I"));
        m.put("libpath", prefixQuoteJoin(libDirs, (isNMake ? "/libpath:" : "-L")));
        m.put("libs", p.libs);
        m.put("defines", prefixQuoteJoin(p.defines, "-D"));
        m.put("makefiledefines", p.makefileDefines);
        m.put("makecommand", makecommand);
        m.put("makefile", isNMake ? "Makefile.vc" : "Makefile");
        m.put("makefrags", makefrags);
        m.put("msgccfiles", quoteJoin(msgccfiles));
        m.put("msghfiles", quoteJoin(msghfiles));
        m.put("msgfiles", quoteJoin(msgfiles));
        m.put("objs", quoteJoin(objs));
        m.put("subdirs", quoteJoin(subdirs));
        m.put("subdirtargets", quoteJoin(subdirTargets));
        m.put("fordllopt", compileForDll ? "/DWIN32_DLL" : "");
        m.put("dllexportmacro", StringUtils.isEmpty(p.dllExportMacro) ? "" : ("-P" + p.dllExportMacro));
        m.put("sourcedirs", sourceDirs);
        m.put("backslashedsourcedirs", backslashedSourceDirs);

        // now generate the makefile
        System.out.println("generating makefile for " + folder.toString());
        if (template == null) {
            template = FileUtils.readTextFile(Makemake.class.getResourceAsStream(MAKEFILE_TEMPLATE_NAME));
            template = template.replace("\r\n", "\n");
        }

        String content = StringUtils.substituteIntoTemplate(template, m);
        content = content.replace("\r\n", "\n");  // make line endings consistent 
        byte[] bytes = content.getBytes();

        // only overwrite file if it does not already exist with the same content,
        // to avoid excessive Eclipse workspace refreshes and infinite builder invocations
        File file = new File(directory.getPath() + File.separator + makefile);
        if (!file.exists() || !Arrays.equals(FileUtils.readBinaryFile(file), bytes)) { // NOTE: byte[].equals does NOT compare the bytes, only the two object references!!!
            FileUtils.writeBinaryFile(file, bytes);
            return true;  // no change
        }
        return false;  // it was already OK
    }

    protected List<String> collectDirs(IContainer dir, List<String> exceptSubdirs) throws CoreException {
        List<String> result = new ArrayList<String>();
        collectDirs(dir, ".", exceptSubdirs, result);
        return result;
    }    

    protected void collectDirs(IContainer dir, String dirPath, List<String> exceptSubdirs, final List<String> result) throws CoreException {
        if (!exceptSubdirs.contains(dirPath)) {
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
                result.add(subFolder+"/"+name);
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
                IPath projectRelativePath = location.removeFirstSegments(projectLocation.segmentCount());
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
        return project.getName().replaceAll("[^0-9a-zA-Z_]", "_").toUpperCase()+"_PROJ";
    }
}
