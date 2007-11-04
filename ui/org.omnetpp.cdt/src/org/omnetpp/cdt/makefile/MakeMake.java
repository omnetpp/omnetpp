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
//- add "-d" option
//- rename old "-d" to "-S"
//FIXME support Linux too
//TODO add dependencies!
public class MakeMake {
    enum Type {EXE, SO, NOLINK};
    private List<String> args;
    private File directory = null;
    private String makefile = "Makefile.vc";
    private String basedir = "";
    private String targetfile = "simulation";
    private boolean force = true;
    private boolean linkwithobjects = false;
    private boolean tstamp = true;
    private boolean recursive = false;
    private String userif = "ALL";
    private Type type = Type.EXE;
    private String ccext = null;
    private String cfgfile = null;
    private String exportdefopt = "";
    private boolean compilefordll;
    private boolean ignorened; // note: no option for this
    private List<String> fragmentfiles = new ArrayList<String>();
    private List<String> subdirs = new ArrayList<String>();
    private List<String> exceptsubdirs = new ArrayList<String>();
    private List<String> includedirs = new ArrayList<String>();
    private List<String> libdirs = new ArrayList<String>();
    private List<String> libs = new ArrayList<String>();
    private List<String> importlibs = new ArrayList<String>();
    private List<String> tstampdirs = new ArrayList<String>();
    private List<String> linkdirs = new ArrayList<String>();
    private List<String> externalobjs = new ArrayList<String>();

    public MakeMake() {
    }

    public void run(File directory, String[] argv) throws IOException {
        parseArgs(directory, argv);
        generateMakefile();
    }

    public void parseArgs(File directory, String[] argv) {
        this.args = Arrays.asList(argv);
        this.directory = directory;
        targetfile = directory.getName();

        // process arguments
        for (int i = 0; i < argv.length; i++) {
            String arg = argv[i];
            if (arg.equals("-h") || arg.equals("--help")) {
                // TODO
            }
            else if (arg.equals("-f") || arg.equals("--force")) {
                force = true;
            }
            else if (arg.equals("-e") || arg.equals("--ext")) {
                ccext = argv[++i];
            }
            else if (arg.equals("-o") || arg.equals("--outputfile")) {
                targetfile = argv[++i];
                targetfile = abs2rel(targetfile, basedir);
            }
            else if (arg.equals("-N") || arg.equals("--ignore-ned")) {
                throw new IllegalArgumentException("opp_nmakemake: "+arg+": obsolete option, please remove (dynamic NED loading is now the default)");
            }
            else if (arg.equals("-r") || arg.equals("--recurse")) {
                recursive = true;
            }
            else if (arg.equals("-X") || arg.equals("--except")) {
                String dir = argv[++i]; //FIXME possible out-of-bounds
                exceptsubdirs.add(dir);
            }
            else if (arg.startsWith("-X")) {
                String dir = StringUtils.removeStart(arg, "-X");
                exceptsubdirs.add(dir);
            }
            else if (arg.equals("-b") || arg.equals("--basedir")) {
                basedir = argv[++i];
            }
            else if (arg.equals("-c") || arg.equals("--configfile")) {
                cfgfile = argv[++i];
                cfgfile = abs2rel(cfgfile, basedir);
            }
            else if (arg.equals("-n") || arg.equals("--nolink")) {
                type = Type.NOLINK;
            }
            else if (arg.equals("-d") || arg.equals("--subdir")) {
                subdirs.add(argv[++i]);
            }
            else if (arg.startsWith("-d")) {
                String dir = StringUtils.removeStart(arg, "-d");
                subdirs.add(dir);
            }
            else if (arg.equals("-s") || arg.equals("--make-so")) {
                compilefordll = true;
                type = Type.SO;
            }
            else if (arg.equals("-t") || arg.equals("--importlib")) {
                importlibs.add(argv[++i]);
            }
            else if (arg.equals("-S") || arg.equals("--fordll")) {
                compilefordll = true;
            }
            else if (arg.equals("-w") || arg.equals("--withobjects")) {
                linkwithobjects = true;
            }
            else if (arg.equals("-x") || arg.equals("--notstamp")) {
                tstamp = false;
            }
            else if (arg.equals("-u") || arg.equals("--userinterface")) {
                userif = argv[++i];
                userif = userif.toUpperCase();
                if (!userif.equals("ALL") && !userif.equals("CMDENV") && !userif.equals("TKENV"))
                    throw new IllegalArgumentException("opp_nmakemake: -u: specify all, Cmdenv or Tkenv");
            }
            else if (arg.equals("-i") || arg.equals("--includefragment")) {
                String frag = argv[++i];
                frag = abs2rel(frag, basedir);
                fragmentfiles.add(frag);
            }
            else if (arg.startsWith("-I")) {
                String dir = StringUtils.removeStart(arg, "-I");
                dir = abs2rel(dir, basedir);
                includedirs.add(dir);
                if (tstamp && !dir.equals("."))
                    tstampdirs.add(dir);
            }
            else if (arg.startsWith("-L")) {
                String dir = StringUtils.removeStart(arg, "-L");
                dir = abs2rel(dir, basedir);
                libdirs.add(dir);
            }
            else if (arg.startsWith("-l")) {
                String lib = StringUtils.removeStart(arg, "-l");
                libs.add(lib);
            }
            else if (arg.equals("-P")) {
                exportdefopt = "-P" + argv[++i];
            }
            else if (arg.startsWith("-P")) {
                exportdefopt = arg;
            }
            else {
                arg = arg.replaceAll("/", "\\");
                if (file(arg).isDirectory()) {
                    arg = abs2rel(arg, basedir);
                    linkdirs.add(arg);
                    if (tstamp)
                        tstampdirs.add(arg);
                }
                else if (file(arg).isFile()) {
                    arg = abs2rel(arg, basedir);
                    externalobjs.add(arg);
                }
                else {
                    throw new IllegalArgumentException("opp_nmakemake: " + arg + " is neither an existing file/dir nor a valid option");
                }
            }
        }
    }

    public void generateMakefile() throws IOException {
        String doxyconf = "doxy.cfg";
        String makecommand = "nmake /nologo /f Makefile.vc";
        List<String> extdirobjs = new ArrayList<String>();
        List<String> extdirtstamps = new ArrayList<String>();
        List<String> objs = new ArrayList<String>();
        List<String> generatedheaders = new ArrayList<String>();

        if (file(makefile).isFile() && !force)
            throw new IllegalStateException("opp_nmakemake: use -f to force overwriting existing " + makefile);

        if (cfgfile == null) {
            // try to find it in obvious places
            for (String f : new String[] {"configuser.vc", "../configuser.vc", "../../configuser.vc", "../../../configuser.vc", "../../../../configuser.vc"}) {
                if (new File(directory.getPath() + File.separator + f).exists()) {
                    cfgfile = f;
                    break;
                }
            }
            if (cfgfile == null)
                throw new RuntimeException("opp_nmakemake: warning: configuser.vc file not found -- specify its location with the -c option");
        }
        else {
            if (!new File(cfgfile).exists())
                throw new RuntimeException("opp_nmakemake: error: file " + cfgfile + " not found");
        }

        // try to determine if .cc or .cpp files are used
        List<String> ccfiles = glob("*.cc");
        List<String> cppfiles = glob("*.cpp");
        if (ccext == null) {
            if (ccfiles.isEmpty() && !cppfiles.isEmpty())
                ccext = "cpp";
            else if (!ccfiles.isEmpty() && cppfiles.isEmpty())
                ccext = "cc";
            else if (!ccfiles.isEmpty() && !cppfiles.isEmpty())
                throw new RuntimeException("opp_nmakemake: you have both .cc and .cpp files -- specify -e cc or -e cpp option to select which set of files to use");
            else
                ccext = "cc";  // if no files, use .cc extension
        }
        else {
            if (ccext.equals("cc") && ccfiles.isEmpty() && !cppfiles.isEmpty())
                System.out.println("opp_nmakemake: warning: you specified -e cc but you have only .cpp files in this directory!");
            if (ccext.equals("cpp") && !ccfiles.isEmpty() && cppfiles.isEmpty())
                System.out.println("opp_nmakemake: warning: you specified -e cpp but you have only .cc files in this directory!");
        }

        if (recursive) {
            File[] list = directory.listFiles(new FileFilter() {
                public boolean accept(File file) {
                    if (file.isDirectory()) {
                        String name = file.getName();
                        if (!name.startsWith(".") && !name.equals("CVS") && !name.equals("backups") && !exceptsubdirs.contains(name))
                            return true;
                    }
                    return false;
                }});
            for (File i : list)
                subdirs.add(i.getName());
        }

        if (linkwithobjects) {
            for (String i : includedirs)
                if (!i.equals("."))
                    extdirobjs.add(i + "/*.obj");
        }

        for (String i : linkdirs) {
            extdirobjs.add(i + "/*.obj");
        }

        for (String i : tstampdirs) {
            extdirtstamps.add(quote(i + "/.tstamp"));
        }

        String[] objpatts = ignorened ? new String[] {"*.msg", "*." + ccext} : new String[] {"*.ned", "*.msg", "*." + ccext};
        for (String objpatt : objpatts) {
            for (String i : glob(objpatt)) {
                i = i.replaceAll("\\*[^ ]*", "");
                i = i.replaceAll("[^ ]*_n\\." + ccext + "$", "");
                i = i.replaceAll("[^ ]*_m\\." + ccext + "$", "");
                i = i.replaceAll("\\.ned$", "_n.obj");
                i = i.replaceAll("\\.msg$", "_m.obj");
                i = i.replaceAll("\\." + ccext + "$", ".obj");
                if (!i.equals(""))
                    objs.add(i);
            }
        }

        for (String i : glob("*.msg")) {
            i = i.replaceAll("\\.msg$", "_m.h");
            generatedheaders.add(i);
        }

        // now generate the makefile
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        PrintStream out = new PrintStream(bos);

        out.println("#");
        out.println("# Makefile for " + targetfile);
        out.println("#");
        out.println("#  ** This file was automatically generated by the command:");
        out.println("#  opp_nmakemake " + join(args));
        out.println("#");
        out.println();

        String suffix = null;
        switch (type) {
            case EXE: suffix = ".exe"; break;
            case SO: suffix = ".dll"; break;
            case NOLINK: suffix = ""; break;
        }

        String c_all = "";
        String c_cmd = "# ";
        String c_tk = "# ";

        if (userif.equals("ALL")) {
            c_all = "";
            c_cmd = "# ";
            c_tk = "# ";
        }
        else if (userif.equals("CMDENV")) {
            c_all = "#";
            c_cmd = "";
            c_tk = "# ";
        }
        else if (userif.equals("TKENV")) {
            c_all = "#";
            c_cmd = "# ";
            c_tk = "";
        }

        String fordllopt = "";
        if (compilefordll) {
            fordllopt = "/DWIN32_DLL";
        }

        out.println("# Name of target to be created (-o option)");
        out.println("TARGET = " + targetfile + suffix);
        out.println();
        out.println("# User interface (uncomment one) (-u option)");
        out.println(c_all + "USERIF_LIBS=$(TKENV_LIBS) $(CMDENV_LIBS)");
        out.println(c_cmd + "USERIF_LIBS=$(CMDENV_LIBS)");
        out.println(c_tk + "USERIF_LIBS=$(TKENV_LIBS)");
        out.println();
        out.println("# .ned or .h include paths with -I");
        out.println("INCLUDE_PATH=" + join(includedirs, "-I"));
        out.println();
        out.println("# misc additional object and library files to link");
        out.println("EXTRA_OBJS=" + join(externalobjs));
        out.println();
        out.println("# object files from other directories to link with");
        out.println("EXT_DIR_OBJS=" + join(extdirobjs));
        out.println();
        out.println("# time stamps of other directories (used as dependency)");
        out.println("EXT_DIR_TSTAMPS=" + join(extdirtstamps));
        out.println();
        out.println("# Additional libraries (-L, -l, -t options)");
        out.println("LIBS=" + join(libdirs, "/libpath:") + join(libs) + join(importlibs));
        out.println();
        out.println("#------------------------------------------------------------------------------");

        // Makefile
        out.println("!include \"" + cfgfile + "\"");
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
        out.println("MSGCOPTS=" + exportdefopt + " $(INCLUDE_PATH)");
        out.println();
        out.println("#------------------------------------------------------------------------------");

        // rules for $(TARGET)
        out.println("# object files in this directory");
        out.println("OBJS= " + join(objs));
        out.println();

        out.println("# header files generated (from msg files)");
        out.println("GENERATEDHEADERS= " + join(generatedheaders));
        out.println();

        out.println("# subdirectories to recurse into");
        out.println("SUBDIRS= " + join(subdirs));
        out.print("SUBDIR_TARGETS= ");
        for (String i : subdirs) {
            out.print(" " + i + "_dir");
        }
        out.println("\n");

        // include external Makefile fragments at top, so that they can
        // override the default target if they want
        if (!fragmentfiles.isEmpty()) {
            for (String frag : fragmentfiles) {
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
        String subdirs_target = subdirs.isEmpty() ? "" : "subdirs";

        switch (type) {
            case EXE:
                out.println("$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) " + subdirs_target + " " + makefile);
                out.println("\t$(LINK) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) $(OMNETPP_LIBS) /out:$(TARGET)");
                out.println("\t@echo.>.tstamp");
                break;
            case SO:
                out.println("$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) " + subdirs_target + " " + makefile);
                out.println("\t$(SHLIB_LD) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) /out:$(TARGET)");
                out.println("\t@echo.>.tstamp");
                break;
            case NOLINK:
                out.println("$(TARGET): $(OBJS) " + makefile + " " + subdirs_target + " .tstamp");
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
        out.println("# purify: $(OBJS) $(EXTRA_OBJS) " + subdirs_target + " " + makefile);
        out.println("# \tpurify $(CXX) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) -L$(OMNETPP_LIB_DIR) $(KERNEL_LIBS) $(USERIF_LIBS) $(SYS_LIBS_PURE) -o $(TARGET).pure");
        out.println();

        if (!extdirtstamps.isEmpty()) {
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

        for (String i : subdirs) {
            out.println(i + "_dir:");
            out.println("\tcd " + i + " && echo [Entering " + i + "] && " + makecommand + " && echo [Leaving " + i + "]\n");
        }

        // rules for ned files
        if (!ignorened) {
            for (String i : glob("*.ned")) {
                String obj = i.replaceAll("\\.ned$", "_n.obj");
                String c = i.replaceAll("\\.ned$", "_n." + ccext);

                out.println(obj + ": " + c);
                out.println("\t$(CXX) -c $(NEDCOPTS) /Tp " + c);
                out.println();

                out.println(c + ": " + i);
                out.println("\t$(NEDC:/=\\) -s _n." + ccext + " $(INCLUDE_PATH) " + i);
                out.println();
            }
        }

        // rules for msg files
        for (String i : glob("*.msg")) {
            String obj = i.replaceAll("\\.msg$", "_m.obj");
            String c = i.replaceAll("\\.msg$", "_m." + ccext);
            String h = i.replaceAll("\\.msg$", "_m.h");

            out.println(obj + ": " + c);
            out.println("\t$(CXX) -c $(NEDCOPTS) /Tp " + c);
            out.println();

            out.println(c + " " + h + ": " + i);
            out.println("\t$(MSGC:/=\\) -s _m." + ccext + " $(MSGCOPTS) " + i);
            out.println();
        }

        // rules for normal (not generated) C++ files
        for (String i : glob("*." + ccext)) {
            if (i.endsWith("_n." + ccext) || i.endsWith("_m." + ccext))
                continue;

            String obj = i;
            obj = obj.replaceAll("\\." + ccext, ".obj");
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
        out.println("\t-del *_n." + ccext + " *_n.h *_m." + ccext + " *_m.h 2>NUL");
        out.println("\t-del *.vec *.sca 2>NUL");
        out.print("\t-for %%i in ( $(SUBDIRS) ) do cd %%i && echo [clean in %%i] && " + makecommand
                + " clean && cd .. || exit /b 1\n");
        out.println();
        out.println("depend:");
        out.println("\t$(MAKEDEPEND) $(INCLUDE_PATH) -f " + makefile + " -- *." + ccext);
        out.print("\tif not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [depend in %%i] && " + makecommand
                + " depend && cd .. || exit /b 1\n");
        out.println();
        out.println("makefiles:");
        out.println("\topp_nmakemake " + (args.contains("-f") ? "" : "-f ") + join(args));
        out.println("\tif not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [makemake in %%i] && " + makecommand
                + " makefiles && cd .. || exit /b 1");
        out.println();
        out.println("# \"re-makemake\" is a deprecated, historic name of the above target");
        out.println("re-makemake: makefiles");
        out.println();
        out.println("# DO NOT DELETE THIS LINE -- make depend depends on it.");
        out.println();

        out.close();
        byte[] content = bos.toByteArray();

        // only overwrite file if it does not already exist with the same content,
        // to avoid excessive Eclipse workspace refreshes and infinite builder invocations
        File file = new File(directory.getPath() + File.separator + makefile);
        if (!file.exists() || !Arrays.equals(FileUtils.readBinaryFile(file), content))  // NOTE: byte[].equals does NOT compare the bytes, only the two object references!!!
            FileUtils.writeBinaryFile(content, file);
    }

    private String join(List<String> args) {
        return join(args, "");
    }

    private String join(List<String> args, String prefix) {
        String result = "";
        for (String i : args)
            result += " " + prefix + quote(i);
        return result.trim();  //XXX split to several backslashed lines if too long?
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

    private String abs2rel(String abs, String base) {
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
