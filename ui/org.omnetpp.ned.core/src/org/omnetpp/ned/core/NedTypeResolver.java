package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
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
    protected Map<IProject, Map<INedTypeInfo, Map<String, INedTypeInfo>>> nedLikeTypeLookupCache = new HashMap<IProject, Map<INedTypeInfo,Map<String,INedTypeInfo>>>();

    // file element to contain built-in declarations (does not correspond to any physical file)
    protected NedFileElementEx builtInDeclarationsFile;

    protected static class ProjectData {
        // NED Source Folders for the project (contents of the .nedfolders file)
        IContainer[] nedSourceFolders;

        // the start package for each NED Source Folder (as defined in root package.ned files)
        // note: array must be the same size as nedSourceFolders
        String[] nedSourceFolderPackages;

        // list of disabled package subtree
        String[] excludedPackageRoots;

        // all projects we reference, directly or indirectly
        IProject[] referencedProjects;

        // non-duplicate toplevel (non-inner) types; keys are fully qualified names
        final Map<String, INedTypeInfo> components = new HashMap<String, INedTypeInfo>();

        // duplicate toplevel (non-inner) types; keys are fully qualified names
        final Map<String, List<INedTypeElement>> duplicates = new HashMap<String, List<INedTypeElement>>();

        // reserved (used) fully qualified names (contains all names including duplicates)
        final Set<String> reservedNames = new HashSet<String>();

        @Override
        public String toString() {
            return "references: " + StringUtils.join(referencedProjects, ",") +
                    "  nedfolders: " + StringUtils.join(nedSourceFolders, ",") +
                    "  nedfolderpackages: " + StringUtils.join(nedSourceFolderPackages, ",") +
                    "  excludedpackages: " + StringUtils.join(excludedPackageRoots, "," );
        }
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
            nedFiles.put(file, (NedFileElementEx)other.nedFiles.get(file).deepDup(this, true, true));

        // fill in reverse mapping
        for (Map.Entry<IFile, NedFileElementEx> entry : nedFiles.entrySet())
            nedElementFiles.put(entry.getValue(), entry.getKey());

        // clone other fields (note: no need to clone nedTypeLookupCache)
        builtInDeclarationsFile = (NedFileElementEx) other.builtInDeclarationsFile.deepDup(this, true, true);
        lastChangeSerial = other.lastChangeSerial;

        // clone projects table
        for (IProject project : other.projects.keySet()) {
            ProjectData projectData = other.projects.get(project);
            ProjectData newProjectData = new ProjectData();
            newProjectData.nedSourceFolders = projectData.nedSourceFolders.clone();
            newProjectData.nedSourceFolderPackages = projectData.nedSourceFolderPackages.clone();
            newProjectData.excludedPackageRoots = projectData.excludedPackageRoots.clone();
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
        Assert.isTrue(typeInfo.getResolver() == this, "cannot use another resolver for lookups than the one that created the element");
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
        Assert.isTrue(nedFileElement.getResolver() == this, "cannot use another resolver for lookups than the one that created the element");
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
                long markerElementId = ((Long)attr).longValue();
                if (!recursive)
                    matches = element.getId()==markerElementId;
                else
                    matches = element.findElementWithId(markerElementId) != null;

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

    public synchronized Collection<INedTypeInfo> getToplevelNedTypesFromAllProjects() {
        return getToplevelNedTypesFromAllProjects(ALL_FILTER);
    }

    public synchronized Collection<INedTypeInfo> getToplevelNedTypesFromAllProjects(IPredicate predicate) {
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

    public synchronized Set<String> getToplevelNedTypeQNamesFromAllProjects() {
        // return everything from everywhere
        Set<String> result = new HashSet<String>();
        for (IFile file : nedFiles.keySet())
            for (INedElement child : nedFiles.get(file))
                if (child instanceof INedTypeElement)
                    result.add(((INedTypeElement)child).getNedTypeInfo().getFullyQualifiedName());
        return result;
    }

    public synchronized Set<INedTypeInfo> getToplevelNedTypesFromAllProjects(String qualifiedName) {
        Set<INedTypeInfo> result = new HashSet<INedTypeInfo>();
        for (IProject project : projects.keySet()) {
            INedTypeInfo type = getToplevelNedType(qualifiedName, project);
            if (type != null)
                result.add(type);
        }
        return result;
    }

    public synchronized Collection<INedTypeInfo> getToplevelNedTypes(IProject context) {
        Collection<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        result.addAll(getToplevelNedTypesInternal(context));
        return result;
    }

    private synchronized Collection<INedTypeInfo> getToplevelNedTypesInternal(IProject context) {
        rehashIfNeeded();
        ProjectData projectData = projects.get(context);
        return projectData==null ? new ArrayList<INedTypeInfo>() : projectData.components.values();
    }

    public synchronized Collection<INedTypeInfo> getToplevelNedTypes(IPredicate predicate, IProject context) {
        Collection<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (INedTypeInfo type : getToplevelNedTypesInternal(context))
            if (predicate.matches(type))
                result.add(type);
        return result;
    }

    public synchronized Collection<INedTypeInfo> getToplevelNedTypesThatImplement(INedTypeInfo interfaceType, IProject context) {
        Assert.isTrue(interfaceType.getResolver() == this, "cannot use another resolver for lookups than the one that created the element");
        Collection<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (INedTypeInfo type : getToplevelNedTypesInternal(context))
            if (type.getInterfaces().contains(interfaceType.getNedElement()))
                result.add(type);
        return result;
    }

    public Collection<INedTypeInfo> getToplevelNedTypesBySimpleName(String simpleName, IProject context) {
        Collection<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        for (INedTypeInfo type : getToplevelNedTypesInternal(context))
            if (type.getName().equals(simpleName))
                result.add(type);
        return result;
    }

    public synchronized Set<String> getToplevelNedTypeQNames(IPredicate predicate, IProject context) {
        Set<String> result = new HashSet<String>();
        for (INedTypeInfo typeInfo : getToplevelNedTypesInternal(context))
            if (predicate.matches(typeInfo))
                result.add(typeInfo.getFullyQualifiedName());
        return result;
    }

    public synchronized Set<String> getToplevelNedTypeQNames(IProject context) {
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
        return getToplevelNedTypeQNames(MODULE_FILTER, context);
    }

    public synchronized Set<String> getNetworkQNames(IProject context) {
        return getToplevelNedTypeQNames(NETWORK_FILTER, context);
    }

    public synchronized Set<String> getChannelQNames(IProject context) {
        return getToplevelNedTypeQNames(CHANNEL_FILTER, context);
    }

    public synchronized Set<String> getModuleInterfaceQNames(IProject context) {
        return getToplevelNedTypeQNames(MODULEINTERFACE_FILTER, context);
    }

    public synchronized Set<String> getChannelInterfaceQNames(IProject context) {
        return getToplevelNedTypeQNames(CHANNELINTERFACE_FILTER, context);
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
        if (typeInfo != null)
            return typeInfo;

        // if not found, try as inner type
        int lastDot = qualifiedName.lastIndexOf('.');
        if (lastDot != -1) {
            INedTypeInfo enclosingType = projectData.components.get(qualifiedName.substring(0,lastDot));
            if (enclosingType != null) {
                INedTypeElement innerType = enclosingType.getInnerTypes().get(qualifiedName.substring(lastDot+1));
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
        Assert.isTrue(lookupContext.getResolver() == this, "cannot use another resolver for lookups than the one that created the element");
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
        Assert.isTrue(interfaceType.getResolver() == this, "cannot use another resolver for lookups than the one that created the element");
        // return cached value if exists, otherwise call doLookupLikeType()
        Map<INedTypeInfo, Map<String, INedTypeInfo>> map = nedLikeTypeLookupCache.get(context);
        if (map == null)
            nedLikeTypeLookupCache.put(context, map = new HashMap<INedTypeInfo, Map<String,INedTypeInfo>>());

        Map<String, INedTypeInfo> map2 = map.get(interfaceType);
        if (map2 == null)
            map.put(interfaceType, map2 = new HashMap<String, INedTypeInfo>());

        INedTypeInfo type = map2.get(name);
        if (type == null && !map2.containsKey(name))
            map2.put(name, type = doLookupLikeType(name, interfaceType, context));
        return type;
    }

    protected INedTypeInfo doLookupLikeType(String name, INedTypeInfo interfaceType, IProject context) {
        if (name.contains(".")) {
            // must be a fully qualified name (as we don't accept partially qualified names)
            return getToplevelNedType(name, context); // note: maybe it does not implement interfaceType, but that needs to be checked elsewhere (it may be quite misleading for the user if we say "no such type" here)
        }
        else {
            // there must be exactly one NED type with that name that implements the given interface
            INedTypeInfo result = null;
            for (INedTypeInfo type : getToplevelNedTypesInternal(context))  // linear search, but it's OK since lookups are cached
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
        Assert.isTrue(lookupContext.getResolver() == this, "cannot use another resolver for lookups than the one that created the element");
        Set<String> result = new HashSet<String>();
        if (lookupContext instanceof NedFileElement) {
            List<INedTypeElement> topLevelTypeNodes = lookupContext.getContainingNedFileElement().getTopLevelTypeNodes();
            for (INedTypeElement element : topLevelTypeNodes) {
                if (predicate.matches(element.getNedTypeInfo()))
                    result.add(element.getNedTypeInfo().getFullyQualifiedName());
            }
        } else {  // CompoundModule - return inner types
            Map<String, INedTypeElement> innerTypes = ((CompoundModuleElementEx)lookupContext).getNedTypeInfo().getInnerTypes();
            for (INedTypeElement element : innerTypes.values()) {
                if (predicate.matches(element.getNedTypeInfo()))
                    result.add(element.getNedTypeInfo().getFullyQualifiedName());
            }
        }

        return result;
    }

    public boolean isNedFile(IResource resource) {
        if (!(resource instanceof IFile) || !NED_EXTENSION.equalsIgnoreCase(((IFile)resource).getFileExtension()))
            return false; // not a .ned file
        IFile file = (IFile) resource;
        String packageName = getPackageFor(file.getParent());
        if (packageName == null)
            return false; // outside all NED source folders
        if (!isPackageEnabled(file.getProject(), packageName))
            return false; // package is disabled
        return true;
    }

    protected boolean isSourceFolderPackageNedFile(IFile file) {
        // assume file has passed the isNedFile() test
        return file.getName().equals(PACKAGE_NED_FILENAME) && ArrayUtils.contains(getNedSourceFolders(file.getProject()), file.getParent());
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

    public String[] getExcludedPackageRoots(IProject project) {
        ProjectData projectData = projects.get(project);
        return projectData == null ? new String[0] : projectData.excludedPackageRoots.clone();
    }

    public boolean isPackageEnabled(IProject project, String packageName) {
        ProjectData projectData = projects.get(project);
        if (projectData == null)
            return true;
        for (String exclusionRoot : projectData.excludedPackageRoots)
            if (isSubpackageOf(exclusionRoot, packageName))
                return false;
        return true;
    }

    public String getPackageFor(IContainer folder) {
        // find project data
        IProject project = folder.getProject();
        ProjectData projectData = projects.get(project);
        if (projectData == null)
            return null;

        // find which NED source folder covers this folder
        int nedSourceFolderIndex = ArrayUtils.INDEX_NOT_FOUND;
        int numLevels = 0;
        for (IContainer container = folder; !(container instanceof IWorkspaceRoot); container = container.getParent(), numLevels++)
            if ((nedSourceFolderIndex = ArrayUtils.indexOf(projectData.nedSourceFolders, container)) != ArrayUtils.INDEX_NOT_FOUND)
                break;
        if (nedSourceFolderIndex == ArrayUtils.INDEX_NOT_FOUND)
            return null; // none

        // assemble package name from NED source folder's package (define by package.ned) and the extra folder levels
        StringBuilder result = new StringBuilder(40);
        result.append(projectData.nedSourceFolderPackages[nedSourceFolderIndex]);
        IPath path = folder.getFullPath();
        int pathLength = path.segmentCount();
        for (int i = pathLength-numLevels; i < pathLength; i++) {  // append last numLevels path segments
            if (result.length() != 0)
                result.append('.');
            result.append(path.segment(i));
        }
        return result.toString();
    }

    public String getExpectedPackageFor(IFile file) {
        if (file.getName().equals(PACKAGE_NED_FILENAME)) {
            IContainer sourceFolder = getNedSourceFolderFor(file);
            if (sourceFolder == null)
                return null; // bad NED file
            if (sourceFolder.equals(file.getParent()))
                return null; // nothing is expected: this file defines the package
        }

        return getPackageFor(file.getParent());
    }

    public IContainer[] getFoldersForPackage(IProject project, String packageName) {
        if (!packageName.matches("([A-Za-z0-9_]+\\.)*[A-Za-z0-9_]+"))
            throw new IllegalArgumentException("Invalid package name: " + packageName);

        ProjectData projectData = projects.get(project);
        if (projectData == null)
            return new IContainer[0];

        List<IContainer> result = new ArrayList<IContainer>();
        for (int i=0; i<projectData.nedSourceFolders.length; i++) {
            String sourceFolderPackage = projectData.nedSourceFolderPackages[i];
            if (packageName.equals(sourceFolderPackage)) {
                result.add(projectData.nedSourceFolders[i]);
            }
            else if (isSubpackageOf(sourceFolderPackage, packageName)) {
                String packageSuffix = packageName.substring(sourceFolderPackage.length());
                IContainer folder = projectData.nedSourceFolders[i].getFolder(new Path(packageSuffix.replace('.', '/')));
                result.add(folder);
            }
        }
        return result.toArray(new IContainer[]{});
    }

    protected static boolean isSubpackageOf(String pkg, String subpkg) {
        // e.g. pkg="inet.examples", and subpkg="inet.examples.wireless.handover"
        return pkg.equals("") ? true : subpkg.startsWith(pkg) && (pkg.length()==subpkg.length() || subpkg.charAt(pkg.length())=='.');
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
            Debug.println("  " + project.getName() + ": " + projectData.toString());
        }
    }
}