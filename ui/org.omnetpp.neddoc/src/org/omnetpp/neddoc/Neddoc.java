package org.omnetpp.neddoc;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.WordUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.image.export.PNGImageExporter;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

import de.unikassel.imageexport.exporters.ImageExporter;
import de.unikassel.imageexport.exporters.ImageExporterDescriptor;
import de.unikassel.imageexport.wizards.ExportImagesOfDiagramFilesOperation;

public class Neddoc {
    private String dotExecutable = "C:\\Workspace\\Tools\\Graphviz\\bin\\dot.exe";
    
    private IFolder neddocPath;

    private IProject project;
    
    private NEDResources resources;
    
    private IProgressMonitor monitor;

    private File outputFileName;

    private Map<File, FileOutputStream> outputStreams = new HashMap<File, FileOutputStream>();
    
    private ArrayList<INedTypeElement> typeElements;
    
    private Map<INedTypeElement, ArrayList<INedTypeElement>> subtypesMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();

    private Map<INedTypeElement, ArrayList<INedTypeElement>> implementorsMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();

    private Map<INedTypeElement, ArrayList<INedTypeElement>> usersMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();

    public Neddoc(IProject project) {
        this.project = project;

        neddocPath = project.getFolder("Documentation/neddoc");
        resources = NEDResourcesPlugin.getNEDResources();
    }
    
    public void generate() throws Exception {
        Job job = new Job("MemoryViewTab PropertyChanged") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    Neddoc.this.monitor = monitor;
                    monitor.beginTask("Generating neddoc...", IProgressMonitor.UNKNOWN);
                    ensureEmptyNeddoc();
                    collectNedTypes();
                    collectSubtypesMap();
                    collectImplementorsMap();
                    collectUsersMap();
                    generateCSS();
                    generateHTMLFrame();
                    generateFileList();
                    generateSelectedTopics();
                    generateChannelIndex();
                    generateChannelInterfaceIndex();
                    generateModuleInterfaceIndex();
                    generateSimpleModuleIndex();
                    generateCompoundModuleIndex();
                    generateAllModuleIndex();
                    generateNetworkIndex();
                    generateMessageIndex();
                    generateClassIndex();
                    generateStructIndex();
                    generateEnumIndex();
                    genereteTypeDiagrams();
                    generateNedFilePages();
                    generateNedTypePages();
                    monitor.done();
                    
                    return Status.OK_STATUS;
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
                finally {
                    for (FileOutputStream outputStream : outputStreams.values()) {
                        try {
                            outputStream.close();
                        }
                        catch (IOException e) {
                            throw new RuntimeException(e);
                        }
                    }
                }
            }
        };
        job.setName("Generating neddoc...");
        job.setUser(true);
        job.schedule();
    }

    private void ensureEmptyNeddoc() throws CoreException {
        for (File file : new File(neddocPath.getLocation().toString()).listFiles())
            file.delete();

        if (!neddocPath.exists())
            neddocPath.create(true , true, null);
    }
    
    private void collectNedTypes() {
        typeElements = new ArrayList<INedTypeElement>();

        for (IFile file : resources.getNedFiles(project))
            for (INedTypeElement typeElement : resources.getNedFileElement(file).getTopLevelTypeNodes())
                typeElements.add(typeElement);
        
        Collections.sort(typeElements, new Comparator<INedTypeElement>() {
            public int compare(INedTypeElement o1, INedTypeElement o2) {
                return o1.getName().compareToIgnoreCase(o2.getName());
            }
        });
    }
    
    private void collectSubtypesMap() {
        for (INedTypeElement subtype : typeElements) {
            INedTypeElement supertype = subtype.getNEDTypeInfo().getFirstExtendsRef();
            
            if (supertype != null) {
                ArrayList<INedTypeElement> subtypes = subtypesMap.get(supertype);
                
                if (subtypes == null)
                    subtypes = new ArrayList<INedTypeElement>();
                
                subtypes.add(subtype);
                subtypesMap.put(supertype, subtypes);
            }
        }
    }
    
    private void collectImplementorsMap() {
        for (INedTypeElement implementor : typeElements) {
            if (!(implementor instanceof IInterfaceTypeElement)) {
                for (INedTypeElement interfaze : implementor.getNEDTypeInfo().getLocalInterfaces()) {
                    ArrayList<INedTypeElement> implementors = implementorsMap.get(interfaze);
                    
                    if (implementors == null)
                        implementors = new ArrayList<INedTypeElement>();
                    
                    implementors.add(implementor);
                    implementorsMap.put(interfaze, implementors);
                }
            }
        }
    }
    
    private void collectUsersMap() {
        for (INedTypeElement userType : typeElements) {
            for (INedTypeElement usedType : userType.getNEDTypeInfo().getLocalUsedTypes()) {
                ArrayList<INedTypeElement> users = usersMap.get(usedType);
                
                if (users == null)
                    users = new ArrayList<INedTypeElement>();
                
                users.add(userType);
                usersMap.put(usedType, users);
            }
        }
    }
    
    private void generateCSS() throws IOException {
        FileUtils.writeTextFile(getFile("style.css"),
                "body,td,p,ul,ol,li,h1,h2,h3,h4 {font-family:arial,sans-serif }\r\n" + 
                "body,td,p,ul,ol,li { font-size:10pt }\r\n" + 
                "h1 { font-size:18pt; text-align:center }\r\n" + 
                "pre.comment { font-size:10pt; padding-left:5pt }\r\n" + 
                "pre.src { font-size:8pt; background:#F8F8F8; padding:5pt; border:1px solid; border-color:#a0a0a0 }\r\n" + 
                "th { font-size:10pt; text-align:left; vertical-align:top; background:#E0E0f0 }\r\n" + 
                "td { font-size:10pt; text-align:left; vertical-align:top }\r\n" + 
                "tt { font-family:Courier,Courier New,Fixed,Terminal }\r\n" + 
                "img          { border:none }\r\n" + 
                ".navbar     { font-size:8pt; }\r\n" + 
                ".navbarlink { font-size:8pt; }\r\n" + 
                ".indextitle { font-size:12pt; }\r\n" + 
                ".comptitle  { font-size:14pt; }\r\n" + 
                ".subtitle   { font-size:12pt; margin-bottom: 3px}\r\n" + 
                ".footer     { font-size:8pt; margin-top:0px; text-align:center; color:#303030; }\r\n" + 
                "FIXME.paramtable { border:2px ridge; border-collapse:collapse;}\r\n" + 
                ".src-keyword { font-weight:bold }\r\n" + 
                ".src-comment { font-style:italic; color:#404040 }\r\n" + 
                ".src-string  { color:#006000 }\r\n" + 
                ".src-number  { color:#0000c0 }\r\n");
    }

    private void generateHTMLFrame() throws IOException {
        FileUtils.writeTextFile(getFile("index.html"),
            "<html>\n" + 
            "   <head>\n" + 
            "      <title>Model documentation -- generated from NED files</title>\n" + 
            "   </head>\n" + 
            "   <frameset cols=\"35%,65%\">\n" + 
            "      <frameset rows=\"60%,40%\">\n" + 
            "         <frame src=\"all-modules.html\" name=\"componentsframe\"/>\n" + 
            "         <frame src=\"files.html\" name=\"filesframe\"/>\n" + 
            "      </frameset>\n" + 
            "      <frame src=\"overview.html\" name=\"mainframe\"/>\n" + 
            "   </frameset>\n" + 
            "   <noframes>\n" + 
            "      <h2>Frame Alert</h2>\n" + 
            "      <p>This document is designed to be viewed using HTML frames. If you see this message,\n" + 
            "      you are using a non-frame-capable browser.</p>\n" + 
            "   </noframes>\n" + 
            "</html>\n"); 
    }
    
    private void generateFileList() throws Exception {
        withGeneratingHTMLFile("files.html", new Runnable() {
            public void run() throws Exception {
                out("<h3 class=\"indextitle\">NED and MSG Files</h3>\r\n" + 
                    "<ul>\r\n");

                for (IFile file : CollectionUtils.toSorted(resources.getNedFiles(project)))
                    out("<li>\r\n" + 
                        "   <a href=\"" + getFileName(file) + "\" target=\"mainframe\">" + file.getProjectRelativePath().toString() + "</a>\r\n" + 
                        "</li>\r\n");

                out("</ul>\r\n");
            }
        });
    }
    
    private void generateNavigationBar(String where) throws IOException {
        out("<p class=\"navbar\">\r\n");
        generateNavigationBarLink("selected topics", where);
        out(" - modules ("); 
        generateNavigationBarLink("simple modules", where);
        out(", "); 
        generateNavigationBarLink("compound modules", where);
        out(", ");
        generateNavigationBarLink("all modules", where);
        out(") - "); 
        generateNavigationBarLink("module interfaces", where);
        out(" - "); 
        generateNavigationBarLink("channels", where);
        out(" - "); 
        generateNavigationBarLink("channel interfaces", where);
        out(" - "); 
        generateNavigationBarLink("networks", where);
        out(" - "); 
        generateNavigationBarLink("messages", where);
        out(" - "); 
        generateNavigationBarLink("classes", where);
        out(" - "); 
        generateNavigationBarLink("structs", where);
        out(" - "); 
        generateNavigationBarLink("enums", where);
        out("</p>\r\n");
    }

    private void generateNavigationBarLink(String label, String where) throws IOException {
        if (label.equals(where))
            out("<span class=\"navbarlink\">" + label + "</span>\r\n");
        else
            out("<a href=\"" + label.replace(" ", "-") + ".html\" class=\"navbarlink\">" + label + "</a>\r\n");
    }

    private void withGeneratingTypeIndexHTMLFile(final String where, final Class<?> clazz) throws Exception {
        withGeneratingNavigationHTMLFile(where, new Runnable() {
            public void run() throws Exception {
                generateTypeIndex(clazz);
            }
        });
    }

    private void withGeneratingNavigationHTMLFile(final String where, final Runnable content) throws Exception {
        withGeneratingHTMLFile(where.replace(" ", "-") + ".html", new Runnable() {
            public void run() throws Exception {
                generateNavigationBar(where);

                out("<h3 class=\"indextitle\">" + WordUtils.capitalize(where) + "</h3>\r\n" + 
                    "<ul>\r\n");
    
                content.run();
                
                out("</ul>\r\n");
            }
        });
    }

    private void generateSelectedTopics() throws Exception {
        withGeneratingNavigationHTMLFile("selected topics", new Runnable() {
            public void run() throws Exception {
                out("<li><a href=\"overview.html\" target=\"mainframe\">HOME</a></li>\r\n");
        
                if (isDotAvailable())
                    out("<li><a href=\"full-usage-diagram.html\" target=\"mainframe\">Module Usage Diagram</a></li>\r\n" + 
                        "<li><a href=\"class-inheritance-diagram.html\" target=\"mainframe\">Class Inheritance Diagram</a></li>\r\n"); 
                
                out("</ul>\r\n<ul>\r\n");
//                    "   <xsl:for-each select=\"//ned-file/comment/@content[contains(.,\'@page\') or contains(.,\'@externalpage\')]\">\r\n" + 
//                    "      <xsl:call-template name=\"do-extract-pageindex\">\r\n" + 
//                    "         <xsl:with-param name=\"comment\" select=\".\"/>\r\n" + 
//                    "      </xsl:call-template>\r\n" + 
//                    "   </xsl:for-each>\r\n" + 
            }
        });
    }
    
    private void generateTypeIndexEntry(INedTypeElement typeElement) throws Exception {
        out("<li>\r\n" + 
            "   <a href=\"" + getFileName(typeElement) + "\" target=\"mainframe\">" + typeElement.getName() + "</a>\r\n" + 
            "</li>\r\n");
    }
    
    private void generateTypeIndex(Class<?> clazz) throws Exception {
        for (INedTypeElement typeElement : typeElements)
            if (clazz.isInstance(typeElement))
                generateTypeIndexEntry(typeElement);
    }

    private void generateChannelIndex() throws Exception {
        withGeneratingTypeIndexHTMLFile("channels", ChannelElementEx.class);
    }
    
    private void generateChannelInterfaceIndex() throws Exception {
        withGeneratingTypeIndexHTMLFile("channel interfaces", ChannelInterfaceElementEx.class);
    }
    
    private void generateModuleInterfaceIndex() throws Exception {
        withGeneratingTypeIndexHTMLFile("module interfaces", ModuleInterfaceElementEx.class);
   }
    
    private void generateSimpleModuleIndex() throws Exception {
        withGeneratingTypeIndexHTMLFile("simple modules", SimpleModuleElementEx.class);
    }
    
    private void generateCompoundModuleIndex() throws Exception {
        withGeneratingTypeIndexHTMLFile("compound modules", CompoundModuleElementEx.class);
    }
    
    private void generateAllModuleIndex() throws Exception {
        withGeneratingTypeIndexHTMLFile("all modules", IModuleTypeElement.class);
    }
    
    private void generateNetworkIndex() throws Exception {
        withGeneratingNavigationHTMLFile("networks", new Runnable() {
            public void run() throws Exception {
                for (INedTypeElement typeElement : typeElements)
                    if (typeElement instanceof CompoundModuleElementEx && ((CompoundModuleElementEx)typeElement).getIsNetwork())
                        generateTypeIndexEntry(typeElement);
            }
        });
    }
    
    private void generateMessageIndex() throws Exception {
        withGeneratingNavigationHTMLFile("messages", new Runnable() {
            public void run() throws Exception {
                // TODO:
            }
        });
    }
    
    private void generateClassIndex() throws Exception {
        withGeneratingNavigationHTMLFile("classes", new Runnable() {
            public void run() throws Exception {
                // TODO:
            }
        });
    }
    
    private void generateStructIndex() throws Exception {
        withGeneratingNavigationHTMLFile("structs", new Runnable() {
            public void run() throws Exception {
                // TODO:
            }
        });
    }
    
    private void generateEnumIndex() throws Exception {
        withGeneratingNavigationHTMLFile("enums", new Runnable() {
            public void run() throws Exception {
                // TODO:
            }
        });
    }
    
    private void generateNedFilePages() throws Exception {
        for (final IFile file : resources.getNedFiles(project)) {
            withGeneratingHTMLFile(getFileName(file), new Runnable() {
                public void run() throws IOException {
                    out("<h2 class=\"comptitle\">File <i>" + file.getProjectRelativePath().toString() + "</i></h2>\r\n" + 
                        "<h3 class=\"subtitle\">Contains:</h3>\r\n" + 
                        "<ul>\r\n");
            
                    for (INedTypeElement typeElement : resources.getNedFileElement(file).getTopLevelTypeNodes())
                    {
                        out("<li>\r\n" + 
                            "   <a href=\"" + getFileName(typeElement) + "\">" + typeElement.getName() + "</a>\r\n" + 
                            "   <i> (" + typeElement.getReadableTagName() + ")</i>\r\n" + 
                            "</li>\r\n");
                    }
                    
                    out("</ul>\r\n"); 
                    generateNedSourceContent(file);
                }
            });
        }
    }
    
    private void generateNedTypePages() throws Exception {
        for (IFile file : resources.getNedFiles(project)) {
            for (final INedTypeElement typeElement : resources.getNedFileElement(file).getTopLevelTypeNodes()) {
                withGeneratingHTMLFile(getFileName(typeElement), new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">" + WordUtils.capitalize(typeElement.getReadableTagName()) + " <i>" + typeElement.getName() + "</i></h2>\r\n"); 
                        generateTypeDiagram(typeElement);
                        generateUsageDiagram(typeElement);
                        generateInheritanceDiagram(typeElement);
                        generateNedSourceContent(typeElement);
                    }
                });
            }
        }
    }

    private void genereteTypeDiagrams() throws InterruptedException, CoreException {
        final ExportImagesOfDiagramFilesOperation exportOperation = 
            new ExportImagesOfDiagramFilesOperation(new ArrayList<IFile>(resources.getNedFiles()),
                    new ImageExporterDescriptor() {
                        public ImageExporter createExporter() {
                            return new PNGImageExporter();
                        }

                        public String getDescription() {
                            return "NED Figure Provider";
                        }

                        public String getFileExtension() {
                            return "png";
                        }

                        public String getName() {
                            return "NED Figure Provider";
                        }
                
            }, neddocPath.getLocation(), true, null);
        exportOperation.setOverwriteMode(ExportImagesOfDiagramFilesOperation.OverwriteMode.ALL);
        DisplayUtils.runNowOrSyncInUIThread(new java.lang.Runnable() {
            public void run() {
                try {
                    exportOperation.run(monitor);
                }
                catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        });
        // KLUDGE: move generated images under neddocPath
        for (IFile file : resources.getNedFiles()) {
            List<INedTypeElement> typeElements = resources.getNedFileElement(file).getTopLevelTypeNodes();

            for (INedTypeElement typeElement : typeElements) {
                String fileName = file.getName().replaceAll(".ned", "");
                String imageName = typeElements.size() == 1 ? fileName : fileName + "_" + typeElement.getName();
                IFile sourceImageFile = file.getParent().getFile(new Path(imageName + ".png"));

                if (sourceImageFile.exists()) {
                    IFile destinationImageFile = neddocPath.getFile(getFileName(typeElement, "type", ".png"));
                    sourceImageFile.move(destinationImageFile.getFullPath(), true, monitor);
                }
            }
        }
    }

    private void appendDotNode(StringBuffer dot, INedTypeElement typeElement) {
        appendDotNode(dot, typeElement, false);
    }

    private void appendDotNode(StringBuffer dot, INedTypeElement typeElement, boolean highlight) {
        String name = typeElement.getName();
        dot.append(name + " ");
        
        dot.append("[URL=\"" + getFileName(typeElement) + "\",");

        String color = "#ff0000";
        if (typeElement instanceof CompoundModuleElementEx && ((CompoundModuleElementEx)typeElement).getIsNetwork())
            color = highlight ? "#9090ff" : "#d0d0ff";
        else if (typeElement instanceof CompoundModuleElementEx)
            color = highlight ? "#fff700" : "#fffcaf";
        else if (typeElement instanceof SimpleModuleElementEx)
            color = highlight ? "#fff700" : "#fffcaf";
        else if (typeElement instanceof ModuleInterfaceElementEx)
            color = highlight ? "#fff700" : "#fffcaf";
        else if (typeElement instanceof ChannelElementEx)
            color = highlight ? "#90ff90" : "#d0ffd0";
        else if (typeElement instanceof ChannelInterfaceElementEx)
            color = highlight ? "#90ff90" : "#d0ffd0";
        // TODO:
//        else if (message)
//            color = highlight ? "#fff700" : "#fffcaf";
//        else if (class)
//            color = highlight ? "#fff700" : "#fffcaf";
//        else if (struct)
//            color = highlight ? "#9090ff" : "#d0d0ff";
//        else if (enum)
//            color = highlight ? "#90ff90" : "#d0ffd0";
        dot.append("fillcolor=\"" + color + "\",");

        if (typeElement instanceof IInterfaceTypeElement)
            dot.append("style=\"filled,dashed\",");
        
        dot.append("tooltip=\"" + WordUtils.capitalize(typeElement.getReadableTagName()) + " " + name + "\"]");
        dot.append(";\n");
    }
    
    private void generateTypeDiagram(INedTypeElement typeElement) throws IOException {
        out("<img src=\"" + getFileName(typeElement, "type", ".png") + "\" ismap=\"yes\" usemap=\"#type-diagram\"/>");
        out("<map name=\"type-diagram\">" + "TODO:" + "</map>\r\n"); 
    }
    
    private void generateUsageDiagram(INedTypeElement typeElement) throws IOException {
        out("<h3 class=\"subtitle\">Usage diagram:</h3>\r\n" + 
                "<p>The following diagram shows usage relationships between modules,\r\n" + 
                "networks and channels. Unresolved types are missing from the diagram.\r\n" + 
                "Click <a href=\"full-usage-diagram.html\">here</a> to see the full picture.</p>\r\n");

        StringBuffer dot = new StringBuffer();

        dot.append("digraph opp {\r\n" + 
                   "   node [fontsize=10,fontname=helvetica,shape=box,height=.25,style=filled];\r\n");

        appendDotNode(dot, typeElement, true);

        for (INedTypeElement usedTypeElement : typeElement.getNEDTypeInfo().getLocalUsedTypes()) {
            appendDotNode(dot, usedTypeElement);
            dot.append(typeElement.getName() + " -> " + usedTypeElement.getName() + ";\n");
        }

        ArrayList<INedTypeElement> users = usersMap.get(typeElement);
        
        if  (users != null) {
            for (INedTypeElement user : users) {
                appendDotNode(dot, user);
                dot.append(user.getName() + " -> " + typeElement.getName() + ";\n");
            }
        }
        
        dot.append("}");

        String imageFileName = getFileName(typeElement, "usage", ".gif");
        String mapFileName = getFileName(typeElement, "usage", ".map");
        generateDotOuput(dot.toString(), getFile(imageFileName), "gif");
        generateDotOuput(dot.toString(), getFile(mapFileName), "cmap");

        out("<img src=\"" + imageFileName + "\" ismap=\"yes\" usemap=\"#usage-diagram\"/>");
        out("<map name=\"usage-diagram\">" + FileUtils.readTextFile(getFile(mapFileName)) + "</map>\r\n"); 
    }
    
    private void generateInheritanceDiagram(INedTypeElement typeElement) throws IOException {
        out("<h3 class=\"subtitle\">Inheritance diagram:</h3>\r\n" + 
            "<p>The following diagram shows inheritance relationships between modules,\r\n" + 
            "networks and channels. Unresolved types are missing from the diagram.\r\n" + 
            "Click <a href=\"full-usage-diagram.html\">here</a> to see the full picture.</p>\r\n");

        StringBuffer dot = new StringBuffer();

        dot.append("digraph opp {\r\n" + 
                   "   rankdir = BT;" +
        		   "   node [fontsize=10,fontname=helvetica,shape=box,height=.25,style=filled];\r\n" + 
        		   "   edge [arrowhead=empty,arrowtail=none];\r\n");
        
        appendDotNode(dot, typeElement, true);
        
        INEDTypeInfo typeInfo = typeElement.getNEDTypeInfo();
        
        // TODO: more extends for interfaces
        if (typeInfo.getFirstExtendsRef() != null) {
            INedTypeElement extendz = typeInfo.getFirstExtendsRef();
            appendDotNode(dot, extendz);
            dot.append(typeElement.getName() + " -> " + extendz.getName() + ";\n");
        }

        if (typeElement instanceof IInterfaceTypeElement) {
            ArrayList<INedTypeElement> implementors = implementorsMap.get(typeElement);
            
            if  (implementors != null) {
                for (INedTypeElement implementor : implementors) {
                    appendDotNode(dot, implementor);
                    dot.append(implementor.getName() + " -> " + typeElement.getName() + " [style=dashed];\n");
                }
            }
        }
        else {
            Set<INedTypeElement> interfaces = typeInfo.getLocalInterfaces();
    
            if  (interfaces != null) {
                for (INedTypeElement interfaze : interfaces) {
                    appendDotNode(dot, interfaze);
                    dot.append(typeElement.getName() + " -> " + interfaze.getName() + " [style=dashed];\n");
                }
            }
        }
        
        ArrayList<INedTypeElement> subtypes = subtypesMap.get(typeElement);

        if (subtypes != null)
            for (INedTypeElement subtype : subtypes) {
                appendDotNode(dot, subtype);
                dot.append(subtype.getName() + " -> " + typeElement.getName() + ";\n");
            }

        dot.append("}");
        
        String imageFileName = getFileName(typeElement, "inheritance", ".gif");
        String mapFileName = getFileName(typeElement, "inheritance", ".map");
        generateDotOuput(dot.toString(), getFile(imageFileName), "gif");
        generateDotOuput(dot.toString(), getFile(mapFileName), "cmap");

        out("<img src=\"" + imageFileName + "\" ismap=\"yes\" usemap=\"#inheritance-diagram\"/>");
        out("<map name=\"inheritance-diagram\">" + FileUtils.readTextFile(getFile(mapFileName)) + "</map>\r\n"); 
    }
    
    private void generateNedSourceContent(IFile file) throws IOException {
        generateNedSourceContent(resources.getNedFileElement(file).getNEDSource());
    }

    private void generateNedSourceContent(INedTypeElement typeElement) throws IOException {
        generateNedSourceContent(typeElement.getNEDSource());
    }

    private void generateNedSourceContent(String source) throws IOException {
        out("<h3 class=\"subtitle\">Source code:</h3>\r\n" + 
            "<pre class=\"src\">" + source + "</pre>\r\n");
    }
    
    private void withGeneratingHTMLFile(String fileName, Runnable content) throws Exception {
        setOutputFile(fileName);
        out("<html>\r\n" + 
            "   <head>\r\n" + 
            "      <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\r\n" + 
            "   </head>\r\n" + 
            "   <body>\r\n");
        content.run();
        out("   </body>\r\n" + 
            "</html>\r\n");
    }

    private boolean isDotAvailable() {
        return new File(dotExecutable).exists();
    }

    private void generateDotOuput(String dot, File outputFile, String format) throws IOException {
        Process process = Runtime.getRuntime().exec(new String[] {dotExecutable, "-T" + format, "-o", outputFile.toString()}, null, getFile("."));
        OutputStream outputStream = process.getOutputStream();
        outputStream.write(dot.getBytes());
        outputStream.close();
        
        long begin = System.currentTimeMillis();
        
        while (System.currentTimeMillis() - begin < 10000) {
            try {
                Thread.sleep(10);
            }
            catch (InterruptedException e) {
                // void
            }

            try {
                if (process.exitValue() != 0)
                    throw new RuntimeException("dot error occured when generating " + outputFile.toString());

                return;
            }
            catch (Exception e) {
                // ignore exitValue errors
            }
        }
        
        throw new RuntimeException("timeout occured when generating " + outputFile.toString());
    }
    
    private void setOutputFile(String fileName) throws FileNotFoundException {
        File file = getFile(fileName);

        if (!outputStreams.containsKey(fileName))
            outputStreams.put(file, new FileOutputStream(file));

        outputFileName = file;
    }

    private void out(String string) throws IOException {
        outputStreams.get(outputFileName).write(string.getBytes());
    }
    
    private String getFileName(IFile file) {
        return file.getName() + "-" + file.hashCode() + ".html";
    }

    private String getFileName(INedTypeElement typeElement) {
        return typeElement.getName() + "-" + typeElement.hashCode() + ".html";
    }

    private String getFileName(INedTypeElement typeElement, String discriminator, String extension) {
        return typeElement.getName() + "-" + discriminator + "-" + typeElement.hashCode() + extension;
    }

    private File getFile(String relativePath) {
        return neddocPath.getFile(new Path(relativePath)).getLocation().toFile();
    }
    
    private interface Runnable {
        public void run() throws Exception;
    }
}
