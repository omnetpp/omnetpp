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
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathFactory;

import org.apache.commons.lang.WordUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.image.export.PNGImageExporter;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import de.unikassel.imageexport.exporters.ImageExporter;
import de.unikassel.imageexport.exporters.ImageExporterDescriptor;
import de.unikassel.imageexport.wizards.ExportImagesOfDiagramFilesOperation;

public class Neddoc {
    private String dotExecutablePath = "C:\\Workspace\\Tools\\Graphviz\\bin\\dot.exe";
    
    private IFolder neddocPath;
    
    private IFolder doxyPath;

    private IFile doxytagsFile;
    
    private IPath doxyRelativePath;

    private IProject project;
    
    private NEDResources resources;
    
    private IProgressMonitor monitor;

    private File outputFile;

    private Map<File, FileOutputStream> outputStreams = new HashMap<File, FileOutputStream>();
    
    private ArrayList<INedTypeElement> typeElements;
    
    private Map<INedTypeElement, ArrayList<INedTypeElement>> subtypesMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();

    private Map<INedTypeElement, ArrayList<INedTypeElement>> implementorsMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();

    private Map<INedTypeElement, ArrayList<INedTypeElement>> usersMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();
    
    private Map<String, INedTypeElement> typeNamesMap = new HashMap<String, INedTypeElement>();
    
    private Map<String, String> doxyMap = new HashMap<String, String>();

    private Pattern preSpanPattern = Pattern.compile("(?s)<(pre|span) class=\"(comment|briefcomment)\">(.*?)</\\1>");
    
    private Pattern typeNamesPattern;

    public Neddoc(IProject project) {
        this.project = project;

        resources = NEDResourcesPlugin.getNEDResources();
        neddocPath = project.getFolder("Documentation/neddoc");
        doxyPath = project.getFolder("Documentation/doxy");
        doxytagsFile = doxyPath.getFile("doxytags.xml");
        doxyRelativePath = new Path("../doxy");
    }
    
    public void generate() throws Exception {
        Job job = new Job("MemoryViewTab PropertyChanged") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    Neddoc.this.monitor = monitor;
                    ensureEmptyNeddoc();
                    collectCaches();
                    generateCSS();
                    generateHTMLFrame();
                    generateFileList();
                    generateSelectedTopics();
                    generateIndexPages();
                    //genereteTypeDiagrams();
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
        monitor.beginTask("Emptying neddoc...", IProgressMonitor.UNKNOWN);
        
        for (File file : new File(neddocPath.getLocation().toString()).listFiles())
            file.delete();

        if (!neddocPath.exists())
            neddocPath.create(true , true, null);

        monitor.done();
    }
    
    private void collectCaches() throws Exception {
        monitor.beginTask("Collecting ned files...", IProgressMonitor.UNKNOWN);

        collectNedTypes();
        collectTypeNames();
        collectSubtypesMap();
        collectImplementorsMap();
        collectUsersMap();
        collectDoxyMap();

        monitor.done();
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
    
    private void collectTypeNames() {
        StringBuffer buffer = new StringBuffer();
        for (INedTypeElement typeElement : typeElements) {
            String qname = typeElement.getNEDTypeInfo().getFullyQualifiedName();
            buffer.append(qname + "|");
            typeNamesMap.put(qname, typeElement);

            String name = typeElement.getNEDTypeInfo().getName();
            if (typeNamesMap.containsKey(name))
                // TODO: warning, multiple names, ignoring short references in comments to those types
                typeNamesMap.put(name, null);
            else {
                buffer.append(name + "|");
                typeNamesMap.put(name, typeElement);
            }
        }
        if (buffer.length() > 0)
            buffer.deleteCharAt(buffer.length() - 1);

        typeNamesPattern = Pattern.compile("\\b(" + buffer.toString() + ")\\b");
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

    private void collectDoxyMap() throws Exception {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        Document document = factory.newDocumentBuilder().parse(doxytagsFile.getContents());;
        NodeList nodes = (NodeList)XPathFactory.newInstance().newXPath().compile("//compound[@kind='class']/filename")
            .evaluate(document, XPathConstants.NODESET);

        for (int i = 0; i < nodes.getLength(); i++) {
            Node node = nodes.item(i);
            String fileName = node.getTextContent();
            String className = node.getParentNode().getFirstChild().getNextSibling().getTextContent();
            doxyMap.put(className, fileName);
        }
    }

    private String processHTMLContent(String html) {
        Matcher matcher = preSpanPattern.matcher(html);
        StringBuffer buffer = new StringBuffer();

        while (matcher.find())
        {
            String clazz = matcher.group(2);
            String comment = matcher.group(3);

            // add sentries to facilitate processing
            comment = "\n\n" + comment + "\n\n";

            // remove '//#' lines (those are comments to be ignored by documentation generation)
            comment = comment.replaceAll("(?s)\n *//#.*?\n", "\n");

            // remove '// ', '/// ' and '//////...' from beginning of lines
            comment = comment.replaceAll("(?s)\n[ \t]*//+ ?", "\n");     

            // extract existing <pre> sections to prevent tampering inside them
            // TODO: $comment =~ s|&lt;pre&gt;(.*?)&lt;/pre&gt;|$pre{++$ctr}=$1;"<pre$ctr>"|gse;

            // a plain '-------' line outside <pre> is replaced by a divider (<hr> tag)
            comment = comment.replaceAll("(?s)\n[ \t]*------+[ \t]*\n", "\n<hr/>\n");

            // lines outside <pre> containing whitespace only count as blank
            comment = comment.replaceAll("(?s)\n[ \t]+\n", "\n\n");

            // insert blank line (for later processing) in front of lines beginning with '- ' or '-# '
            comment = comment.replaceAll("(?s)\n( *-#? )", "\n\n$1");

            // if briefcomment, keep only the 1st paragraph
            if (clazz.equals("briefcomment"))
               comment = comment.replaceAll("(?s)(.*?[^ \t\n].*?)\n\n.*", "$1\n\n");

            // format @author, @date, @todo, @bug, @see, @since, @warning, @version
            comment = comment.replaceAll("@author\\b", "\n\n<b>Author:</b>");
            comment = comment.replaceAll("@date\\b", "\n\n<b>Date:</b>");
            comment = comment.replaceAll("@todo\\b", "\n\n<b>TODO:</b>");
            comment = comment.replaceAll("@bug\\b", "\n\n<b>BUG:</b>");
            comment = comment.replaceAll("@see\\b", "\n\n<b>See also:</b>");
            comment = comment.replaceAll("@since\\b", "\n\n<b>Since:</b>");
            comment = comment.replaceAll("@warning\\b", "\n\n<b>WARNING:</b>");
            comment = comment.replaceAll("@version\\b", "\n\n<b>Version:</b>");
            comment = comment.replaceAll("\n\n\n+", "\n\n");

            // wrap paragraphs NOT beginning with '-' into <p></p>.
            // well, we should write "paragraphs not beginning with '- ' or '-# '", but
            // how do you say that in a Perl regex?
            // (note: (?=...) and (?<=...) constructs are lookahead and lookbehind assertions,
            // see e.g. http://tlc.perlarchive.com/articles/perl/pm0001_perlretut.shtml).
            comment = comment.replaceAll("(?s)(?<=\n\n)[ \t]*([^- \t\n].*?)(?=\n\n)", "<p>$1</p>");

            // wrap paragraphs beginning with '-' into <li></li> and <ul></ul>
            // every 3 spaces increase indent level by one.
            comment = comment.replaceAll("(?s)(?<=\n\n)          *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><ul><ul><li>$1</li></ul></ul></ul></ul>");
            comment = comment.replaceAll("(?s)(?<=\n\n)       *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><ul><li>$1</li></ul></ul></ul>");
            comment = comment.replaceAll("(?s)(?<=\n\n)    *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><li>$1</li></ul></ul>");
            comment = comment.replaceAll("(?s)(?<=\n\n) *-[ \t]+(.*?)(?=\n\n)", "  <ul><li>$1</li></ul>");
            for (int i = 0; i < 4; i++) {
                comment = comment.replaceAll("(?s)</ul>[ \t\n]*<ul>", "\n\n  ");
            }

            // wrap paragraphs beginning with '-#' into <li></li> and <ol></ol>.
            // every 3 spaces increase indent level by one.
            comment = comment.replaceAll("(?s)(?<=\n\n)          *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><ol><ol><li>$1</li></ol></ol></ol></ol>");
            comment = comment.replaceAll("(?s)(?<=\n\n)       *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><ol><li>$1</li></ol></ol></ol>");
            comment = comment.replaceAll("(?s)(?<=\n\n)    *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><li>$1</li></ol></ol>");
            comment = comment.replaceAll("(?s)(?<=\n\n) *-#[ \t]+(.*?)(?=\n\n)", "  <ol><li>$1</li></ol>");
            for (int i = 0; i < 4; i++) {
                comment = comment.replaceAll("(?s)</ol>[ \t\n]*<ol>", "\n\n  ");
            }

            // now we can put back <pre> regions
            // TODO: $comment =~ s|\<pre(\d+)\>|'<pre>'.$pre{$1}.'</pre>'|gse;

            // now we can trim excess blank lines
            comment = comment.replaceAll("\n\n+", "\n");

            // extract <nohtml> sections to prevent substituting inside them;
            // also backslashed words to prevent putting hyperlinks on them
            // TODO: $comment =~ s|&lt;nohtml&gt;(.*?)&lt;/nohtml&gt;|$nohtml{++$ctr}=$1;"<nohtml$ctr>"|gsei;
            // TODO: $comment =~ s|(\\[a-z_]+)|$nohtml{++$ctr}=$1;"<nohtml$ctr>"|gsei;
            
            // put hyperlinks on type names
            comment = replaceTypeReferences(comment);
            
            // put back <nohtml> sections and backslashed words
            // TODO: $comment =~ s|\<nohtml(\d+)\>|$nohtml{$1}|gse;

            // remove backslashes; double backslashes become single ones
            comment = comment.replaceAll("\\\\(.)", "$1");

            // escape $ signs to avoid referring groups in appendReplacement
            comment = comment.replace("$", "\\$");
            
            // finished with this comment, put it back into original html file
            matcher.appendReplacement(buffer, comment);
        }

        matcher.appendTail(buffer);

        return buffer.toString();
    }

    private String replaceTypeReferences(String comment) {
        Matcher matcher = typeNamesPattern.matcher(comment);
        StringBuffer buffer = new StringBuffer();
        
        while (matcher.find()) {
            INedTypeElement typeElement = typeNamesMap.get(matcher.group(1));

            if (typeElement != null)
                matcher.appendReplacement(buffer, "<a href=\"" + getFileName(typeElement) + "\">" + typeElement.getName() + "</a>");
        }

        matcher.appendTail(buffer);
        
        return buffer.toString();
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
    
    private void generateIndexPages() throws Exception {
        monitor.beginTask("Generating index pages...", IProgressMonitor.UNKNOWN);

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

        monitor.done();
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
            private Pattern externalPagesPattern = Pattern.compile("(?m)@externalpage (.*?), *(.*)");

            private Pattern pagePattern = Pattern.compile("(?s) +(.*?), *(.*?)\n(.*)");

            public void run() throws Exception {
                out("<li><a href=\"overview.html\" target=\"mainframe\">HOME</a></li>\r\n");
        
                if (isDotAvailable())
                    out("<li><a href=\"full-usage-diagram.html\" target=\"mainframe\">Module Usage Diagram</a></li>\r\n" + 
                        "<li><a href=\"class-inheritance-diagram.html\" target=\"mainframe\">Class Inheritance Diagram</a></li>\r\n"); 
                
                out("</ul>\r\n<ul>\r\n");

                for (IFile file : resources.getNedFiles(project)) {
                    String comment = resources.getNedFileElement(file).getComment();

                    if (comment != null) {
                        Matcher matcher = externalPagesPattern.matcher(comment);
                        while (matcher.find())
                            generatePageReference(matcher.group(1), matcher.group(2));
                        comment = comment.replaceAll("(?m)@externalpage .*?\n", "");

                        if (comment.contains("@page") || comment.contains("@titlepage")) {
                            String[] pages = comment.split("@page|@titlepage");
                            for (String page : pages) {
                                matcher = pagePattern.matcher(page);
        
                                if (matcher.matches()) {
                                    generatePageReference(matcher.group(1), matcher.group(2));
                                    withGeneratingHTMLFile(matcher.group(1), 
                                        processHTMLContent("<h2>" + matcher.group(2) + "</h2>" +
                                                           "<pre class=\"comment\">" + matcher.group(3) + "</pre>"));
                                }
                                else if (page.charAt(0) == '\n')
                                    withGeneratingHTMLFile("overview.html", 
                                        processHTMLContent("<pre class=\"comment\">" + page + "</pre>" +
                                                           "<hr/>\r\n" + 
                                                           "<p>Generated by neddoc.</p>\r\n"));
                            }
                        }
                    }
                }
            }
        });
    }
    
    private void generatePageReference(String fileName, String title) throws IOException {
        out("<li>\r\n" + 
            "   <a href=\"" + fileName + "\" target=\"mainframe\">" + title + "</a>\r\n" + 
            "</li>\r\n");
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
        monitor.beginTask("Generating ned file pages...", IProgressMonitor.UNKNOWN);

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

        monitor.done();
    }
    
    private void generateNedTypePages() throws Exception {
        monitor.beginTask("Generating ned type pages...", IProgressMonitor.UNKNOWN);

        for (final IFile file : resources.getNedFiles(project)) {
            for (final INedTypeElement typeElement : resources.getNedFileElement(file).getTopLevelTypeNodes()) {
                withGeneratingHTMLFile(getFileName(typeElement), new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">" + WordUtils.capitalize(typeElement.getReadableTagName()) + " <i>" + typeElement.getName() + "</i></h2>\r\n");
                        generateFileReference(file);
                        
                        if (typeElement instanceof SimpleModuleElementEx)
                            generateCppDefinitionReference(typeElement);

                        String comment = typeElement.getComment();
                        if (comment != null)
                            out(processHTMLContent("<pre class=\"comment\">" + comment + "</pre>"));

                        generateTypeDiagram(typeElement);
                        generateUsageDiagram(typeElement);
                        generateInheritanceDiagram(typeElement);
                        generateUsedInTables(typeElement);
                        generateParametersTable(typeElement);
                        generateGatesTable(typeElement);
                        generateUnassignedParametersTable(typeElement);
                        generateNedSourceContent(typeElement);
                    }
                });
            }
        }
        
        monitor.done();
    }

    private void generateUsedInTables(INedTypeElement typeElement) throws IOException {
        if (usersMap.containsKey(typeElement)) {
            ArrayList<INedTypeElement> compoundModules = new ArrayList<INedTypeElement>();
            ArrayList<INedTypeElement> networks = new ArrayList<INedTypeElement>();
    
            for (INedTypeElement userElement : usersMap.get(typeElement)) {
                if (userElement instanceof CompoundModuleElementEx) {
                    if (((CompoundModuleElementEx)userElement).getIsNetwork())
                        networks.add(userElement);
                    else
                        compoundModules.add(userElement);
                }
            }
    
            if (compoundModules.size() != 0) {
                out("<h3 class=\"subtitle\">Used in compound modules:</h3>\r\n" + 
            		"<p>If a module type shows up more than once, that means it has been defined in more than one NED file.</p>\r\n" + 
            		"<table>\r\n"); 
                for (INedTypeElement userElement : compoundModules)
                    generateTypeReference(userElement);
        		out("</table>\r\n");
            }

            if (networks.size() != 0) {
                out("<h3 class=\"subtitle\">Networks:</h3>\r\n" + 
            		"<table>\r\n");
                for (INedTypeElement userElement : networks)
                    generateTypeReference(userElement);
        		out("</table>\r\n");
            }
        }
    }
    
    private void generateParametersTable(INedTypeElement typeElement) throws IOException {
        Map<String, ParamElementEx> paramsDeclarations = typeElement.getParamDeclarations();
        Map<String, ParamElementEx> paramsAssignments = typeElement.getParamAssignments();

        if (!paramsDeclarations.isEmpty()) {
            out("<h3 class=\"subtitle\">Parameters:</h3>\r\n" + 
        		"<table class=\"paramtable\">\r\n" + 
        		"   <tr>\r\n" + 
        		"      <th>Name</th>\r\n" + 
        		"      <th>Type</th>\r\n" + 
                "      <th>Default value</th>\r\n" + 
        		"      <th>Description</th>\r\n" + 
        		"   </tr>\r\n");
            
            for (String name : paramsDeclarations.keySet()) {
                ParamElementEx paramDeclaration = paramsDeclarations.get(name);
                ParamElementEx paramAssignment = paramsAssignments.get(name);

                out("<tr>\r\n" + 
            		"   <td width=\"150\">" + name + "</td>\r\n" + 
            		"   <td width=\"100\">\r\n" + 
            		"      <i>" + getParamTypeAsString(paramDeclaration) + "</i>\r\n" + 
            		"   </td>\r\n" + 
            		"   <td width=\"120\">" + (paramAssignment == null ? "" : paramAssignment.getValue()) + "</td>" +
                    "   <td>");
                generateTableComment(paramDeclaration.getComment());  
        		out("</td>\r\n" + 
            		"</tr>\r\n");
            }

            out("</table>\r\n");
        }
    }

    private void generateUnassignedParametersTable(INedTypeElement typeElement) throws IOException {
        if (typeElement instanceof CompoundModuleElementEx) {
            ArrayList<ArrayList<Object>> params = new ArrayList<ArrayList<Object>>();
            collectUnassignedParameters(null, typeElement.getNEDTypeInfo().getSubmodules(), params);

            if (params.size() != 0) {
                out("<h3 class=\"subtitle\">Unassigned submodule parameters:</h3>\r\n" + 
                    "<table class=\"paramtable\">\r\n" + 
                    "   <tr>\r\n" + 
                    "      <th>Name</th>\r\n" + 
                    "      <th>Type</th>\r\n" + 
                    "      <th>Default value</th>\r\n" + 
                    "      <th>Description</th>\r\n" + 
                    "   </tr>\r\n");
                for (ArrayList<Object> tuple : params) {
                    ParamElementEx paramDeclaration = (ParamElementEx)tuple.get(1);
                    ParamElementEx paramAssignment = (ParamElementEx)tuple.get(2);

                    out("<tr>\r\n" + 
                            "   <td>" + (String)tuple.get(0) + "</td>\r\n" + 
                            "   <td width=\"100\">\r\n" + 
                            "      <i>" + getParamTypeAsString(paramDeclaration) + "</i>\r\n" + 
                            "   </td>\r\n" + 
                            "   <td width=\"120\">" + (paramAssignment == null ? "" : paramAssignment.getValue()) + "</td>\r\n" + 
                            "   <td>");
                        generateTableComment(paramDeclaration.getComment());
                        out("   </td>\r\n" +
                            "</tr>\r\n");
                }
                out("</table>\r\n");
            }
        }
    }

    private void collectUnassignedParameters(String prefix, Map<String, SubmoduleElementEx> typeElementMap, ArrayList<ArrayList<Object>> params) throws IOException {
        for (SubmoduleElementEx submodule : typeElementMap.values()) {
            INedTypeElement typeElement = submodule.getEffectiveTypeRef();

            if (typeElement != null) {
                String newPrefix = (prefix == null ? "" : prefix + ".") + "<a href=\"" + getFileName(typeElement) + "\">" + submodule.getName() + "</a>";
    
                if (typeElement instanceof CompoundModuleElementEx)
                    collectUnassignedParameters(newPrefix, typeElement.getNEDTypeInfo().getSubmodules(), params);
                else {
                    INEDTypeInfo typeInfo = typeElement.getNEDTypeInfo();
                    Map<String, ParamElementEx> declarations = typeInfo.getParamDeclarations();
                    Map<String, ParamElementEx> assigments = submodule.getParamAssignments();
                    
                    for (String name : declarations.keySet()) {
                        ParamElementEx paramDeclaration = declarations.get(name);
                        ParamElementEx paramAssignment = assigments.get(name);

                        if (paramAssignment == null || paramAssignment.getIsDefault()) {
                            ArrayList<Object> tuple = new ArrayList<Object>();
                            tuple.add(newPrefix + "." + name);
                            tuple.add(paramDeclaration);
                            tuple.add(paramAssignment);
                            params.add(tuple);
                        }
                    }
                }
            }
        }
    }

    private void generateGatesTable(INedTypeElement typeElement) throws IOException {
        if (typeElement instanceof IModuleTypeElement) {
            IModuleTypeElement module = (IModuleTypeElement)typeElement;
    
            Map<String, GateElementEx> gateDeclarations = module.getGateDeclarations();
            Map<String, GateElementEx> gatesSizes = module.getGateSizes();
    
            if (!gateDeclarations.isEmpty()) {
                out("<h3 class=\"subtitle\">Gates:</h3>\r\n" + 
                    "<table class=\"paramtable\">\r\n" + 
                    "   <tr>\r\n" + 
                    "      <th>Name</th>\r\n" + 
                    "      <th>Direction</th>\r\n" + 
                    "      <th>Size</th>\r\n" + 
                    "      <th>Description</th>\r\n" + 
                    "   </tr>\r\n");
        
                for (String name : gateDeclarations.keySet()) {
                    GateElementEx gateDeclaration = gateDeclarations.get(name);
                    GateElementEx gateSize = gatesSizes.get(name);
    
                    out("<tr>\r\n" + 
                        "   <td width=\"150\">" + name + (gateDeclaration.getIsVector() ? " [ ]" : "") + "</xsl:if></td>\r\n" + 
                        "   <td width=\"100\"><i>" + gateDeclaration.getAttribute(GateElementEx.ATT_TYPE) + "</i></td>\r\n" + 
                        "   <td width=\"50\">" + (gateSize != null && gateSize.getIsVector() ? gateSize.getVectorSize() : "") + "</td>" +
                        "   <td>");
                    generateTableComment(gateDeclaration.getComment());  
                    out("</td>\r\n" +
                        "</tr>\r\n");
                }
                
                out("</table>\r\n");
            }
        }
    }

    private void generateTableComment(String comment) throws IOException {
        if (comment != null)
            out(processHTMLContent("<span class=\"comment\">" + comment + "</span>"));
    }

    private void generateTypeReference(INedTypeElement typeElement) throws IOException {
        out("<tr>\r\n" + 
            "   <td>\r\n" + 
            "      <a href=\"" + getFileName(typeElement) + "\">" + typeElement.getName() + "</a>\r\n" + 
            "   </td>\r\n" + 
            "   <td>\r\n"); 

        String comment = typeElement.getComment();
        if (comment != null)
            out(processHTMLContent("<span class=\"briefcomment\">" + comment + "</span>")); 
        else
            out("<i>(no description)</i>\r\n");
        
        out("   </td>\r\n" + 
            "</tr>\r\n");
    }

    private void generateCppDefinitionReference(INedTypeElement typeElement) throws IOException {
        ParamElementEx param = typeElement.getParamDeclarations().get("className");
        String className = param != null ? param.getValue() : typeElement.getName();

        out("<p><b>C++ definition: <a href=\"" + doxyRelativePath.toString() + "/" + doxyMap.get(className) + "\" target=\"_top\">click here</a></b></p>\r\n");
    }

    private void generateFileReference(IFile file) throws IOException {
        out("<p><b>File: <a href=\"" + getFileName(file) + "\">" + file.getProjectRelativePath().toString() + "</a></b></p>\r\n");
    }

    private void genereteTypeDiagrams() throws InterruptedException, CoreException {
        monitor.beginTask("Generating type diagrams...", IProgressMonitor.UNKNOWN);

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
        exportOperation.run(monitor);

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
        
        monitor.done();
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
        // TODO: out("<map name=\"type-diagram\">" +  + "</map>\r\n"); 
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
        
        // TODO: what if there are more extends for interfaces
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

    private void withGeneratingHTMLFile(String fileName, final String content) throws Exception {
        withGeneratingHTMLFile(fileName, new Runnable() {
            public void run() throws Exception {
                out(content);
            }
        });
    }
    
    private void withGeneratingHTMLFile(String fileName, Runnable content) throws Exception {
        File oldOutputFile = outputFile;

        setOutputFile(fileName);
        out("<html>\r\n" + 
            "   <head>\r\n" + 
            "      <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\r\n" + 
            "   </head>\r\n" + 
            "   <body>\r\n");
        content.run();
        out("   </body>\r\n" + 
            "</html>\r\n");
        
        if (oldOutputFile != null)
            outputFile = oldOutputFile;
    }

    private boolean isDotAvailable() {
        return new File(dotExecutablePath).exists();
    }

    private void generateDotOuput(String dot, File outputFile, String format) throws IOException {
        Process process = Runtime.getRuntime().exec(new String[] {dotExecutablePath, "-T" + format, "-o", outputFile.toString()}, null, getFile("."));
        OutputStream outputStream = process.getOutputStream();
        outputStream.write(dot.getBytes());
        outputStream.close();
        
        long begin = System.currentTimeMillis();
        
        while (System.currentTimeMillis() - begin < 10000) {
            try {
                Thread.sleep(10);
            }
            catch (InterruptedException e) {
                // void, ignore
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
    
    private String getParamTypeAsString(ParamElementEx param) {
        String type = param.getAttribute(GateElementEx.ATT_TYPE);

        if (type == null)
            return "numeric";
        else
            return type;
    }

    private void setOutputFile(String fileName) throws FileNotFoundException {
        File file = getFile(fileName);

        if (!outputStreams.containsKey(fileName))
            outputStreams.put(file, new FileOutputStream(file));

        outputFile = file;
    }

    private void out(String string) throws IOException {
        outputStreams.get(outputFile).write(string.getBytes());
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
