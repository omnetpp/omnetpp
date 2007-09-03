package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;

import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.INEDErrorStore;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.model.NEDTreeDifferenceUtils;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
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
import org.omnetpp.ned.model.notification.NEDModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.NEDElementFactory;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;

/**
 * Parses all NED files in the workspace and makes them available for other
 * plugins for consistency checks among NED files etc.
 *
 * It listens to workspace resource changes and modifies it content based on
 * change notifications.
 *
 * @author andras
 */
public class NEDResources implements INEDTypeResolver, IResourceChangeListener {

    // filters for component access with getAllComponentsFilteredBy
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

    private static final String NED_EXTENSION = "ned";

    // list of objects that listen on *all* NED changes
    private NEDChangeListenerList nedModelChangeListenerList = null;

    // associate IFiles with their NEDElement trees
    private final Map<IFile, NedFileElementEx> nedFiles = new HashMap<IFile, NedFileElementEx>();
    private final Map<NedFileElementEx, IFile> nedElementFiles = new HashMap<NedFileElementEx,IFile>();

    // number of editors connected to a given NED file
    private final Map<IFile, Integer> connectCount = new HashMap<IFile, Integer>();

    // non-duplicate toplevel (non-inner) types; keys are fully qualified names
    private final Map<String, INEDTypeInfo> components = new HashMap<String, INEDTypeInfo>();

    // duplicate toplevel (non-inner) types; keys are fully qualified names
    private final Map<String, List<INedTypeElement>> duplicates = new HashMap<String, List<INedTypeElement>>();

    // reserved (used) fully qualified names (contains all names including duplicates)
    private final Set<String> reservedNames = new HashSet<String>();

    // if tables need to be rebuilt
    private boolean needsRehash = false;

    // We use this counter to increment whenever a rehash occurred. Checks can be made
    // to assert that the function is not called unnecessarily
    private int debugRehashCounter = 0;

    private NedFileElementEx builtInDeclarationsFile;
    private INEDTypeInfo basicChannelType = null;
    private INEDTypeInfo nullChannelType = null;
    private INEDTypeInfo bidirChannelType = null;
    private INEDTypeInfo unidirChannelType = null;

    private boolean nedModelChangeNotificationDisabled = false;

    private DelayedJob validationJob = new DelayedJob(400) {
		public void run() {
			DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
				public void run() {
					validateAllFiles();
				}
			});
		}
    };

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
    // FIXME should use built-in NED text:
    // String src = NEDParser.getBuiltInDeclarations(); ...
    protected void createBuiltInNEDTypes() {
        // create built-in channel type cIdealChannel
        NEDElementFactory factory = NEDElementFactoryEx.getInstance();
		ChannelElementEx nullChannel = (ChannelElementEx) factory.createElement(NEDElementTags.NED_CHANNEL);
        nullChannel.setName("cIdealChannel");
//        nullChannel.setIsWithcppclass(true);
        nullChannelType = nullChannel.getNEDTypeInfo();

        // create built-in channel type cBasicChannel
        ChannelElementEx basicChannel = (ChannelElementEx) factory.createElement(NEDElementTags.NED_CHANNEL);
        basicChannel.setName("cBasicChannel");
//        basicChannel.setIsWithcppclass(true);
        ParametersElement params = (ParametersElement) factory.createElement(
                NEDElementTags.NED_PARAMETERS, basicChannel);
        params.appendChild(createImplicitChannelParameter("delay", NEDElementConstants.NED_PARTYPE_DOUBLE));
        params.appendChild(createImplicitChannelParameter("error", NEDElementConstants.NED_PARTYPE_DOUBLE));
        params.appendChild(createImplicitChannelParameter("datarate", NEDElementConstants.NED_PARTYPE_DOUBLE));
        basicChannelType = basicChannel.getNEDTypeInfo();

        //
        // create built-in interfaces that allow modules to be used as channels
        // interface IBidirectionalChannel { gates: inout a; inout b; }
        // interface IUnidirectionalChannel {gates: input i; output o; }
        //
        ModuleInterfaceElementEx bidirChannel = (ModuleInterfaceElementEx) factory.createElement(NEDElementTags.NED_MODULE_INTERFACE);
        bidirChannel.setName("IBidirectionalChannel");
        GatesElement gates = (GatesElement) factory.createElement(NEDElementTags.NED_GATES, bidirChannel);
        gates.appendChild(createGate("a", NEDElementConstants.NED_GATETYPE_INOUT));
        gates.appendChild(createGate("b", NEDElementConstants.NED_GATETYPE_INOUT));
        bidirChannelType = bidirChannel.getNEDTypeInfo();

        ModuleInterfaceElementEx unidirChannel = (ModuleInterfaceElementEx) factory.createElement(NEDElementTags.NED_MODULE_INTERFACE);
        unidirChannel.setName("IUnidirectionalChannel");
        GatesElement gates2 = (GatesElement) factory.createElement(NEDElementTags.NED_GATES,unidirChannel);
        gates2.appendChild(createGate("i", NEDElementConstants.NED_GATETYPE_INPUT));
        gates2.appendChild(createGate("o", NEDElementConstants.NED_GATETYPE_OUTPUT));
        unidirChannelType = unidirChannel.getNEDTypeInfo();

        builtInDeclarationsFile = (NedFileElementEx) factory.createElement(NEDElementTags.NED_NED_FILE);
        builtInDeclarationsFile.setFilename("[builtin-declarations]");
        builtInDeclarationsFile.setPackage("ned");
        builtInDeclarationsFile.appendChild(nullChannel);
        builtInDeclarationsFile.appendChild(basicChannel);
        builtInDeclarationsFile.appendChild(bidirChannel);
        builtInDeclarationsFile.appendChild(unidirChannel);
    }

    /* utility method */
    protected INEDElement createGate(String name, int type) {
        GateElement g = (GateElement) NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_GATE);
        g.setName(name);
        g.setType(type);
        return g;
    }

    /* utility method */
    protected INEDElement createImplicitChannelParameter(String name, int type) {
        ParamElement param = (ParamElement) NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_PARAM);
        param.setName(name);
        param.setType(type);
        param.setIsVolatile(false);
        param.setIsDefault(false);
        // TODO add default value of zero
        param.setSourceLocation("internal");
        return param;
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
     * Returns the textual (reformatted) content of the NED file, generated
     * from the model that belongs to the given file.
     */
    public synchronized String getNedFileText(IFile file) {
        return NEDTreeUtil.generateNedSource(getNedFileElement(file), true);
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

    // FIXME it seems that the markers do not contain NEDELEMENT_ID attribute
	public IMarker[] getMarkersForElement(INEDElement node, IFile file) {
		try {
			Assert.isTrue(getNedFileElement(file)==node.getContainingNedFileElement()); //XXX find file from element? (needs element-to-file mapping)
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

    public synchronized INEDTypeInfo getNedTypeAt(IFile file, int lineNumber) {
		rehashIfNeeded();
        for (INEDTypeInfo component : components.values()) {
            if (file.equals(component.getNEDFile())) {
                NEDSourceRegion region = component.getNEDElement().getSourceRegion();
                if (region != null && region.containsLine(lineNumber))
                    return component;
            }
        }
        return null;
    }

    public synchronized INEDElement getNedElementAt(IFile file, int line, int column) {
        line++;  // IDocument is 0-based

        // find component and NEDElements under the cursor
        INEDTypeInfo typeInfo = getNedTypeAt(file, line);
        if (typeInfo != null) {
            INEDElement[] elements = typeInfo.getNEDElementsAt(line, column);
            if (elements != null && elements.length > 0)
                return elements[elements.length-1];
        }
        return null;
    }

    public synchronized Collection<INEDTypeInfo> getAllNedTypes() {
		rehashIfNeeded();
        return components.values();
    }

    public Set<String> getNedTypeQNames(IPredicate predicate) {
        Set<String> result = new HashSet<String>();
        for (INEDTypeInfo typeInfo : getAllNedTypes())
            if (predicate.matches(typeInfo))
                result.add(typeInfo.getFullyQualifiedName());
        return result;
    }

    public synchronized Set<String> getAllNedTypeQNames() {
		rehashIfNeeded();
        return components.keySet();
    }

    public synchronized Set<String> getReservedQNames() {
		rehashIfNeeded();
        return reservedNames;
    }

    public synchronized Set<String> getModuleQNames() {
		rehashIfNeeded();
    	return getNedTypeQNames(MODULE_FILTER);
    }

    public synchronized Set<String> getNetworkQNames() {
    	return getNedTypeQNames(NETWORK_FILTER);
    }

    public synchronized Set<String> getChannelQNames() {
		rehashIfNeeded();
    	return getNedTypeQNames(CHANNEL_FILTER);
    }

    public synchronized Set<String> getModuleInterfaceQNames() {
		rehashIfNeeded();
    	return getNedTypeQNames(MODULEINTERFACE_FILTER);
    }

    public synchronized Set<String> getChannelInterfaceQNames() {
		rehashIfNeeded();
    	return getNedTypeQNames(CHANNELINTERFACE_FILTER);
    }

	public synchronized INEDTypeInfo getNedType(String qualifiedName) {
		rehashIfNeeded();
		return components.get(qualifiedName);
	}

    public synchronized INEDTypeInfo lookupNedType(String name, INedTypeLookupContext context) {
		rehashIfNeeded();
		if (context == null) {
			NEDResourcesPlugin.logError(new IllegalArgumentException("lookupNedType(): called with context==null!!!"));
			return components.get(name); //FIXME temp fix, remove it!!!!
		}

		// inner type?
		if (context instanceof INedTypeElement) {
			INEDTypeInfo contextTypeInfo = ((INedTypeElement)context).getNEDTypeInfo();
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
				// inner type with simple name
				INedTypeElement innerType = contextTypeInfo.getInnerTypes().get(name);
				if (innerType != null)
					return innerType.getNEDTypeInfo();
			}
		}

		// type from the same package?
		String packageName = context.getContainingNedFileElement().getPackage();
        // FIXME (all below) if not contains "."
		INEDTypeInfo samePackageType = components.get(StringUtils.isNotEmpty(packageName) ? packageName+"."+name : name);
		if (samePackageType != null)
			return samePackageType;

		// imported type?
		// try a shortcut first: if the import doesn't contain wildcards
		List<String> imports = context.getContainingNedFileElement().getImports();
		for (String importSpec : imports)
			if (components.containsKey(importSpec) && importSpec.endsWith("." + name))
				return components.get(importSpec);
		// try harder, using wildcards
		for (String importSpec : imports) {
			String importRegex = NEDElementUtilEx.importToRegex(importSpec);
			for (String qualifiedName : components.keySet())
				if (qualifiedName.matches(importRegex) && qualifiedName.endsWith("." + name))
					return components.get(qualifiedName);
		}

		// maybe it's already a fully qualified name
		if (name.contains(".") && components.get(name) != null)
			return components.get(name);

		return null;
    }

	public synchronized Set<String> getVisibleTypeNames(INedTypeLookupContext context) {
		return getVisibleTypeNames(context, new IPredicate() {
			public boolean matches(INEDTypeInfo typeInfo) {return true;}
		});
	}

	public synchronized Set<String> getVisibleTypeNames(INedTypeLookupContext context, IPredicate predicate) {
		rehashIfNeeded();

		// types from the same package
		String prefix = context.getQNameAsPrefix();
        // FIXME quote the dots in prefix
		String regex = prefix + "[^.]+";
		Set<String> result = new HashSet<String>();
		for (INEDTypeInfo typeInfo : components.values())
			if (typeInfo.getFullyQualifiedName().matches(regex) && predicate.matches(typeInfo))
				result.add(typeInfo.getName());

		// imported types
		List<String> imports = context.getContainingNedFileElement().getImports();
		for (String importSpec : imports) {
            // FIXME use util func
			String importRegex = importSpec.replace(".", "\\.").replace("**", ".*").replace("*", "[^.]*");
			for (INEDTypeInfo typeInfo : components.values())
				if (typeInfo.getFullyQualifiedName().matches(importRegex) && predicate.matches(typeInfo))
					result.add(typeInfo.getName());
		}
		return result;
	}


    /**
     * Determines if a resource is a NED file, that is, if it should be parsed.
     */
    public static boolean isNEDFile(IResource resource) {
        // TODO should only regard files within a folder designated as "source
        // folder" (persistent attribute!)
        return resource instanceof IFile && NED_EXTENSION.equalsIgnoreCase(((IFile) resource).getFileExtension());
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

        System.out.println("reading from disk: " + file.toString());

        // parse the NED file and put it into the hash table
        String fileName = file.getLocation().toOSString();
        NEDMarkerErrorStore errorStore = new NEDMarkerErrorStore(file, markerSync, NEDSYNTAXPROBLEM_MARKERID);
        NedFileElementEx tree = NEDTreeUtil.loadNedSource(fileName, errorStore);
        Assert.isNotNull(tree);

        storeNEDFileModel(file, tree);
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

        components.clear();
        reservedNames.clear();
        duplicates.clear();

        components.put(nullChannelType.getFullyQualifiedName(), nullChannelType);
        components.put(basicChannelType.getFullyQualifiedName(), basicChannelType);
        components.put(bidirChannelType.getFullyQualifiedName(), bidirChannelType);
        components.put(unidirChannelType.getFullyQualifiedName(), unidirChannelType);
        reservedNames.addAll(components.keySet());

        // find NED types in each file, and register them
        for (IFile file : nedFiles.keySet()) {

        	// collect types (including inner types) from the NED file, and process them one by one
        	for (INEDElement child : nedFiles.get(file)) {
        		if (child instanceof INedTypeElement) {
        			INedTypeElement typeElement = (INedTypeElement) child;
        			INEDTypeInfo typeInfo = typeElement.getNEDTypeInfo();
        			String qualifiedName = typeInfo.getFullyQualifiedName();

        			if (reservedNames.contains(qualifiedName)) {
        				if (!duplicates.containsKey(qualifiedName))
        					duplicates.put(qualifiedName, new ArrayList<INedTypeElement>());
        				duplicates.get(qualifiedName).add(typeElement);
        			}
        			else {
        				// normal case: not duplicate. Add the type info to our tables.
        				components.put(qualifiedName, typeInfo);
        			}

        			// add to the name list even if it was duplicate
        			reservedNames.add(qualifiedName);
                }
        	}
        }

        // now we should remove all types that were duplicates
        for (String name : duplicates.keySet()) {
        	duplicates.get(name).add(components.get(name).getNEDElement());
            components.remove(name);
        }

        // invalidate all inherited members on all typeInfo objects
        for (NedFileElementEx file : nedElementFiles.keySet())
			invalidateTypeInfo(file);

        long dt = System.currentTimeMillis() - startMillis;
        System.out.println("rehash(): " + dt + "ms, " + nedFiles.size() + " files, " + components.size() + " registered types");
        System.out.println("types: " + StringUtils.join(components.keySet(), ", ", " and "));

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
			for (String name : duplicates.keySet()) {
				List<INedTypeElement> duplicateList = duplicates.get(name);
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

			// validate all files
			for (IFile file : nedFiles.keySet()) {
				NedFileElementEx nedFileElement = nedFiles.get(file);
				markerSync.registerFile(file);
				INEDErrorStore errorStore = new NEDMarkerErrorStore(file, markerSync);
				//INEDErrorStore errorStore = new INEDErrorStore.SysoutNedErrorStore(); // for debugging
				new NEDValidator(this, errorStore).validate(nedFileElement);
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
     * Reads all NED files in the project.
     */
    public synchronized void readAllNedFilesInWorkspace() {
        // do not allow access to the plugin until the whole parsing is finished
        try {
            // disable all ned model notifications until all files have been processed
            nedModelChangeNotificationDisabled = true;
            debugRehashCounter = 0;
            IResource wsroot = ResourcesPlugin.getWorkspace().getRoot();
            final ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            wsroot.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (isNEDFile(resource) && !hasConnectedEditor((IFile)resource))
                        readNEDFile((IFile)resource, sync);
                    return true;
                }
            });
            sync.runAsWorkspaceJob();
            rehashIfNeeded();
        } catch (CoreException e) {
            NEDResourcesPlugin.logError("Error during workspace refresh: ",e);
        } finally {
            nedModelChangeNotificationDisabled = false;
            Assert.isTrue(debugRehashCounter <= 1, "Too many rehash operations during readAllNedFilesInWorkspace()");
            nedModelChanged(new NEDModelChangeEvent(null));
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
            // printResourceChangeEvent(event);

            final ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    // continue visiting the children if it is not a NED file
                    if (!isNEDFile(resource))
                        return true;

                    // printDelta(delta);
                    IFile file = (IFile)resource;
                    switch (delta.getKind()) {
                        case IResourceDelta.REMOVED:
                            // handle removed resource
                            forgetNEDFile(file);
                            invalidate();
                            break;
                        case IResourceDelta.ADDED:
                            readNEDFile(file, sync);
                            invalidate();
                            break;
                        case IResourceDelta.CHANGED:
                            // handle changed resource, but we are interested
                            // only in content change
                            // we don't care about marker and property changes
                            if ((delta.getFlags() & IResourceDelta.CONTENT) != 0 && !hasConnectedEditor(file)) {
                                readNEDFile(file, sync);
                                invalidate();
                            }
                            break;
                    }
                    // ned files do not have children
                    return false;
                }
            });
            sync.runAsWorkspaceJob();
        } catch (CoreException e) {
            NEDResourcesPlugin.logError("Error during workspace change notification: ", e);
        } finally {
            rehashIfNeeded();
        }

    }

    // Utility functions for debugging
    public static void printResourceChangeEvent(IResourceChangeEvent event) {
        System.out.println("event type: "+event.getType());
    }

    public static void printDelta(IResourceDelta delta) {
        System.out.println("  delta: "+delta.getResource().getProjectRelativePath()+" kind:"+delta.getKind()+
                " isContent:"+((delta.getFlags() & IResourceDelta.CONTENT) != 0)+
                " isMarkerChange:"+((delta.getFlags() & IResourceDelta.MARKERS) != 0));
    }

	public int getConnectCount(IFile file) {
		return connectCount.containsKey(file) ? connectCount.get(file) : 0;
	}

    protected boolean hasConnectedEditor(IFile file) {
        return connectCount.containsKey(file);
	}

}
