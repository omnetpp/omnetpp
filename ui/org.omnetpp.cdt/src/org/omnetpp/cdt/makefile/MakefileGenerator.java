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
 * as well as via a Java API
 * 
 * @author Andras
 */
//XXX half-cooked.
//TODO add getters/setters once code becomes stable...
public class MakefileGenerator {
    private File directory;
    private String outfile;
    private boolean recursive;
    private String userif;
    private String type;
    private String includes;
    private String libpath;
    private String libs;
    private List<String> xobjs;
    private List<String> fragments;
    private String doxyconf;
    private String ccext;
    private String cfgfile;
    private boolean ignorened;
    private String exportdefopt = "";
    private String importlibs;
    private boolean fordll;
    private String makecommand;
    private String args;
    private String makefile;
    private List<String> subdirs;
    private List<String> extdirobjs;
    private List<String> extdirtstamps;
    private List<String> objs;
    private List<String> generatedheaders;

    public MakefileGenerator() {
        directory = null;
        cfgfile = "";
        recursive = false;
        userif = "ALL";
        includes = "";
        libpath = "";
        libs = "";
        type = "exe";
        makecommand = "nmake /nologo /f Makefile.vc";
        doxyconf = "doxy.cfg";
        ignorened = true;
        importlibs = "";
        makefile = "Makefile.vc";
        generatedheaders = new ArrayList<String>();
        objs = new ArrayList<String>();
        extdirtstamps = new ArrayList<String>();
        extdirobjs = new ArrayList<String>();
        subdirs = new ArrayList<String>();
        xobjs = new ArrayList<String>();
        fragments = new ArrayList<String>();
        ccext = ""; // we'll try to autodetect it
    }

    public void run(File directory, String[] argv) throws IOException {
        parseArgs(directory, argv);
        generateMakefile();
    }

    public void parseArgs(File directory, String[] argv) {
        // initialize vars
        List<String> linkdirs = new ArrayList<String>();
        String basedir = "";
        boolean linkwithobjects = false;
        boolean force = true;
        boolean tstamp = true;
        List<String> tstampdirs = new ArrayList<String>();
        final List<String> exceptdirs = new ArrayList<String>();

        this.directory = directory;
        outfile = directory.getName();
        this.args = StringUtils.join(argv, " ");

        // process arg vector
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
                outfile = argv[++i];
                outfile = abs2rel(outfile, basedir);
            }
            else if (arg.equals("-N") || arg.equals("--ignore-ned")) {
                // XXX print STDERR "opp_nmakemake: $arg: obsolete option,
                // please remove (dynamic NED loading is now the default)";
            }
            else if (arg.equals("-r") || arg.equals("--recurse")) {
                recursive = true;
            }
            else if (arg.equals("-X") || arg.equals("--except")) {
                String dir = argv[++i];
                exceptdirs.add(dir);
            }
            else if (arg.startsWith("-X")) {
                String dir = StringUtils.removeStart(arg, "-X");
                exceptdirs.add(dir);
            }
            else if (arg.equals("-b") || arg.equals("--basedir")) {
                basedir = argv[++i];
            }
            else if (arg.equals("-c") || arg.equals("--configfile")) {
                cfgfile = argv[++i];
                cfgfile = abs2rel(cfgfile, basedir);
            }
            else if (arg.equals("-n") || arg.equals("--nolink")) {
                type = "o";
            }
            else if (arg.equals("-s") || arg.equals("--make-so")) {
                fordll = true;
                type = "so";
            }
            else if (arg.equals("-t") || arg.equals("--importlib")) {
                importlibs += ' ' + argv[++i];
            }
            else if (arg.equals("-d") || arg.equals("--fordll")) {
                fordll = true;
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
                fragments.add(frag);
            }
            else if (arg.startsWith("-I")) {
                String dir = StringUtils.removeStart(arg, "-I");
                dir = abs2rel(dir, basedir);
                includes += " -I" + quote(dir);
                if (tstamp && !dir.equals("."))
                    tstampdirs.add(dir);
            }
            else if (arg.startsWith("-L")) {
                String dir = StringUtils.removeStart(arg, "-L");
                dir = abs2rel(dir, basedir);
                libpath += " /libpath:" + quote(dir);
            }
            else if (arg.startsWith("-l")) {
                String lib = StringUtils.removeStart(arg, "-l");
                libs += " " + lib;
            }
            else if (arg.equals("-P")) {
                exportdefopt = "-P" + argv[++i];
            }
            else if (arg.startsWith("-P")) {
                exportdefopt = arg;
            }
            else {
                arg = arg.replaceAll("/", "\\");
                if (isValidDir(arg)) {
                    arg = abs2rel(arg, basedir);
                    linkdirs.add(arg);
                    if (tstamp)
                        tstampdirs.add(arg);
                }
                else if (exists(arg)) {
                    arg = abs2rel(arg, basedir);
                    xobjs.add(arg);
                }
                else {
                    throw new IllegalArgumentException("opp_nmakemake: " + arg + " is neither an existing file/dir nor a valid option");
                }
            }
        }

        if (exists(makefile) && !force)
            throw new IllegalStateException("opp_nmakemake: use -f to force overwriting existing " + makefile);

         if (StringUtils.isEmpty(cfgfile)) {
             // try to find it
             for (String f : new String[] {"configuser.vc", "../configuser.vc", "../../configuser.vc", "../../../configuser.vc", "../../../../configuser.vc"}) {
                 if (new File(directory.getPath() + f).exists()) {
                     cfgfile = directory.getPath() + f;
                     break;
                 }
             }
             if (StringUtils.isEmpty(cfgfile))
                 System.out.println("opp_nmakemake: warning: configuser.vc file not found -- try -c option or edit generated makefile");
         }
         else {
             if (!new File(cfgfile).exists())
                 throw new RuntimeException("opp_nmakemake: error: file " + cfgfile + " not found");
         }

        // try to determine if .cc or .cpp files are used
        List<String> ccfiles = glob("*.cc");
        List<String> cppfiles = glob("*.cpp");
        if (StringUtils.isEmpty(ccext)) {
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
                        if (!name.startsWith(".") && !name.equals("CVS") && !name.equals("backups") && !exceptdirs.contains(name))
                            return true;
                    }
                    return false;
                }});
            for (File i : list)
                subdirs.add(i.getName());
        }

        if (linkwithobjects) {
            for (String i : includes.split(" ")) {
                i = StringUtils.removeStart(i, "-I");
                if (!i.equals(".")) {
                    extdirobjs.add(i + "/*.obj");
                }
            }
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
    }

    public void generateMakefile() throws IOException {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        PrintStream out = new PrintStream(bos);

        out.print("#\n");
        out.print("# Makefile for " + outfile + "\n");
        out.print("#\n");
        out.print("#  ** This file was automatically generated by the command:\n");
        out.print("#  opp_nmakemake " + args + "\n");
        out.print("#\n");
        out.print("\n");

        String suffix = ".exe";
        if (type.equals("so"))
            suffix = ".dll";
        if (type.equals("o"))
            suffix = "";

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

        String xobjs2 = StringUtils.join(xobjs, " \\\n            ");
        String extdirobjs2 = StringUtils.join(extdirobjs, " \\\n            ");
        String extdirtstamps2 = StringUtils.join(extdirtstamps, " \\\n            ");

        String fordllopt = "";
        if (fordll) {
            fordllopt = "/DWIN32_DLL";
        }

        out.print("# Name of target to be created (-o option)\n");
        out.print("TARGET = " + outfile + suffix + "\n");
        out.print("\n");
        out.print("# User interface (uncomment one) (-u option)\n");
        out.print(c_all + "USERIF_LIBS=$(TKENV_LIBS) $(CMDENV_LIBS)\n");
        out.print(c_cmd + "USERIF_LIBS=$(CMDENV_LIBS)\n");
        out.print(c_tk + "USERIF_LIBS=$(TKENV_LIBS)\n");
        out.print("\n");
        out.print("# .ned or .h include paths with -I\n");
        out.print("INCLUDE_PATH=" + includes + "\n");
        out.print("\n");
        out.print("# misc additional object and library files to link\n");
        out.print("EXTRA_OBJS=" + xobjs2 + "\n");
        out.print("\n");
        out.print("# object files from other directories to link with\n");
        out.print("EXT_DIR_OBJS=" + extdirobjs2 + "\n");
        out.print("\n");
        out.print("# time stamps of other directories (used as dependency)\n");
        out.print("EXT_DIR_TSTAMPS=" + extdirtstamps2 + "\n");
        out.print("\n");
        out.print("# Additional libraries (-L, -l, -t options)\n");
        out.print("LIBS=" + libpath + libs + importlibs + "\n");
        out.print("\n");
        out.print("#------------------------------------------------------------------------------\n");

        // Makefile
        out.print("!include \"" + cfgfile + "\"\n");
        out.print("\n");
        out.print("# User interface libs\n");
        out.print("CMDENV_LIBS=/include:_cmdenv_lib envir.lib cmdenv.lib\n");
        out.print("TKENV_LIBS=/include:_tkenv_lib envir.lib tkenv.lib layout.lib $(TK_LIBS) $(ZLIB_LIBS)\n");
        out.print("\n");
        out.print("# Simulation kernel\n");
        out.print("KERNEL_LIBS=common.lib sim_std.lib\n");
        out.print("\n");
        out.print("!if \"$(WITH_NETBUILDER)\"==\"yes\"\n");
        out.print("KERNEL_LIBS= $(KERNEL_LIBS) nedxml.lib $(XML_LIBS)\n");
        out.print("!endif\n");
        out.print("\n");
        out.print("!if \"$(WITH_PARSIM)\"==\"yes\"\n");
        out.print("KERNEL_LIBS= $(KERNEL_LIBS) $(MPI_LIBS)\n");
        out.print("!endif\n");
        out.print("\n");
        out.print("# Simulation kernel and user interface libraries\n");
        out.print("OMNETPP_LIBS=/libpath:$(OMNETPP_LIB_DIR) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)\n");
        out.print("\n");
        out.print("COPTS=$(CFLAGS) " + fordllopt + " $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)\n");
        out.print("MSGCOPTS=" + exportdefopt + " $(INCLUDE_PATH)\n");
        out.print("\n");
        out.print("#------------------------------------------------------------------------------\n");

        // rules for $(TARGET)
        String objs2 = StringUtils.join(objs, " ");
        out.print("# object files in this directory\n");
        out.print("OBJS= " + objs2 + "\n");
        out.print("\n");

        String generatedheaders2 = StringUtils.join(generatedheaders, " ");
        out.print("# header files generated (from msg files)\n");
        out.print("GENERATEDHEADERS= " + generatedheaders2 + "\n");
        out.print("\n");

        String subdirs2 = StringUtils.join(subdirs, " ");
        out.print("# subdirectories to recurse into\n");
        out.print("SUBDIRS= " + subdirs2 + "\n");
        out.print("SUBDIR_TARGETS= ");
        for (String i : subdirs) {
            out.print(" " + i + "_dir");
        }
        out.print("\n\n");

        // include external Makefile fragments at top, so that they can
        // override the default target if they want
        if (!fragments.isEmpty()) {
            for (String frag : fragments) {
                out.print("# inserted from file '" + frag + "':\n");
                // open(FRAG, " + frag + ");
                // while(<FRAG>) {print OUT;}
                // close FRAG;
                out.print("\n\n");
            }
        }
        else {
            if (exists("makefrag.vc")) {
                out.print("# inserted from file 'makefrag.vc':\n");
                // open(FRAG,"makefrag.vc");
                // while(<FRAG>) {print OUT;}
                // close FRAG;
                out.print("\n\n");
            }
        }

        // note: if "subdirs" were always among the target dependencies, it
        // would cause the executable to re-link every time (which is not good).
        String subdirs_target = recursive ? "subdirs" : "";

        if (type.equals("exe")) {
            out.print("$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) " + subdirs_target + " " + makefile + "\n");
            out.print("\t$(LINK) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) $(OMNETPP_LIBS) /out:$(TARGET)\n");
            out.print("\t@echo.>.tstamp\n");
        }
        else if (type.equals("so")) {
            out.print("$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) " + subdirs_target + " " + makefile + "\n");
            out.print("\t$(SHLIB_LD) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) /out:$(TARGET)\n");
            out.print("\t@echo.>.tstamp\n");
        }
        else if (type.equals("o")) {
            out.print("$(TARGET): $(OBJS) " + makefile + " " + subdirs_target + " .tstamp\n");
            out.print("\n");
            out.print(".tstamp: $(OBJS)\n");
            out.print("\t@echo.>.tstamp\n");
        }
        out.print("\n");

        if (!objs.isEmpty()) {
            out.print("$(OBJS) : $(GENERATEDHEADERS)\n");
            out.print("\n");
        }

        // rule for Purify
        out.print("# purify: $(OBJS) $(EXTRA_OBJS) " + subdirs_target + " " + makefile + "\n");
        out.print("# \tpurify $(CXX) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) -L$(OMNETPP_LIB_DIR) $(KERNEL_LIBS) $(USERIF_LIBS) $(SYS_LIBS_PURE) -o $(TARGET).pure\n");
        out.print("\n");

        if (!extdirtstamps.isEmpty()) {
            out.print("$(EXT_DIR_TSTAMPS):\n");
            out.print("\t@echo Error: $(@:/=\\) does not exist.\n");
            out.print("\t@echo This means that at least the above dependency directory has not been built.\n");
            out.print("\t@echo Maybe you need to do a top-level make?\n");
            out.print("\t@echo.\n");
            out.print("\t@exit /b 1\n");
            out.print("\n");
        }

        // rules for recursive build
        // simplified because @#$!? microsoft doesn"t know about phony targets
        out.print("subdirs: $(SUBDIR_TARGETS)\n");
        out.print("\n");

        for (String i : subdirs) {
            out.print(i + "_dir:\n");
            out.print("\tcd " + i + " && echo [Entering " + i + "] && " + makecommand + " && echo [Leaving " + i + "]\n\n");
        }

        // rules for ned files
        if (!ignorened) {
            for (String i : glob("*.ned")) {
                String obj = i.replaceAll("\\.ned$", "_n.obj");
                String c = i.replaceAll("\\.ned$", "_n." + ccext);

                out.print(obj + ": " + c + "\n");
                out.print("\t$(CXX) -c $(NEDCOPTS) /Tp " + c + "\n");
                out.print("\n");

                out.print(c + ": " + i + "\n");
                out.print("\t$(NEDC:/=\\) -s _n." + ccext + " $(INCLUDE_PATH) " + i + "\n");
                out.print("\n");
            }
        }

        // rules for msg files
        for (String i : glob("*.msg")) {
            String obj = i.replaceAll("\\.msg$", "_m.obj");
            String c = i.replaceAll("\\.msg$", "_m." + ccext);
            String h = i.replaceAll("\\.msg$", "_m.h");

            out.print(obj + ": " + c + "\n");
            out.print("\t$(CXX) -c $(NEDCOPTS) /Tp " + c + "\n");
            out.print("\n");

            out.print(c + " " + h + ": " + i + "\n");
            out.print("\t$(MSGC:/=\\) -s _m." + ccext + " $(MSGCOPTS) " + i + "\n");
            out.print("\n");
        }

        // rules for normal (not generated) C++ files
        for (String i : glob("*." + ccext)) {
            if (i.endsWith("_n." + ccext) || i.endsWith("_m." + ccext))
                continue;

            String obj = i;
            obj = obj.replaceAll("\\." + ccext, ".obj");
            out.print(obj + ": " + i + "\n");
            out.print("\t$(CXX) -c $(COPTS) /Tp " + i + "\n");
            out.print("\n");
        }

        // documentation targets
        out.print("#doc: neddoc doxy\n");
        out.print("\n");
        out.print("#neddoc:\n");
        out.print("#\t@opp_neddoc -a\n");
        out.print("\n");
        out.print("#doxy: " + doxyconf + "\n");
        out.print("#\t@doxygen " + doxyconf + "\n");
        out.print("\n");

        // prevent accumulating of "-f" options during subsequent remakes
        String args1 = " " + args + " ";
        args1 = args1.replace(" -f ", "");

        // rule for clean and other recursive targets
        // note funky `if not "$(SUBDIRS)"=="" ..." -- it is necessary because
        // cmd.exe thinks that no "for" args is an error (!?!?)
        out.print("generateheaders: $(GENERATEDHEADERS)\n");
        out.print("\t@if not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do @cd %%i && echo [opp_msgc in %%i] && "
                + makecommand + " generateheaders && cd .. || exit /b 1\n");
        out.print("\n");
        out.print("clean:\n");
        out.print("\t-del *.obj .tstamp *.idb *.pdb *.ilk *.exp $(TARGET) $(TARGET:.exe=.lib) $(TARGET:.dll=.lib) 2>NUL\n");
        out.print("\t-del *_n." + ccext + " *_n.h *_m." + ccext + " *_m.h 2>NUL\n");
        out.print("\t-del *.vec *.sca 2>NUL\n");
        out.print("\t-for %%i in ( $(SUBDIRS) ) do cd %%i && echo [clean in %%i] && " + makecommand
                + " clean && cd .. || exit /b 1\n");
        out.print("\n");
        out.print("depend:\n");
        out.print("\t$(MAKEDEPEND) $(INCLUDE_PATH) -f " + makefile + " -- *." + ccext + "\n");
        out.print("\tif not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [depend in %%i] && " + makecommand
                + " depend && cd .. || exit /b 1\n");
        out.print("\n");
        out.print("makefiles:\n");
        out.print("\topp_nmakemake -f " + args1 + "\n");
        out.print("\tif not \"$(SUBDIRS)\"==\"\" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [makemake in %%i] && " + makecommand
                + " makefiles && cd .. || exit /b 1\n");
        out.print("\n");
        out.print("# \"re-makemake\" is a deprecated, historic name of the above target\n");
        out.print("re-makemake: makefiles\n");
        out.print("\n");
        out.print("# DO NOT DELETE THIS LINE -- make depend depends on it.\n");
        out.print("\n");

        out.close();
        byte[] content = bos.toByteArray();

        // only overwrite file if it does not already exist with the same content,
        // to avoid excessive Eclipse workspace refreshes and infinite builder invocations
        File file = new File(directory.getPath() + File.separator + makefile);
        if (!file.exists() || !Arrays.equals(FileUtils.readBinaryFile(file), content))  // NOTE: byte[].equals does NOT compare the bytes, only the two object references!!!
            FileUtils.writeBinaryFile(content, file);
    }

    private boolean isValidDir(String arg) {
        // TODO Auto-generated method stub
        return true;
    }

    private boolean exists(String string) {
        // TODO Auto-generated method stub
        return false;
    }

    private String abs2rel(String abs, String base) {
        return abs2rel(abs, base, null);
    }

    /** 
     * Converts absolute path $abs to relative path (relative to the current
     * directory $cur), provided that both $abs and $cur are under a
     * "project base directory" $base. Otherwise it returns the original path.
     * All "\" are converted to "/".
     */
    private String abs2rel(String abs, String base, String cur) {
        return abs; // XXX see below the perl version
        // if ($base eq "") {
        // return $abs;
        // }
        // if ($cur eq "") {
        // $cur = cwd;
        // }

        // # some normalization
        // $abs =~ s|\\|/|g;
        // $cur =~ s|\\|/|g;
        // $base =~ s|\\|/|g;

        // $abs =~ s|/\./|/|g;
        // $cur =~ s|/\./|/|g;
        // $base =~ s|/\./|/|g;

        // $abs =~ s|//+|/|g;
        // $cur =~ s|//+|/|g;
        // $base =~ s|//+|/|g;

        // //$cur =~ s|/*$|/|;
        // //$base =~ s|/*$|/|;

        // if (!($abs =~ /^\Q$base\E/i && $cur =~ /^\Q$base\E/i)) {
        // return $abs;
        // }

        // while (1)
        // {
        // // keep cutting off common prefixes until we can
        // $abs =~ m|^(.*?/)|;
        // my $prefix = $1;
        // last if ($prefix eq "");
        // if ($cur =~ /^\Q$prefix\E/i) {
        // $abs =~ s/^\Q$prefix\E//i;
        // $cur =~ s/^\Q$prefix\E//i;
        // } else {
        // last;
        // }
        // }

        // # assemble relative path: change every directory name in $cur to "..",
        // # then add $abs to it.
        // $cur =~ s|[^/]+|..|g;
        // my $rel = $cur.$abs;

        //  return $rel;
    }

    private String quote(String string) {
        return string.contains(" ") ? ("\"" + string + "\"") : string;
    }

    private List<String> glob(String pattern) {
        final String regex = pattern.replace(".", "\\.").replace("*", ".*").replace("?", ".?"); // good enough for what we need here
        return Arrays.asList(directory.list(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.matches(regex);
            }}));
    }
}
