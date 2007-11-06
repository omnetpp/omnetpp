package org.omnetpp.cdt.makefile;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.util.FileUtils;

/**
 * An opp_nmakemake implementation. May be invoked as a command-line tool
 * as well as via a Java API. Note: we work with filesystem dirs and files 
 * not workspace resources, because "make" will know about the filesystem only.  
 * 
 * @author Andras
 */
//FIXME adjust perl/bash versions: 
//- commandline: rename old "-d" to "-S"
//- commandline: add "-d" option
//- here we store -P option (p.exportDefOpt) without the "-P"
//- here we understand "-I" and "-L" both with and without space
//- commandline: "--notstamp" should take effect with linkDirs as well!
//- todo: must add support for "-DSYMBOL=value"
//FIXME support Linux too
//TODO optimize per-file dependencies -- bulk of execution time is spent there!
//
public class MakeMake {
    // parameters for the makemake function
    private File directory;
    private MakemakeOptions p;

    public MakeMake() {
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public boolean run(File directory, String[] argv, Map<IFile, Set<IFile>> perFileDeps) throws IOException {
        p = new MakemakeOptions(argv);
        return generateMakefile(directory, perFileDeps);
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public boolean generateMakefile(File dir, Map<IFile, Set<IFile>> perFileDeps) throws IOException {
        this.directory = dir;
        String doxyconf = "doxy.cfg";
        String makecommand = "nmake /nologo /f Makefile.vc";
        String target = p.target == null ? directory.getName() : p.target;
        List<String> externaldirobjs = new ArrayList<String>();
        List<String> externaldirtstamps = new ArrayList<String>();
        List<String> objs = new ArrayList<String>();
        List<String> generatedHeaders = new ArrayList<String>();
        List<String> linkDirs = new ArrayList<String>();
        List<String> externalObjects = new ArrayList<String>();
        List<String> tstampDirs = new ArrayList<String>();

        if (file(p.makefile).isFile() && !p.force)
            throw new IllegalStateException("opp_nmakemake: use -f to force overwriting existing " + p.makefile);

        if (p.configFile == null) {
            // try to find it in obvious places
            for (String f : new String[] {"configuser.vc", "../configuser.vc", "../../configuser.vc", "../../../configuser.vc", "../../../../configuser.vc"}) {
                if (new File(directory.getPath() + File.separator + f).exists()) {
                    p.configFile = f;
                    break;
                }
            }
            if (p.configFile == null)
                throw new RuntimeException("opp_nmakemake: warning: configuser.vc file not found -- specify its location with the -c option");
        }
        else {
            if (!new File(p.configFile).exists())
                throw new RuntimeException("opp_nmakemake: error: file " + p.configFile + " not found");
        }

        // try to determine if .cc or .cpp files are used
        List<String> ccfiles = glob("*.cc");
        List<String> cppfiles = glob("*.cpp");
        if (p.ccext == null) {
            if (ccfiles.isEmpty() && !cppfiles.isEmpty())
                p.ccext = "cpp";
            else if (!ccfiles.isEmpty() && cppfiles.isEmpty())
                p.ccext = "cc";
            else if (!ccfiles.isEmpty() && !cppfiles.isEmpty())
                throw new RuntimeException("opp_nmakemake: you have both .cc and .cpp files -- specify -e cc or -e cpp option to select which set of files to use");
            else
                p.ccext = "cc";  // if no files, use .cc extension
        }
        else {
            if (p.ccext.equals("cc") && ccfiles.isEmpty() && !cppfiles.isEmpty())
                System.out.println("opp_nmakemake: warning: you specified -e cc but you have only .cpp files in this directory!");
            if (p.ccext.equals("cpp") && !ccfiles.isEmpty() && cppfiles.isEmpty())
                System.out.println("opp_nmakemake: warning: you specified -e cpp but you have only .cc files in this directory!");
        }

        if (p.recursive) {
            File[] list = directory.listFiles(new FileFilter() {
                public boolean accept(File file) {
                    if (file.isDirectory()) {
                        String name = file.getName();
                        if (!name.startsWith(".") && !name.equals("CVS") && !name.equals("backups") && !p.exceptSubdirs.contains(name))
                            return true;
                    }
                    return false;
                }});
            for (File i : list)
                p.subdirs.add(i.getName());
        }

        for (String arg : p.extraArgs) {
            if (file(arg).isDirectory()) {
                arg = abs2rel(arg, p.baseDir);
                linkDirs.add(arg);
            }
            else if (file(arg).isFile()) {
                arg = abs2rel(arg, p.baseDir);
                externalObjects.add(arg);
            }
            else {
                throw new IllegalArgumentException("opp_makemake: " + arg + " is neither an existing file/dir nor a valid option");
            }
        }
        
        if (p.linkWithObjects) {
            for (String i : p.includeDirs)
                if (!i.equals("."))
                    externaldirobjs.add(i + "/*.obj");
        }

        for (String i : linkDirs) {
            externaldirobjs.add(i + "/*.obj");
        }

        for (String i : p.includeDirs)
            if (p.tstamp && !i.equals("."))
                tstampDirs.add(i);
        for (String i : linkDirs)
            if (p.tstamp)
                tstampDirs.add(i);
        
        for (String i : tstampDirs) {
            externaldirtstamps.add(quote(i + "/.tstamp"));
        }

        String[] objpatts = p.ignoreNedFiles ? new String[] {"*.msg", "*." + p.ccext} : new String[] {"*.ned", "*.msg", "*." + p.ccext};
        for (String objpatt : objpatts) {
            for (String i : glob(objpatt)) {
                i = i.replaceAll("\\*[^ ]*", "");
                i = i.replaceAll("[^ ]*_n\\." + p.ccext + "$", "");
                i = i.replaceAll("[^ ]*_m\\." + p.ccext + "$", "");
                i = i.replaceAll("\\.ned$", "_n.obj");
                i = i.replaceAll("\\.msg$", "_m.obj");
                i = i.replaceAll("\\." + p.ccext + "$", ".obj");
                if (!i.equals(""))
                    objs.add(i);
            }
        }

        for (String i : glob("*.msg")) {
            i = i.replaceAll("\\.msg$", "_m.h");
            generatedHeaders.add(i);
        }

        // now generate the makefile
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        PrintStream out = new PrintStream(bos);

        out.println("#");
        out.println("# Makefile for " + target);
        out.println("#");
        out.println("#  ** This file was automatically generated by the command:");
        out.println("#  opp_nmakemake " + join(p.args));
        out.println("#");
        out.println();

        String suffix = null;
        switch (p.type) {
            case EXE: suffix = ".exe"; break;
            case SO: suffix = ".dll"; break;
            case NOLINK: suffix = ""; break;
        }

        String c_all = "";
        String c_cmd = "# ";
        String c_tk = "# ";

        if (p.userInterface.equals("ALL")) {
            c_all = "";
            c_cmd = "# ";
            c_tk = "# ";
        }
        else if (p.userInterface.equals("CMDENV")) {
            c_all = "#";
            c_cmd = "";
            c_tk = "# ";
        }
        else if (p.userInterface.equals("TKENV")) {
            c_all = "#";
            c_cmd = "# ";
            c_tk = "";
        }

        String fordllopt = p.compileForDll ? "/DWIN32_DLL" : "";
        String exportDefOpt = p.exportDefOpt==null ? "" : ("-P" + p.exportDefOpt);

        out.println("# Name of target to be created (-o option)");
        out.println("TARGET = " + target + suffix);
        out.println();
        out.println("# User interface (uncomment one) (-u option)");
        out.println(c_all + "USERIF_LIBS=$(TKENV_LIBS) $(CMDENV_LIBS)");
        out.println(c_cmd + "USERIF_LIBS=$(CMDENV_LIBS)");
        out.println(c_tk + "USERIF_LIBS=$(TKENV_LIBS)");
        out.println();
        out.println("# .ned or .h include paths with -I");
        out.println("INCLUDE_PATH=" + join(p.includeDirs, "-I"));
        out.println();
        out.println("# misc additional object and library files to link");
        out.println("EXTRA_OBJS=" + join(externalObjects));
        out.println();
        out.println("# object files from other directories to link with");
        out.println("EXT_DIR_OBJS=" + join(externaldirobjs));
        out.println();
        out.println("# time stamps of other directories (used as dependency)");
        out.println("EXT_DIR_TSTAMPS=" + join(externaldirtstamps));
        out.println();
        out.println("# Additional libraries (-L, -l, -t options)");
        out.println("LIBS=" + join(p.libDirs, "/libpath:") + join(p.libs) + join(p.importLibs));
        out.println();
        out.println("#------------------------------------------------------------------------------");

        // Makefile
        out.println("!include \"" + p.configFile + "\"");
        out.println();
        out.println("# User interface libs");
        out.println("CMDENV_LIBS=/include:_cmdenv_lib envir.lib cmdenv.lib");
        out.println("TKENV_LIBS=/include:_tkenv_lib envir.lib tkenv.lib layout.lib $(TK_LIBS) $(ZLIB_LIBS)");
        out.println();
        out.println("# Simulation kernel");
        out.println("KERNEL_LIBS=common.lib sim_std.lib");
        out.println();
        out.println("!if \"$(WITH_NETBUILDER)\"==\"yes\"");
        out.println("KERNEL_LIBS= $(KERNEL_LIBS) nedxml.lib $(XML_LIBS)");
        out.println("!endif");
        out.println();
        out.println("!if \"$(WITH_PARSIM)\"==\"yes\"");
        out.println("KERNEL_LIBS= $(KERNEL_LIBS) $(MPI_LIBS)");
        out.println("!endif");
        out.println();
        out.println("# Simulation kernel and user interface libraries");
        out.println("OMNETPP_LIBS=/libpath:$(OMNETPP_LIB_DIR) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)");
        out.println();
        out.println("COPTS=$(CFLAGS) " + fordllopt + " $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)");
        out.println("MSGCOPTS=" + exportDefOpt + " $(INCLUDE_PATH)");
        out.println();
        out.println("#------------------------------------------------------------------------------");

        // rules for $(TARGET)
        out.println("# object files in this directory");
        out.println("OBJS= " + join(objs));
        out.println();

        out.println("# header files generated (from msg files)");
        out.println("GENERATEDHEADERS= " + join(generatedHeaders));
        out.println();

        out.println("# subdirectories to recurse into");
        out.println("SUBDIRS= " + join(p.subdirs));
        out.print("SUBDIR_TARGETS= ");
        for (String i : p.subdirs) {
            out.print(" " + i + "_dir");
        }
        out.println("\n");

        // include external Makefile fragments at top, so that they can
        // override the default target if they want
        if (!p.fragmentFiles.isEmpty()) {
            for (String frag : p.fragmentFiles) {
                out.println("# inserted from file '" + frag + "':");
                String text = FileUtils.readTextFile(file(frag));
                out.println(text);
                out.println();
            }
        }
        else {
            if (file("makefrag.vc").isFile()) {
                out.println("# inserted from file 'makefrag.vc':");
                String text = FileUtils.readTextFile(file("makefrag.vc"));
                out.println(text);
                out.println();
            }
        }

        // note: if "subdirs" were always among the target dependencies, it
        // would cause the executable to re-link every time (which is not good).
        String subdirs_target = p.subdirs.isEmpty() ? "" : "subdirs";

        switch (p.type) {
            case EXE:
                out.println("$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) " + subdirs_target + " " + p.makefile);
                out.println("\t$(LINK) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) $(OMNETPP_LIBS) /out:$(TARGET)");
                out.println("\t@echo.>.tstamp");
                break;
            case SO:
                out.println("$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) " + subdirs_target + " " + p.makefile);
                out.println("\t$(SHLIB_LD) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) /out:$(TARGET)");
                out.println("\t@echo.>.tstamp");
                break;
            case NOLINK:
                out.println("$(TARGET): $(OBJS) " + p.makefile + " " + subdirs_target + " .tstamp");
                out.println();
                out.println(".tstamp: $(OBJS)");
                out.println("\t@echo.>.tstamp");
                break;
        }
        out.println();

        if (!objs.isEmpty()) {
            out.println("$(OBJS) : $(GENERATEDHEADERS)");
            out.println();
        }

        // rule for Purify
        out.println("# purify: $(OBJS) $(EXTRA_OBJS) " + subdirs_target + " " + p.makefile);
        out.println("# \tpurify $(CXX) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) -L$(OMNETPP_LIB_DIR) $(KERNEL_LIBS) $(USERIF_LIBS) $(SYS_LIBS_PURE) -o $(TARGET).pure");
        out.println();

        if (!externaldirtstamps.isEmpty()) {
            out.println("$(EXT_DIR_TSTAMPS):");
            out.println("\t@echo Error: $(@:/=\\) does not exist.");
            out.println("\t@echo This means that at least the above dependency directory has not been built.");
            out.println("\t@echo Maybe you need to do a top-level make?");
            out.println("\t@echo.");
            out.println("\t@exit /b 1");
            out.println();
        }

        // rules for recursive build
        // simplified because nmake doesn't support phony targets
        out.println("subdirs: $(SUBDIR_TARGETS)");
        out.println();

        for (String i : p.subdirs) {
            out.println(i + "_dir:");
            out.println("\tcd " + i + " && echo [Entering " + i + "] && " + makecommand + " && echo [Leaving " + i + "]\n");
        }

        // rules for ned files
        if (!p.ignoreNedFiles) {
            for (String i : glob("*.ned")) {
                String obj = i.replaceAll("\\.ned$", "_n.obj");
                String c = i.replaceAll("\\.ned$", "_n." + p.ccext);

                out.println(obj + ": " + c);
                out.println("\t$(CXX) -c $(NEDCOPTS) /Tp " + c);
                out.println();

                out.println(c + ": " + i);
                out.println("\t$(NEDC:/=\\) -s _n." + p.ccext + " $(INCLUDE_PATH) " + i);
                out.println();
            }
        }

        // rules for msg files
        for (String i : glob("*.msg")) {
            String obj = i.replaceAll("\\.msg$", "_m.obj");
            String c = i.replaceAll("\\.msg$", "_m." + p.ccext);
            String h = i.replaceAll("\\.msg$", "_m.h");

            out.println(obj + ": " + c);
            out.println("\t$(CXX) -c $(COPTS) /Tp " + c);
            out.println();

            out.println(c + " " + h + ": " + i);
            out.println("\t$(MSGC:/=\\) -s _m." + p.ccext + " $(MSGCOPTS) " + i);
            out.println();
        }

        // rules for normal (not generated) C++ files
        for (String i : glob("*." + p.ccext)) {
            if (i.endsWith("_n." + p.ccext) || i.endsWith("_m." + p.ccext))
                continue;

            String obj = i;
            obj = obj.replaceAll("\\." + p.ccext, ".obj");
            out.println(obj + ": " + i);
            out.println("\t$(CXX) -c $(COPTS) /Tp " + i);
            out.println();
        }

        // documentation targets
        out.println("#doc: neddoc doxy");
        out.println();
        out.println("#neddoc:");
        out.println("#\t@opp_neddoc -a");
        out.println();
        out.println("#doxy: " + doxyconf);
        out.println("#\t@doxygen " + doxyconf);
        out.println();

        // rule for clean and other recursive targets
        // note funky `if not "$(SUBDIRS)"=="" ..." -- it is necessary because
        // cmd.exe thinks that no "for" args is an error (!?!?)
        out.println("generateheaders: $(GENERATEDHEADERS)");
        out.println("\t@if not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do @cd %%i && echo [opp_msgc in %%i] && "
                + makecommand + " generateheaders && cd .. || exit /b 1");
        out.println();
        out.println("clean:");
        out.println("\t-del *.obj .tstamp *.idb *.pdb *.ilk *.exp $(TARGET) $(TARGET:.exe=.lib) $(TARGET:.dll=.lib) 2>NUL");
        out.println("\t-del *_n." + p.ccext + " *_n.h *_m." + p.ccext + " *_m.h 2>NUL");
        out.println("\t-del *.vec *.sca 2>NUL");
        out.print("\t-for %%i in ( $(SUBDIRS) ) do cd %%i && echo [clean in %%i] && " + makecommand
                + " clean && cd .. || exit /b 1\n");
        out.println();
        out.println("depend:");
        out.println("\t$(MAKEDEPEND) $(INCLUDE_PATH) -f " + p.makefile + " -- *." + p.ccext);
        out.print("\tif not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [depend in %%i] && " + makecommand
                + " depend && cd .. || exit /b 1\n");
        out.println();
        out.println("makefiles:");
        out.println("\topp_nmakemake " + (p.args.contains("-f") ? "" : "-f ") + join(p.args));
        out.println("\tif not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [makemake in %%i] && " + makecommand
                + " makefiles && cd .. || exit /b 1");
        out.println();
        out.println("# \"re-makemake\" is a deprecated, historic name of the above target");
        out.println("re-makemake: makefiles");
        out.println();
        out.println("# DO NOT DELETE THIS LINE -- make depend depends on it.");
        IPath directoryPath = new Path(directory.getPath());
        for (IFile f : perFileDeps.keySet()) {
            if (f.getParent().getLocation().equals(directoryPath)) {  //FIXME THIS LINE IS VERY COSTLY!!! with INET, 1000ms out of total 1500ms is spent here 
                out.print(f.getName() + ":");
                for (IFile f2 : perFileDeps.get(f))
                    out.print(" " + f2.getLocation().toString());  //XXX make it relative to this folder or to the project
                out.println();
            }
        }
        out.println();

        out.close();
        byte[] content = bos.toByteArray();

        // only overwrite file if it does not already exist with the same content,
        // to avoid excessive Eclipse workspace refreshes and infinite builder invocations
        File file = new File(directory.getPath() + File.separator + p.makefile);
        if (!file.exists() || !Arrays.equals(FileUtils.readBinaryFile(file), content)) { // NOTE: byte[].equals does NOT compare the bytes, only the two object references!!!
            FileUtils.writeBinaryFile(content, file);
            return true;  // no change
        }
        return false;  // it was already OK
    }

    private String join(List<String> args) {
        return join(args, "");
    }

    private String join(List<String> args, String prefix) {
        StringBuilder result = new StringBuilder(args.size() * 32);
        for (String i : args)
            result.append(" ").append(prefix).append(quote(i));
        //XXX split to several backslashed lines if too long?
        return result.length()==0 ? "" : result.substring(1); // chop off leading space
    }

    private String quote(String string) {
        return string.contains(" ") ? ("\"" + string + "\"") : string;
    }

    private File file(String path) {
        File file = new File(path);
        if (!file.isAbsolute())
            file = new File(directory.getPath() + File.separator + path);
        return file;
    }

    private List<String> glob(String pattern) {
        final String regex = pattern.replace(".", "\\.").replace("*", ".*").replace("?", ".?"); // good enough for what we need here
        return Arrays.asList(directory.list(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.matches(regex);
            }}));
    }

    String abs2rel(String abs, String base) {
        return abs2rel(abs, base, null);
    }

    /** 
     * Converts absolute path abs to relative path (relative to the current
     * directory cur), provided that both abs and cur are under a
     * "project base directory" base. Otherwise it returns the original path.
     * All "\" are converted to "/".
     */
    private String abs2rel(String abs, String base, String cur) {
        return abs;
        
//         if (base == null)
//             return abs;
//         if (cur == null)
//             cur = directory.getAbsolutePath();
//
//         //TODO: if abs is not within base, return abs unchanged
//         abs = normalize(abs);
//         cur = normalize(cur);
//         base = normalize(base);
//
//         // # some normalization
//        // $abs =~ s|\\|/|g;
//        // $cur =~ s|\\|/|g;
//        // $base =~ s|\\|/|g;
//
//        // $abs =~ s|/\./|/|g;
//        // $cur =~ s|/\./|/|g;
//        // $base =~ s|/\./|/|g;
//
//        // $abs =~ s|//+|/|g;
//        // $cur =~ s|//+|/|g;
//        // $base =~ s|//+|/|g;
//
//        // //$cur =~ s|/*$|/|;
//        // //$base =~ s|/*$|/|;
//
//        // if (!($abs =~ /^\Q$base\E/i && $cur =~ /^\Q$base\E/i)) {
//        // return $abs;
//        // }
//
//        // while (1)
//        // {
//        // // keep cutting off common prefixes until we can
//        // $abs =~ m|^(.*?/)|;
//        // my $prefix = $1;
//        // last if ($prefix eq "");
//        // if ($cur =~ /^\Q$prefix\E/i) {
//        // $abs =~ s/^\Q$prefix\E//i;
//        // $cur =~ s/^\Q$prefix\E//i;
//        // } else {
//        // last;
//        // }
//        // }
//
//        // # assemble relative path: change every directory name in $cur to "..",
//        // # then add $abs to it.
//        // $cur =~ s|[^/]+|..|g;
//        // my $rel = $cur.$abs;
//
//        //  return $rel;
    }

    private String normalize(String abs) {
        // TODO Auto-generated method stub
        return null;
    }
}
