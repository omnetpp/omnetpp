/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.neddoc;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.TreeMap;
import java.util.concurrent.CancellationException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathFactory;

import org.apache.commons.lang3.StringEscapeUtils;
import org.apache.commons.lang3.text.WordUtils;
import org.eclipse.core.internal.preferences.Base64;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.ITokenScanner;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.editor.text.Keywords;
import org.omnetpp.common.editor.text.NedCommentFormatter;
import org.omnetpp.common.editor.text.NedCommentFormatter.INeddocProcessor;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.ProcessUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.StringUtils.IRegexpReplacementProvider;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.msg.editor.highlight.MsgCodeColorizerScanner;
import org.omnetpp.msg.editor.highlight.MsgDocColorizerScanner;
import org.omnetpp.msg.editor.highlight.MsgPrivateDocColorizerScanner;
import org.omnetpp.msg.editor.highlight.MsgSyntaxHighlightPartitionScanner;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.MsgResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.export.ExportDiagramFilesOperation;
import org.omnetpp.ned.editor.export.NedFigureProvider;
import org.omnetpp.ned.editor.export.PNGDiagramExporter;
import org.omnetpp.ned.editor.graph.figures.CompoundModuleTypeFigure;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.NedEditPart;
import org.omnetpp.ned.editor.graph.parts.NedTypeEditPart;
import org.omnetpp.ned.editor.graph.parts.SubmoduleEditPart;
import org.omnetpp.ned.editor.text.highlight.NedCodeColorizerScanner;
import org.omnetpp.ned.editor.text.highlight.NedDocColorizerScanner;
import org.omnetpp.ned.editor.text.highlight.NedPrivateDocColorizerScanner;
import org.omnetpp.ned.editor.text.highlight.NedSyntaxHighlightPartitionScanner;
import org.omnetpp.ned.model.INedElement;
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
import org.omnetpp.ned.model.ex.PacketElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.StructElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.IMsgTypeElement;
import org.omnetpp.ned.model.interfaces.INedFileElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.ITypeElement;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.LiteralElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;
import org.omnetpp.neddoc.properties.DocumentationGeneratorPropertyPage;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This class generates documentation for a single OMNeT++ project. It calls
 * doxygen if requested, and generates documentation from NED and MSG files
 * found in the project. The result is a bunch of HTML and PNG files.
 *
 * The tool relies on the doxygen and graphviz dot executables which are
 * invoked through the runtime's exec facility. The documentation generation
 * takes place in a background job (thread) and a progress monitor is used to present
 * its state to the user. The whole process might take several minutes for
 * large projects such as the INET framework.
 *
 * The generated documentation consists of the following things:
 *  - doxygen documentation (several different kind of pages)
 *  - one page for each NED and MSG file showing its content and a list of declared types
 *  - one page for each type defined in NED and MSG files showing the type's figure,
 *    an inheritance and a usage diagram and various other tables.
 *  - several index pages each listing the declared types of a kind such as
 *    simple modules, compound modules, channels, etc.
 *  - other pages written by the user and extracted from NED and MSG file comments
 *  - separate full inheritance and usage diagrams for NED and MSG files
 *
 * @author levy
 */
@SuppressWarnings("restriction")
public class DocumentationGenerator {
    // Watermark images. We don't load them from the jar file -- that would
    // make them a little too easy to get rid of.  --Andras
    private static final Image CC16_IMAGE = createImage(
            "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAA" +
            "AARnQU1BAACxjwv8YQUAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAA" +
            "OpgAABdwnLpRPAAAAAlwSFlzAAAOwwAADsMBx2+oZAAAABh0RVh0U29mdHdhcmUA" +
            "UGFpbnQuTkVUIHYzLjM2qefiJQAAAVRJREFUOE+1ks8rRFEUx8ePaMSKEBmlsbKy" +
            "YGHB3oKY/4CaPRuUlY0ov1JsbITmD6CwmIxomlLTbBQlMsjGjo2ifL46T7fpvXkr" +
            "tz6de8/5vvPOvedEIv+wKsk5AjtwZmg/CoqVXZ1Ec/ABBzALM7AP73AF8aAM7QQe" +
            "4QRafETN+A7hFbr8kujDDNSUqbGa2DFcQIWr6+fwBd3mbLJSPZGqi1msA/sJg26C" +
            "BQ5pc4xj36AAezBtZd9i501zil10E+jBNsxxjp2DKmiEFxiDeqgzzTo25SbY5bBl" +
            "Dt1x1cTD2DtIQhsMmGbTqvvLoVbl7dSHvYFnmAL1vwj3kDDNJda7zq9LvdUjDrll" +
            "Bex78X9DT2l8G8eDlRqUR925Li3fE0fZaHRVrkbZ7bP2eg9dIwsNQX+oJbAC6vMT" +
            "HIGmT0nlWwP9KHS1opiEZViCCfAb79BEoYIfHho+6oefKLwAAAAASUVORK5CYII=");
    private static final Image CC20_IMAGE = createImage(
            "iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAAAXNSR0IArs4c6QAA" +
            "AARnQU1BAACxjwv8YQUAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAA" +
            "OpgAABdwnLpRPAAAAAlwSFlzAAAOwwAADsMBx2+oZAAAABh0RVh0U29mdHdhcmUA" +
            "UGFpbnQuTkVUIHYzLjM2qefiJQAAAcRJREFUOE+9lEso5XEUx5E8IjXIBjGxmZFx" +
            "JY+yMhvZ6E5iSmMzZeOxGGSpPHayp2xs2GClmUlkJaVEYkGjbGi8kijk+fnWubdf" +
            "13XvRTn16fzu93fO+T3u+f2jot7ZslnvO3QYDXhpL7YiMv7CPTwEIG0WSiKt+pPA" +
            "aziBfiiDNEiFUuiFY7iB9nBFG21XC/iMEMHpdgLtvuW5uFwmLmAJEsOtzHwczNlp" +
            "PgeLH0W8grwIivlCMm0TE4E5CQjnMGYT0XaUGbwWygfF6P5+wxDoTmXDcAlJbtFy" +
            "fug+6k3UvdxBH6joiCWe4rtgGVot1mu5lW7Bbyb6WuEPv1csIMb8Hl67lfk0jYst" +
            "t84tWGtihYnT+B1LVMtkwT+YsvmP+BQbq5V0Ou3UbwUmNptSjVefLcI2jINei5pa" +
            "uz+Abov9Ybket6COoCPNO+IXxr9A9xprehW+E746cZOMj5wY/1SPrVTjrhRmrFd0" +
            "C4PB4pIRdU+H8CmCojnE7MI++FroSVohit6w0FdG/Rho0rzwH85ALRfS9Iw2Qf/c" +
            "BgxAk6GPxbrNbeH1VYrI4olqgzVwP2Ear9qcXs6rTPfjsd18eFWFtyQ9Ar+WYlGW" +
            "Qvp8AAAAAElFTkSuQmCC");
    private static final boolean APPLY_CC = !IConstants.IS_COMMERCIAL;

    private static final Pattern pageSeparatorPattern = Pattern.compile("(?m)^//[ \t]*(@page|@titlepage)(.*?)$");

    // matches a @titlepage
    private static final Pattern titlePagePattern = Pattern.compile("(?m)^//[ \t]*@titlepage(.*?)$");

    // matches @externalpage with name and URL
    private static final Pattern externalPagePattern = Pattern.compile("(?m)^//[ \t]*@externalpage +([^,\n]+),? *(.*?)$");

    // matches @page with name and URL
    private static final Pattern pagePattern = Pattern.compile("(?m)^//[ \t]*@page +([^,\n]+),? *(.*?)$");

    // matches @include and extracts filename
    private final static Pattern includePattern = Pattern.compile("(?m)^//[ \t]*@include[ \t]+(.*?)$");

    // configuration flags
    protected boolean generateNedTypeFigures = true;
    protected boolean generatePerTypeUsageDiagrams = true;
    protected boolean generatePerTypeInheritanceDiagrams = true;
    protected boolean generateFullUsageDiagrams = false;
    protected boolean generateFullInheritanceDiagrams = false;
    protected boolean generateNedSourceListings = true;
    protected boolean generateExplicitLinksOnly = false;  // true: tilde notation; false: autolinking
    protected boolean generateDoxy = true;
    protected boolean generateCppSourceListings = false;

    // path vars
    protected String dotExecutablePath;
    protected String doxyExecutablePath;
    protected IPath documentationRootPath;
    protected IPath rootRelativeDoxyPath;
    protected IPath doxyRelativeRootPath;
    protected IPath absoluteDoxyConfigFilePath; //FIXME should be IFile! because error message about missing doxyfile should show workspace path not filesystem path!
    protected IPath rootRelativeNeddocPath;
    protected IPath neddocRelativeRootPath;
    protected IPath customCssPath;
    protected IProject project;
    protected String template = "doxy";
    protected INedResources nedResources;
    protected MsgResources msgResources;
    protected IProgressMonitor monitor;

    protected FileOutputStream currentOutputStream;

    protected ArrayList<IFile> files = new ArrayList<IFile>();
    protected ArrayList<ITypeElement> typeElements = new ArrayList<ITypeElement>();
    protected Map<ITypeElement, ArrayList<ITypeElement>> subtypesMap = new HashMap<ITypeElement, ArrayList<ITypeElement>>();
    protected Map<INedTypeElement, ArrayList<INedTypeElement>> implementorsMap = new HashMap<INedTypeElement, ArrayList<INedTypeElement>>();
    protected Map<ITypeElement, ArrayList<ITypeElement>> usersMap = new HashMap<ITypeElement, ArrayList<ITypeElement>>();
    protected Map<String, List<ITypeElement>> typeNamesMap = new HashMap<String, List<ITypeElement>>();
    protected Map<String, String> doxyMap = new HashMap<String, String>();
    protected Map<INedElement, String> commentCache = new HashMap<INedElement, String>();
    protected Map<File, String> includedFileCache = new HashMap<File,String>();
    protected Pattern possibleTypeReferencesPattern;

    protected ArrayList<String> packageNames;
    protected int level0Index = 0;
    protected TreeMap<String, ArrayList> navigationItemIndex;

    static Image createImage(String base64Data) {
        return new Image(Display.getDefault(), new ImageData(new ByteArrayInputStream(Base64.decode(base64Data.getBytes()))));
    }

    public DocumentationGenerator(IProject project) {
        this.project = project;

        nedResources = NedResourcesPlugin.getNedResources();
        msgResources = NedResourcesPlugin.getMsgResources();

        IPreferenceStore store = CommonPlugin.getConfigurationPreferenceStore();
        dotExecutablePath = ProcessUtils.lookupExecutable(store.getString(IConstants.PREF_GRAPHVIZ_DOT_EXECUTABLE));
        doxyExecutablePath = ProcessUtils.lookupExecutable(store.getString(IConstants.PREF_DOXYGEN_EXECUTABLE));
    }

    public void setGenerateNedTypeFigures(boolean generateNedTypeFigures) {
        this.generateNedTypeFigures = generateNedTypeFigures;
    }

    public void setGeneratePerTypeInheritanceDiagrams(boolean generatePerTypeInheritanceDiagrams) {
        this.generatePerTypeInheritanceDiagrams = generatePerTypeInheritanceDiagrams;
    }

    public void setGeneratePerTypeUsageDiagrams(boolean generatePerTypeUsageDiagrams) {
        this.generatePerTypeUsageDiagrams = generatePerTypeUsageDiagrams;
    }

    public void setGenerateFullUsageDiagrams(boolean generateFullUsageDiagrams) {
        this.generateFullUsageDiagrams = generateFullUsageDiagrams;
    }

    public void setGenerateFullInheritanceDiagrams(boolean generateFullInheritanceDiagrams) {
        this.generateFullInheritanceDiagrams = generateFullInheritanceDiagrams;
    }

    public void setGenerateNedSourceListings(boolean generateNedSourceListings) {
        this.generateNedSourceListings = generateNedSourceListings;
    }

    public void setGenerateExplicitLinksOnly(boolean generateExplicitLinksOnly) {
        this.generateExplicitLinksOnly = generateExplicitLinksOnly;
    }

    public void setGenerateDoxy(boolean generateDoxy) {
        this.generateDoxy = generateDoxy;
    }

    public void setGenerateCppSourceListings(boolean generateCppSourceListings) {
        this.generateCppSourceListings = generateCppSourceListings;
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

    public void setCustomCssPath(IPath customCssPath) {
        this.customCssPath = customCssPath;
    }

    public void generate() throws Exception {
        WorkspaceJob job = new WorkspaceJob("Generating NED Documentation...") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) {
                try {
                    DocumentationGenerator.this.monitor = monitor;

                    ensureEmptyNeddoc();
                    collectCaches();
                    generateDoxy();
                    collectDoxyMap();
                    copyCSS();
                    copyJavaScript();
                    generateRedirectPage();
                    generateNavigationTree();
                    generateNedTypeFigures();
                    generatePackagePages();
                    generateFilePages();
                    generateTypePages();
                    generateFullDiagrams();
                    generateNedTagFile();
                    generateMsgTagFile();

                    return Status.OK_STATUS;
                }
                catch (CancellationException e) {
                    return Status.CANCEL_STATUS;
                }
                catch (NeddocException e) {
                    return NeddocPlugin.getErrorStatus("Error during generating NED documentation", e);
                }
                catch (final IllegalStateException e) {
                    if (e.getMessage() != null) {
                        DisplayUtils.runNowOrSyncInUIThread(() -> {
                                MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error during generating NED documentation", e.getMessage());
                            }
                        );
                    }

                    return Status.CANCEL_STATUS;
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
                finally {
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

    protected void ensureEmptyNeddoc() throws CoreException {
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

    protected void refreshFolder(IPath path) throws CoreException {
        IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();

        if (path.toString().startsWith(workspaceRoot.getLocation().toString())) {
            IFolder folder = workspaceRoot.getFolder(path.removeFirstSegments(workspaceRoot.getLocation().segmentCount()));
            folder.refreshLocal(IResource.DEPTH_INFINITE, monitor);
        }
    }

    protected void collectCaches() throws Exception {
        try {
            monitor.beginTask("Collecting data...", 6);

            navigationItemIndex = new TreeMap<String, ArrayList>();
            level0Index = 0;
            collectFiles();
            collectTypes();
            collectPackageNames();
            collectTypeNames();
            collectSubtypesMap();
            collectImplementorsMap();
            collectUsersMap();
        }
        finally {
            monitor.done();
        }
    }

    protected void collectFiles() throws CoreException {
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

    protected void collectTypes() throws CoreException, IOException {
        monitor.subTask("Collecting types");
        for (INedTypeInfo nedTypeInfo : nedResources.getBuiltInDeclarations())
            typeElements.add(nedTypeInfo.getNedElement());
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

    protected String getPackageName(INedTypeElement typeElement) {
        String packageName = typeElement.getNedTypeInfo().getPackageName();

        if (packageName != null)
            return packageName;
        else
            return "default";
    }

    protected void collectPackageNames() {
        monitor.subTask("Collecting package names");
        Set<String> packageNames = new HashSet<String>();

        for (ITypeElement typeElement : typeElements)
            if (typeElement instanceof INedTypeElement)
                packageNames.add(getPackageName((INedTypeElement)typeElement));

        this.packageNames = (ArrayList<String>)org.omnetpp.common.util.CollectionUtils.toSorted(new ArrayList<String>(packageNames));
    }

    protected void collectTypeNames() {
        monitor.subTask("Collecting type names");
        StringBuffer buffer = new StringBuffer();
        for (ITypeElement typeElement : typeElements) {
            String name = typeElement.getName();

            if (typeElement instanceof INedTypeElement) {
                String qname = ((INedTypeElement)typeElement).getNedTypeInfo().getFullyQualifiedName();
                if (!qname.equals(name)) {
                    buffer.append(qname + "|");
                    typeNamesMap.put(qname, Arrays.asList(new ITypeElement[] {typeElement} ));
                }
            }

            if (!typeNamesMap.containsKey(name)) {
                buffer.append(name + "|");
                typeNamesMap.put(name, new ArrayList<ITypeElement>());
            }
            typeNamesMap.get(name).add(typeElement);
        }
        if (buffer.length() > 0)
            buffer.deleteCharAt(buffer.length() - 1);  // remove last "|"

        String typeNamesPattern = buffer.toString().replace(".", "\\.");
        if (generateExplicitLinksOnly) {
            // tilde syntax; we match any name prefixed with a tilde (or more tildes);
            // a double tilde means one literal tilde, so we'll have to count them when we do the replacement
            possibleTypeReferencesPattern = Pattern.compile("(~+)(" + Keywords.NED_IDENT_REGEX + "(\\." + Keywords.NED_IDENT_REGEX + ")*)\\b");
        } else {
            // autolinking: match recognized names, optionally prefixed with a backslash (or more backslashes);
            // a double backslash means one literal backslash, so we'll have to count them when we do the replacement
            possibleTypeReferencesPattern = Pattern.compile("(\\\\*)\\b(" + typeNamesPattern + ")\\b");
        }
        monitor.worked(1);
    }

    protected void collectSubtypesMap() {
        monitor.subTask("Collecting subtypes");
        for (ITypeElement subtype : typeElements) {
            if (subtype instanceof IInterfaceTypeElement)
                for (INedTypeElement supertype : ((IInterfaceTypeElement)subtype).getNedTypeInfo().getLocalInterfaces())
                    addSubtypeMapping(subtype, supertype);
            else {
                ITypeElement supertype = subtype.getSuperType();
                if (supertype != null) {
                    addSubtypeMapping(subtype, supertype);
                }
            }
        }
        monitor.worked(1);
    }

    private void addSubtypeMapping(ITypeElement subtype, ITypeElement supertype) {
        ArrayList<ITypeElement> subtypes = subtypesMap.get(supertype);

        if (subtypes == null)
            subtypes = new ArrayList<ITypeElement>();

        subtypes.add(subtype);
        subtypesMap.put(supertype, subtypes);
    }

    protected void collectImplementorsMap() {
        monitor.subTask("Collecting implementors");
        for (ITypeElement typeElement : typeElements) {
            if (typeElement instanceof INedTypeElement && !(typeElement instanceof IInterfaceTypeElement)) {
                INedTypeElement implementor = (INedTypeElement)typeElement;

                for (INedTypeElement interfaze : implementor.getNedTypeInfo().getLocalInterfaces()) {
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

    protected void collectUsersMap() {
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

    protected void collectDoxyMap() throws Exception {
        File doxyTagsFile = getFullDoxyPath().append("doxytags.xml").toFile();

        if (doxyTagsFile.exists()) {
            FileInputStream stream = new FileInputStream(doxyTagsFile);
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            org.w3c.dom.Document document = factory.newDocumentBuilder().parse(stream);
            NodeList nodes = (NodeList)XPathFactory.newInstance().newXPath().compile("//compound[@kind='class']/filename")
                .evaluate(document, XPathConstants.NODESET);

            try {
                monitor.beginTask("Collecting doxygen...", nodes.getLength());

                for (int i = 0; i < nodes.getLength(); i++) {
                    Node node = nodes.item(i);
                    String fileName = node.getTextContent();
                    Node nameNode = node.getParentNode().getFirstChild().getNextSibling();
                    Assert.isTrue(nameNode.getNodeName().equals("name"));
                    String className = nameNode.getTextContent();
                    doxyMap.put(className, fileName);
                    monitor.worked(1);
                }
            }
            finally {
                monitor.done();
            }
        }
    }

    private boolean isCppProject(IProject project) throws CoreException {
        return project.getNature("org.eclipse.cdt.core.ccnature") != null;
    }

    protected void generateDoxy() throws Exception {
        if (generateDoxy && isCppProject(project)) {
            if (doxyExecutablePath == null || !new File(doxyExecutablePath).exists())
                throw new IllegalStateException("The Doxygen executable path is invalid, set it using Window/Preferences...\nThe currently set path is: " + doxyExecutablePath);

            try {
                monitor.beginTask("Running doxygen...", IProgressMonitor.UNKNOWN);
                monitor.subTask("Waiting for doxygen to complete"); // needed, otherwise Eclipse progress dialog will show last subtask of previous task
                File doxyConfigFile = absoluteDoxyConfigFilePath.toFile();

                if (!doxyConfigFile.exists()) {
                    DisplayUtils.runNowOrSyncInUIThread(() -> {
                            if (MessageDialog.openQuestion(null, "Missing configuration file", "Doxygen configuration file does not exist: " + absoluteDoxyConfigFilePath + "\n\nDo you want to create a default one?"))
                                DocumentationGeneratorPropertyPage.generateDefaultDoxyConfigurationFile(project, doxyExecutablePath, absoluteDoxyConfigFilePath.toString());
                        }
                    );
                }

                if (doxyConfigFile.exists()) {
                    String content = FileUtils.readTextFile(doxyConfigFile, null);
                    // these options must be always overwritten
                    IPath fullPath = getFullDoxyPath().makeRelativeTo(new Path(project.getLocation().toString()));
                    content = DocumentationGeneratorPropertyPage.replaceDoxygenConfigurationEntry(content, "OUTPUT_DIRECTORY", fullPath.toString());
                    content = DocumentationGeneratorPropertyPage.replaceDoxygenConfigurationEntry(content, "HTML_HEADER", fullPath.append("../../doxy-header.tmpl").toString());
                    content = DocumentationGeneratorPropertyPage.replaceDoxygenConfigurationEntry(content, "GENERATE_TAGFILE", fullPath.append("doxytags.xml").toString());
                    content = DocumentationGeneratorPropertyPage.replaceDoxygenConfigurationEntry(content, "SOURCE_BROWSER", (generateCppSourceListings ? "YES" : "NO"));
                    File modifiedDoxyConfigFile = documentationRootPath.append("temp-doxy.cfg").toFile();
                    File headerTemplateFile = documentationRootPath.append("doxy-header.tmpl").toFile();

                    try {
                        String header = readTextFromResource("doxy-header.tmpl");
                        FileUtils.writeTextFile(headerTemplateFile, header, null);

                        FileUtils.writeTextFile(modifiedDoxyConfigFile, content, null);
                        ProcessUtils.exec(doxyExecutablePath, new String[] {modifiedDoxyConfigFile.toString()}, project.getLocation().toString(), monitor);
                    }
                    finally {
                        headerTemplateFile.delete();
                        modifiedDoxyConfigFile.delete();
                    }
                }
                else
                    throw new IllegalStateException("Doxygen configuration file not found: " + absoluteDoxyConfigFilePath);
            }
            finally {
                monitor.done();
            }
        }
    }

    protected String processHTMLContent(String clazz, String comment) {
        return NedCommentFormatter.makeHtmlDocu(comment, clazz.equals("briefcomment"), generateExplicitLinksOnly, new INeddocProcessor() {
            public String process(String comment) {
                return replaceTypeReferences(comment);
            }});
    }

    protected String replaceTypeReferences(String comment) {
        // note: Chinese characters in the comment must be unescaped at this point (not as numeric entities),
        // otherwise we our regex won't find them and Chinese identifiers won't be hyperlinked
        return StringUtils.replaceMatches(comment, possibleTypeReferencesPattern, new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                String prefix = matcher.group(1); // one or more tildes, or zero or more backslashes, depending on generateExplicitLinksOnly
                boolean evenPrefixes = prefix.length() % 2 == 0;
                String identifier = matcher.group(2);
                List<ITypeElement> typeElements = typeNamesMap.get(identifier);

                if ((generateExplicitLinksOnly && !evenPrefixes) || (!generateExplicitLinksOnly && evenPrefixes && typeElements != null))
                {
                    // literal backslashes and tildes are doubled in the neddoc source when they are in front of an identifier
                    prefix = prefix.substring(0, prefix.length() / 2);

                    if (typeElements == null) {
                        return prefix + "<span class=\"error\" title=\"Unresolved link\">" + identifier + "</span>";
                    }
                    else if (typeElements.size() == 1) {
                        return prefix + "<a href=\"" + getOutputFileName(typeElements.get(0)) + "\">" + typeElements.get(0).getName() + "</a>"; // use simple name in hyperlink
                    }
                    else {
                        // several types with the same simple name
                        String replacement = prefix + typeElements.get(0).getName() + "(";
                        int i = 1;
                        for (ITypeElement typeElement : typeElements)
                            replacement += "<a href=\"" + getOutputFileName(typeElement) + "\">" + (i++) + "</a>" + ",";
                        replacement = replacement.substring(0, replacement.length()-1) + ")";
                        return replacement;
                    }
                }
                else
                    return null;
            }
        });
    }

    protected void copyFileFromResource(String resourceName) throws Exception {
        copyFileFromResource(resourceName, resourceName);
    }

    protected void copyFileFromResource(final String resourcePath, String fileName) throws Exception {
        InputStream stream = getClass().getResourceAsStream("templates/" + template +"/" + resourcePath);
        if (stream == null)
            throw new RuntimeException("Resource not found: " + resourcePath);
        FileUtils.copy(stream, getOutputFile(fileName));
    }

    protected String readTextFromResource(final String resourcePath) throws Exception {
        InputStream stream = getClass().getResourceAsStream("templates/" + template + "/" + resourcePath);
        if (stream == null)
            throw new RuntimeException("Resource not found: " + resourcePath);
        return  new String(FileUtils.readBinaryFile(stream));
    }

    protected void copyCSS() throws Exception {
        if (customCssPath == null)
            copyFileFromResource("doxygen.css");
        else
            FileUtils.copy(new FileInputStream(customCssPath.toPortableString()), getOutputFile("style.css"));
        copyFileFromResource("navtree.css");
        copyFileFromResource("tabs.css");
    }

    protected void copyJavaScript() throws Exception {
        copyFileFromResource("dynsections.js");
        copyFileFromResource("jquery.js");
        copyFileFromResource("navtree.js");
        copyFileFromResource("resize.js");
    }

    protected void generateRedirectPage() throws Exception {
        int count = rootRelativeDoxyPath.matchingFirstSegments(rootRelativeNeddocPath);
        final IPath remainingPath = rootRelativeNeddocPath.removeFirstSegments(count);
        final IPath reversePath = getReversePath(remainingPath);

        FileUtils.writeTextFile(getOutputFile(reversePath.append("index.html").toPortableString()),
                "<html>\n" +
                "   <head>\n" +
                "      <title>Redirect Page</title>\n" +
                "      <meta http-equiv=\"refresh\" content=\"0;url=" + remainingPath.append("index.html").toPortableString() + "\"></head>\n" +
                "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\n" +
                "   <body/>\n" +
                "</html>",
                "UTF-8"
        );
    }

    protected void generatePage(String fileName, final String content) throws Exception {
        generatePage(fileName, () -> {
                out(content);
            }
        );
    }

    protected void generatePage(String fileName, Runnable content) throws Exception {
        FileOutputStream oldCurrentOutputStream = currentOutputStream;

        File file = getOutputFile(fileName);
        currentOutputStream = new FileOutputStream(file);

        String splitPage[] = readTextFromResource("page.tmpl").split("@content@");
        if (splitPage.length != 2)
            throw new RuntimeException("page.tmpl must contain exactly one @content@ placeholder ");

        out(splitPage[0]);
        content.run();

        if (APPLY_CC) {
            String atag = "<a href=\"http://creativecommons.org/licenses/by-sa/3.0\" target=\"_top\">";
            out("   <hr><p class=\"footer\">"+atag+"<img src=\"by-sa.png\"></a>" +
                " This documentation is released under the "+atag+"Creative Commons license</a></p>\r\n");
        }

        out(splitPage[1]);

        currentOutputStream.close();
        currentOutputStream = oldCurrentOutputStream;
    }

    protected void generateNavigationTree(String fileName, Runnable content) throws Exception {
        FileOutputStream oldCurrentOutputStream = currentOutputStream;

        File file = getOutputFile(fileName);
        currentOutputStream = new FileOutputStream(file);

        String splitNavigation[] = readTextFromResource("navtree.tmpl").split("@content@");
        if (splitNavigation.length != 2)
            throw new RuntimeException("navtree.tmpl must contain exactly one @content@ placeholder ");

        out(splitNavigation[0]);
        content.run();
        out(splitNavigation[1]);

        currentOutputStream.close();
        currentOutputStream = oldCurrentOutputStream;
    }

    protected void generateProjectIndexReference(IProject project) throws Exception {
        String projectName = project.getName();
        IPath indexPath = new Path("..").append(neddocRelativeRootPath).append(projectName).append(DocumentationGeneratorPropertyPage.getNeddocPath(project)).append("index.html");
        generateNavigationMenuItem(1, projectName, indexPath.toPortableString(), null);
    }

    protected void generateProjectIndexReferences(String title, final IProject[] projects) throws Exception {
        if (projects.length != 0)
            generateNavigationMenuItem(1, title, null, () -> {
                    for (IProject referencedProject : projects)
                        generateProjectIndexReference(referencedProject);
                }
            );
    }

    protected void generateProjectIndex() throws Exception {
        generateProjectIndexReferences("Referenced projects", project.getReferencedProjects());
        generateProjectIndexReferences("Referencing projects", project.getReferencingProjects());
    }

    protected void generatePackageReference(String packageName) throws IOException {
        out("<b>Package:</b> " + packageName);
    }

    protected void generatePackageIndex() throws Exception {
        if (packageNames.size() != 0)
            generateNavigationMenuItem(1, "Packages", null, () -> {
                    for (String packageName : packageNames) {
                        generateNavigationMenuItem(2, packageName, packageName + "-package.html", null);
                    }
                }
            );
    }

    protected void generateFileIndex() throws Exception {
        if (files.size() != 0)
            generateNavigationMenuItem(1, "Files", null, () -> {
                    for (IFile file : files)
                        generateNavigationMenuItem(2, file.getProjectRelativePath().toPortableString(), getOutputFileName(file), null);
                }
            );
    }

    protected void generateNavigationTree() throws Exception {
        try {
            monitor.beginTask("Generating navigation tree...", IProgressMonitor.UNKNOWN);

            generateNavigationTreeIcons();

            generateNavigationTree("navtreedata.js", () -> {
                    generateNavigationMenuItem(0, project.getName(), "overview.html", () -> {

                        generateOverview();
                        generateSelectedTopics();

                        generateDiagrams();
                        generateProjectIndex();
                        generatePackageIndex();

                        generateTypeIndex("simple modules", SimpleModuleElementEx.class);
                        generateTypeIndex("compound modules", object -> {
                                return object instanceof CompoundModuleElementEx && !((CompoundModuleElementEx)object).isNetwork();
                            });
                        generateTypeIndex("networks", object -> {
                                return object instanceof CompoundModuleElementEx && ((CompoundModuleElementEx)object).isNetwork();
                            });
                        generateTypeIndex("all modules", IModuleTypeElement.class);
                        generateTypeIndex("channels", ChannelElementEx.class);
                        generateTypeIndex("channel interfaces", ChannelInterfaceElementEx.class);
                        generateTypeIndex("module interfaces", ModuleInterfaceElementEx.class);
                        generateTypeIndex("messages", MessageElementEx.class);
                        generateTypeIndex("packets", PacketElementEx.class);
                        generateTypeIndex("classes", ClassElementEx.class);
                        generateTypeIndex("structs", StructElementEx.class);
                        generateTypeIndex("enums", EnumElementEx.class);

                        generateFileIndex();
                        generateCppIndex();
                    });
                }
            );
            // out("\nvar NAVTREEINDEX = [");
            // TODO add all files
            // out("\n];");
        }
        finally {
            monitor.done();
        }
    }

    protected void generateNavigationTreeIcons() throws Exception {
        copyFileFromResource("bc_s.png");
        copyFileFromResource("nav_h.png");
        copyFileFromResource("splitbar.png");
        copyFileFromResource("sync_on.png");
        copyFileFromResource("sync_off.png");
        copyFileFromResource("tab_a.png");
        copyFileFromResource("tab_b.png");
        copyFileFromResource("tab_s.png");
        copyFileFromResource("by-sa.png");
    }

    protected void generateNavigationMenuItem(int level, String title, String url, Runnable content) throws Exception {
        String indent = String.format("%1$"+(2+level*2)+"s", "");
        out("\n"+indent+"['" + title + "', " + (url==null ? "null" : "'"+url+"'") + ", ");
        if (content != null) {
            out("[");
            content.run();
            out("\n"+indent+"] ],");
        } else {
            out("null");
            out("],");
        }
    }

    protected void generateCppMenuItem(String title, String url) throws Exception {
        if (project.getFile(rootRelativeDoxyPath.append(url)).getLocation().toFile().exists())
            generateNavigationMenuItem(2, title, neddocRelativeRootPath.append(rootRelativeDoxyPath).append(url).toPortableString(), null);
    }

    private void generateCppIndex() throws Exception {
        if (isCppProject(project) && generateDoxy)
            generateNavigationMenuItem(1, "C++", null, () -> {
                    generateCppMenuItem("Main Page", "main.html");
                    generateCppMenuItem("File List", "files.html");
                    generateCppMenuItem("Class List", "annotated.html");
                    generateCppMenuItem("Class Hierarchy", "hierarchy.html");
                    generateCppMenuItem("Class Members", "functions.html");
                    generateCppMenuItem("Namespace List", "namespaces.html");
                    generateCppMenuItem("File Members", "globals.html");
                    generateCppMenuItem("Namespace Members", "namespacemembers.html");
                }
            );
    }

    protected void generateOverview() throws Exception {
        generateNavigationMenuItem(1, "Overview", "overview.html", null);
        final boolean[] titlePageFound = new boolean[1];
        mapPageMatchers(new IPageMatcherVisitor() {
        public boolean visit(PageType pageType, String file, String title, String content) throws Exception {
            if (pageType == PageType.TITLE_PAGE && content.length() > 0) {
                    titlePageFound[0] = true;
                    generatePage("overview.html",
                        processHTMLContent("comment", content + "<hr/>\r\n" + "<p>Generated by neddoc.</p>\r\n"));
                    return false;
                }
                else
                    return true;
            }
        });
        // generate default if @titlepage not found
        if (!titlePageFound[0])
            generatePage("overview.html",
                "<center><h1>OMNeT++ Model Documentation</h1></center>\r\n" +
                "<center><i>Generated from NED and MSG files</i></center>\r\n" +
                "<p>This documentation has been generated from NED and MSG files.</p>\r\n" +
                "<p>Use the links in the left frame to navigate around.</p>\r\n" +
                "<p>Hint for model developers: if you don\'t like this page, try creating\r\n" +
                "a <tt>package.ned</tt> file with the <tt>@titlepage</tt> directive\r\n" +
                "embedded in a comment.</p>\r\n" +
                "<hr/>\r\n" +
                "<p>Generated by neddoc.</p>\r\n");
    }

    protected void generateSelectedTopics() throws Exception {
        final boolean[] pageFound = new boolean[1];
        // see if there's a @page at all
        mapPageMatchers(new IPageMatcherVisitor() {
            public boolean visit(PageType pageType, String file, String title, String content) throws Exception {
                if (pageType == PageType.PAGE || pageType == PageType.EXTERNAL_PAGE) {
                    pageFound[0] = true;
                    return false;
                }
                else
                    return true;
            }
        });
        if (pageFound[0]) {
            generateNavigationMenuItem(1, "Selected Topics", null, () -> {
                    mapPageMatchers(new IPageMatcherVisitor() {
                        public boolean visit(PageType pageType, String file, String title, String content) throws Exception {
                            if (pageType == PageType.PAGE) {
                                generateNavigationMenuItem(2, title, file, null);
                                generatePage(file,
                                        "<h2>" + title + "</h2>" + processHTMLContent("comment", content));

                            }
                            else if (pageType == PageType.EXTERNAL_PAGE) {
                                generateNavigationMenuItem(2, title, file, null);
                            }
                            return true;
                        }
                    });
                }
            );
        }
    }

    protected enum PageType {
        PAGE,
        TITLE_PAGE,
        EXTERNAL_PAGE
    }

    protected static interface IPageMatcherVisitor {
        public boolean visit(PageType pageType, String file, String title, String content) throws Exception;
    }

    protected void mapPageMatchers(IPageMatcherVisitor visitor) throws Exception {
        for (IFile file : files) {
            String comment = null;

            if (nedResources.isNedFile(file))
                comment = getExpandedComment(nedResources.getNedFileElement(file));
            else if (msgResources.isMsgFile(file))
                comment = getExpandedComment(msgResources.getMsgFileElement(file));

            if (comment != null) {
                if (comment.contains("@page") || comment.contains("@titlepage") || comment.contains("@externalpage")) {

                    Matcher matcher = externalPagePattern.matcher(comment);
                    StringBuffer buffer = new StringBuffer();

                    while (matcher.find()) {
                        matcher.appendReplacement(buffer, "");
                        String fileName = matcher.group(1);
                        String title = StringUtils.isEmpty(matcher.group(2)) ? fileName : matcher.group(2);
                        if (!visitor.visit(PageType.EXTERNAL_PAGE, fileName, title, null))
                            return;
                    }

                    matcher.appendTail(buffer);
                    comment = buffer.toString();

                    List<String> pages = StringUtils.splitPreservingSeparators(comment, pageSeparatorPattern);

                    for (int i = 1; i < pages.size() - 1; i++) {
                        String header = pages.get(i);
                        String content = pages.get(i+1);

                        if (titlePagePattern.matcher(header).find()) {
                            if (!visitor.visit(PageType.TITLE_PAGE, null, null, content))
                                return;
                        }
                        else {
                            matcher = pagePattern.matcher(header);
                            if (matcher.find()) {
                                String fileName = matcher.group(1);
                                String title = StringUtils.isEmpty(matcher.group(2)) ? fileName : matcher.group(2);
                                fileName = sanitizeFileName(fileName, ".html");
                                if (!visitor.visit(PageType.PAGE, fileName, title, content))
                                    return;
                            }
                        }
                    }
                }
            }
        }
    }

    protected String sanitizeFileName(String fileName, String requiredSuffix) {
        // Replace characters that may cause trouble: control characters, characters that may
        // be invalid in a file name in some platform, may need quoting when put into a HTML
        // attribute (href), or may have different representations in different encodings
        // (i.e. non-ASCII chars). When changing this code, make sure that the page "Nasty-Filename"
        // in test/neddoc can be generated and also opened!
        fileName = fileName.trim().replaceAll("\\s+", "-").replace('[', '(').replace(']', ')');
        fileName = fileName.replaceAll("[^-A-Za-z0-9!@$(){};+=.]", "_");

        // add suffix
        if (!fileName.endsWith(requiredSuffix))
            fileName += requiredSuffix;
        return fileName;
    }

    protected void generateDiagrams() throws Exception {
        boolean found = false;
        for (ITypeElement typeElement : typeElements) {
            if (typeElement instanceof INedTypeElement || typeElement instanceof IMsgTypeElement) {
                found = true;
                break;
            }
        }

        if (found && (generateFullUsageDiagrams || generateFullInheritanceDiagrams)) {
            generateNavigationMenuItem(1, "Diagrams", null, () -> {
                    for (ITypeElement typeElement : typeElements) {
                        if (typeElement instanceof INedTypeElement) {
                            if (generateFullUsageDiagrams)
                                generateNavigationMenuItem(2, "Full NED Usage Diagram", "full-ned-usage-diagram.html", null);

                            if (generateFullInheritanceDiagrams)
                                generateNavigationMenuItem(2, "Full NED Inheritance Diagram", "full-ned-inheritance-diagram.html", null);

                            break;
                        }
                    }

                    for (ITypeElement typeElement : typeElements) {
                        if (typeElement instanceof IMsgTypeElement) {
                            if (generateFullUsageDiagrams)
                                generateNavigationMenuItem(2, "Full MSG Usage Diagram", "full-msg-usage-diagram.html", null);

                            if (generateFullInheritanceDiagrams)
                                generateNavigationMenuItem(2, "Full MSG Inheritance Diagram", "full-msg-inheritance-diagram.html", null);

                            break;
                        }
                    }
                }
            );
        }
    }

    protected void generatePageReference(String fileName, String title) throws Exception {
        if (title == null || title.equals(""))
            title = fileName;

        generateNavigationMenuItem(2, title, fileName, null);
    }

    protected void generateTypeIndexEntry(ITypeElement typeElement) throws Exception {
        generateNavigationMenuItem(2, typeElement.getName(), getOutputFileName(typeElement), null);
    }

    protected void generateTypeIndex(String title, final Class<?> clazz) throws Exception {
        generateTypeIndex(title, object -> {
                return clazz.isInstance(object);
            }
        );
    }

    protected void generateTypeIndex(String title, IPredicate predicate) throws Exception {
        final ArrayList<ITypeElement> selectedElements = new ArrayList<ITypeElement>();

        for (ITypeElement typeElement : typeElements)
            if (predicate.matches(typeElement))
                selectedElements.add(typeElement);

        if (selectedElements.size() != 0) {
            generateNavigationMenuItem(1, WordUtils.capitalize(title), null, () -> {
                    for (ITypeElement typeElement : selectedElements)
                        generateTypeIndexEntry(typeElement);
                }
            );
        }
    }

    protected void generateNedTagFile() throws Exception {

        FileOutputStream oldCurrentOutputStream = currentOutputStream;
        currentOutputStream = new FileOutputStream(getOutputFile("nedtags.xml"));

        out("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n");
        out("<tagfile>\n");
        for (ITypeElement typeElement : typeElements)
            if (typeElement instanceof INedTypeElement){
                INedTypeElement e = (INedTypeElement)typeElement;
                out("  <compound kind=\"class\">\n");
                String pkgname = getPackageName(e);
                out("    <name>"+ (("default".equals(pkgname)) ? "" : pkgname+".") +e.getName()+"</name>\n");
                out("    <filename>"+getOutputFileName(e)+"</filename>\n");
                out("  </compound>\n");
            }
        out("</tagfile>\n");

        currentOutputStream.close();
        currentOutputStream = oldCurrentOutputStream;
    }

    protected void generateMsgTagFile() throws Exception {

        FileOutputStream oldCurrentOutputStream = currentOutputStream;
        currentOutputStream = new FileOutputStream(getOutputFile("msgtags.xml"));

        out("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n");
        out("<tagfile>\n");
        for (ITypeElement typeElement : typeElements)
            if (typeElement instanceof IMsgTypeElement){
                out("  <compound kind=\"class\">\n");
                out("    <name>"+ typeElement.getName()+"</name>\n");
                out("    <filename>"+getOutputFileName(typeElement)+"</filename>\n");
                out("  </compound>\n");
            }
        out("</tagfile>\n");

        currentOutputStream.close();
        currentOutputStream = oldCurrentOutputStream;
    }

    protected void generatePackagePages() throws Exception {
        for (final String packageName : packageNames) {
            generatePage(packageName + "-package.html", () -> {
                    out("<h2>Package " + packageName + "</h2>\r\n");

                    out("<table class=\"typestable\">\r\n");
                    generateTypesTableHead();

                    for (ITypeElement typeElement : typeElements)
                        if (typeElement instanceof INedTypeElement)
                            if (packageName.equals(getPackageName((INedTypeElement)typeElement)))
                                generateTypeReferenceLine(typeElement);

                    out("</table>\r\n");
                }
            );
        }
    }

    protected void generateFilePages() throws Exception {
        try {
            monitor.beginTask("Generating file pages...", files.size());

            for (final IFile file : files) {
                generatePage(getOutputFileName(file), () -> {
                        monitor.subTask(file.getFullPath().toString());
                        String fileType = nedResources.isNedFile(file) ? "NED" : msgResources.isMsgFile(file) ? "Msg" : "";

                        out("<h2 class=\"comptitle\">" + fileType + " File <i>" + file.getProjectRelativePath() + "</i></h2>\r\n");

                        INedFileElement fileElement = msgResources.isMsgFile(file) ?
                                msgResources.getMsgFileElement(file) : nedResources.getNedFileElement(file);
                        List<? extends ITypeElement> typeElements = fileElement.getTopLevelTypeNodes();

                        if (!typeElements.isEmpty()) {
                            out("<table class=\"typestable\">\r\n");
                            generateTypesTableHead();

                            for (ITypeElement typeElement : typeElements)
                                generateTypeReferenceLine(typeElement);

                            out("</table>\r\n");
                        }

                        generateSourceContent(file);
                        monitor.worked(1);
                    }
                );
            }
        }
        finally {
            monitor.done();
        }
    }

    protected void generateTypePages() throws Exception {
        try {
            monitor.beginTask("Generating NED type pages...", typeElements.size());

            for (final ITypeElement typeElement : typeElements) {
                generatePage(getOutputFileName(typeElement), () -> {
                        boolean isNedTypeElement = typeElement instanceof INedTypeElement;
                        boolean isMsgTypeElement = typeElement instanceof IMsgTypeElement;

                        out("<h2 class=\"comptitle\">" + WordUtils.capitalize(typeElement.getReadableTagName()) + " <i>" + typeElement.getName() + "</i></h2>\r\n");

                        if (isNedTypeElement) {
                            generatePackageReference(getPackageName((INedTypeElement)typeElement));
                            out("<br/>");
                        }

                        if (typeElement instanceof INedTypeElement && nedResources.isBuiltInDeclaration(((INedTypeElement)typeElement).getNedTypeInfo()))
                            generateBuiltinTypeReference();
                        else
                            generateFileReference(getNedOrMsgFile(typeElement));
                        out("<br/>");

                        if (typeElement instanceof SimpleModuleElementEx || typeElement instanceof IMsgTypeElement)
                            generateNedTypeCppDefinitionReference(typeElement);

                        String comment = getExpandedComment(typeElement);
                        if (comment == null)
                            out("<p>(no description)</p>");
                        else
                            out(processHTMLContent("comment", comment));

                        if (isNedTypeElement) {
                            INedTypeElement nedTypeElement = (INedTypeElement)typeElement;
                            monitor.subTask(nedTypeElement.getReadableTagName() + ": " + nedTypeElement.getNedTypeInfo().getFullyQualifiedName());

                            generateTypeDiagram(nedTypeElement);
                            generateUsageDiagram(nedTypeElement);
                            generateInheritanceDiagram(nedTypeElement);
                            generateExtendsTable(nedTypeElement);
                            generateKnownSubtypesTable(nedTypeElement);
                            generateUsedInTables(nedTypeElement);
                            generateParametersTable(nedTypeElement);
                            generatePropertiesTable(nedTypeElement);
                            if (typeElement instanceof IModuleTypeElement)
                                generateGatesTable(nedTypeElement);
                            generateSignalsTable(nedTypeElement);
                            generateStatisticsTable(nedTypeElement);
                            if (typeElement instanceof CompoundModuleElementEx)
                                generateUnassignedParametersTable(nedTypeElement);
                        }
                        else if (isMsgTypeElement) {
                            IMsgTypeElement msgTypeElement = (IMsgTypeElement)typeElement;
                            monitor.subTask(msgTypeElement.getReadableTagName() + ": " + msgTypeElement.getName());

                            generateUsageDiagram(msgTypeElement);
                            generateInheritanceDiagram(msgTypeElement);
                            generateExtendsTable(msgTypeElement);
                            generateKnownSubtypesTable(msgTypeElement);
                            generateFieldsTable(msgTypeElement);
                            generatePropertiesTable(msgTypeElement);
                        }

                        if (generateNedSourceListings)
                            generateSourceContent(typeElement);

                        monitor.worked(1);
                    }
                );
            }
        }
        finally {
            monitor.done();
        }
    }

    protected void generateFieldsTable(IMsgTypeElement msgTypeElement) throws IOException {
        Map<String, FieldElement> fields = msgTypeElement.getMsgTypeInfo().getFields();
        Map<String, FieldElement> localFields = msgTypeElement.getMsgTypeInfo().getLocalFields();
        if (fields.size() != 0) {
            out("<h3 class=\"subtitle\">Fields:</h3>\r\n" +
                "<table class=\"fieldstable\">\r\n" +
                "   <tr>\r\n" +
                "      <th class=\"name\">Name</th>\r\n" +
                "      <th class=\"type\">Type</th>\r\n" +
                "      <th class=\"description\">Description</th>\r\n" +
                "   </tr>\r\n");
            for (String name : fields.keySet())
            {
                FieldElement field = fields.get(name);
                String trClass = localFields.containsKey(name) ? "local" : "inherited";
                out("<tr class=\"" + trClass + "\">\r\n" +
                    "   <td>" + name + "</td>\r\n" +
                    "   <td>\r\n" +
                    "      <i>\r\n");
                String dataType = field.getDataType();
                IMsgTypeElement fieldTypeElement = msgResources.lookupMsgType(dataType);
                if (fieldTypeElement != null)
                    generateTypeReference(fieldTypeElement);
                else
                    out(dataType);
                if (field.getIsVector())
                    out("[" + field.getVectorSize() + "]"); // note: no whitespace between type name and "["
                out("</i>\r\n" +
                    "   </td>\r\n" +
                    "   <td>");
                generateTableComment(getExpandedComment(field));
                out("</td>\r\n" +
                    "</tr>\r\n");
            }
            out("</table>\r\n");
        }
    }

    protected void generatePropertiesTable(ITypeElement typeElement) throws IOException {
        Map<String, Map<String, PropertyElementEx>> properties = typeElement.getProperties();

        if (properties.size() != 0) {
            out("<h3 class=\"subtitle\">Properties:</h3>\r\n" +
                "<table class=\"propertiestable\">\r\n" +
                "   <tr>\r\n" +
                "      <th class=\"name\">Name</th>\r\n" +
                "      <th class=\"value\">Value</th>\r\n" +
                "      <th class=\"description\">Description</th>\r\n" +
                "   </tr>\r\n");

            for (String name : properties.keySet())
            {
                PropertyElementEx property = properties.get(name).get(PropertyElementEx.DEFAULT_PROPERTY_INDEX);

                if (property != null) {
                    out("<tr>\r\n" +
                        "   <td>" + name + "</td>\r\n" +
                        "   <td><i>");
                    generatePropertyLiteralValues(property);
                    out("</i></td>\r\n" +
                        "   <td>");
                    generateTableComment(getExpandedComment(property));
                    out("</td>\r\n" +
                        "</tr>\r\n");
                }
            }

            out("</table>\r\n");
        }
    }

    protected void generatePropertyLiteralValues(PropertyElementEx property) throws IOException {
        for (PropertyKeyElement key = property.getFirstPropertyKeyChild(); key != null; key = key.getNextPropertyKeySibling()) {
            out(key.getFirstLiteralChild().getValue());

            if (key.getNextPropertyKeySibling() != null)
                out(",");
        }
    }

    protected void generateUsedInTables(INedTypeElement typeElement) throws IOException {
        if (usersMap.containsKey(typeElement)) {
            ArrayList<CompoundModuleElementEx> compoundModules = new ArrayList<CompoundModuleElementEx>();
            ArrayList<CompoundModuleElementEx> networks = new ArrayList<CompoundModuleElementEx>();

            for (ITypeElement userElement : usersMap.get(typeElement)) {
                if (userElement instanceof CompoundModuleElementEx) {
                    CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)userElement;

                    if (compoundModule.isNetwork())
                        networks.add(compoundModule);
                    else
                        compoundModules.add(compoundModule);
                }
            }

            if (compoundModules.size() != 0) {
                out("<h3 class=\"subtitle\">Used in compound modules:</h3>\r\n");

                out("<table class=\"typestable\">\r\n");
                generateTypesTableHead();

                for (INedTypeElement userElement : compoundModules)
                    generateTypeReferenceLine(userElement);

                out("</table>\r\n");
            }

            if (networks.size() != 0) {
                out("<h3 class=\"subtitle\">Networks:</h3>\r\n");

                out("<table class=\"typestable\">\r\n");
                generateTypesTableHead();

                for (INedTypeElement userElement : networks)
                    generateTypeReferenceLine(userElement);

                out("</table>\r\n");
            }
        }
    }

    protected void generateParametersTable(INedTypeElement typeElement) throws IOException {
        Map<String, ParamElementEx> localParamsDeclarations = typeElement.getNedTypeInfo().getLocalParamDeclarations();
        Map<String, ParamElementEx> paramsDeclarations = typeElement.getParamDeclarations();
        Map<String, ParamElementEx> paramsAssignments = typeElement.getParamAssignments();

        if (!paramsDeclarations.isEmpty()) {
            out("<h3 class=\"subtitle\">Parameters:</h3>\r\n" +
                "<table class=\"paramstable\">\r\n" +
                "   <tr>\r\n" +
                "      <th class=\"name\">Name</th>\r\n" +
                "      <th class=\"type\">Type</th>\r\n" +
                "      <th class=\"defaultvalue\">Default value</th>\r\n" +
                "      <th class=\"description\">Description</th>\r\n" +
                "   </tr>\r\n");

            for (String name : paramsDeclarations.keySet()) {
                ParamElementEx paramDeclaration = paramsDeclarations.get(name);
                ParamElementEx paramAssignment = paramsAssignments.get(name);
                String trClass = localParamsDeclarations.containsKey(name) ? "local" : "inherited";

                out("<tr class=\"" + trClass + "\">\r\n" +
                    "   <td>" + name + "</td>\r\n" +
                    "   <td>\r\n" +
                    "      <i>" + getParamTypeAsString(paramDeclaration) + "</i>\r\n" +
                    "   </td>\r\n" +
                    "   <td>" + (paramAssignment == null ? "" : paramAssignment.getValue()) + "</td>\r\n" +
                    "   <td>");
                generateTableComment(getExpandedComment(paramDeclaration));
                out("   </td>\r\n" +
                    "</tr>\r\n");
            }

            out("</table>\r\n");
        }
    }

    protected void generateSignalsTable(INedTypeElement typeElement) throws IOException {
        Map<String, PropertyElementEx> propertyMap = typeElement.getProperties().get("signal");
        if (propertyMap != null) {
            out("<h3 class=\"subtitle\">Signals:</h3>\r\n" +
                    "<table class=\"signalstable\">\r\n" +
                    "   <tr>\r\n" +
                    "      <th class=\"name\">Name</th>\r\n" +
                    "      <th class=\"type\">Type</th>\r\n" +
                    "      <th class=\"unit\">Unit</th>\r\n" +
                    "   </tr>\r\n");
            for (String name : propertyMap.keySet()) {
                PropertyElementEx propertyElement = propertyMap.get(name);
                out("<tr class=\"local\">\r\n" +
                    "   <td>" + name + "</td>\r\n" +
                    "   <td><i>\r\n");
                String cppType = getPropertyElementValue(propertyElement, "type");
                generateCppReference(cppType, cppType);
                out("   </i></td>\r\n" +
                    "   <td>" + getPropertyElementValue(propertyElement, "unit") + "</td>\r\n");
                out("</tr>\r\n");
            }
            out("</table>\r\n");
        }
    }

    protected void generateStatisticsTable(INedTypeElement typeElement) throws IOException {
        Map<String, PropertyElementEx> propertyMap = typeElement.getProperties().get("statistic");
        if (propertyMap != null) {
            out("<h3 class=\"subtitle\">Statistics:</h3>\r\n" +
                    "<table class=\"statisticstable\">\r\n" +
                    "   <tr>\r\n" +
                    "      <th class=\"name\">Name</th>\r\n" +
                    "      <th class=\"title\">Title</th>\r\n" +
                    "      <th class=\"source\">Source</th>\r\n" +
                    "      <th class=\"record\">Record</th>\r\n" +
                    "      <th class=\"unit\">Unit</th>\r\n" +
                    "      <th class=\"interpolationmode\">Interpolation Mode</th>\r\n" +
                    "   </tr>\r\n");
            for (String name : propertyMap.keySet()) {
                PropertyElementEx propertyElement = propertyMap.get(name);
                out("<tr class=\"local\">\r\n" +
                    "   <td>" + name + "</td>\r\n" +
                    "   <td>" + getPropertyElementValue(propertyElement, "title") + "</td>\r\n" +
                    "   <td>" + getPropertyElementValue(propertyElement, "source") + "</td>\r\n" +
                    "   <td>" + getPropertyElementValue(propertyElement, "record") + "</td>\r\n" +
                    "   <td>" + getPropertyElementValue(propertyElement, "unit") + "</td>\r\n" +
                    "   <td>" + getPropertyElementValue(propertyElement, "interpolationmode") + "</td>\r\n");
                out("</tr>\r\n");
            }
            out("</table>\r\n");
        }
    }

    protected void generateUnassignedParametersTable(INedTypeElement typeElement) throws IOException {
        ArrayList<ArrayList<Object>> params = new ArrayList<ArrayList<Object>>();
        collectUnassignedParameters(null, typeElement.getNedTypeInfo().getSubmodules(), params);

        if (params.size() != 0) {
            out("<h3 class=\"subtitle\">Unassigned submodule parameters:</h3>\r\n" +
                "<table class=\"deepparamstable\">\r\n" +
                "   <tr>\r\n" +
                "      <th class=\"name\">Name</th>\r\n" +
                "      <th class=\"type\">Type</th>\r\n" +
                "      <th class=\"defaultvalue\">Default value</th>\r\n" +
                "      <th class=\"description\">Description</th>\r\n" +
                "   </tr>\r\n");
            for (ArrayList<Object> tuple : params) {
                ParamElementEx paramDeclaration = (ParamElementEx)tuple.get(1);
                ParamElementEx paramAssignment = (ParamElementEx)tuple.get(2);

                out("<tr>\r\n" +
                        "   <td>" + (String)tuple.get(0) + "</td>\r\n" +
                        "   <td>\r\n" +
                        "      <i>" + getParamTypeAsString(paramDeclaration) + "</i>\r\n" +
                        "   </td>\r\n" +
                        "   <td>" + (paramAssignment == null ? "" : paramAssignment.getValue()) + "</td>\r\n" +
                        "   <td>");
                    generateTableComment(getExpandedComment(paramDeclaration));
                    out("   </td>\r\n" +
                        "</tr>\r\n");
            }
            out("</table>\r\n");
        }
    }

    protected void collectUnassignedParameters(String prefix, Map<String, SubmoduleElementEx> typeElementMap, ArrayList<ArrayList<Object>> params) throws IOException {
        for (SubmoduleElementEx submodule : typeElementMap.values()) {
            INedTypeElement typeElement = submodule.getEffectiveTypeRef();

            if (typeElement != null) {
                String newPrefix = (prefix == null ? "" : prefix + ".") + "<a href=\"" + getOutputFileName(typeElement) + "\">" + submodule.getName() + "</a>";

                if (typeElement instanceof CompoundModuleElementEx)
                    collectUnassignedParameters(newPrefix, typeElement.getNedTypeInfo().getSubmodules(), params);
                else {
                    INedTypeInfo typeInfo = typeElement.getNedTypeInfo();
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

    protected void generateGatesTable(INedTypeElement typeElement) throws IOException {
        IModuleTypeElement module = (IModuleTypeElement)typeElement;

        Map<String, GateElementEx> localGateDeclarations = module.getNedTypeInfo().getLocalGateDeclarations();
        Map<String, GateElementEx> gateDeclarations = module.getGateDeclarations();
        Map<String, GateElementEx> gatesSizes = module.getGateSizes();

        if (!gateDeclarations.isEmpty()) {
            out("<h3 class=\"subtitle\">Gates:</h3>\r\n" +
                "<table class=\"gatestable\">\r\n" +
                "   <tr>\r\n" +
                "      <th class=\"name\">Name</th>\r\n" +
                "      <th class=\"type\">Direction</th>\r\n" +
                "      <th class=\"gatesize\">Size</th>\r\n" +
                "      <th class=\"description\">Description</th>\r\n" +
                "   </tr>\r\n");

            for (String name : gateDeclarations.keySet()) {
                GateElementEx gateDeclaration = gateDeclarations.get(name);
                GateElementEx gateSize = gatesSizes.get(name);
                String trClass = localGateDeclarations.containsKey(name) ? "local" : "inherited";

                out("<tr class=\"" + trClass + "\">\r\n" +
                    "   <td>" + name + (gateDeclaration.getIsVector() ? " [ ]" : "") + "</xsl:if></td>\r\n" +
                    "   <td><i>" + gateDeclaration.getAttribute(GateElementEx.ATT_TYPE) + "</i></td>\r\n" +
                    "   <td>" + (gateSize != null && gateSize.getIsVector() ? gateSize.getVectorSize() : "") + "</td>" +
                    "   <td>");
                generateTableComment(getExpandedComment(gateDeclaration));
                out("</td>\r\n" +
                    "</tr>\r\n");
            }

            out("</table>\r\n");
        }
    }

    protected void generateTableComment(String comment) throws IOException {
        if (comment != null)
            out(processHTMLContent("comment", comment));
    }

    protected void generateTypesTableHead() throws IOException {
        out("   <tr>\r\n" +
            "      <th class=\"name\">Name</th>\r\n" +
            "      <th class=\"type\">Type</th>\r\n" +
            "      <th class=\"description\">Description</th>\r\n" +
            "   </tr>\r\n");
    }

    protected void generateTypeReferenceLine(ITypeElement typeElement) throws IOException {
        out("<tr>\r\n" +
            "   <td>\r\n");
        generateTypeReference(typeElement); out("\r\n");
        out("   </td>\r\n" +
                "   <td>\r\n");
        generateTypeType(typeElement); out("\r\n");
        out("   </td>\r\n" +
            "   <td>\r\n");
        generateTypeComment(typeElement); out("\r\n");
        out("   </td>\r\n" +
            "</tr>\r\n");
    }

    protected void generateTypeReference(ITypeElement typeElement) throws IOException {
        out("<a href=\"" + getOutputFileName(typeElement) + "\">" + typeElement.getName() + "</a>");
    }

    protected void generateTypeType(ITypeElement typeElement) throws IOException {
        out("<i>" + typeElement.getReadableTagName().replaceAll(" ", "&nbsp;") + "</i>");
    }

    protected void generateTypeComment(ITypeElement typeElement) throws IOException {
        String comment = getExpandedComment(typeElement);
        if (comment != null)
            out(processHTMLContent("briefcomment", comment));
        else
            out("<i>(no description)</i>");
    }

    protected void generateUnresolvedTypeReferenceLine(String name) throws IOException {
        out("<tr>\r\n" +
            "   <td>\r\n");
        generateUnresolvedTypeReference(name);
        out("   </td>\r\n" +
            "   <td>\r\n" +
            "      <i>(unknown -- not in documented files)</i>\r\n" +
            "   </td>\r\n" +
            "</tr>\r\n");
    }

    protected void generateUnresolvedTypeReference(String name) throws IOException {
        out(name);
    }

    protected void generateNedTypeCppDefinitionReference(ITypeElement typeElement) throws IOException {
        String cppClassName;

        if (typeElement instanceof INedTypeElement)
            cppClassName = ((INedTypeElement)typeElement).getNedTypeInfo().getFullyQualifiedCppClassName();
        else if (typeElement instanceof IMsgTypeElement)
            cppClassName = ((IMsgTypeElement)typeElement).getMsgTypeInfo().getFullyQualifiedCppClassName();
        else
            throw new RuntimeException("Unknown type element: " + typeElement);

        if (doxyMap.containsKey(cppClassName)) {
            out("<p>");
            generateCppReference(cppClassName, "<b>C++ definition</b>");
            out("</p>\r\n");
        }
    }

    protected void generateCppReference(String cppClassName, String label) throws IOException {
        if (doxyMap.containsKey(cppClassName))
            out("<a href=\"" + getRelativePath(rootRelativeNeddocPath, rootRelativeDoxyPath).append(doxyMap.get(cppClassName)) + "\" target=\"mainframe\">" + label + "</a>");
        else
            out(label);
    }

    protected void generateBuiltinTypeReference() throws IOException {
        out("<b>Built-in type</b>");
    }

    protected void generateFileReference(IFile file) throws IOException {
        out("<b>File: <a href=\"" + getOutputFileName(file) + "\">" + file.getProjectRelativePath() + "</a></b>");
    }

    protected void generateKnownSubtypesTable(ITypeElement typeElement) throws IOException {
        if (subtypesMap.containsKey(typeElement)) {
            out("<h3 class=\"subtitle\">Known subclasses:</h3>\r\n");

            out("<table class=\"typestable\">\r\n");
            generateTypesTableHead();

            for (ITypeElement subtype : subtypesMap.get(typeElement))
                generateTypeReferenceLine(subtype);

            out("</table>\r\n");
        }
    }

    protected void generateExtendsTable(ITypeElement typeElement) throws IOException {
        if (typeElement.getFirstExtends() != null) {
            out("<h3 class=\"subtitle\">Extends:</h3>\r\n");

            out("<table class=\"typestable\">\r\n");
            generateTypesTableHead();

            if (typeElement instanceof IInterfaceTypeElement)
                for (INedTypeElement supertype : ((IInterfaceTypeElement)typeElement).getNedTypeInfo().getLocalInterfaces())
                    generateTypeReferenceLine(supertype);
            else {
                ITypeElement supertype = typeElement.getSuperType();

                if (supertype != null)
                    generateTypeReferenceLine(supertype);
                else
                    generateUnresolvedTypeReferenceLine(typeElement.getFirstExtends());
            }

            out("</table>\r\n");
        }
    }

    protected void generateNedTypeFigures() throws InterruptedException, CoreException {
        if (generateNedTypeFigures) {
            ArrayList<IFile> nedFiles = new ArrayList<IFile>(nedResources.getNedFiles(project));

            final ExportDiagramFilesOperation exportOperation =
                new ExportDiagramFilesOperation(nedFiles,
                    new PNGDiagramExporter() {
                        public String getName() {
                            return "NED Figure Provider";
                        }

                        public String getDescription() {
                            return "NED Figure Provider";
                        }
                }, true, null) {

                @Override
                protected void refreshContainer(IContainer container, IProgressMonitor monitor) {
                    if (monitor.isCanceled())
                        throw new CancellationException();
                }
            };
            exportOperation.setOverwriteMode(ExportDiagramFilesOperation.OverwriteMode.ALL);
            exportOperation.run(monitor);

            try {
                // KLUDGE: move generated images under neddocPath
                monitor.beginTask("Moving type diagrams...", nedFiles.size());

                for (IFile file : nedFiles) {
                    monitor.subTask(file.getFullPath().toString());
                    List<INedTypeElement> typeElements = nedResources.getNedFileElement(file).getTopLevelTypeNodes();

                    for (INedTypeElement typeElement : typeElements) {
                        String fileName = file.getName().replaceAll(".ned", "");
                        String imageName = NedFigureProvider.getFigureName(typeElements, typeElement, fileName);
                        File sourceImageFile = file.getParent().getFile(new Path(imageName + ".png")).getLocation().toFile();

                        if (sourceImageFile.exists()) {
                            IPath destinationImagePath = getFullNeddocPath().append(getOutputFileName(typeElement, "type", ".png"));
                            sourceImageFile.renameTo(destinationImagePath.toFile());
                            if (APPLY_CC)
                                watermark(destinationImagePath.toString());
                        }
                        else
                            throw new RuntimeException("Cannot generate image for " + typeElement.getNedTypeInfo().getFullyQualifiedName());
                    }

                    monitor.worked(1);
                }
            }
            finally {
                monitor.done();
            }
        }
    }

    protected void watermark(String imagePath) {
        // load the image
        ImageDescriptor desc = ImageDescriptor.createFromFile(null, imagePath);
        Image image = desc.createImage();
        int width = image.getBounds().width;
        int height = image.getBounds().height;

        // draw cc image on it
        GC gc = new GC(image);
        gc.setAlpha(20);
        if (width >= 50 && height >= 50)
            gc.drawImage(CC20_IMAGE, width-22, height-22);
        else if (width >= 24 && height >= 24)
            gc.drawImage(CC16_IMAGE, width-16, height-16);
        gc.dispose();

        // save it back
        ImageLoader loader = new ImageLoader();
        loader.data = new ImageData[] { image.getImageData() };
        loader.save(imagePath, SWT.IMAGE_PNG);
        image.dispose();
    }

    protected void generateTypeDiagram(final INedTypeElement typeElement) throws IOException {
        if (generateNedTypeFigures && !nedResources.isBuiltInDeclaration(typeElement.getNedTypeInfo())) {
            out("<img src=\"" + getOutputFileName(typeElement, "type", ".png") + "\" ismap=\"yes\" usemap=\"#type-diagram\"/>");
            DisplayUtils.runNowOrSyncInUIThread(() -> {
                    try {
                        NedFileElementEx modelRoot = typeElement.getContainingNedFileElement();
                        ScrollingGraphicalViewer viewer = NedFigureProvider.createNedViewer(modelRoot);
                        NedEditPart editPart = (NedEditPart)viewer.getEditPartRegistry().get(typeElement);

                        out("<map name=\"type-diagram\">\r\n");

                        if (editPart instanceof CompoundModuleEditPart) {
                            CompoundModuleEditPart compoundModuleEditPart = (CompoundModuleEditPart)editPart;

                            for (Object child : compoundModuleEditPart.getChildren()) {
                                if (child instanceof SubmoduleEditPart) {
                                    SubmoduleEditPart submoduleEditPart = (SubmoduleEditPart)child;
                                    SubmoduleElementEx submoduleElement = submoduleEditPart.getModel();
                                    outMapReference(submoduleElement.getEffectiveTypeRef(), submoduleEditPart.getFigure());
                                }
                            }
                        }
                        else if (editPart instanceof NedTypeEditPart) {
                            NedTypeEditPart nedTypeEditPart = (NedTypeEditPart)editPart;
                            outMapReference(nedTypeEditPart.getModel(), nedTypeEditPart.getFigure());
                        }

                        out("</map>\r\n");
                    }
                    catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }
            );
        }
    }

    protected void generateFullDiagrams() throws Exception {
        try {
            monitor.beginTask("Generating full diagrams...", (generateFullUsageDiagrams ? 2 : 0) + (generateFullInheritanceDiagrams ? 2 : 0));
            final ArrayList<INedTypeElement> nedTypeElements = new ArrayList<INedTypeElement>();
            final ArrayList<IMsgTypeElement> msgTypeElements = new ArrayList<IMsgTypeElement>();

            for (ITypeElement typeElement : this.typeElements) {
                if (typeElement instanceof INedTypeElement)
                    nedTypeElements.add((INedTypeElement)typeElement);
                else if (typeElement instanceof IMsgTypeElement)
                    msgTypeElements.add((IMsgTypeElement)typeElement);
            }

            if (generateFullUsageDiagrams) {
                generatePage("full-ned-usage-diagram.html", () -> {
                        out("<h2 class=\"comptitle\">Full NED Usage Diagram</h2>\r\n" +
                            "<p>The following diagram shows usage relationships between simple and compound modules, module interfaces, networks, channels and channel interfaces.\r\n" +
                            "Unresolved types are missing from the diagram.</p>\r\n");
                        generateUsageDiagram(nedTypeElements, "full-ned-usage-diagram.png", "full-ned-usage-diagram.map");
                    }
                );
                monitor.worked(1);
            }

            if (generateFullInheritanceDiagrams) {
                generatePage("full-ned-inheritance-diagram.html", () -> {
                        out("<h2 class=\"comptitle\">Full NED Inheritance Diagram</h2>\r\n" +
                            "<p>The following diagram shows the inheritance hierarchy between simple and compound modules, module interfaces, networks, channels and channel interfaces.\r\n" +
                            "Unresolved types are missing from the diagram.</p>\r\n");
                        generateInheritanceDiagram(nedTypeElements, "full-ned-inheritance-diagram.png", "full-ned-inheritance-diagram.map");
                    }
                );
                monitor.worked(1);
            }

            if (generateFullUsageDiagrams) {
                generatePage("full-msg-usage-diagram.html", () -> {
                        out("<h2 class=\"comptitle\">Full MSG Usage Diagram</h2>\r\n" +
                            "<p>The following diagram shows usage relationships between messages, packets, classes and structs.\r\n" +
                            "Unresolved types are missing from the diagram.</p>\r\n");
                        generateUsageDiagram(msgTypeElements, "full-msg-usage-diagram.png", "full-msg-usage-diagram.map");
                    }
                );
                monitor.worked(1);
            }

            if (generateFullInheritanceDiagrams) {
                generatePage("full-msg-inheritance-diagram.html", () -> {
                        out("<h2 class=\"comptitle\">Full MSG Inheritance Diagram</h2>\r\n" +
                            "<p>The following diagram shows the inheritance hierarchy between messages, packets, classes and structs.\r\n" +
                            "Unresolved types are missing from the diagram.</p>\r\n");
                        generateInheritanceDiagram(msgTypeElements, "full-msg-inheritance-diagram.png", "full-msg-inheritance-diagram.map");
                    }
                );
                monitor.worked(1);
            }
        }
        finally {
            monitor.done();
        }
    }

    protected void generateUsageDiagram(ITypeElement typeElement) throws IOException {
        if (generatePerTypeUsageDiagrams) {
            ArrayList<ITypeElement> typeElements = new ArrayList<ITypeElement>();
            typeElements.add(typeElement);

            String diagramType = typeElement instanceof INedTypeElement ? "ned" : "msg";

            out("<h3 class=\"subtitle\">Usage diagram:</h3>\r\n" +
                "<p>The following diagram shows usage relationships between types.\r\n" +
                "Unresolved types are missing from the diagram.");
            if (generateFullUsageDiagrams)
                out(" Click <a href=\"full-" + diagramType + "-usage-diagram.html\">here</a> to see the full picture.");
            out("</p>\r\n");

            generateUsageDiagram(typeElements, getOutputFileName(typeElement, "usage", ".png"), getOutputFileName(typeElement, "usage", ".map"));
        }
    }

    protected void generateUsageDiagram(List<? extends ITypeElement> typeElements, String imageFileName, String cmapFileName) throws IOException {
        if (generatePerTypeUsageDiagrams || generateFullUsageDiagrams) {
            DotGraph dot = new DotGraph();

            dot.append("digraph opp {\r\n" +
                       "   node [fontsize=10,fontname=Helvetica,shape=box,height=.25,style=filled];\r\n");

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

            generateDotOuput(dot, getOutputFile(imageFileName), "png");
            generateDotOuput(dot, getOutputFile(cmapFileName), "cmap");
            if (APPLY_CC)
                watermark(getOutputFile(imageFileName).toString());

            out("<img src=\"" + imageFileName + "\" ismap=\"yes\" usemap=\"#usage-diagram\"/>");
            out("<map name=\"usage-diagram\">" + FileUtils.readTextFile(getOutputFile(cmapFileName), null) + "</map>\r\n");
        }
    }

    protected void generateInheritanceDiagram(ITypeElement typeElement) throws IOException {
        if (generatePerTypeInheritanceDiagrams) {
            ArrayList<ITypeElement> typeElements = new ArrayList<ITypeElement>();
            typeElements.add(typeElement);

            String diagramType = typeElement instanceof INedTypeElement ? "ned" : "msg";

            out("<h3 class=\"subtitle\">Inheritance diagram:</h3>\r\n" +
                "<p>The following diagram shows inheritance relationships for this type.\r\n" +
                "Unresolved types are missing from the diagram.");
            if (generateFullInheritanceDiagrams)
                out(" Click <a href=\"full-" + diagramType + "-inheritance-diagram.html\">here</a> to see the full picture.");
            out("</p>\r\n");

            generateInheritanceDiagram(typeElements, getOutputFileName(typeElement, "inheritance", ".png"), getOutputFileName(typeElement, "inheritance", ".map"));
        }
    }

    protected void generateInheritanceDiagram(List<? extends ITypeElement> typeElements, String imageFileName, String cmapFileName) throws IOException {
        if (generatePerTypeInheritanceDiagrams || generateFullInheritanceDiagrams) {
            DotGraph dot = new DotGraph();

            dot.append("digraph opp {\r\n" +
                       "   node [fontsize=10,fontname=Helvetica,shape=box,height=.25,style=filled];\r\n" +
                       "   edge [arrowhead=none,arrowtail=empty];\r\n");

            for (ITypeElement typeElement : typeElements) {
                dot.appendNode(typeElement, typeElements.size() == 1);

                if (typeElement instanceof IInterfaceTypeElement) {
                    for (INedTypeElement supertype : ((IInterfaceTypeElement)typeElement).getNedTypeInfo().getLocalInterfaces()) {
                        dot.appendNode(supertype);
                        dot.appendEdge(supertype, typeElement);
                    }
                }
                else {
                    if (typeElement.getSuperType() != null) {
                        ITypeElement extendz = typeElement.getSuperType();
                        dot.appendNode(extendz);
                        dot.appendEdge(extendz, typeElement);
                    }
                }

                if (typeElement instanceof IInterfaceTypeElement) {
                    ArrayList<INedTypeElement> implementors = implementorsMap.get(typeElement);

                    if  (implementors != null) {
                        for (INedTypeElement implementor : implementors) {
                            dot.appendNode(implementor);
                            dot.appendEdge(typeElement, implementor, "style=dashed");
                        }
                    }
                }
                else if (typeElement instanceof INedTypeElement) {
                    INedTypeInfo typeInfo = ((INedTypeElement)typeElement).getNedTypeInfo();
                    Set<INedTypeElement> interfaces = typeInfo.getLocalInterfaces();

                    if  (interfaces != null) {
                        for (INedTypeElement interfaze : interfaces) {
                            dot.appendNode(interfaze);
                            dot.appendEdge(interfaze, typeElement, "style=dashed");
                        }
                    }
                }

                ArrayList<ITypeElement> subtypes = subtypesMap.get(typeElement);

                if (subtypes != null) {
                    for (ITypeElement subtype : subtypes) {
                        dot.appendNode(subtype);
                        dot.appendEdge(typeElement, subtype);
                    }
                }
            }

            dot.append("}");

            generateDotOuput(dot, getOutputFile(imageFileName), "png");
            generateDotOuput(dot, getOutputFile(cmapFileName), "cmap");
            if (APPLY_CC)
                watermark(getOutputFile(imageFileName).toString());

            out("<img src=\"" + imageFileName + "\" ismap=\"yes\" usemap=\"#inheritance-diagram\"/>");
            out("<map name=\"inheritance-diagram\">" + FileUtils.readTextFile(getOutputFile(cmapFileName), null) + "</map>\r\n");
        }
    }

    protected void generateSourceContent(IFile file) throws IOException, CoreException {
        generateSourceContent(FileUtils.readTextFile(file.getContents(), file.getCharset()), nedResources.isNedFile(file));
    }

    protected void generateSourceContent(ITypeElement typeElement) throws IOException {
        generateSourceContent(typeElement.getNedSource(), typeElement instanceof INedTypeElement);
    }

    protected void generateSourceContent(String source, boolean nedSource) throws IOException {
        out("<h3 class=\"subtitle\">Source code:</h3>\r\n" +
            "<pre class=\"src\">");

        org.eclipse.jface.text.Document document = new org.eclipse.jface.text.Document(source);
        ITokenScanner partitioner = nedSource ? new NedSyntaxHighlightPartitionScanner() : new MsgSyntaxHighlightPartitionScanner();
        partitioner.setRange(document, 0, document.getLength());

        StringBuffer buffer = new StringBuffer();
        Object bufferTokenData = null;
        ITokenScanner scanner = null;
        NedPrivateDocColorizerScanner nedPrivateDocColorizerScanner = new NedPrivateDocColorizerScanner();
        NedDocColorizerScanner nedDocColorizerScanner = new NedDocColorizerScanner();
        NedCodeColorizerScanner nedCodeColorizerScanner = new NedCodeColorizerScanner();
        MsgPrivateDocColorizerScanner msgPrivateDocColorizerScanner = new MsgPrivateDocColorizerScanner();
        MsgDocColorizerScanner msgDocColorizerScanner = new MsgDocColorizerScanner();
        MsgCodeColorizerScanner msgCodeColorizerScanner = new MsgCodeColorizerScanner();

        while (true) {
            IToken token = partitioner.nextToken();
            Object data = token.getData();

            if (token.isEOF()) {
                if (buffer.length() != 0)
                    generateSourcePartition(scanner, buffer.toString());

                break;
            }

            if (bufferTokenData != data && buffer.length() != 0) {
                generateSourcePartition(scanner, buffer.toString());
                buffer = new StringBuffer();
                bufferTokenData = null;
            }

            int offset = partitioner.getTokenOffset();
            int length = partitioner.getTokenLength();
            buffer.append(source.substring(offset, offset + length));
            bufferTokenData = data;

            if (nedSource) {
                if (NedSyntaxHighlightPartitionScanner.NED_PRIVATE_DOC.equals(data))
                    scanner = nedPrivateDocColorizerScanner;
                else if (NedSyntaxHighlightPartitionScanner.NED_DOC.equals(data))
                    scanner = nedDocColorizerScanner;
                else
                    scanner = nedCodeColorizerScanner;
            }
            else {
                if (MsgSyntaxHighlightPartitionScanner.MSG_PRIVATE_DOC.equals(data))
                    scanner = msgPrivateDocColorizerScanner;
                else if (MsgSyntaxHighlightPartitionScanner.MSG_DOC.equals(data))
                    scanner = msgDocColorizerScanner;
                else
                    scanner = msgCodeColorizerScanner;
            }
        }

        out("</pre>\r\n");
    }

    protected void generateSourcePartition(ITokenScanner scanner, String source) throws IOException {
        org.eclipse.jface.text.Document partition = new org.eclipse.jface.text.Document(source);
        scanner.setRange(partition, 0, source.length());
        StringBuffer buffer = new StringBuffer();
        Object bufferTokenData = null;

        while (true) {
            IToken token = scanner.nextToken();

            if (token.isEOF()) {
                if (buffer.length() != 0)
                    generateHTMLSpan(buffer.toString(), (TextAttribute)bufferTokenData);

                break;
            }

            Object data = token.getData();

            if (bufferTokenData != data && buffer.length() != 0) {
                generateHTMLSpan(buffer.toString(), (TextAttribute)bufferTokenData);
                buffer = new StringBuffer();
                bufferTokenData = null;
            }

            int offset = scanner.getTokenOffset();
            int length = scanner.getTokenLength();
            buffer.append(source.substring(offset, offset + length));
            bufferTokenData = data;
        }
    }

    protected void generateHTMLSpan(String source, TextAttribute textAttribute) throws IOException {
        out("<span style=\"");
        if (textAttribute != null)
            generateHTMLStyle(textAttribute);
        out("\">");
        out(StringEscapeUtils.escapeHtml4(source));
        out("</span>");
    }

    protected void generateHTMLStyle(TextAttribute textAttribute) throws IOException {
        Color foregroundColor = textAttribute.getForeground();
        Color backgroundColor = textAttribute.getBackground();

        if (backgroundColor != null) {
            out("background-color: ");
            generateHTMLColor(backgroundColor);
            out("; ");
        }

        if (foregroundColor != null) {
            out("color: ");
            generateHTMLColor(foregroundColor);
            out("; ");
        }

        int style = textAttribute.getStyle();

        if ((style & SWT.ITALIC) != 0)
            out("font-style: italic; ");

        if ((style & SWT.BOLD) != 0)
            out("font-weight: bold; ");
    }

    protected void generateHTMLColor(Color color) throws IOException {
        out("#" + colorToHexString(color.getRed()) + colorToHexString(color.getGreen()) + colorToHexString(color.getBlue()));
    }

    protected String colorToHexString(int number) {
        return ((number < 16) ? "0" : "") + Integer.toHexString(number);

    }

    protected void generateDotOuput(DotGraph dot, File outputFile, String format) throws IOException {
        if (dotExecutablePath == null || !new File(dotExecutablePath).exists())
            throw new IllegalStateException("The GraphViz Dot executable path is invalid, set it using Window/Preferences...\nThe currently set path is: " + dotExecutablePath);

        // dot has a width/height limit of 32768 pixels, see bug #149.
        ProcessUtils.exec(dotExecutablePath, new String[] {"-T" + format, "-Gsize=300,300", "-Gdpi=96" ,"-o", outputFile.toString()}, ".", dot.toString(), 10, monitor);
    }

    protected String getParamTypeAsString(ParamElementEx param) {
        String type = param.getAttribute(GateElementEx.ATT_TYPE);

        if (type == null)
            return "numeric";
        else
            return type;
    }

    protected String getPropertyElementValue(PropertyElementEx propertyElement, String key) {
        StringBuffer result = new StringBuffer();
        for (PropertyKeyElement propertyKey = propertyElement.getFirstPropertyKeyChild(); propertyKey != null; propertyKey = propertyKey.getNextPropertyKeySibling()) {
            if (propertyKey.getName().equals(key)) {
                for (LiteralElement literal = propertyKey.getFirstLiteralChild(); literal != null; literal = literal.getNextLiteralSibling()) {
                    if (result.length() != 0)
                        result.append(", ");
                    result.append(literal.getValue());
                }
            }
        }
        return result.toString();
    }

    /**
     * Returns the comment of the given NED element with '@include' directives expanded.
     */
    protected String getExpandedComment(INedElement element) throws IOException {
        if (commentCache.containsKey(element))
            return commentCache.get(element);

        String comment = element.getComment();
        if (comment != null && comment.contains("@include")) {
            Stack<File> parents = new Stack<File>();
            IFile file = getNedOrMsgFile(element);
            parents.push(file.getLocation().toFile().getCanonicalFile());
            comment = processIncludes(comment, parents);
        }
        commentCache.put(element, comment);
        return comment;
    }

    private String getFileContent(File file, Stack<File> parents) {
        try {
            if (includedFileCache.containsKey(file))
                return includedFileCache.get(file);

            int index = parents.indexOf(file);
            if (index >= 0) {
                StringBuilder sb = new StringBuilder();
                sb.append("Circular @includes: ");
                for (int i = index; i < parents.size(); ++i)
                    sb.append(parents.get(i).getPath()).append(" --> ");
                sb.append(file.getPath());

                throw new NeddocException(sb.toString());
            }

            parents.push(file);
            String content = FileUtils.readTextFile(file, null);
            // add '// ' to the beginning of lines
            content = content.replaceAll("(?m)^(.*)$", "// $1");
            content = processIncludes(content, parents);
            parents.pop();
            includedFileCache.put(file, content);
            return content;
        }
        catch (IOException e) {
            throw new NeddocException("Cannot include file: " + file.getAbsolutePath(), e);
        }
    }

    private String processIncludes(String comment, Stack<File> parents) throws IOException {
        if (comment.contains("@include")) {
            Matcher matcher = includePattern.matcher(comment);
            StringBuffer buffer = new StringBuffer();

            while (matcher.find()) {
                File dir = parents.peek().getParentFile();
                File file = new File(dir, matcher.group(1)).getCanonicalFile();
                String content = getFileContent(file, parents);
                matcher.appendReplacement(buffer, content.replace("\\", "\\\\").replace("$", "\\$"));
            }

            matcher.appendTail(buffer);
            comment = buffer.toString();
        }
        return comment;
    }

    protected void out(String string) throws IOException {
        if (monitor.isCanceled())
            throw new CancellationException();

        currentOutputStream.write(string.getBytes("UTF-8"));
    }

    protected void out(byte[] data) throws IOException {
        if (monitor.isCanceled())
            throw new CancellationException();

        currentOutputStream.write(data);
    }

    protected void outMapReference(INedTypeElement model, IFigure figure) throws IOException {
        //FigureUtils.debugPrintFigureAncestors(figure, "");
        Rectangle bounds = new Rectangle(figure.getBounds());
        if (figure instanceof SubmoduleFigure) {
            // translate it to the compound module type figure's coordinate system
            IFigure f = figure;
            while (!(f instanceof CompoundModuleTypeFigure)) {
                f.translateToParent(bounds);
                f = f.getParent();
                Assert.isTrue(f != null);
            }
        }
        outMapReference(model, bounds);
    }

    protected void outMapReference(INedTypeElement typeElement, Rectangle bounds) throws IOException {
        outMapReference(typeElement, bounds.x, bounds.y, bounds.x + bounds.width, bounds.y + bounds.height);
    }

    protected void outMapReference(INedTypeElement typeElement, int x1, int y1, int x2, int y2) throws IOException {
        String name = typeElement.getName();
        String url = getOutputFileName(typeElement);
        out("<area shape=\"rect\" href=\"" + url + "\" title=\"" + name + "\" alt=\"" + name + "\" coords=\"" + x1 + "," + y1 + "," + x2 + "," + y2 + "\">\r\n");
    }

    protected IPath getOutputFilePath(IFile file) {
        return new Path("");
    }

    protected IPath getOutputFilePath(ITypeElement typeElement) {
        IFile file = getNedOrMsgFile(typeElement);

        if (file != null) {
            IProject elementProject = file.getProject();

            if (!elementProject.equals(project))
                return new Path("../").append(neddocRelativeRootPath).append(elementProject.getName()).append(rootRelativeNeddocPath);
        }

        return new Path("");
    }

    protected String getOutputFileName(IFile file) {
        return getOutputFilePath(file).append(file.getProjectRelativePath().toString().replace("/", "-")) + ".html";
    }

    protected String getOutputFileName(ITypeElement typeElement) {
        return getOutputFileName(typeElement, null, null);
    }

    protected String getOutputFileName(ITypeElement typeElement, String discriminator, String extension) {
        String fileName = "";

        if (typeElement instanceof INedTypeElement)
            fileName += ((INedTypeElement)typeElement).getNedTypeInfo().getFullyQualifiedName();
        else if (typeElement instanceof IMsgTypeElement) {
            MsgFileElementEx msgFileElement = typeElement.getContainingMsgFileElement();
            IFile file = msgResources.getMsgFile(msgFileElement);
            fileName += file.getProjectRelativePath().removeLastSegments(1).toPortableString().replace('/', '-') + "-" + typeElement.getName();
        }

        if (discriminator != null)
            fileName += "-" + discriminator;

        if (extension != null)
            fileName += extension;
        else
            fileName += ".html";

        return getOutputFilePath(typeElement).append(fileName).toString();
    }

    protected File getOutputFile(String relativePath) {
        return getFullNeddocPath().append(relativePath).toFile();
    }

    protected IFile getNedOrMsgFile(INedElement element) {
        NedFileElementEx nedFileElement = element.getContainingNedFileElement();

        if (nedFileElement != null)
            return nedResources.getNedFile(nedFileElement);

        MsgFileElementEx msgFileElement = element.getContainingMsgFileElement();

        if (msgFileElement != null)
            return msgResources.getMsgFile(msgFileElement);

        return null;
    }

    protected IPath getReversePath(IPath path) {
        return new Path(StringUtils.repeat("../", path.segmentCount()));
    }

    protected IPath getRelativePath(IPath sourcePath, IPath targetPath) {
        if (sourcePath.equals(targetPath))
            return new Path(".");
        else {
            int commonPrefixLength = targetPath.matchingFirstSegments(sourcePath);
            return new Path(StringUtils.removeEnd(StringUtils.repeat("../", commonPrefixLength), "/")).append(targetPath.removeFirstSegments(commonPrefixLength));
        }
    }

    protected IPath getFullNeddocPath() {
        return documentationRootPath.append(rootRelativeNeddocPath);
    }

    protected IPath getFullDoxyPath() {
        return documentationRootPath.append(rootRelativeDoxyPath);
    }

    protected interface Runnable {
        public void run() throws Exception;
    }

    protected class DotGraph {
        StringBuffer buffer = new StringBuffer();

        Set<ITypeElement> nodes = new HashSet<ITypeElement>();

        Set<Pair<ITypeElement, ITypeElement>> edges = new HashSet<Pair<ITypeElement, ITypeElement>>();

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
                append("\"" + name + "\" ");

                append("[URL=\"" + getOutputFileName(typeElement) + "\",");

                String color = "#ff0000";
                if (typeElement instanceof CompoundModuleElementEx && ((CompoundModuleElementEx)typeElement).isNetwork())
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
                else if (typeElement instanceof PacketElementEx)
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
                append("\"" + sourceTypeElement.getName() + "\" -> \"" + targetTypeElement.getName() + "\" [" + attributes + "];\n");
            }
        }

        @Override
        public String toString() {
            return buffer.toString();
        }
    }
}
