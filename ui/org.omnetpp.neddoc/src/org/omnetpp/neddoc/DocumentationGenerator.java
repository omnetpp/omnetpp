package org.omnetpp.neddoc;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CancellationException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathFactory;

import org.apache.commons.lang.WordUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.image.export.PNGImageExporter;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.Pair;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;
import org.omnetpp.ned.core.MsgResources;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.ClassElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.EnumElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.MessageElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.StructElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import de.unikassel.imageexport.exporters.ImageExporter;
import de.unikassel.imageexport.exporters.ImageExporterDescriptor;
import de.unikassel.imageexport.wizards.ExportImagesOfDiagramFilesOperation;

public class DocumentationGenerator {
    private String dotExecutablePath;
    
    private String doxyExecutablePath;
    
    private IPath documentationRootPath;
    
    private IPath rootRelativeDoxyPath;

    private IPath doxyRelativeRootPath;

    private IPath absoluteDoxyConfigFilePath;

    private IPath rootRelativeNeddocPath;

    private IPath neddocRelativeRootPath;
    
    private IProject project;
    
    private NEDResources nedResources;
    
    private MsgResources msgResources;
    
    private IProgressMonitor monitor;

    private File currentOutputFile;

    private Map<File, FileOutputStream> outputStreams = new HashMap<File, FileOutputStream>();
    
    private ArrayList<IFile> files = new ArrayList<IFile>(); 
    
    private ArrayList<ITypeElement> typeElements = new ArrayList<ITypeElement>();

    private Map<ITypeElement, ArrayList<ITypeElement>> subtypesMap = new HashMap<ITypeElement, ArrayList<ITypeElement>>();

    private Map<INedTypeElement, ArrayList<INedTypeElement>> implementorsMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();

    private Map<ITypeElement, ArrayList<ITypeElement>> usersMap = new HashMap<ITypeElement, ArrayList<ITypeElement>>();
    
    private Map<String, ITypeElement> typeNamesMap = new HashMap<String, ITypeElement>();
    
    private Map<String, String> doxyMap = new HashMap<String, String>();

    private Pattern preSpanPattern = Pattern.compile("(?s)<(pre|span) class=\"(comment|briefcomment)\">(.*?)</\\1>");
    
    private Pattern typeNamesPattern;

    private GeneratorConfiguration configuration;

    public DocumentationGenerator(IProject project) {
        this.project = project;

        nedResources = NEDResourcesPlugin.getNEDResources();
        msgResources = NEDResourcesPlugin.getMSGResources();
        
        IPreferenceStore store = org.omnetpp.ide.Activator.getDefault().getPreferenceStore();
        dotExecutablePath = store.getString(OmnetppPreferencePage.GRAPHVIZ_DOT_EXECUTABLE);
        doxyExecutablePath = store.getString(OmnetppPreferencePage.DOXYGEN_EXECUTABLE);
    }
    
    public void setDocumentationRootPath(IPath documentationRootPath) {
        this.documentationRootPath = documentationRootPath;
    }
    
    public void setRootRelativeDoxyPath(IPath rootRelativeDoxyPath) {
        this.rootRelativeDoxyPath = rootRelativeDoxyPath;
        doxyRelativeRootPath = getReversePath(rootRelativeDoxyPath);
    }
    
    public void setAbsoluteDoxyConfigFilePath(IPath absoluteDoxyConfigFilePath) {
        this.absoluteDoxyConfigFilePath = absoluteDoxyConfigFilePath;
    }
    
    public void setRootRelativeNeddocPath(IPath rootRelativeNeddocPath) {
        this.rootRelativeNeddocPath = rootRelativeNeddocPath;
        neddocRelativeRootPath = getReversePath(rootRelativeNeddocPath);
    }

    public void setConfiguration(GeneratorConfiguration configuration) {
        this.configuration = configuration;
    }
    
    public void generate() throws Exception {
        Job job = new Job("Generating neddoc...") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    DocumentationGenerator.this.monitor = monitor;

                    ensureEmptyNeddoc();
                    collectCaches();
                    generateDoxy();
                    collectDoxyMap();
                    generateCSS();
                    generateHTMLFrame();
                    generateFileList();
                    generateSelectedTopics();
                    generateIndexPages();
                    generateNedTypeFigures();
                    generateFilePages();
                    generateTypePages();
                    generateFullDiagrams();
                    
                    return Status.OK_STATUS;
                }
                catch (CancellationException e) {
                    return Status.CANCEL_STATUS;
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
                    
                    try {
                        refreshFolder(getFullDoxyPath());
                        refreshFolder(getFullNeddocPath());
                    }
                    catch (Exception e) {
                        throw new RuntimeException(e);
                    }

                    monitor.done();
                }
            }
        };
        job.setUser(true);
        job.schedule();
    }

    private void ensureEmptyNeddoc() throws CoreException {
        IPath neddocPath = getFullNeddocPath();
        File neddocFile = neddocPath.toFile();

        if (!neddocFile.exists())
            neddocFile.mkdirs();
        
        File[] files = neddocFile.listFiles();

        if (files != null) {
            try {
                monitor.beginTask("Emptying neddoc...", files.length);
                
                if (files != null) {
                    for (File file : files) {
                        monitor.subTask(file.toString());
                        file.delete();
                        monitor.worked(1);
                    }
                }
            }
            finally {
                monitor.done();
            }
        }

        refreshFolder(neddocPath);
    }

    private void refreshFolder(IPath path) throws CoreException {
        IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();

        if (path.toString().startsWith(workspaceRoot.getLocation().toString())) {
            IFolder folder = workspaceRoot.getFolder(path.removeFirstSegments(workspaceRoot.getLocation().segmentCount()));
            folder.refreshLocal(IResource.DEPTH_INFINITE, monitor);
        }
    }
    
    private void collectCaches() throws Exception {
        try {
            monitor.beginTask("Collecting data...", 6);

            collectFiles();
            collectTypes();
            collectTypeNames();
            collectSubtypesMap();
            collectImplementorsMap();
            collectUsersMap();
        }
        finally {
            monitor.done();
        }
    }

    private void collectFiles() throws CoreException {
        monitor.subTask("Collecting NED and MSG files");
        files = new ArrayList<IFile>();
        files.addAll(nedResources.getNedFiles(project));
        files.addAll(msgResources.getMsgFiles(project));
        Collections.sort(files, new Comparator<IFile>() {
            public int compare(IFile o1, IFile o2) {
                return o1.toString().compareToIgnoreCase(o2.toString());
            }
        });
        monitor.worked(1);
    }

    private void collectTypes() throws CoreException, IOException {
        monitor.subTask("Collecting types");
        for (IFile file : files) {
            if (nedResources.isNedFile(file))
                typeElements.addAll(nedResources.getNedFileElement(file).getTopLevelTypeNodes());
            else if (msgResources.isMsgFile(file))
                typeElements.addAll(msgResources.getMsgFileElement(file).getTopLevelTypeNodes());
        }

        Collections.sort(typeElements, new Comparator<ITypeElement>() {
            public int compare(ITypeElement o1, ITypeElement o2) {
                return o1.getName().compareToIgnoreCase(o2.getName());
            }
        });
        monitor.worked(1);
    }

    private void collectTypeNames() {
        monitor.subTask("Collecting type names");
        StringBuffer buffer = new StringBuffer();
        for (ITypeElement typeElement : typeElements) {
            if (typeElement instanceof INedTypeElement) {
                String qname = ((INedTypeElement)typeElement).getNEDTypeInfo().getFullyQualifiedName();
                buffer.append(qname + "|");
                typeNamesMap.put(qname, typeElement);
            }

            String name = typeElement.getName();
            if (typeNamesMap.containsKey(name))
                // multiple names, ignoring short references in comments to those types
                typeNamesMap.put(name, null);
            else {
                buffer.append(name + "|");
                typeNamesMap.put(name, typeElement);
            }
        }
        if (buffer.length() > 0)
            buffer.deleteCharAt(buffer.length() - 1);

        typeNamesPattern = Pattern.compile("\\b(" + buffer.toString() + ")\\b");
        monitor.worked(1);
    }

    private void collectSubtypesMap() {
        monitor.subTask("Collecting subtypes");
        for (ITypeElement subtype : typeElements) {
            ITypeElement supertype = subtype.getFirstExtendsRef();

            if (supertype != null) {
                ArrayList<ITypeElement> subtypes = subtypesMap.get(supertype);

                if (subtypes == null)
                    subtypes = new ArrayList<ITypeElement>();

                subtypes.add(subtype);
                subtypesMap.put(supertype, subtypes);
            }
        }
        monitor.worked(1);
    }
    
    private void collectImplementorsMap() {
        monitor.subTask("Collecting implementors");
        for (ITypeElement typeElement : typeElements) {
            if (typeElement instanceof INedTypeElement && !(typeElement instanceof IInterfaceTypeElement)) {
                INedTypeElement implementor = (INedTypeElement)typeElement;

                for (INedTypeElement interfaze : implementor.getNEDTypeInfo().getLocalInterfaces()) {
                    ArrayList<INedTypeElement> implementors = implementorsMap.get(interfaze);
                    
                    if (implementors == null)
                        implementors = new ArrayList<INedTypeElement>();
                    
                    implementors.add(implementor);
                    implementorsMap.put(interfaze, implementors);
                }
            }
        }
        monitor.worked(1);
    }
    
    private void collectUsersMap() {
        monitor.subTask("Collecting uses");
        for (ITypeElement userType : typeElements) {
            for (ITypeElement usedType : userType.getLocalUsedTypes()) {
                ArrayList<ITypeElement> users = usersMap.get(usedType);
                
                if (users == null)
                    users = new ArrayList<ITypeElement>();
                
                users.add(userType);
                usersMap.put(usedType, users);
            }
        }
        monitor.worked(1);
    }

    private void collectDoxyMap() throws Exception {
        File doxyTagsFile = getFullDoxyPath().append("doxytags.xml").toFile();

        if (doxyTagsFile.exists()) {
            FileInputStream stream = new FileInputStream(doxyTagsFile);
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            Document document = factory.newDocumentBuilder().parse(stream);
            NodeList nodes = (NodeList)XPathFactory.newInstance().newXPath().compile("//compound[@kind='class']/filename")
                .evaluate(document, XPathConstants.NODESET);
            XPathExpression classNameXPath = XPathFactory.newInstance().newXPath().compile("name/text()");

            try {
                monitor.beginTask("Collecting doxygen...", nodes.getLength());
        
                for (int i = 0; i < nodes.getLength(); i++) {
                    Node node = nodes.item(i);
                    String fileName = node.getTextContent();
                    String className = (String)classNameXPath.evaluate(node.getParentNode(), XPathConstants.STRING);
                    doxyMap.put(className, fileName);
                    monitor.worked(1);
                }
            }
            finally {
                monitor.done();
            }
        }
    }

    private void generateDoxy() throws Exception {
        if (configuration.generateDoxy) {
            try {
                monitor.beginTask("Generating doxy...", IProgressMonitor.UNKNOWN);
                File doxyConfigFile = absoluteDoxyConfigFilePath.toFile();

                if (doxyConfigFile.exists()) {
                    String content = FileUtils.readTextFile(doxyConfigFile);
                    content = content.replaceAll("(?m)^\\s*OUTPUT_DIRECTORY\\s*=.*?$", "OUTPUT_DIRECTORY=" + getFullDoxyPath().toOSString().replace("\\", "\\\\"));
                    content = content.replaceAll("(?m)^\\s*GENERATE_TAGFILE\\s*=.*?$", "GENERATE_TAGFILE=" + getFullDoxyPath().append("doxytags.xml").toOSString().replace("\\", "\\\\"));
                    File modifiedDoxyConfigFile = documentationRootPath.append("temp-doxy.cfg").toFile();
                    FileUtils.writeTextFile(modifiedDoxyConfigFile, content);

                    Process process = Runtime.getRuntime().exec(new String[] {doxyExecutablePath, modifiedDoxyConfigFile.toString()}, null, project.getLocation().toFile());
                    
                    if (process.waitFor() != 0)
                        throw new RuntimeException("doxy error occured");
                    
                    modifiedDoxyConfigFile.delete();
                }
            }
            finally {
                monitor.done();
            }
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
            final ArrayList<String> preList = new ArrayList<String>();
            comment = replaceMatches(comment, "(?s)<pre>(.*?)</pre>", new IRegexpReplacementProvider() {
                public String getReplacement(Matcher matcher) {
                    preList.add(matcher.group(0));
                    
                    return "<pre" + (preList.size() - 1) + "/>";
                }
            });
 
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
            comment = replaceMatches(comment, "<pre(\\d+)/>", new IRegexpReplacementProvider() {
                public String getReplacement(Matcher matcher) {
                    return preList.get(Integer.parseInt(matcher.group(1)));
                }
            });

            // now we can trim excess blank lines
            comment = comment.replaceAll("\n\n+", "\n");

            // extract <nohtml> sections to prevent substituting inside them;
            // also backslashed words to prevent putting hyperlinks on them
            final ArrayList<String> nohtmlList = new ArrayList<String>();
            comment = replaceMatches(comment, "(?s)<nohtml>(.*?)</nohtml>", new IRegexpReplacementProvider() {
                public String getReplacement(Matcher matcher) {
                    nohtmlList.add(matcher.group(0));
                    
                    return "<nohtml" + (nohtmlList.size() - 1) + "/>";
                }
            });
            
            // put hyperlinks on type names
            comment = replaceTypeReferences(comment);
            
            comment = replaceMatches(comment, "<nohtml(\\d+)/>", new IRegexpReplacementProvider() {
                public String getReplacement(Matcher matcher) {
                    return nohtmlList.get(Integer.parseInt(matcher.group(1)));
                }
            });

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
    
    private interface IRegexpReplacementProvider {
        public String getReplacement(Matcher matcher);
    }
    
    private String replaceMatches(String comment, String regexp, IRegexpReplacementProvider provider) {
        Matcher matcher = Pattern.compile(regexp).matcher(comment);
        StringBuffer buffer = new StringBuffer();
        
        while (matcher.find())
            matcher.appendReplacement(buffer, provider.getReplacement(matcher));

        matcher.appendTail(buffer);
        
        return buffer.toString();
    }

    private String replaceTypeReferences(String comment) {
        Matcher matcher = typeNamesPattern.matcher(comment);
        StringBuffer buffer = new StringBuffer();
        
        while (matcher.find()) {
            ITypeElement typeElement = typeNamesMap.get(matcher.group(1));

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
                ".subtitle   { font-size:12pt; margin-bottom: 3px }\r\n" + 
                ".footer     { font-size:8pt; margin-top:0px; text-align:center; color:#303030; }\r\n" + 
                ".inherited  { color:#A0A0A0 }\r\n" + 
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

                for (IFile file : files)
                    out("<li>\r\n" + 
                        "   <a href=\"" + getFileName(file) + "\" target=\"mainframe\">" + file.getProjectRelativePath() + "</a>\r\n" + 
                        "</li>\r\n");

                out("</ul>\r\n");
            }
        });
    }
    
    private void generateIndexPages() throws Exception {
        try {
            monitor.beginTask("Generating index pages...", IProgressMonitor.UNKNOWN);
    
            withGeneratingTypeIndexHTMLFile("channels", ChannelElementEx.class);
            withGeneratingTypeIndexHTMLFile("channel interfaces", ChannelInterfaceElementEx.class);
            withGeneratingTypeIndexHTMLFile("module interfaces", ModuleInterfaceElementEx.class);
            withGeneratingTypeIndexHTMLFile("simple modules", SimpleModuleElementEx.class);
            withGeneratingTypeIndexHTMLFile("compound modules", CompoundModuleElementEx.class);
            withGeneratingTypeIndexHTMLFile("all modules", IModuleTypeElement.class);
            generateNetworkIndex();
            withGeneratingTypeIndexHTMLFile("messages", MessageElementEx.class);
            withGeneratingTypeIndexHTMLFile("classes", ClassElementEx.class);
            withGeneratingTypeIndexHTMLFile("structs", StructElementEx.class);
            withGeneratingTypeIndexHTMLFile("enums", EnumElementEx.class);
        }
        finally {
            monitor.done();
        }
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
                out("<li><a href=\"overview.html\" target=\"mainframe\">Overview</a></li>\r\n");
        
                if (configuration.generateUsageDiagrams)
                    out("<li><a href=\"full-ned-usage-diagram.html\" target=\"mainframe\">Full NED Usage Diagram</a></li>\r\n"); 

                if (configuration.generateInheritanceDiagrams)
                    out("<li><a href=\"full-ned-inheritance-diagram.html\" target=\"mainframe\">Class NED Inheritance Diagram</a></li>\r\n");
                
                if (configuration.generateUsageDiagrams)
                    out("<li><a href=\"full-msg-usage-diagram.html\" target=\"mainframe\">Full MSG Usage Diagram</a></li>\r\n");
                
                if (configuration.generateInheritanceDiagrams)
                    out("<li><a href=\"full-msg-inheritance-diagram.html\" target=\"mainframe\">Class MSG Inheritance Diagram</a></li>\r\n");
                
                out("</ul>\r\n<ul>\r\n");

                for (IFile file : files) {
                    String comment = null;
                    
                    if (nedResources.isNedFile(file))
                        comment = nedResources.getNedFileElement(file).getComment();
                    else if (msgResources.isMsgFile(file))
                        comment = msgResources.getMsgFileElement(file).getComment();

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
    
    private void generateTypeIndexEntry(ITypeElement typeElement) throws Exception {
        out("<li>\r\n" + 
            "   <a href=\"" + getFileName(typeElement) + "\" target=\"mainframe\">" + typeElement.getName() + "</a>\r\n" + 
            "</li>\r\n");
    }
    
    private void generateTypeIndex(Class<?> clazz) throws Exception {
        for (ITypeElement typeElement : typeElements)
            if (clazz.isInstance(typeElement))
                generateTypeIndexEntry(typeElement);
    }
    
    private void generateNetworkIndex() throws Exception {
        withGeneratingNavigationHTMLFile("networks", new Runnable() {
            public void run() throws Exception {
                for (ITypeElement typeElement : typeElements)
                    if (typeElement instanceof CompoundModuleElementEx && ((CompoundModuleElementEx)typeElement).getIsNetwork())
                        generateTypeIndexEntry(typeElement);
            }
        });
    }
    
    private void generateFilePages() throws Exception {
        try {
            monitor.beginTask("Generating file pages...", files.size());
    
            for (final IFile file : files) {
                withGeneratingHTMLFile(getFileName(file), new Runnable() {
                    public void run() throws IOException, CoreException {
                        monitor.subTask(file.toString());

                        out("<h2 class=\"comptitle\">Ned File <i>" + file.getProjectRelativePath() + "</i></h2>\r\n" + 
                            "<h3 class=\"subtitle\">Contains:</h3>\r\n" + 
                            "<ul>\r\n");
    
                        List<? extends ITypeElement> typeElements;
                        
                        if (msgResources.isMsgFile(file))
                            typeElements = msgResources.getMsgFileElement(file).getTopLevelTypeNodes();
                        else
                            typeElements = nedResources.getNedFileElement(file).getTopLevelTypeNodes();
    
                        for (ITypeElement typeElement : typeElements)
                        {
                            out("<li>\r\n" + 
                                "   <a href=\"" + getFileName(typeElement) + "\">" + typeElement.getName() + "</a>\r\n" + 
                                "   <i> (" + typeElement.getReadableTagName() + ")</i>\r\n" + 
                                "</li>\r\n");
                        }
                        
                        out("</ul>\r\n"); 
                        generateSourceContent(file);
                        monitor.worked(1);
                    }
                });
            }
        }
        finally {
            monitor.done();
        }
    }
    
    private void generateTypePages() throws Exception {
        try {
            monitor.beginTask("Generating ned type pages...", typeElements.size());
    
            for (final ITypeElement typeElement : typeElements) {
                withGeneratingHTMLFile(getFileName(typeElement), new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">" + WordUtils.capitalize(typeElement.getReadableTagName()) + " <i>" + typeElement.getName() + "</i></h2>\r\n");
                        generateFileReference(getNedOrMsgFile(typeElement));
                        
                        if (typeElement instanceof SimpleModuleElementEx || typeElement instanceof IMsgTypeElement)
                            generateCppDefinitionReference(typeElement);
    
                        String comment = typeElement.getComment();
                        if (comment != null)
                            out(processHTMLContent("<pre class=\"comment\">" + comment + "</pre>"));
    
                        if (typeElement instanceof INedTypeElement) {
                            INedTypeElement nedTypeElement = (INedTypeElement)typeElement;
                            monitor.subTask(nedTypeElement.getReadableTagName() + ": " + nedTypeElement.getNEDTypeInfo().getFullyQualifiedName());
    
                            generateTypeDiagram(nedTypeElement);
                            generateUsageDiagram(nedTypeElement);
                            generateInheritanceDiagram(nedTypeElement);
                            generateExtendsTable(nedTypeElement);
                            generateKnownSubtypesTable(nedTypeElement);
                            generateUsedInTables(nedTypeElement);
                            generateParametersTable(nedTypeElement);
                            generatePropertiesTable(nedTypeElement);
    
                            if (typeElement instanceof IModuleTypeElement)
                                generateGatesTable((IModuleTypeElement)nedTypeElement);
    
                            if (typeElement instanceof CompoundModuleElementEx)
                                generateUnassignedParametersTable((CompoundModuleElementEx)nedTypeElement);
                        }
                        else if (typeElement instanceof IMsgTypeElement) {
                            IMsgTypeElement msgTypeElement = (IMsgTypeElement)typeElement;
                            monitor.subTask(msgTypeElement.getReadableTagName() + ": " + msgTypeElement.getName());
    
                            generateUsageDiagram(msgTypeElement);
                            generateInheritanceDiagram(msgTypeElement);
                            generateExtendsTable(msgTypeElement);
                            generateKnownSubtypesTable(msgTypeElement);
                            generateFieldsTable(msgTypeElement);
                            generatePropertiesTable(msgTypeElement);
                        }
    
                        if (configuration.generateSourceContent)
                            generateSourceContent(typeElement);
                        monitor.worked(1);
                    }
                });
            }
        }
        finally {
            monitor.done();
        }
    }

    private void generateFieldsTable(IMsgTypeElement msgTypeElement) throws IOException {
        Map<String, FieldElement> fields = msgTypeElement.getMsgTypeInfo().getFields();
        Map<String, FieldElement> localFields = msgTypeElement.getMsgTypeInfo().getLocalFields();

        if (fields.size() != 0) {
            out("<h3 class=\"subtitle\">Fields:</h3>\r\n" + 
        		"<table class=\"paramtable\">\r\n" + 
        		"   <tr>\r\n" + 
        		"      <th>Name</th>\r\n" + 
        		"      <th>Type</th>\r\n" + 
        		"      <th>Description</th>\r\n" + 
        		"   </tr>\r\n");
    
            for (String name : fields.keySet())
            {
                FieldElement field = fields.get(name);
                String trClass = localFields.containsKey(name) ? "local" : "inherited";

                out("<tr class=\"" + trClass + "\">\r\n" + 
            		"   <td width=\"150\">" + name + "</td>\r\n" + 
            		"   <td width=\"100\">\r\n" + 
            		"      <i>\r\n");
        		
                out(field.getDataType()); 
        		if (field.getIsVector())
                    out("[" + field.getVectorSize() + "]"); 

        		out("</i>\r\n" + 
            		"   </td>\r\n" + 
            		"   <td>");
        		generateTableComment(field.getComment());
        		out("</td>\r\n" + 
            		"</tr>\r\n"); 
            }
    
            out("</table>\r\n");
        }
    }

    private void generatePropertiesTable(ITypeElement typeElement) throws IOException {
        Map<String, PropertyElementEx> properties = typeElement.getProperties();
        
        if (properties.size() != 0) {
            out("<h3 class=\"subtitle\">Properties:</h3>\r\n" + 
        		"<table class=\"paramtable\">\r\n" + 
        		"   <tr>\r\n" + 
        		"      <th>Name</th>\r\n" + 
        		"      <th>Value</th>\r\n" + 
        		"      <th>Description</th>\r\n" + 
        		"   </tr>\r\n");
    
            for (String name : properties.keySet())
            {
                PropertyElementEx property = properties.get(name);
                
            	out("<tr>\r\n" + 
            		"   <td width=\"150\">" + name + "</td>\r\n" + 
            		"   <td width=\"100\"><i>");
            	generatePropertyLiteralValues(property);
            	out("</i></td>\r\n" + 
            		"   <td>");
            	generateTableComment(property.getComment());
        		out("</td>\r\n" + 
            		"</tr>\r\n"); 
            }
    
            out("</table>\r\n");
        }
    }
    
    private void generatePropertyLiteralValues(PropertyElementEx property) throws IOException {
        for (PropertyKeyElement key = property.getFirstPropertyKeyChild(); key != null; key = key.getNextPropertyKeySibling()) {
            out(key.getFirstLiteralChild().getValue());

            if (key.getNextPropertyKeySibling() != null)
                out(",");
        }
    }

    private void generateUsedInTables(INedTypeElement typeElement) throws IOException {
        if (usersMap.containsKey(typeElement)) {
            ArrayList<CompoundModuleElementEx> compoundModules = new ArrayList<CompoundModuleElementEx>();
            ArrayList<CompoundModuleElementEx> networks = new ArrayList<CompoundModuleElementEx>();
    
            for (ITypeElement userElement : usersMap.get(typeElement)) {
                if (userElement instanceof CompoundModuleElementEx) {
                    CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)userElement;

                    if (compoundModule.getIsNetwork())
                        networks.add(compoundModule);
                    else
                        compoundModules.add(compoundModule);
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
        Map<String, ParamElementEx> localParamsDeclarations = typeElement.getNEDTypeInfo().getLocalParamDeclarations();
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
                String trClass = localParamsDeclarations.containsKey(name) ? "local" : "inherited";

                out("<tr class=\"" + trClass + "\">\r\n" + 
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
        IModuleTypeElement module = (IModuleTypeElement)typeElement;

        Map<String, GateElementEx> localGateDeclarations = module.getNEDTypeInfo().getLocalGateDeclarations();
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
                String trClass = localGateDeclarations.containsKey(name) ? "local" : "inherited"; 

                out("<tr class=\"" + trClass + "\">\r\n" + 
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

    private void generateTableComment(String comment) throws IOException {
        if (comment != null)
            out(processHTMLContent("<span class=\"comment\">" + comment + "</span>"));
    }

    private void generateTypeReference(ITypeElement typeElement) throws IOException {
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

    private void generateUnresolvedTypeReference(String name) throws IOException {
        out("<tr>\r\n" + 
    		"   <td>\r\n" + 
    		name + 
    		"   </td>\r\n" + 
    		"   <td>\r\n" + 
    		"      <i>(unknown -- not in documented files)</i>\r\n" + 
    		"   </td>\r\n" + 
    		"</tr>\r\n");
    }

    private void generateCppDefinitionReference(ITypeElement typeElement) throws IOException {
        String className = typeElement.getName();

        if (typeElement instanceof INedTypeElement) {
            ParamElementEx param = ((INedTypeElement)typeElement).getParamDeclarations().get("className");
            
            if (param != null)
                className = param.getValue();
        }

        if (doxyMap.containsKey(className))
            out("<p><b>C++ definition: <a href=\"" + doxyRelativeRootPath.append(rootRelativeDoxyPath).append(doxyMap.get(className)) + "\" target=\"_top\">click here</a></b></p>\r\n");
    }

    private void generateFileReference(IFile file) throws IOException {
        out("<p><b>File: <a href=\"" + getFileName(file) + "\">" + file.getProjectRelativePath() + "</a></b></p>\r\n");
    }

    private void generateKnownSubtypesTable(ITypeElement typeElement) throws IOException {
        if (subtypesMap.containsKey(typeElement)) {
            out("<h3 class=\"subtitle\">Known subclasses:</h3>\r\n" + 
        		"<table>\r\n");
    
            for (ITypeElement subtype : subtypesMap.get(typeElement))
                generateTypeReference(subtype);
    
            out("</table>\r\n");
        }
    }

    private void generateExtendsTable(ITypeElement typeElement) throws IOException {
        if (typeElement.getFirstExtends() != null) {
            out("<h3 class=\"subtitle\">Extends:</h3>\r\n" + 
        		"<table>\r\n");

            // TODO: more extends for interfaces
            ITypeElement supertype = typeElement.getFirstExtendsRef();

            if (supertype != null)
                generateTypeReference(supertype);
            else
                generateUnresolvedTypeReference(typeElement.getFirstExtends());
    		
            out("</table>\r\n");
        }
    }

    private void generateNedTypeFigures() throws InterruptedException, CoreException {
        if (configuration.generateNedTypeFigures) {
            ArrayList<IFile> nedFiles = new ArrayList<IFile>(nedResources.getNedFiles(project));
            
            final ExportImagesOfDiagramFilesOperation exportOperation = 
                new ExportImagesOfDiagramFilesOperation(nedFiles,
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
                    
                }, getFullNeddocPath(), true, null) {
                
                @Override
                protected void refreshContainer(IContainer container, IProgressMonitor monitor) {
                    if (monitor.isCanceled())
                        throw new CancellationException();
                }
            };
            exportOperation.setOverwriteMode(ExportImagesOfDiagramFilesOperation.OverwriteMode.ALL);
            exportOperation.run(monitor);
        
            try {
                // KLUDGE: move generated images under neddocPath
                monitor.beginTask("Moving type diagrams...", nedFiles.size());
    
                for (IFile file : nedFiles) {
                    monitor.subTask(file.toString());
                    List<INedTypeElement> typeElements = nedResources.getNedFileElement(file).getTopLevelTypeNodes();
        
                    for (INedTypeElement typeElement : typeElements) {
                        String fileName = file.getName().replaceAll(".ned", "");
                        String imageName = typeElements.size() == 1 ? fileName : fileName + "_" + typeElement.getName();
                        File sourceImageFile = file.getParent().getFile(new Path(imageName + ".png")).getLocation().toFile();
        
                        if (sourceImageFile.exists()) {
                            // TODO: what if not project relative output directory is used
                            IPath destinationImagePath = getFullNeddocPath().append(getFileName(typeElement, "type", ".png"));
                            sourceImageFile.renameTo(destinationImagePath.toFile());
                        }
                    }
                    
                    monitor.worked(1);
                }
            }
            finally {
                monitor.done();
            }
        }
    }

    private void generateTypeDiagram(INedTypeElement typeElement) throws IOException {
        if (configuration.generateNedTypeFigures) {
            out("<img src=\"" + getFileName(typeElement, "type", ".png") + "\" ismap=\"yes\" usemap=\"#type-diagram\"/>");
            // TODO: out("<map name=\"type-diagram\">" +  + "</map>\r\n"); 
        }
    }
    
    private void generateFullDiagrams() throws Exception {
        try {
            monitor.beginTask("Generating full diagrams...", (configuration.generateUsageDiagrams ? 2 : 0) + (configuration.generateInheritanceDiagrams ? 2 : 0));
            final ArrayList<INedTypeElement> nedTypeElements = new ArrayList<INedTypeElement>();
            final ArrayList<IMsgTypeElement> msgTypeElements = new ArrayList<IMsgTypeElement>();
            
            for (ITypeElement typeElement : this.typeElements) {
                if (typeElement instanceof INedTypeElement)
                    nedTypeElements.add((INedTypeElement)typeElement);
                else if (typeElement instanceof IMsgTypeElement)
                    msgTypeElements.add((IMsgTypeElement)typeElement);
            }

            if (configuration.generateUsageDiagrams) {
                withGeneratingHTMLFile("full-ned-usage-diagram.html", new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">Full NED Usage Diagram</h2>\r\n" + 
                    		"<p>The following diagram shows usage relationships between simple and compound modules, module interfaces, networks, channels and channel interfaces.\r\n" + 
                    		"Unresolved types are missing from the diagram.</p>\r\n");
                        generateUsageDiagram(nedTypeElements, "full-ned-usage-diagram.png", "full-ned-usage-diagram.map");
                    }
                });
                monitor.worked(1);
            }
        
            if (configuration.generateInheritanceDiagrams) {
                withGeneratingHTMLFile("full-ned-inheritance-diagram.html", new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">Full NED Inheritance Diagram</h2>\r\n" + 
                            "<p>The following diagram shows the inheritance hierarchy between simple and compound modules, module interfaces, networks, channels and channel interfaces.\r\n" + 
                    		"Unresolved types are missing from the diagram.</p>\r\n");
                        generateInheritanceDiagram(nedTypeElements, "full-ned-inheritance-diagram.png", "full-ned-inheritance-diagram.map");
                    }
                });
                monitor.worked(1);
            }
    
            if (configuration.generateUsageDiagrams) {
                withGeneratingHTMLFile("full-msg-usage-diagram.html", new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">Full MSG Usage Diagram</h2>\r\n" + 
                            "<p>The following diagram shows usage relationships between messages, classes and structs.\r\n" + 
                            "Unresolved types are missing from the diagram.</p>\r\n");
                        generateUsageDiagram(msgTypeElements, "full-msg-usage-diagram.png", "full-msg-usage-diagram.map");
                    }
                });
                monitor.worked(1);
            }
    
            if (configuration.generateInheritanceDiagrams) {
                withGeneratingHTMLFile("full-msg-inheritance-diagram.html", new Runnable() {
                    public void run() throws Exception {
                        out("<h2 class=\"comptitle\">Full MSG Inheritance Diagram</h2>\r\n" + 
                            "<p>The following diagram shows the inheritance hierarchy between messages, classes and structs.\r\n" + 
                            "Unresolved types are missing from the diagram.</p>\r\n");
                        generateInheritanceDiagram(msgTypeElements, "full-msg-inheritance-diagram.png", "full-msg-inheritance-diagram.map");
                    }
                });
                monitor.worked(1);
            }
        }
        finally {
            monitor.done();
        }
    }

    private void generateUsageDiagram(ITypeElement typeElement) throws IOException {
        if (configuration.generateUsageDiagrams) {
            ArrayList<ITypeElement> typeElements = new ArrayList<ITypeElement>();
            typeElements.add(typeElement);
    
            String diagramType = typeElement instanceof INedTypeElement ? "ned" : "msg";
    
            out("<h3 class=\"subtitle\">Usage diagram:</h3>\r\n" + 
                "<p>The following diagram shows usage relationships between types.\r\n" + 
                "Unresolved types are missing from the diagram.\r\n" + 
                "Click <a href=\"full-" + diagramType + "-usage-diagram.html\">here</a> to see the full picture.</p>\r\n");
    
            generateUsageDiagram(typeElements, getFileName(typeElement, "usage", ".png"), getFileName(typeElement, "usage", ".map"));
        }
    }

    private void generateUsageDiagram(List<? extends ITypeElement> typeElements, String imageFileName, String cmapFileName) throws IOException {
        if (configuration.generateUsageDiagrams) {
            DotGraph dot = new DotGraph();
    
            dot.append("digraph opp {\r\n" + 
                       "   node [fontsize=10,fontname=helvetica,shape=box,height=.25,style=filled];\r\n");
    
            for (ITypeElement typeElement : typeElements) {
                dot.appendNode(typeElement, typeElements.size() == 1);
        
                for (ITypeElement usedTypeElement : typeElement.getLocalUsedTypes()) {
                    dot.appendNode(usedTypeElement);
                    dot.appendEdge(typeElement, usedTypeElement);
                }
        
                ArrayList<ITypeElement> users = usersMap.get(typeElement);
                
                if  (users != null) {
                    for (ITypeElement user : users) {
                        dot.appendNode(user);
                        dot.appendEdge(user, typeElement);
                    }
                }
            }
            
            dot.append("}");
    
            generateDotOuput(dot, getFile(imageFileName), "png");
            generateDotOuput(dot, getFile(cmapFileName), "cmap");
    
            out("<img src=\"" + imageFileName + "\" ismap=\"yes\" usemap=\"#usage-diagram\"/>");
            out("<map name=\"usage-diagram\">" + FileUtils.readTextFile(getFile(cmapFileName)) + "</map>\r\n"); 
        }
    }
    
    private void generateInheritanceDiagram(ITypeElement typeElement) throws IOException {
        if (configuration.generateInheritanceDiagrams) {
            ArrayList<ITypeElement> typeElements = new ArrayList<ITypeElement>();
            typeElements.add(typeElement);
            
            String diagramType = typeElement instanceof INedTypeElement ? "ned" : "msg";
    
            out("<h3 class=\"subtitle\">Inheritance diagram:</h3>\r\n" + 
                "<p>The following diagram shows inheritance relationships for this type.\r\n" + 
                "Unresolved types are missing from the diagram.\r\n" + 
                "Click <a href=\"full-" + diagramType + "-inheritance-diagram.html\">here</a> to see the full picture.</p>\r\n");
    
            generateInheritanceDiagram(typeElements, getFileName(typeElement, "inheritance", ".png"), getFileName(typeElement, "inheritance", ".map"));
        }
    }

    private void generateInheritanceDiagram(List<? extends ITypeElement> typeElements, String imageFileName, String cmapFileName) throws IOException {
        if (configuration.generateInheritanceDiagrams) {
            DotGraph dot = new DotGraph();
    
            dot.append("digraph opp {\r\n" + 
                       "   rankdir = BT;" +
            		   "   node [fontsize=10,fontname=helvetica,shape=box,height=.25,style=filled];\r\n" + 
            		   "   edge [arrowhead=empty,arrowtail=none];\r\n");
    
            for (ITypeElement typeElement : typeElements) {
                dot.appendNode(typeElement, typeElements.size() == 1);
                
                // TODO: what if there are more extends for interfaces
                if (typeElement.getFirstExtendsRef() != null) {
                    ITypeElement extendz = typeElement.getFirstExtendsRef();
                    dot.appendNode(extendz);
                    dot.appendEdge(typeElement, extendz);
                }
        
                if (typeElement instanceof IInterfaceTypeElement) {
                    ArrayList<INedTypeElement> implementors = implementorsMap.get(typeElement);
                    
                    if  (implementors != null) {
                        for (INedTypeElement implementor : implementors) {
                            dot.appendNode(implementor);
                            dot.appendEdge(implementor, typeElement, "style=dashed");
                        }
                    }
                }
                else if (typeElement instanceof INedTypeElement) {
                    INEDTypeInfo typeInfo = ((INedTypeElement)typeElement).getNEDTypeInfo();
                    Set<INedTypeElement> interfaces = typeInfo.getLocalInterfaces();
            
                    if  (interfaces != null) {
                        for (INedTypeElement interfaze : interfaces) {
                            dot.appendNode(interfaze);
                            dot.appendEdge(typeElement, interfaze, "style=dashed");
                        }
                    }
                }
                
                ArrayList<ITypeElement> subtypes = subtypesMap.get(typeElement);
        
                if (subtypes != null)
                    for (ITypeElement subtype : subtypes) {
                        dot.appendNode(subtype);
                        dot.appendEdge(subtype, typeElement);
                    }
            }
    
            dot.append("}");
            
            generateDotOuput(dot, getFile(imageFileName), "png");
            generateDotOuput(dot, getFile(cmapFileName), "cmap");
    
            out("<img src=\"" + imageFileName + "\" ismap=\"yes\" usemap=\"#inheritance-diagram\"/>");
            out("<map name=\"inheritance-diagram\">" + FileUtils.readTextFile(getFile(cmapFileName)) + "</map>\r\n"); 
        }
    }
    
    private void generateSourceContent(IFile file) throws IOException, CoreException {
        generateSourceContent(FileUtils.readTextFile(file.getContents()));
    }

    private void generateSourceContent(ITypeElement typeElement) throws IOException {
        generateSourceContent(typeElement.getNEDSource());
    }

    private void generateSourceContent(String source) throws IOException {
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
        File oldCurrentOutputFile = currentOutputFile;

        setCurrentOutputFile(fileName);
        out("<html>\r\n" + 
            "   <head>\r\n" + 
            "      <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\r\n" + 
            "   </head>\r\n" + 
            "   <body>\r\n");
        content.run();
        out("   </body>\r\n" + 
            "</html>\r\n");
        
        if (oldCurrentOutputFile != null)
            currentOutputFile = oldCurrentOutputFile;
    }

    private void generateDotOuput(DotGraph dot, File outputFile, String format) throws IOException {
        Process process = Runtime.getRuntime().exec(new String[] {dotExecutablePath, "-T" + format, "-o", outputFile.toString()}, null, getFile("."));
        OutputStream outputStream = process.getOutputStream();
        outputStream.write(dot.toString().getBytes());
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

    private void setCurrentOutputFile(String fileName) throws FileNotFoundException {
        File file = getFile(fileName);

        if (!outputStreams.containsKey(fileName))
            outputStreams.put(file, new FileOutputStream(file));

        currentOutputFile = file;
    }

    private void out(String string) throws IOException {
        if (monitor.isCanceled())
            throw new CancellationException();

        outputStreams.get(currentOutputFile).write(string.getBytes());
    }
    
    private IPath getFilePath(IFile file) {
        return new Path("");
    }
    
    private IPath getFilePath(ITypeElement typeElement) {
        IFile file = getNedOrMsgFile(typeElement);

        if (file != null) {
            IProject elementProject = file.getProject();

            if (!elementProject.equals(project))
                return new Path("../").append(neddocRelativeRootPath).append(elementProject.getName()).append(rootRelativeNeddocPath);
        }

        return new Path("");
    }

    private String getFileName(IFile file) {
        return getFilePath(file).append(file.getProjectRelativePath().toString().replace("/", "-")) + ".html";
    }

    private String getFileName(ITypeElement typeElement) {
        return getFileName(typeElement, null, null);
    }

    private String getFileName(ITypeElement typeElement, String discriminator, String extension) {
        String fileName = "";
        
        if (typeElement instanceof INedTypeElement)
            fileName += ((INedTypeElement)typeElement).getNEDTypeInfo().getFullyQualifiedName();
        else if (typeElement instanceof IMsgTypeElement)
            fileName += typeElement.getName();
        
        if (discriminator != null)
            fileName += "-" + discriminator;
        
        if (extension != null)
            fileName += extension;
        else
            fileName += ".html";
        
        return getFilePath(typeElement).append(fileName).toString();
    }
    
    private File getFile(String relativePath) {
        return getFullNeddocPath().append(relativePath).toFile();
    }
    
    public synchronized IFile getNedOrMsgFile(INEDElement element) {
        NedFileElementEx nedFileElement = element.getContainingNedFileElement();
        
        if (nedFileElement != null)
            return nedResources.getNedFile(nedFileElement);

        MsgFileElementEx msgFileElement = element.getContainingMsgFileElement();
        
        if (msgFileElement != null)
            return msgResources.getMsgFile(msgFileElement);
        
        return null;
    }
    
    private IPath getReversePath(IPath path) {
        String result = "";

        for (int i = 0; i < path.segmentCount(); i++)
            result = result + "../";
        
        return new Path(result);
    }

    private IPath getFullNeddocPath() {
        return documentationRootPath.append(rootRelativeNeddocPath);
    }

    private IPath getFullDoxyPath() {
        return documentationRootPath.append(rootRelativeDoxyPath);
    }

    private interface Runnable {
        public void run() throws Exception;
    }
    
    private class DotGraph {
        private StringBuffer buffer = new StringBuffer();
        
        private Set<ITypeElement> nodes = new HashSet<ITypeElement>();
        
        private Set<Pair<ITypeElement, ITypeElement>> edges = new HashSet<Pair<ITypeElement, ITypeElement>>();
        
        public void append(String text) {
            buffer.append(text);
        }
        
        public void appendNode(ITypeElement typeElement) {
            appendNode(typeElement, false);
        }

        public void appendNode(ITypeElement typeElement, boolean highlight) {
            if (!nodes.contains(typeElement)) {
                nodes.add(typeElement);

                String name = typeElement.getName();
                append(name + " ");
                
                append("[URL=\"" + getFileName(typeElement) + "\",");
    
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
                else if (typeElement instanceof MessageElementEx)
                    color = highlight ? "#fff700" : "#fffcaf";
                else if (typeElement instanceof ClassElementEx)
                    color = highlight ? "#fff700" : "#fffcaf";
                else if (typeElement instanceof StructElementEx)
                    color = highlight ? "#9090ff" : "#d0d0ff";
                else if (typeElement instanceof EnumElementEx)
                    color = highlight ? "#90ff90" : "#d0ffd0";
                append("fillcolor=\"" + color + "\",");
    
                if (typeElement instanceof IInterfaceTypeElement)
                    append("style=\"filled,dashed\",");
                
                append("tooltip=\"" + WordUtils.capitalize(typeElement.getReadableTagName()) + " " + name + "\"]");
                append(";\n");
            }
        }
        
        public void appendEdge(ITypeElement sourceTypeElement, ITypeElement targetTypeElement) {
            appendEdge(sourceTypeElement, targetTypeElement, "");
        }

        public void appendEdge(ITypeElement sourceTypeElement, ITypeElement targetTypeElement, String attributes) {
            Pair<ITypeElement, ITypeElement> pair = new Pair<ITypeElement, ITypeElement>(sourceTypeElement, targetTypeElement);

            if (!edges.contains(pair)) {
                edges.add(pair);
                append(sourceTypeElement.getName() + " -> " + targetTypeElement.getName() + " [" + attributes + "];\n");
            }
        }
        
        @Override
        public String toString() {
            return buffer.toString();
        }
    }
}
