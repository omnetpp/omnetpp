package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.engine.NEDParser;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.INedErrorStore;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.SysoutNedErrorStore;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.NedFileElement;

/**
 * Default implementation of INedTypeResolver
 * 
 * @author Andras
 */
public class NedTypeResolver implements INedTypeResolver {
    protected boolean debug = true;

    protected final Map<IFile, NedFileElementEx> nedFiles = new HashMap<IFile, NedFileElementEx>();
    protected final Map<NedFileElementEx, IFile> nedElementFiles = new HashMap<NedFileElementEx,IFile>();
    protected final Map<IProject,ProjectData> projects = new HashMap<IProject, ProjectData>();
    protected long lastChangeSerial = 1;
    protected Map<INedTypeLookupContext, Map<String, INedTypeInfo>> nedTypeLookupCache = new HashMap<INedTypeLookupContext, Map<String,INedTypeInfo>>();

    // file element to contain built-in declarations (does not correspond to any physical file)
    protected NedFileElementEx builtInDeclarationsFile;

    protected static class ProjectData {
        // NED Source Folders for the project (contents of the .nedfolders file)
        IContainer[] nedSourceFolders;
        
        // all projects we reference, directly or indirectly
        IProject[] referencedProjects;
        
        // non-duplicate toplevel (non-inner) types; keys are fully qualified names
        final Map<String, INedTypeInfo> components = new HashMap<String, INedTypeInfo>();
        
        // duplicate toplevel (non-inner) types; keys are fully qualified names
        final Map<String, List<INedTypeElement>> duplicates = new HashMap<String, List<INedTypeElement>>();
        
        // reserved (used) fully qualified names (contains all names including duplicates)
        final Set<String> reservedNames = new HashSet<String>();
    }

    /**
     * Empty constructor, for subclasses
     */
    protected NedTypeResolver() {
    }

    /**
     * Cloning constructor, for getImmutableCopy()
     */
    protected NedTypeResolver(NedTypeResolver other) {
        // clone NED file parse trees
        for (IFile file : other.nedFiles.keySet())
            nedFiles.put(file, (NedFileElementEx)other.nedFiles.get(file).deepDup(this, true));

        // fill in reverse mapping
        for (Map.Entry<IFile, NedFileElementEx> entry : nedFiles.entrySet())
            nedElementFiles.put(entry.getValue(), entry.getKey());

        // clone other fields (note: no need to clone nedTypeLookupCache)
        builtInDeclarationsFile = (NedFileElementEx) other.builtInDeclarationsFile.deepDup(this, true);
        lastChangeSerial = other.lastChangeSerial;
        
        // clone projects table
        for (IProject project : other.projects.keySet()) {
            ProjectData projectData = other.projects.get(project);
            ProjectData newProjectData = new ProjectData();
            newProjectData.nedSourceFolders = projectData.nedSourceFolders.clone();
            newProjectData.referencedProjects = projectData.referencedProjects.clone();
            projects.put(project, newProjectData);
        }
        
        internalRehash();
    }

    protected void internalRehash() {
        long startMillis = System.currentTimeMillis();

        // clear tables and re-register built-in declarations for all projects
        for (ProjectData projectData : projects.values()) {
            projectData.components.clear();
            projectData.duplicates.clear();
            projectData.reservedNames.clear();
            for (INedElement child : builtInDeclarationsFile) {
                if (child instanceof INedTypeElement) {
                    INedTypeInfo typeInfo = ((INedTypeElement)child).getNedTypeInfo();
                    projectData.components.put(typeInfo.getFullyQualifiedName(), typeInfo);
                }
            }
            projectData.reservedNames.addAll(projectData.components.keySet());
        }

        // register NED types in all projects
        for (IProject project : projects.keySet()) {
            ProjectData projectData = projects.get(project);

            // find NED types in each file, and register them
            for (IFile file : nedFiles.keySet()) {
                // file must be in this project or a referenced project
                if (file.getProject().equals(project) || ArrayUtils.contains(projectData.referencedProjects, file.getProject())) {

                    // collect toplevel types from the NED file, and process them one by one
                    for (INedElement child : nedFiles.get(file)) {
                        if (child instanceof INedTypeElement) {
                            INedTypeElement typeElement = (INedTypeElement) child;
                            Assert.isTrue(typeElement.getResolver() == this);
                            INedTypeInfo typeInfo = typeElement.getNedTypeInfo();
                            String qualifiedName = typeInfo.getFullyQualifiedName();

                            if (projectData.reservedNames.contains(qualifiedName)) {
                                if (!projectData.duplicates.containsKey(qualifiedName))
                                    projectData.duplicates.put(qualifiedName, new ArrayList<INedTypeElement>());
                                projectData.duplicates.get(qualifiedName).add(typeElement);
                            }
                            else {
                                // normal case: not duplicate. Add the type info to our tables.
                                projectData.components.put(qualifiedName, typeInfo);
                            }

                            // add to the name list even if it was duplicate
                            projectData.reservedNames.add(qualifiedName);
                        }
                    }
                }
            }

            // now we should remove all types that were duplicates
            for (String name : projectData.duplicates.keySet()) {
                projectData.duplicates.get(name).add(projectData.components.get(name).getNedElement());
                projectData.components.remove(name);
            }

            if (debug)
                Debug.println("types in project " + project.getName() + ": " + StringUtils.join(projectData.components.keySet(), ", ", " and "));

        }

        if (debug) {
            long dt = System.currentTimeMillis() - startMillis;
            Debug.println("rehash(): " + dt + "ms, " + nedFiles.size() + " files, " + projects.size() + " projects");
        }
        
    }

    /**
     * Create channel and interface types that are predefined in NED.
     */
    protected void createBuiltInNedTypes() {
        String source = NEDParser.getBuiltInDeclarations();
        INedErrorStore errorStore = new SysoutNedErrorStore();
        builtInDeclarationsFile = NedTreeUtil.parseNedText(source, errorStore, "[builtin-declarations]", this);
        Assert.isTrue(errorStore.getNumProblems()==0);
    }

    public boolean isBuiltInDeclaration(INedTypeInfo typeInfo) {
        return typeInfo.getNedElement().getContainingNedFileElement() == builtInDeclarationsFile;
    }

    public ArrayList<INedTypeInfo> getBuiltInDeclarations() {
        ArrayList<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (INedElement child : builtInDeclarationsFile)
            if (child instanceof INedTypeElement)
                result.add(((INedTypeElement)child).getNedTypeInfo());
   
        return result;
    }

    public long getLastChangeSerial() {
        return lastChangeSerial;
    }

    public INedTypeInfo createTypeInfoFor(INedTypeElement node) {
        return new NedTypeInfo(node);
    }

    public synchronized Set<IFile> getNedFiles() {
        return nedFiles.keySet();
    }

    public synchronized Set<IFile> getNedFiles(IProject project) {
        Set<IFile> files = new HashSet<IFile>();
    
        for (IFile file : nedFiles.keySet())
            if (project.equals(file.getProject()))
                files.add(file);
    
        return files;
    }

    public synchronized boolean containsNedFileElement(IFile file) {
        return nedFiles.containsKey(file);
    }

    public synchronized NedFileElementEx getNedFileElement(IFile file) {
        Assert.isTrue(nedFiles.containsKey(file), "file is not a NED file, or not parsed yet");
        return nedFiles.get(file);
    }

    public synchronized IFile getNedFile(NedFileElementEx nedFileElement) {
        Assert.isTrue(nedElementFiles.containsKey(nedFileElement) || nedFileElement==builtInDeclarationsFile, "NedFileElement is not in the resolver");
        return nedElementFiles.get(nedFileElement);
    }

    public IMarker[] getMarkersForElement(INedElement element, boolean recursive, int limit) {
        try {
            IFile file = getNedFile(element.getContainingNedFileElement());
            List<IMarker> result = new ArrayList<IMarker>();
            for (IMarker marker : file.findMarkers(IMarker.PROBLEM, true, IFile.DEPTH_ZERO)) {
                // determine whether this marker belongs to the element in question
                Object attr = marker.getAttribute(NedMarkerErrorStore.NEDELEMENT_ID);
                boolean matches = false;
                if (attr instanceof Long) {
                    long markerElementId = ((Long)attr).longValue();
                    if (!recursive)
                        matches = element.getId()==markerElementId;
                    else
                        matches = element.findElementWithId(markerElementId) != null;
                }
                else if (attr instanceof long[]) {
                    long[] markerElementIds = (long[])attr;
                    if (!recursive)
                        matches = ArrayUtils.contains(markerElementIds, element.getId());
                    else 
                        for (long id : markerElementIds)
                            if (element.findElementWithId(id) != null)
                                matches = true;
                }

                // if so, collect this marker
                if (matches)
                    result.add(marker);
    
                // skip the remaining after reaching limit
                if (result.size() >= limit)
                    break;
            }
            return result.toArray(new IMarker[]{});
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
    }

    public synchronized INedElement getNedElementAt(IFile file, int line, int column) {
        return getNedElementAt(getNedFileElement(file), line, column);
    }

    public synchronized INedElement getNedElementAt(INedElement parent, int line, int column) {
        for (INedElement child : parent)
            if (child.getSourceRegion() != null && child.getSourceRegion().contains(line, column))
                return getNedElementAt(child, line, column);
        return parent.getSourceRegion() != null && parent.getSourceRegion().contains(line, column) ? parent : null;
    }

    public synchronized Collection<INedTypeInfo> getNedTypesFromAllProjects() {
        return getNedTypesFromAllProjects(ALL_FILTER);
    }

    public synchronized Collection<INedTypeInfo> getNedTypesFromAllProjects(IPredicate predicate) {
        // return everything from everywhere, including duplicates
        List<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (IFile file : nedFiles.keySet())
            for (INedElement child : nedFiles.get(file))
                if (child instanceof INedTypeElement) {
                    INedTypeInfo typeInfo = ((INedTypeElement)child).getNedTypeInfo();
                    if (predicate.matches(typeInfo))
                        result.add(typeInfo);
                }
        return result;
    }

    public synchronized Set<String> getNedTypeQNamesFromAllProjects() {
        // return everything from everywhere
        Set<String> result = new HashSet<String>();
        for (IFile file : nedFiles.keySet())
            for (INedElement child : nedFiles.get(file))
                if (child instanceof INedTypeElement)
                    result.add(((INedTypeElement)child).getNedTypeInfo().getFullyQualifiedName());
        return result;
    }

    public synchronized Set<INedTypeInfo> getNedTypesFromAllProjects(String qualifiedName) {
        Set<INedTypeInfo> result = new HashSet<INedTypeInfo>();
        for (IProject project : projects.keySet()) {
            INedTypeInfo type = getToplevelOrInnerNedType(qualifiedName, project);
            if (type != null)
                result.add(type);
        }
        return result;
    }

    public synchronized Collection<INedTypeInfo> getNedTypes(IProject context) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        return projectData==null ? new ArrayList<INedTypeInfo>() : projectData.components.values();
    }

    public synchronized Collection<INedTypeInfo> getNedTypes(IPredicate predicate, IProject context) {
        Collection<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (INedTypeInfo type : getNedTypes(context))
            if (predicate.matches(type))
                result.add(type);
        return result;
    }

    public synchronized Collection<INedTypeInfo> getNedTypesThatImplement(INedTypeInfo interfaceType, IProject context) {
        Collection<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (INedTypeInfo type : getNedTypes(context))
            if (type.getInterfaces().contains(interfaceType.getNedElement()))
                result.add(type);
        return result;
    }

    public synchronized Set<String> getNedTypeQNames(IPredicate predicate, IProject context) {
        Set<String> result = new HashSet<String>();
        for (INedTypeInfo typeInfo : getNedTypes(context))
            if (predicate.matches(typeInfo))
                result.add(typeInfo.getFullyQualifiedName());
        return result;
    }

    public synchronized Set<String> getNedTypeQNames(IProject context) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        return projectData==null ? new HashSet<String>() : projectData.components.keySet();
    }

    public synchronized Set<String> getReservedQNames(IProject context) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        return projectData==null ? new HashSet<String>() : projectData.reservedNames;
    }

    public synchronized Set<String> getReservedNames(IProject context, String packageName) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        Set<String> result = new HashSet<String>();
        if (projectData != null) {
            String packagePrefix = StringUtils.isEmpty(packageName) ? "" : packageName + ".";
            for (String qualifiedName : projectData.reservedNames)
                if (qualifiedName.startsWith(packagePrefix))
                    result.add(StringUtils.removeStart(qualifiedName, packagePrefix));
        }
        return result;
    }

    public synchronized Set<String> getModuleQNames(IProject context) {
        return getNedTypeQNames(MODULE_FILTER, context);
    }

    public synchronized Set<String> getNetworkQNames(IProject context) {
        return getNedTypeQNames(NETWORK_FILTER, context);
    }

    public synchronized Set<String> getChannelQNames(IProject context) {
        return getNedTypeQNames(CHANNEL_FILTER, context);
    }

    public synchronized Set<String> getModuleInterfaceQNames(IProject context) {
        return getNedTypeQNames(MODULEINTERFACE_FILTER, context);
    }

    public synchronized Set<String> getChannelInterfaceQNames(IProject context) {
        return getNedTypeQNames(CHANNELINTERFACE_FILTER, context);
    }

    public synchronized INedTypeInfo getToplevelNedType(String qualifiedName, IProject context) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        return projectData==null ? null : projectData.components.get(qualifiedName);
    }

    public synchronized INedTypeInfo getToplevelOrInnerNedType(String qualifiedName, IProject context) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        if (projectData == null)
            return null;
    
        // try as toplevel type
        INedTypeInfo typeInfo = projectData.components.get(qualifiedName);
    
        // if not found, try as inner type
        if (typeInfo == null && qualifiedName.contains(".")) {
            INedTypeInfo enclosingType = projectData.components.get(StringUtils.substringBeforeLast(qualifiedName, "."));
            if (enclosingType != null) {
                INedTypeElement innerType = enclosingType.getInnerTypes().get(StringUtils.substringAfterLast(qualifiedName, "."));
                if (innerType != null)
                    typeInfo = innerType.getNedTypeInfo();
            }
        }
        return typeInfo;
    }

    public synchronized String getSimplePropertyFor(NedFileElementEx nedFileElement, String propertyName) {
        PropertyElementEx property = getPropertyFor(nedFileElement, propertyName);
        return property != null ? property.getSimpleValue() : null;
    }

    public synchronized String getSimplePropertyFor(IContainer folder, String propertyName) {
        PropertyElementEx property = getPropertyFor(folder, propertyName);
        return property != null ? property.getSimpleValue() : null;
    }

    public synchronized PropertyElementEx getPropertyFor(NedFileElementEx nedFileElement, String propertyName) {
        // look into this file, then into package.ned files in this folder and up
        PropertyElementEx property = NedElementUtilEx.getProperty(nedFileElement, propertyName, null);
        if (property != null)
            return property;
        return getPropertyFor(getNedFile(nedFileElement).getParent(), propertyName);
    }

    public synchronized PropertyElementEx getPropertyFor(IContainer folder, String propertyName) {
        // look for package.ned in this folder and up
        IContainer sourceFolder = getNedSourceFolderFor(folder);
        while (true) {
            IFile packageFile = folder.getFile(new Path(PACKAGE_NED_FILENAME));
            if (packageFile.exists()) {
                NedFileElementEx nedFileElement = getNedFileElement(packageFile);
                PropertyElementEx property = NedElementUtilEx.getProperty(nedFileElement, propertyName, null);
                if (property != null)
                    return property;
            }
            if (folder.equals(sourceFolder) || folder instanceof IProject)
                break;
            folder = folder.getParent();
        }
        return null;
    }

    public synchronized INedTypeInfo lookupNedType(String name, INedTypeLookupContext lookupContext) {
        // return cached value if exists, otherwise call doLookupNedType()
        Map<String, INedTypeInfo> map = nedTypeLookupCache.get(lookupContext);
        if (map == null)
            nedTypeLookupCache.put(lookupContext, map = new HashMap<String, INedTypeInfo>());
        INedTypeInfo typeInfo = map.get(name);
        // note: we need to distinguish between "null" meaning "not yet looked up", and
        // "looked up but no such type" (represented as: no such key vs value is null)
        if (typeInfo == null && !map.containsKey(name))
            map.put(name, typeInfo = doLookupNedType(name, lookupContext));
        return typeInfo;
    }

    protected INedTypeInfo doLookupNedType(String name, INedTypeLookupContext lookupContext) {
        rehashIfNeeded();
        Assert.isTrue(lookupContext!=null, "lookupNedType() cannot be called with context==null");
    
        // if (debug) Debug.println("looking up: " + name + " in " + lookupContext.debugString());
    
        // note: this method is to be kept consistent with NEDResourceCache::resolveNedType() in the C++ code
        // note2: partially qualified names are not supported: name must be either simple name or fully qualified
        IProject project = getNedFile(lookupContext.getContainingNedFileElement()).getProject();
        ProjectData projectData = projects.get(project);
        if (projectData == null)  // do not return type if the project is closed
            return null;
        if (name.indexOf('.') != -1) {
            // contains dot, so it is a fully qualified name
            INedTypeInfo type = getToplevelOrInnerNedType(name, project);
            if (type == null)
                return null;
            if (!type.isInnerType())
                return type; // toplevel types are visible from anywhere
            else {
                // inner types are only visible from the same toplevel type
                if (!(lookupContext instanceof CompoundModuleElementEx))
                    return null; // no inner type is visible at file level
                else {
                    CompoundModuleElementEx compoundModule = (CompoundModuleElementEx) lookupContext; 
                    // if lookupContext is an inner type itself, go up to toplevel type to make sure an inner type sees its sibling inner type
                    if (compoundModule.getEnclosingLookupContext() instanceof CompoundModuleElementEx)
                        compoundModule = (CompoundModuleElementEx) compoundModule.getEnclosingLookupContext(); 
                    if (type.getEnclosingType() == compoundModule)
                        return type;
                    else if (compoundModule.getNedTypeInfo().getInnerTypes().values().contains(type.getNedElement()))
                        return type;
                    else 
                        return null;
                }
            }
        }
        else {
            // no dot: name is an unqualified name (simple name); so, it can be:
            // (a) inner type, (b) an exactly imported type, (c) from the same package, (d) a wildcard imported type
    
            // inner type?
            if (lookupContext instanceof CompoundModuleElementEx) {
                // always lookup in the topmost compound module's context because "types:" is not allowed elsewhere
                CompoundModuleElementEx topLevelCompoundModule = (CompoundModuleElementEx)lookupContext.getParent().getSelfOrAncestorWithTag(NedElementTags.NED_COMPOUND_MODULE);
                if (topLevelCompoundModule != null)
                    lookupContext = topLevelCompoundModule;
                INedTypeInfo contextTypeInfo = ((CompoundModuleElementEx)lookupContext).getNedTypeInfo();
                INedTypeElement innerType = contextTypeInfo.getInnerTypes().get(name);
                if (innerType != null)
                    return innerType.getNedTypeInfo();
            }
    
            // exactly imported type?
            // try a shortcut first: if the import doesn't contain wildcards
            List<String> imports = lookupContext.getContainingNedFileElement().getImports();
            for (String importSpec : imports)
                if (projectData.components.containsKey(importSpec) && (importSpec.endsWith("." + name) || importSpec.equals(name)))
                    return projectData.components.get(importSpec);
    
            // from the same package?
            String packagePrefix = lookupContext.getContainingNedFileElement().getQNameAsPrefix();
            INedTypeInfo samePackageType = projectData.components.get(packagePrefix + name);
            if (samePackageType != null)
                return samePackageType;
    
            // try harder, using wildcards
            String nameWithDot = "." + name;
            for (String importSpec : imports) {
                String importRegex = NedElementUtilEx.importToRegex(importSpec);
                for (String qualifiedName : projectData.components.keySet())
                    if ((qualifiedName.endsWith(nameWithDot) || qualifiedName.equals(name)) && qualifiedName.matches(importRegex))
                        return projectData.components.get(qualifiedName);
            }
        }
        return null;
    }

    public INedTypeInfo lookupLikeType(String name, INedTypeInfo interfaceType, IProject context) {
        if (name.contains(".")) {
            // must be a fully qualified name (as we don't accept partially qualified names)
            return getToplevelNedType(name, context);
        }
        else {
            // there must be exactly one NED type with that name that implements the given interface
            INedTypeInfo result = null;
            for (INedTypeInfo type : getNedTypes(context))  //XXX linear search
                if (type.getName().equals(name))
                    if (type.getInterfaces().contains(interfaceType.getNedElement()))
                        if (result != null)
                            return null; // more than one match --> error
                        else
                            result = type;
            return result;
        }
    }

    public synchronized Set<String> getLocalTypeNames(INedTypeLookupContext lookupContext, IPredicate predicate) {
        Set<String> result = new HashSet<String>();
        if (lookupContext instanceof NedFileElement) {
            List<INedTypeElement> topLevelTypeNodes = lookupContext.getContainingNedFileElement().getTopLevelTypeNodes();
            for (INedTypeElement element : topLevelTypeNodes) {
                if (predicate.matches(element.getNedTypeInfo()))
                    result.add(element.getNedTypeInfo().getFullyQualifiedName());
            }
        } else {  // CompounModule - return inner types
            Map<String, INedTypeElement> innerTypes = ((CompoundModuleElementEx)lookupContext).getNedTypeInfo().getInnerTypes();
            for (INedTypeElement element : innerTypes.values()) {
                if (predicate.matches(element.getNedTypeInfo()))
                    result.add(element.getNedTypeInfo().getFullyQualifiedName());
            }
        }
    
        return result;
    }

    public boolean isNedFile(IResource resource) {
        return (resource instanceof IFile &&
                NED_EXTENSION.equalsIgnoreCase(((IFile)resource).getFileExtension()) &&
                getNedSourceFolderFor((IFile)resource) != null);
    }

    public IContainer[] getNedSourceFolders(IProject project) {
        ProjectData projectData = projects.get(project);
        return projectData == null ? new IContainer[0] : projectData.nedSourceFolders;
    }

    public IContainer getNedSourceFolderFor(IFile file) {
        return getNedSourceFolderFor(file.getParent());
    }

    public IContainer getNedSourceFolderFor(IContainer folder) {
        IProject project = folder.getProject();
        ProjectData projectData = projects.get(project);
        if (projectData == null)
            return null;
    
        IContainer[] nedSourceFolders = projectData.nedSourceFolders;
        if (nedSourceFolders.length == 1 && nedSourceFolders[0] == project) // shortcut
            return project;
    
        for (IContainer container = folder; !container.equals(project); container = container.getParent())
            if (ArrayUtils.contains(nedSourceFolders, container))
                return container;
        return null;
    }

    public String getExpectedPackageFor(IFile file) {
        IContainer sourceFolder = getNedSourceFolderFor(file);
        if (sourceFolder == null)
            return null; // bad NED file
        if (sourceFolder.equals(file.getParent()) && file.getName().equals(PACKAGE_NED_FILENAME))
            return null; // nothing is expected: this file defines the package
    
        // first half is the package declared in the root "package.ned" file
        String packagePrefix = "";
        IFile packageNedFile = sourceFolder.getFile(new Path(PACKAGE_NED_FILENAME));
        if (getNedFiles().contains(packageNedFile))
            packagePrefix = StringUtils.nullToEmpty(getNedFileElement(packageNedFile).getPackage());
    
        // second half consists of the directories this file is down from the source folder
        String fileFolderPath = StringUtils.join(file.getParent().getFullPath().segments(), ".");
        String sourceFolderPath = StringUtils.join(sourceFolder.getFullPath().segments(), ".");
        Assert.isTrue(fileFolderPath.startsWith(sourceFolderPath));
        String packageSuffix = fileFolderPath.substring(sourceFolderPath.length());
        if (packageSuffix.length() > 0 && packageSuffix.charAt(0) == '.')
            packageSuffix = packageSuffix.substring(1);
    
        // concatenate
        String packageName = packagePrefix.length()>0 && packageSuffix.length()>0 ?
                packagePrefix + "." + packageSuffix :
                    packagePrefix + packageSuffix;
        return packageName;
    }

    public void rehashIfNeeded() {
        // redefine
    }

    public void addErrorsForDuplicates(INedErrorStore errorStore) {
        // issue error message for duplicates
        for (IProject project : projects.keySet()) {
            ProjectData projectData = projects.get(project);
            for (String name : projectData.duplicates.keySet()) {
                List<INedTypeElement> duplicateList = projectData.duplicates.get(name);
                for (int i = 0; i < duplicateList.size(); i++) {
                    INedTypeElement element = duplicateList.get(i);
                    INedTypeElement otherElement = duplicateList.get(i==0 ? 1 : 0);
                    IFile file = getNedFile(element.getContainingNedFileElement());
                    IFile otherFile = getNedFile(otherElement.getContainingNedFileElement());

                    if (otherFile == null) {
                        errorStore.setFile(file);
                        errorStore.addError(element, element.getReadableTagName() + " '" + name + "' is a built-in type and cannot be redefined");
                    }
                    else {
                        // add error message to both files
                        errorStore.setFile(file);
                        String messageHalf = element.getReadableTagName() + " '" + name + "' already defined in ";
                        errorStore.addError(element, messageHalf + otherFile.getFullPath().toString());
                        errorStore.setFile(otherFile);
                        errorStore.addError(otherElement, messageHalf + file.getFullPath().toString());
                    }
                }
            }
        }
    }
    
    public void dumpProjectsTable() {
        Debug.println(projects.size() + " projects:");
        for (IProject project : projects.keySet()) {
            ProjectData projectData = projects.get(project);
            Debug.println("  " + project.getName() + ":" +
                    "  deps: " + StringUtils.join(projectData.referencedProjects, ",") +
                    "  nedfolders: " + StringUtils.join(projectData.nedSourceFolders, ","));
        }
    }
}