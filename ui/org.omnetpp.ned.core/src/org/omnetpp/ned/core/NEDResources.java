package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.internal.events.ResourceDelta;
import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.engine.NEDParser;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.INEDErrorStore;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeDifferenceUtils;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.SysoutNedErrorStore;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDBeginModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDChangeListenerList;
import org.omnetpp.ned.model.notification.NEDEndModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDFileRemovedEvent;
import org.omnetpp.ned.model.notification.NEDModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;

/**
 * Parses all NED files in the workspace and makes them available for other
 * plugins for consistency checks among NED files etc.
 *
 * It listens to workspace resource changes and modifies its content based on
 * change notifications.
 *
 * @author andras
 */
public class NEDResources implements INEDTypeResolver, IResourceChangeListener {

	private static final String PACKAGE_NED_FILENAME = "package.ned";
    private static final String NED_EXTENSION = "ned";

    // list of objects that listen on *all* NED changes
    private NEDChangeListenerList nedModelChangeListenerList = null;

    // associate IFiles with their NEDElement trees
    private final Map<IFile, NedFileElementEx> nedFiles = new HashMap<IFile, NedFileElementEx>();
    private final Map<NedFileElementEx, IFile> nedElementFiles = new HashMap<NedFileElementEx,IFile>();

    // number of the editors connected to a given NED file
    private final Map<IFile, Integer> connectCount = new HashMap<IFile, Integer>();

    static class ProjectData {
        // NED Source Folders for the project (contents of the .nedfolders file)
        IContainer[] nedSourceFolders;

        // all projects we reference, directly or indirectly
    	IProject[] referencedProjects;

        // non-duplicate toplevel (non-inner) types; keys are fully qualified names
        final Map<String, INEDTypeInfo> components = new HashMap<String, INEDTypeInfo>();

        // duplicate toplevel (non-inner) types; keys are fully qualified names
        final Map<String, List<INedTypeElement>> duplicates = new HashMap<String, List<INedTypeElement>>();

        // reserved (used) fully qualified names (contains all names including duplicates)
        final Set<String> reservedNames = new HashSet<String>();
    }

    // per-project tables. key-set is kept *strictly* up to date with the OMNeT++ projects,
    // so that projects.contains() should be used to determine whether a project is an OMNeT++ project
    private final Map<IProject,ProjectData> projects = new HashMap<IProject, ProjectData>();

    // if tables need to be rebuilt
    private boolean needsRehash = false;

    // We use this counter to increment whenever a rehash occurred. Checks can be made
    // to assert that the function is not called unnecessarily
    private int debugRehashCounter = 0;

    // file element to contain built-in declarations (does not correspond to any physical file)
    private NedFileElementEx builtInDeclarationsFile;

    private boolean nedModelChangeNotificationDisabled = false;


    // utilities for predicate-based filtering of NED types using getAllNedTypes()
    public static class InstanceofPredicate implements IPredicate {
    	private Class<? extends INedTypeElement> clazz;
    	public InstanceofPredicate(Class<? extends INedTypeElement> clazz) {
    		this.clazz = clazz;
    	}
        public boolean matches(INEDTypeInfo component) {
            return clazz.isInstance(component.getNEDElement());
        }
    };
    public static final IPredicate MODULE_FILTER = new InstanceofPredicate(IModuleTypeElement.class);
    public static final IPredicate SIMPLE_MODULE_FILTER = new InstanceofPredicate(SimpleModuleElementEx.class);
    public static final IPredicate COMPOUND_MODULE_FILTER = new InstanceofPredicate(CompoundModuleElementEx.class);
    public static final IPredicate MODULEINTERFACE_FILTER = new InstanceofPredicate(ModuleInterfaceElementEx.class);
    public static final IPredicate CHANNEL_FILTER = new InstanceofPredicate(ChannelElementEx.class);
    public static final IPredicate CHANNELINTERFACE_FILTER = new InstanceofPredicate(ChannelInterfaceElementEx.class);
    public static final IPredicate NETWORK_FILTER = new IPredicate() {
        public boolean matches(INEDTypeInfo component) {
            return component.getNEDElement() instanceof CompoundModuleElementEx &&
                   ((CompoundModuleElementEx)component.getNEDElement()).getIsNetwork();
        }
    };

    // delayed validation job
    private DelayedJob validationJob = new DelayedJob(400) {
		public void run() {
			DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
				public void run() {
					validateAllFiles();
				}
			});
		}
    };

    // listener, so that we don't need to make our nedModelChanged() method public
    private INEDChangeListener nedModelChangeListener = new INEDChangeListener() {
    	public void modelChanged(NEDModelEvent event) {
    		nedModelChanged(event);
    	}
    };

    /**
     * Constructor.
     */
    public NEDResources() {
		NEDElement.setDefaultTypeResolver(this);
        createBuiltInNEDTypes();
    }

    /**
     * Create channel and interface types that are predefined in NED.
     */
    protected void createBuiltInNEDTypes() {
    	String source = NEDParser.getBuiltInDeclarations();
    	INEDErrorStore errorStore = new SysoutNedErrorStore();
    	builtInDeclarationsFile = (NedFileElementEx) NEDTreeUtil.parseNedSource(source, errorStore, "[builtin-declarations]");
    	Assert.isTrue(errorStore.getNumProblems()==0);
    }

    // currently unused
    protected INEDTypeInfo getBuiltInDeclaration(String name) {
    	for (INEDElement child : builtInDeclarationsFile)
    		if (child instanceof INedTypeElement && ((INedTypeElement)child).getName().equals(name))
    			return ((INedTypeElement)child).getNEDTypeInfo();
    	return null;
    }

	public INEDTypeInfo createTypeInfoFor(INedTypeElement node) {
		return new NEDTypeInfo(node);
	}

    public synchronized Set<IFile> getNedFiles() {
        return nedFiles.keySet();
    }

    public synchronized NedFileElementEx getNedFileElement(IFile file) {
    	Assert.isTrue(nedFiles.containsKey(file), "file is not a NED file, or not parsed yet");
        return nedFiles.get(file);
    }

	public synchronized IFile getNedFile(NedFileElementEx nedFileElement) {
    	Assert.isTrue(nedElementFiles.containsKey(nedFileElement) || nedFileElement==builtInDeclarationsFile, "NedFileElement is not in the resolver");
		return nedElementFiles.get(nedFileElement);
	}

    /**
     * NED text editors should call this when editor content changes.
     * Parses the given text, and synchronizes the stored NED model tree to it.
     * @param file - which file should be set
     * @param text - the textual content of the ned file
     */
    public synchronized void setNedFileText(IFile file, String text) {
        NedFileElementEx currentTree = getNedFileElement(file);

        // parse
        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(NEDSYNTAXPROBLEM_MARKERID);
        markerSync.registerFile(file);
        NEDMarkerErrorStore errorStore = new NEDMarkerErrorStore(file, markerSync);
        INEDElement targetTree = NEDTreeUtil.parseNedSource(text, errorStore, file.getLocation().toOSString());

        if (targetTree.getSyntaxProblemMaxCumulatedSeverity() == INEDElement.SEVERITY_NONE) {
        	NEDTreeDifferenceUtils.Applier treeDifferenceApplier = new NEDTreeDifferenceUtils.Applier();
	        NEDTreeDifferenceUtils.applyTreeDifferences(currentTree, targetTree, treeDifferenceApplier);

	        if (treeDifferenceApplier.hasDifferences()) {
	        	// push tree differences into the official tree
	        	System.out.println("pushing text editor changes into NEDResources tree:\n  " + treeDifferenceApplier);
		        currentTree.fireModelEvent(new NEDBeginModelChangeEvent(currentTree));
	        	currentTree.setSyntaxProblemMaxLocalSeverity(INEDElement.SEVERITY_NONE);
		        treeDifferenceApplier.apply();
		        currentTree.fireModelEvent(new NEDEndModelChangeEvent(currentTree));

		        // perform marker synchronization in a background job, to avoid deadlocks
		        markerSync.runAsWorkspaceJob();

		        // force rehash now, so that validation errors appear immediately
		        rehash();
	        }
        }
        else {
        	// mark the tree as having a syntax error, so that the graphical doesn't allow editing
        	currentTree.setSyntaxProblemMaxLocalSeverity(IMarker.SEVERITY_ERROR);

	        // perform marker synchronization in a background job, to avoid deadlocks
	        markerSync.runAsWorkspaceJob();
        }
    }

	public IMarker[] getMarkersForElement(INEDElement node) {
		try {
            IFile file = getNedFile(node.getContainingNedFileElement());
			List<IMarker> result = new ArrayList<IMarker>();
			for (IMarker marker : file.findMarkers(IMarker.PROBLEM, true, IFile.DEPTH_ZERO)) {
				int elementId = marker.getAttribute(NEDMarkerErrorStore.NEDELEMENT_ID, -1);
				if (elementId != -1 && node.findElementWithId(elementId) != null)
					result.add(marker);
			}
			return result.toArray(new IMarker[]{});
		}
		catch (CoreException e) {
			throw new RuntimeException(e);
		}
	}

    public synchronized INEDElement getNedElementAt(IFile file, int line, int column) {
        return getNedElementAt(getNedFileElement(file), line, column);
    }

    public synchronized INEDElement getNedElementAt(INEDElement parent, int line, int column) {
        for (INEDElement child : parent)
        	if (child.getSourceRegion() != null && child.getSourceRegion().contains(line, column))
        		return getNedElementAt(child, line, column);
    	return parent.getSourceRegion() != null && parent.getSourceRegion().contains(line, column) ? parent : null;
    }

	public synchronized Collection<INEDTypeInfo> getNedTypesFromAllProjects() {
		// return everything from everywhere, including duplicates
		List<INEDTypeInfo> result = new ArrayList<INEDTypeInfo>();
		for (IFile file : nedFiles.keySet())
			for (INEDElement child : nedFiles.get(file))
				if (child instanceof INedTypeElement)
					result.add(((INedTypeElement)child).getNEDTypeInfo());
		return result;
	}

    public synchronized Collection<INEDTypeInfo> getNedTypes(IProject context) {
		rehashIfNeeded();
        ProjectData projectData = projects.get(context);
		return projectData==null ? new ArrayList<INEDTypeInfo>() : projectData.components.values();
    }

    public synchronized Set<String> getNedTypeQNames(IPredicate predicate, IProject context) {
        Set<String> result = new HashSet<String>();
        for (INEDTypeInfo typeInfo : getNedTypes(context))
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

	public synchronized INEDTypeInfo getToplevelNedType(String qualifiedName, IProject context) {
		rehashIfNeeded();
		ProjectData projectData = projects.get(context);
		return projectData==null ? null : projectData.components.get(qualifiedName);
	}

	public synchronized INEDTypeInfo getToplevelOrInnerNedType(String qualifiedName, IProject context) {
		rehashIfNeeded();
		ProjectData projectData = projects.get(context);
		if (projectData == null)
			return null;

		// try as toplevel type
		INEDTypeInfo typeInfo = projectData.components.get(qualifiedName);

		// if not found, try as inner type
		if (typeInfo == null && qualifiedName.contains(".")) {
			INEDTypeInfo enclosingType = projectData.components.get(StringUtils.substringBeforeLast(qualifiedName, "."));
			if (enclosingType != null) {
				INedTypeElement innerType = enclosingType.getInnerTypes().get(StringUtils.substringAfterLast(qualifiedName, "."));
				if (innerType != null)
					typeInfo = innerType.getNEDTypeInfo();
			}
		}
		return typeInfo;
	}

    public synchronized INEDTypeInfo lookupNedType(String name, INedTypeLookupContext lookupContext) {
		rehashIfNeeded();
		Assert.isTrue(lookupContext!=null, "lookupNedType() cannot be called with context==null");

		// context is a compound module, so it may be an inner type
		if (lookupContext instanceof CompoundModuleElementEx) {
			INEDTypeInfo contextTypeInfo = ((CompoundModuleElementEx)lookupContext).getNEDTypeInfo();
			if (name.contains(".")) {
				// inner type with fully qualified name?
				String prefix = StringUtils.substringBeforeLast(name, ".");
				String simpleName = StringUtils.substringAfterLast(name, ".");
				if (contextTypeInfo.getFullyQualifiedName().equals(prefix)) {
					INedTypeElement innerType = contextTypeInfo.getInnerTypes().get(simpleName);
					if (innerType != null)
						return innerType.getNEDTypeInfo();
				}
			}
			else {
				INedTypeElement innerType = contextTypeInfo.getInnerTypes().get(name);
				if (innerType != null)
					return innerType.getNEDTypeInfo();
			}
		}

		// not an inner type: look up as toplevel type
		IProject project = getNedFile(lookupContext.getContainingNedFileElement()).getProject();
		ProjectData projectData = projects.get(project);
		if (name.contains(".")) {
			// fully qualified name (as we don't accept partially qualified names)
			if (projectData.components.get(name) != null)
				return projectData.components.get(name);
		}
		else {
			// name is an unqualified name (simple name).

			// from the same package?
			String packagePrefix = lookupContext.getContainingNedFileElement().getQNameAsPrefix();
			INEDTypeInfo samePackageType = projectData.components.get(packagePrefix + name);
			if (samePackageType != null)
				return samePackageType;

			// imported type?
			// try a shortcut first: if the import doesn't contain wildcards
			List<String> imports = lookupContext.getContainingNedFileElement().getImports();
			for (String importSpec : imports)
				if (projectData.components.containsKey(importSpec) && importSpec.endsWith("." + name))
					return projectData.components.get(importSpec);

			// try harder, using wildcards
			for (String importSpec : imports) {
				String importRegex = NEDElementUtilEx.importToRegex(importSpec);
				for (String qualifiedName : projectData.components.keySet())
					if (qualifiedName.matches(importRegex) && qualifiedName.endsWith("." + name))
						return projectData.components.get(qualifiedName);
			}
		}
		return null;
    }

	public synchronized Set<String> getVisibleTypeNames(INedTypeLookupContext lookupContext) {
		return getVisibleTypeNames(lookupContext, new IPredicate() {
			public boolean matches(INEDTypeInfo typeInfo) {return true;}
		});
	}

	public synchronized Set<String> getVisibleTypeNames(INedTypeLookupContext lookupContext, IPredicate predicate) {
		rehashIfNeeded();

		// types from the same package
		String prefix = lookupContext.getQNameAsPrefix();
		String regex = prefix.replace(".", "\\.") + "[^.]+";
		Set<String> result = new HashSet<String>();

		IProject project = getNedFile(lookupContext.getContainingNedFileElement()).getProject();
		ProjectData projectData = projects.get(project);
		for (INEDTypeInfo typeInfo : projectData.components.values())
			if (typeInfo.getFullyQualifiedName().matches(regex) && predicate.matches(typeInfo))
				result.add(typeInfo.getName());

		// imported types
		List<String> imports = lookupContext.getContainingNedFileElement().getImports();
		for (String importSpec : imports) {
			String importRegex = NEDElementUtilEx.importToRegex(importSpec);
			for (INEDTypeInfo typeInfo : projectData.components.values())
				if (typeInfo.getFullyQualifiedName().matches(importRegex) && predicate.matches(typeInfo))
					result.add(typeInfo.getName());
		}
		return result;
	}


    /**
     * Determines if a resource is a NED file, that is, if it should be parsed.
     * It checks the file extension (".ned"), and whether the file is in one of
     * the NED source folders designated for the project.
     */
    public boolean isNEDFile(IResource resource) {
    	return (resource instanceof IFile &&
    			NED_EXTENSION.equalsIgnoreCase(((IFile)resource).getFileExtension()) &&
    			getNedSourceFolderFor((IFile)resource) != null);
    }

    public IContainer[] getNedSourceFolders(IProject project) {
		ProjectData projectData = projects.get(project);
		return projectData == null ? new IContainer[0] : projectData.nedSourceFolders;
    }

    public IContainer getNedSourceFolderFor(IFile file) {
    	IProject project = file.getProject();
		ProjectData projectData = projects.get(project);
		if (projectData == null)
			return null;

		IContainer[] nedSourceFolders = projectData.nedSourceFolders;
		if (nedSourceFolders.length == 1 && nedSourceFolders[0] == project) // shortcut
			return project;

		for (IContainer container = file.getParent(); container != project; container = container.getParent())
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

    /**
     * NED editors should call this when they get opened.
     */
    public synchronized void connect(IFile file) {
        if (connectCount.containsKey(file))
            connectCount.put(file, connectCount.get(file) + 1);
        else {
        	if (!nedFiles.containsKey(file))
        		readNEDFile(file);
            connectCount.put(file, 1);
        }
    }

    /**
     * NED editors should call this when they get closed.
     */
    public synchronized void disconnect(IFile file) {
        int count = connectCount.get(file); // must exist
        if (count <= 1) {
            // there's no open editor -- remove counter and re-read last saved
            // state from disk
            connectCount.remove(file);
            readNEDFile(file);
        }
        else {
            connectCount.put(file, count - 1);
        }
    }

	public int getConnectCount(IFile file) {
		return connectCount.containsKey(file) ? connectCount.get(file) : 0;
	}

    protected boolean hasConnectedEditor(IFile file) {
        return connectCount.containsKey(file);
	}

    /**
     * May only be called if the file is not already open in an editor.
     */
    public synchronized void readNEDFile(IFile file) {
        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer();
        readNEDFile(file, markerSync);
        markerSync.runAsWorkspaceJob();
        rehash();
    }

    private synchronized void readNEDFile(IFile file, ProblemMarkerSynchronizer markerSync) {
    	Assert.isTrue(!hasConnectedEditor(file));
    	//Note: the following is a bad idea, because of undefined startup order: the editor calling us might run sooner than readAllNedFiles()
    	//Assert.isTrue(isNEDFile(file), "file is outside the NED source folders, or not a NED file at all");

        System.out.println("reading from disk: " + file.toString());

        // parse the NED file and put it into the hash table
        String fileName = file.getLocation().toOSString();
        NEDMarkerErrorStore errorStore = new NEDMarkerErrorStore(file, markerSync, NEDSYNTAXPROBLEM_MARKERID);
        NedFileElementEx tree = NEDTreeUtil.loadNedSource(fileName, errorStore);
        Assert.isNotNull(tree);

        storeNEDFileModel(file, tree);
        invalidate();
    }

    /**
     * Forget a NED file, and throws out all cached info.
     */
    public synchronized void forgetNEDFile(IFile file) {
        if (nedFiles.containsKey(file)) {
            // remove our model change listener from the file
            NedFileElementEx nedFileElement = nedFiles.get(file);
			nedFileElement.removeNEDChangeListener(nedModelChangeListener);

			// unregister
            nedFiles.remove(file);
            nedElementFiles.remove(nedFileElement);
            invalidate();

            // fire notification.
            nedModelChanged(new NEDFileRemovedEvent(file)); //XXX this involves immediate rehash() (??)
        }
    }

    // store NED file contents
    private synchronized void storeNEDFileModel(IFile file, NedFileElementEx tree) {
        Assert.isTrue(!connectCount.containsKey(file), "cannot replace the tree while an editor is open");

        NedFileElementEx oldTree = nedFiles.get(file);
        // if the new tree has changed, we have to rehash everything
        if (oldTree == null || !NEDTreeUtil.isNEDTreeEqual(oldTree, tree)) {
            invalidate();
            nedFiles.put(file, tree);
            nedElementFiles.put(tree, file);
            // add ourselves to the tree root as a listener
            tree.addNEDChangeListener(nedModelChangeListener);
            // remove ourselves from the old tree which is no longer used
            if (oldTree != null)
                oldTree.removeNEDChangeListener(nedModelChangeListener);
            // fire a ned change notification (new tree added)
            nedModelChanged(new NEDStructuralChangeEvent(tree, tree, NEDStructuralChangeEvent.Type.INSERTION, tree, tree));
        }
    }

	private void rehash() {
		invalidate();
		rehashIfNeeded();
	}

    /**
     * Rebuild hash tables after NED resource change. Note: some errors such as
     * duplicate names only get detected when this gets run!
     */
    public synchronized void rehashIfNeeded() {
        if (!needsRehash)
            return;

        long startMillis = System.currentTimeMillis();

        needsRehash = false;
        debugRehashCounter++;

        // clear tables and re-register built-in declarations for all projects
        for (ProjectData projectData : projects.values()) {
        	projectData.components.clear();
        	projectData.duplicates.clear();
        	projectData.reservedNames.clear();
        	for (INEDElement child : builtInDeclarationsFile) {
        		if (child instanceof INedTypeElement) {
        			INEDTypeInfo typeInfo = ((INedTypeElement)child).getNEDTypeInfo();
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
        			for (INEDElement child : nedFiles.get(file)) {
        				if (child instanceof INedTypeElement) {
        					INedTypeElement typeElement = (INedTypeElement) child;
        					INEDTypeInfo typeInfo = typeElement.getNEDTypeInfo();
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
        		projectData.duplicates.get(name).add(projectData.components.get(name).getNEDElement());
        		projectData.components.remove(name);
        	}

        	System.out.println("types in project " + project.getName() + ": " + StringUtils.join(projectData.components.keySet(), ", ", " and "));

        }

        // invalidate all inherited members on all typeInfo objects
        for (NedFileElementEx file : nedElementFiles.keySet())
			invalidateTypeInfo(file);

        long dt = System.currentTimeMillis() - startMillis;
        System.out.println("rehash(): " + dt + "ms, " + nedFiles.size() + " files, " + projects.size() + " projects");

        // schedule a validation
        validationJob.restartTimer();
    }

	protected void invalidateTypeInfo(INEDElement parent) {
		for (INEDElement element : parent) {
			if (element instanceof INedTypeElement) {
				// invalidate
				((INedTypeElement)element).getNEDTypeInfo().invalidateInherited();

				// do inner types too
				if (element instanceof CompoundModuleElementEx) {
					INEDElement typesSection = ((CompoundModuleElementEx)element).getFirstTypesChild();
					if (typesSection != null)
						invalidateTypeInfo(typesSection);
				}
			}
		}
	}

    /**
     * Validates all NED files for consistency (no such parameter/gate/module-type, redeclarations,
     * duplicate types, cycles in the inheritance chain, etc). All consistency problem markers
     * (NEDCONSISTENCYPROBLEM_MARKERID) are managed within this method.
     */
	public synchronized void validateAllFiles() {
		long startMillis = System.currentTimeMillis();

		// During validation, we potentially fire a large number of NEDMarkerChangeEvents.
		// So we'll surround the code with begin..end notifications, which allows the
		// graphical editor to optimize refresh() calls. Otherwise it would have to
		// refresh on each notification, which can be a disaster performance-wise.

		// fake a begin change event, then "finally" an end change event
		nedModelChanged(new NEDBeginModelChangeEvent(null));
		ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(NEDCONSISTENCYPROBLEM_MARKERID);
		try {
			// clear consistency error markers from the ned tree
			for (IFile file : nedFiles.keySet())
				nedFiles.get(file).clearConsistencyProblemMarkerSeverities();

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

						NEDMarkerErrorStore errorStore = new NEDMarkerErrorStore(file, markerSync);
						if (otherFile == null) {
							errorStore.addError(element, element.getReadableTagName() + " '" + name + "' is a built-in type and cannot be redefined");
						}
						else {
							// add error message to both files
							String messageHalf = element.getReadableTagName() + " '" + name + "' already defined in ";
							errorStore.addError(element, messageHalf + otherFile.getFullPath().toString());
							NEDMarkerErrorStore otherErrorStore = new NEDMarkerErrorStore(otherFile, markerSync);
							otherErrorStore.addError(otherElement, messageHalf + file.getFullPath().toString());
						}
					}
				}
			}

			// validate all files
			for (IFile file : nedFiles.keySet()) {
				NedFileElementEx nedFileElement = nedFiles.get(file);
				markerSync.registerFile(file);
				INEDErrorStore errorStore = new NEDMarkerErrorStore(file, markerSync);
				//INEDErrorStore errorStore = new INEDErrorStore.SysoutNedErrorStore(); // for debugging
				new NEDValidator(this, file.getProject(), errorStore).validate(nedFileElement);
			}

			// we need to do the synchronization in a background job, to avoid deadlocks
			markerSync.runAsWorkspaceJob();

		}
        finally {
            nedModelChanged(new NEDEndModelChangeEvent(null));
        }

        long dt = System.currentTimeMillis() - startMillis;
        System.out.println("validateAllFiles(): " + dt + "ms, " + markerSync.getNumberOfMarkers() + " markers on " + markerSync.getNumberOfFiles() + " files");
        System.out.println("typeinfo: refreshLocalCount:" + NEDTypeInfo.debugRefreshLocalCount + "  refreshInheritedCount:" + NEDTypeInfo.debugRefreshInheritedCount);
	}


	public synchronized void invalidate() {
		needsRehash = true;
    }

	/**
	 * To be called on project-level changes: project open/close, project description change
	 * (i.e. nature & referred projects), ".nedfolders" file.
	 *
	 * Also needs to be invoked right on startup, to prevent race conditions with editors.
	 * (When an editor starts, the projects table must already be up to date, otherwise
	 * the editor's input file might not qualify as "NED file" and that'll cause an error).
	 */
	//FIXME call from ctor?
	public synchronized void rebuildProjectsTable() {
		// rebuild table
		projects.clear();
		IProject[] omnetppProjects = ProjectUtils.getOmnetppProjects();
		for (IProject project : omnetppProjects) {
        	try {
        		ProjectData projectData = new ProjectData();
        		projectData.referencedProjects = ProjectUtils.getAllReferencedOmnetppProjects(project);
				projectData.nedSourceFolders = ProjectUtils.readNedFoldersFile(project);
				projects.put(project, projectData);
			}
        	catch (Exception e) {
				NEDResourcesPlugin.logError(e);
			}
        }
		dumpProjectsTable();


		// forget those files which are no longer in our projects or NED folders
		List <IFile> trash = new ArrayList<IFile>();
		for (IFile file : nedFiles.keySet())
			if (!isNEDFile(file))
				trash.add(file);
		for (IFile file : trash)
		    forgetNEDFile(file);

		// we'll need rehash() too
		invalidate();

		//FIXME optimize notifications? (e.g. add begin/end)
		nedModelChanged(new NEDModelChangeEvent(null));  // "anything might have changed"

		scheduleReadMissingNedFiles();
	}

	/**
	 * Schedules a background job to read NED files that are not yet loaded.
	 */
	public void scheduleReadMissingNedFiles() {
        WorkspaceJob startupJob = new WorkspaceJob("Parsing NED files...") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                readMissingNedFiles();
                return Status.OK_STATUS;
            }
        };
        startupJob.setRule(ResourcesPlugin.getWorkspace().getRoot());
        startupJob.setPriority(Job.INTERACTIVE);
        startupJob.setSystem(true);
        startupJob.schedule();
	}

	/**
     * Reads NED files that are not yet loaded (not in our nedFiles table).
     * This should be run on startup and after rebuildProjectsTable();
     * individual file changes are handled by loadNedFile() calls from the
     * workspace listener.
     */
    public synchronized void readMissingNedFiles() {
        try {
            // disable all ned model notifications until all files have been processed
            nedModelChangeNotificationDisabled = true;
            debugRehashCounter = 0;
            IWorkspaceRoot wsroot = ResourcesPlugin.getWorkspace().getRoot();

            // read NED files that are not yet loaded
            final ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            wsroot.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (!nedFiles.containsKey(resource) && isNEDFile(resource))
                        readNEDFile((IFile)resource, sync);
                    return true;
                }
            });
            sync.runAsWorkspaceJob();
            rehashIfNeeded();
        }
        catch (CoreException e) {
            NEDResourcesPlugin.logError("Error during workspace refresh: ",e);
		} finally {
            nedModelChangeNotificationDisabled = false;
            Assert.isTrue(debugRehashCounter <= 1, "Too many rehash operations during readAllNedFilesInWorkspace()");
            nedModelChanged(new NEDModelChangeEvent(null));  // "everything changed"
        }
    }

    // ******************* notification helpers ************************************

    public void addNEDModelChangeListener(INEDChangeListener listener) {
        if (nedModelChangeListenerList == null)
            nedModelChangeListenerList = new NEDChangeListenerList();
        nedModelChangeListenerList.add(listener);
    }

    public void removeNEDModelChangeListener(INEDChangeListener listener) {
        if (nedModelChangeListenerList != null)
            nedModelChangeListenerList.remove(listener);
    }

    /**
     * Respond to model changes
     */
    protected void nedModelChanged(NEDModelEvent event) {
        if (nedModelChangeNotificationDisabled)
            return;

        if (event instanceof NEDModelChangeEvent) {
            // NOTE: the rehash causes NEDMarkerChangeEvent
            rehash();
        }

        // notify generic listeners (like NedFileEditParts who refresh themselves
        // in response to this notification)
        // long startMillis = System.currentTimeMillis();

        if (nedModelChangeListenerList != null)
            nedModelChangeListenerList.fireModelChanged(event);

        // long dt = System.currentTimeMillis() - startMillis;
        // System.out.println("visual notification took " + dt + "ms");
    }

    /**
     * Synchronize the plugin with the resources in the workspace
     */
    public synchronized void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null)
                return;
            //printResourceChangeEvent(event);
            final ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    printDelta(delta);
                    if (isNEDFile(resource)) {
                    	IFile file = (IFile)resource;
                    	switch (delta.getKind()) {
                    	case IResourceDelta.REMOVED:
                    		forgetNEDFile(file);
                    		break;
                    	case IResourceDelta.ADDED:
                    		readNEDFile(file, sync);
                    		break;
                    	case IResourceDelta.CHANGED:
                    		// we are only interested in content changes; ignore marker and property changes
                    		if ((delta.getFlags() & IResourceDelta.CONTENT) != 0 && !hasConnectedEditor(file))
                    			readNEDFile(file, sync);
                    		break;
                    	}
                    }
                    else if (ProjectUtils.isNedFoldersFile(resource)) {
                    	rebuildProjectsTable();
                    }
                    else if (resource instanceof IProject) {
                    	switch (delta.getKind()) {
                    	case IResourceDelta.REMOVED:
                    	case IResourceDelta.ADDED:
                    	case IResourceDelta.OPEN:
                        	rebuildProjectsTable();
                    		break;
                    	case IResourceDelta.CHANGED:
                    		// change in natures and referenced projects will be reported as description changes
                    		if ((delta.getFlags() & IResourceDelta.DESCRIPTION) != 0)
                            	rebuildProjectsTable();
                    		break;
                    	}
                    }
                    return true;
                }
            });
            sync.runAsWorkspaceJob();
        }
        catch (CoreException e) {
            NEDResourcesPlugin.logError("Error during workspace change notification: ", e);
        } finally {
            rehashIfNeeded();
        }

    }

    // Utility functions for debugging
    public static void printResourceChangeEvent(IResourceChangeEvent event) {
        System.out.println("event type: "+event.getType());
    }

    @SuppressWarnings("restriction")
	public static void printDelta(IResourceDelta delta) {
    	// LEGEND: [+] added, [-] removed, [*] changed, [>] and [<] phantom added/removed;
    	// then: {CONTENT, MOVED_FROM, MOVED_TO, OPEN, TYPE, SYNC, MARKERS, REPLACED, DESCRIPTION, ENCODING}
    	System.out.println("  "+((ResourceDelta)delta).toDebugString());
    }

    public void dumpProjectsTable() {
		System.out.println(projects.size() + " projects:");
    	for (IProject project : projects.keySet()) {
    		ProjectData projectData = projects.get(project);
    		System.out.println("  " + project.getName() + ":" +
    				"  deps: " + StringUtils.join(projectData.referencedProjects, ",") +
    				"  nedfolders: " + StringUtils.join(projectData.nedSourceFolders, ","));
    	}
	}


}
