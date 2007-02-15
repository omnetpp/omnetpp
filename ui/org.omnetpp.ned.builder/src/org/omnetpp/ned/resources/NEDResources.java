package org.omnetpp.ned.resources;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.StringTokenizer;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRunnable;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.engine.NEDErrorCategory;
import org.omnetpp.ned.engine.NEDErrorStore;

/**
 * Parses all NED files in the workspace and makes them available
 * for other plugins for consistence checks among NED files etc.
 *  
 * Its readNEDFile(IFile file) method gets invoked by NEDBuilder.
 * 
 * XXX display error markers in the Explorer view: see org.eclipse.jdt.ui.ProblemsLabelDecorator
 * XXX should do full rebuild when Eclipse starts up!!!
 * XXX default installation should have "workspace auto refresh" enabled, and "Problems view" shown!!! 
 * XXX when something changes, we always rebuild INEDComponents. This is not needed -- rather, we
 *     should just call NEDComponent.componentsChanged()! (PERFORMANCE)
 *  
 * @author andras
 */
public class NEDResources implements INEDTypeResolver {

	// markers created during parsing
	public static final String NEDPROBLEM_MARKERID = "org.omnetpp.ned.builder.nedproblem";

	// markers created in rehash()
	public static final String NEDCONSISTENCYPROBLEM_MARKERID = "org.omnetpp.ned.builder.nedconsistencyproblem";
	
	// stores parsed contents of NED files
	private HashMap<IFile, NEDElement> nedFiles = new HashMap<IFile, NEDElement>();

	private HashMap<IFile, Integer> connectCount = new HashMap<IFile, Integer>();
	
	// table of toplevel components (points into nedFiles trees)
	private HashMap<String, INEDTypeInfo> components = new HashMap<String, INEDTypeInfo>();
    
    // reserved (used) names (contains all names including dupliates)
	private Set<String> reservedNames = new HashSet<String>();
    
    // last event serials processed by the components. used to be able to skip duplicated
    // event notification in NEDComponent
    HashMap<String, Long> eventSerials = new HashMap<String, Long>(); 
    
	// tables of toplevel components, classified (points into nedFiles trees)
    private boolean needsRehash = false;  // if tables below need to be rebuilt
	private HashMap<String, INEDTypeInfo> modules = new HashMap<String, INEDTypeInfo>();
	private HashMap<String, INEDTypeInfo> channels = new HashMap<String, INEDTypeInfo>();
	private HashMap<String, INEDTypeInfo> moduleInterfaces = new HashMap<String, INEDTypeInfo>();
	private HashMap<String, INEDTypeInfo> channelInterfaces = new HashMap<String, INEDTypeInfo>();

	private INEDTypeInfo basicChannelType = null;
	private INEDTypeInfo nullChannelType = null;
	private INEDTypeInfo bidirChannelType = null;
	private INEDTypeInfo unidirChannelType = null;

    private long lastEventSerial;

	/**
	 * Constructor.
	 */
	public NEDResources() {
		createBuiltInNEDTypes(); 
	}

	/**
	 * Create channel and interface types that are predefined in NED.
	 */
	//FIXME should use built-in NED text from nedxml lib!!!
	protected void createBuiltInNEDTypes() {
		// create built-in channel type NullChannel
		ChannelNode nullChannel = (ChannelNode)NEDElementFactoryEx
                                    .getInstance().createNodeWithTag(NEDElementTags.NED_CHANNEL);
		nullChannel.setName("cIdealChannel");
		nullChannel.setIsWithcppclass(true);
		nullChannelType = new NEDComponent(nullChannel, null, this); 

		// create built-in channel type BasicChannel
		ChannelNode basicChannel = (ChannelNode) NEDElementFactoryEx
                                    .getInstance().createNodeWithTag(NEDElementTags.NED_CHANNEL);
		basicChannel.setName("cBasicChannel");
		basicChannel.setIsWithcppclass(true);
		ParametersNode params = (ParametersNode) NEDElementFactoryEx
		                    .getInstance().createNodeWithTag(NEDElementTags.NED_PARAMETERS);
		createImplicitChannelParameter(params, "delay", NEDElementUtil.NED_PARTYPE_DOUBLE);
		createImplicitChannelParameter(params, "error", NEDElementUtil.NED_PARTYPE_DOUBLE);
		createImplicitChannelParameter(params, "datarate", NEDElementUtil.NED_PARTYPE_DOUBLE);
		basicChannelType = new NEDComponent(basicChannel, null, this); 
		
        //
		// create built-in interfaces that allow modules to be used as channels
        //  interface IBidirectionalChannel { gates: inout a; inout b; }
		//  interface IUnidirectionalChannel {gates: input i; output o; }
		//
		ModuleInterfaceNode bidirChannel =(ModuleInterfaceNode)NEDElementFactoryEx
                                .getInstance().createNodeWithTag(NEDElementTags.NED_MODULE_INTERFACE);
		bidirChannel.setName("IBidirectionalChannel");
		GatesNode gates = (GatesNode) NEDElementFactoryEx
                                .getInstance().createNodeWithTag(NEDElementTags.NED_GATES);
		createGate(gates, "a", NEDElementUtil.NED_GATETYPE_INOUT);
		createGate(gates, "b", NEDElementUtil.NED_GATETYPE_INOUT);
		bidirChannelType = new NEDComponent(bidirChannel, null, this); 
		
		ModuleInterfaceNode unidirChannel = (ModuleInterfaceNode) NEDElementFactoryEx
                                .getInstance().createNodeWithTag(NEDElementTags.NED_MODULE_INTERFACE);
		unidirChannel.setName("IUnidirectionalChannel");
        GatesNode gates2 = (GatesNode) NEDElementFactoryEx
                                        .getInstance().createNodeWithTag(NEDElementTags.NED_GATES);
		createGate(gates2, "i", NEDElementUtil.NED_GATETYPE_INPUT);
		createGate(gates2, "o", NEDElementUtil.NED_GATETYPE_OUTPUT);
		unidirChannelType = new NEDComponent(unidirChannel, null, this);
	}	

	/* utility method */
	protected NEDElement createGate(GatesNode gates, String name, int type) {
		GateNode g = (GateNode) NEDElementFactoryEx
                            .getInstance().createNodeWithTag(NEDElementTags.NED_GATE);
		g.setName(name);
		g.setType(type);
		return g;
	}

	/* utility method */
	protected NEDElement createImplicitChannelParameter(NEDElement parent, String name, int type) {
		ParamNode param = (ParamNode) NEDElementFactoryEx
                                .getInstance().createNodeWithTag(NEDElementTags.NED_PARAM);
		param.setName(name);
		param.setType(type);
		param.setIsVolatile(false);
		param.setIsDefault(false);
		//TODO add default value of zero
		param.setSourceLocation("internal");
		return param;
	}
	
	public Set<IFile> getNEDFiles() {
		return nedFiles.keySet();
	}

	public NEDElement getNEDFileContents(IFile file) {
		if (needsRehash)
			rehash();
		return nedFiles.get(file);
	}

	public boolean containsNEDErrors(IFile file) {
        return hasErrorMarker(file, NEDPROBLEM_MARKERID) || hasErrorMarker(file, NEDCONSISTENCYPROBLEM_MARKERID);
	}

	/**
	 * True if the file has any marker of the given marker type id (or subclass) 
	 * with severity ERROR; or if an error occurred while checking the markers. 
	 */
	private static boolean hasErrorMarker(IFile file, String markerType) {
		try {
        	IMarker[] markers = file.findMarkers(markerType, true, IFile.DEPTH_ZERO);
        	for (IMarker marker : markers) 
        		if (marker.getAttribute(IMarker.SEVERITY,IMarker.SEVERITY_ERROR)==IMarker.SEVERITY_ERROR)
        			return true;
        	return false;
        } catch (CoreException e) {
			return true;
		}
	}
	
	public INEDTypeInfo getComponentAt(IFile file, int lineNumber) {
		if (needsRehash)
			rehash();
		for (INEDTypeInfo component : components.values()) {
			if (file.equals(component.getNEDFile())) {
				NEDSourceRegion region = component.getNEDElement().getSourceRegion();
				if (region!=null && region.containsLine(lineNumber))
					return component;
			}
		}
		return null;
	}

	public Collection<INEDTypeInfo> getAllComponents() {
		if (needsRehash)
			rehash();
		return components.values();
	}

	public Collection<INEDTypeInfo> getModules() {
		if (needsRehash)
			rehash();
		return modules.values();
	}

	public Collection<INEDTypeInfo> getChannels() {
		if (needsRehash)
			rehash();
		return channels.values();
	}

	public Collection<INEDTypeInfo> getModuleInterfaces() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.values();
	}

	public Collection<INEDTypeInfo> getChannelInterfaces() {
		if (needsRehash)
			rehash();
		return channelInterfaces.values();
	}

    public Set<String> getAllComponentNames() {
        if (needsRehash)
            rehash();
        return components.keySet();
    }

    public Set<String> getReservedNames() {
        if (needsRehash)
            rehash();
        return reservedNames;
    }
    
    public Set<String> getModuleNames() {
		if (needsRehash)
			rehash();
		return modules.keySet();
	}

	public Set<String> getChannelNames() {
		if (needsRehash)
			rehash();
		return channels.keySet();
	}

	public Set<String> getModuleInterfaceNames() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.keySet();
	}

	public Set<String> getChannelInterfaceNames() {
		if (needsRehash)
			rehash();
		return channelInterfaces.keySet();
	}

	public INEDTypeInfo getComponent(String name) {
		if (needsRehash)
			rehash();
		return components.get(name);
	}

	public INEDTypeInfo wrapNEDElement(NEDElement componentNode) {
		return new NEDComponent(componentNode, null, this); 
	}

	/**
	 * Determines if a resource is a NED file, that is, if it should be parsed.
	 */
	public boolean isNEDFile(IResource resource) {
		// XXX should only regard files within a folder designated as "source folder" (persistent attribute!)
		return resource instanceof IFile && "ned".equals(((IFile) resource).getFileExtension()); // XXX hardcoded constant
	}

	/**
	 * NED editors should call this when they get opened.
	 */
	public void connect(IFile file) {
		if (connectCount.containsKey(file))
			connectCount.put(file, connectCount.get(file)+1);
		else {
			connectCount.put(file, 1);
		}
	}

	/**
	 * NED editors should call this when they get closed.
	 */
	public void disconnect(IFile file) {
		int count = connectCount.get(file);  // must exist
		if (count<=1) {
			// there's no open editor -- remove counter and re-read last saved state from disk
			connectCount.remove(file);
			readNEDFile(file);
		}
		else {
			connectCount.put(file, count-1);
		}
	}

    /**
     * NED editors should call this when editor content changes.
     */
    public void setNEDFileModel(IFile file, NEDElement tree) {
        if (tree==null)
            forgetNEDFile(file);  // XXX rather: it should never be called with tree==null!
        else
            storeNEDFileModel(file, tree);
        rehashIfNeeded();
    }
    
    /**
     * NED editors should call this when editor content changes.
     */
    public void setNEDFileText(IFile file, String text) {
        // parse the NED text and put it into the hash table
        NEDErrorStore errors = new NEDErrorStore();
        errors.setPrintToStderr(false);

        NEDElement tree = NEDTreeUtil.parseNedSource(text, errors, file.getLocation().toOSString());
        setNEDFileModel(file, tree);
        convertErrorsToMarkers(file, errors);
    }
    
	/**
	 * Gets called from incremental builder. 
	 */
	public void readNEDFile(IFile file) {
		// XXX for debugging
		// System.out.println(file.toString());
		
		// if this file is currently loaded in an editor, we don't read it from disk
		if (connectCount.containsKey(file))
			return;

		// parse the NED file and put it into the hash table
		String fileName = file.getLocation().toOSString();
		NEDErrorStore errors = new NEDErrorStore();
		NEDElement tree = NEDTreeUtil.loadNedSource(fileName, errors);
		convertErrorsToMarkers(file, errors);

		// only store it if there were no errors
		if (tree==null || !errors.empty())
			forgetNEDFile(file);
		else 
			storeNEDFileModel(file, tree);
	}

	private void convertErrorsToMarkers(IFile file, NEDErrorStore errors) {
		try {
			file.deleteMarkers(NEDPROBLEM_MARKERID, true, IResource.DEPTH_ZERO);
			for (int i=0; i<errors.numMessages(); i++) {
                // XXX hack: parse out line number from string. NEDErrorStore should rather store line number as int...
				String loc = errors.errorLocation(i);
				int line = parseLineNumber(loc);
				int markerSeverity = IMarker.SEVERITY_INFO;
                NEDErrorCategory category = NEDErrorCategory.swigToEnum(errors.errorCategoryCode(i));
                switch (category) {
                case ERRCAT_FATAL:
                    markerSeverity = IMarker.SEVERITY_ERROR;
                    break;
                case ERRCAT_ERROR:
                    markerSeverity = IMarker.SEVERITY_ERROR;
                    break;
                case ERRCAT_WARNING:
                    markerSeverity = IMarker.SEVERITY_WARNING;
                    break;
                case ERRCAT_INFO:
                    markerSeverity = IMarker.SEVERITY_INFO;
                    break;
                default:
                    markerSeverity = IMarker.SEVERITY_ERROR;
                    break;
                }
                addMarker(file, NEDPROBLEM_MARKERID, markerSeverity, errors.errorText(i), line);
			}
		} catch (CoreException e) {
		}
	}

	private int parseLineNumber(String loc) {
		if (loc == null) return 1;
		StringTokenizer t = new StringTokenizer(loc,":");
		while (t.hasMoreTokens()) loc = t.nextToken();
		int line = 1;
		try {line = Integer.parseInt(loc);} catch (Exception e) {}
		return line;
	}

	private void addMarker(final IFile file, final String type, int severity, String message, int line) throws CoreException {

        // taken from MarkerUtilities see. Eclipse FAQ 304
        final HashMap map = new HashMap();
        map.put(IMarker.MESSAGE, message);
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.LINE_NUMBER, line);

        IWorkspaceRunnable r= new IWorkspaceRunnable() {
            public void run(IProgressMonitor monitor) throws CoreException {
                IMarker marker= file.createMarker(type);
                marker.setAttributes(map);
            }
        };

        file.getWorkspace().run(r, null,IWorkspace.AVOID_UPDATE, null);
        System.out.println("marker added: "+type+" on "+file+" line "+line+": "+message);
	}

	public void forgetNEDFile(IFile file) {
        if (nedFiles.containsKey(file)) {
            nedFiles.remove(file);
            needsRehash = true;
        }
	}

	private void storeNEDFileModel(IFile file, NEDElement tree) {
		// store NED file contents
		Assert.isTrue(tree!=null);

        NEDElement oldTree = nedFiles.get(file);
        // if the new tree has changed, we have to rehash everything
        if (oldTree == null || !NEDTreeUtil.isNEDTreeEqual(oldTree, tree)) {
            needsRehash = true;
            nedFiles.put(file, tree);
        }
	}
 
	/**
	 * Calls rehash() if internal tables are out of date.
	 */
	public void rehashIfNeeded() {
		if (needsRehash)
			rehash();
	}

	/**
	 * Rebuild hash tables after NED resource change. Note: some errors 
	 * such as duplicate names only get detected when this gets run! 
	 */
	private synchronized void rehash() {
		long startMillis = System.currentTimeMillis();
		
        if (!needsRehash)
			return;
        
        Set<String> duplicates = new HashSet<String>();
		components.clear();
		channels.clear();
		channelInterfaces.clear();
		modules.clear();
		moduleInterfaces.clear();
        reservedNames.clear();

		components.put(nullChannelType.getName(), nullChannelType);
		components.put(basicChannelType.getName(), basicChannelType);
		components.put(bidirChannelType.getName(), bidirChannelType);
		components.put(unidirChannelType.getName(), unidirChannelType);
		
		// find toplevel components in each file, and register them
		for (IFile file : nedFiles.keySet()) {
			// remove old consistency problem markers from file, before we proceed to add new ones
			try {
				file.deleteMarkers(NEDCONSISTENCYPROBLEM_MARKERID, true, IResource.DEPTH_ZERO);
				// System.out.println("markers removed: "+NEDCONSISTENCYPROBLEM_MARKERID+" from "+file);
			} catch (CoreException e) {
			}

			// iterate on NED file contents, and register each componentt in our hash tables
			NEDElement tree = nedFiles.get(file);
            for (NEDElement node : tree) {
            	// find node's name and where it should be inserted
            	String name = null;
            	HashMap<String, INEDTypeInfo> map = null;
            	if (node instanceof ChannelNode) {
            		name = ((ChannelNode)node).getName();
            		map = channels;
            	}
            	else if (node instanceof ChannelInterfaceNode) {
            		name = ((ChannelInterfaceNode)node).getName();
            		map = channelInterfaces;
            	}
            	else if (node instanceof SimpleModuleNode) {
            		name = ((SimpleModuleNode)node).getName();
            		map = modules;
            	}
            	else if (node instanceof CompoundModuleNode) {
            		name = ((CompoundModuleNode)node).getName();
            		map = modules;
            	}
            	else if (node instanceof ModuleInterfaceNode) {
            		name = ((ModuleInterfaceNode)node).getName();
            		map = moduleInterfaces;
            	}

            	// if node is a component (name!=null), check if duplicate and store it if not
            	if (name!=null) {
            		if (components.containsKey(name)) {
            			// it is a duplicate: issue warning
            			try {
                			IFile otherFile = components.get(name).getNEDFile();
                			int line = parseLineNumber(node.getSourceLocation());
							if (otherFile==null) {
								String message = node.getTagName()+" '"+name+"' is a built-in type and cannot be redefined";
								addMarker(file, NEDCONSISTENCYPROBLEM_MARKERID, IMarker.SEVERITY_ERROR, message, line); 
							}
							else {
                                // add it to the duplicate set so we can remove them before the end
                                duplicates.add(name);
								String message = node.getTagName()+" '"+name+"' already defined in "+otherFile.getLocation().toOSString();
							    addMarker(file, NEDCONSISTENCYPROBLEM_MARKERID, IMarker.SEVERITY_ERROR, message, line);
							}
						} catch (CoreException e) {
						}
            		}
            		else {
            			INEDTypeInfo component = new NEDComponent(node, file, this); 
            			map.put(name, component);
            			components.put(name, component);
            		}
                    // add to the name list even if it was duplicate
                    reservedNames.add(name);
            	}
 			}
		}
        // now we should remove all types that were duplicates
        for (String dupName : duplicates) {
            channels.remove(dupName);
            channelInterfaces.remove(dupName);
            modules.remove(dupName);
            moduleInterfaces.remove(dupName);
            components.remove(dupName);
        }

		// rehash done!
		needsRehash = false;

		// run "semantic validator" for each file
        // FIXME this must be run in the background (it's a long running task)
        // top level element insertion is slow because of this validation
		for (IFile file : nedFiles.keySet()) {
			final IFile ifile = file;
			INEDErrorStore errors = new INEDErrorStore() {  // XXX make a better one
				public void add(NEDElement context, String message) {
					int line = parseLineNumber(context.getSourceLocation());
					try {
						addMarker(ifile, NEDCONSISTENCYPROBLEM_MARKERID, IMarker.SEVERITY_ERROR, message, line);
					} catch (CoreException e) {
					} 
				}
			};
			NEDFileValidator validator = new NEDFileValidator(this, errors);
			NedFileNode tree = (NedFileNode) nedFiles.get(file);
			validator.validate(tree);
		}

		
		//XXX temporary code, just testing:
		//for (INEDComponent c : components.values()) {
		//	c.getMemberNames();  // force resolution of inheritance
		//}

		long dt = System.currentTimeMillis() - startMillis;
		System.out.println("rehash() took "+dt+"ms");
		//System.out.println("memleak check: native NEDElements: "+org.omnetpp.ned.model.swig.NEDElement.getNumExisting()+" / "+org.omnetpp.ned.model.swig.NEDElement.getNumCreated());
	}

    public void invalidate() {
        System.out.println("NEDResources invalidated");
        needsRehash = true;
    }

    public void modelChanged(NEDModelEvent event) {
        // skip the event processing if te last serial is greater or equal. only newer
        // events should be processed. this prevent the processing of the same event multiple times
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the even and remeber this serial
            lastEventSerial = event.getSerial();
        
        // if a name property has changed everything should be rebuilt because inheritence might have changed
        // we may check only for toplevel component names and extends attributes

        if (inheritanceMayHaveChanged(event))
        {
            System.out.println("Invalidating because of: "+event);
            invalidate();
            rehashIfNeeded();
        }
    }

    /**
     * @param event
     * @return Whether the inheritance chain has changed somewhere so the whole cache should be
     * invalidated
     */
    public static boolean inheritanceMayHaveChanged(NEDModelEvent event) {
        // if we have changed a toplevel element's name we should rehash 
        if (event.getSource() instanceof ITopLevelElement 
                && event instanceof NEDAttributeChangeEvent 
                && SimpleModuleNode.ATT_NAME.equals(((NEDAttributeChangeEvent)event).getAttribute()))
            return true;
        
        // check for removal or insertion of top level nodes 
        if (event.getSource() instanceof NedFileNodeEx 
                && event instanceof NEDStructuralChangeEvent)
            return true;
                
        // check for extends name change  
        if (event.getSource() instanceof ExtendsNode 
                && event instanceof NEDAttributeChangeEvent 
                && ExtendsNode.ATT_NAME.equals(((NEDAttributeChangeEvent)event).getAttribute()))
            return true;
        // refresh if we have removed an extend node
        if (event instanceof NEDStructuralChangeEvent &&
                ((NEDStructuralChangeEvent)event).getChild() instanceof ExtendsNode &&
                ((NEDStructuralChangeEvent)event).getType() == NEDStructuralChangeEvent.Type.REMOVAL)
            return true;

        // TODO missing the handling of type, like attribute change, interface node insert,remove, change
        
        // none of the above, so do not refresh
        return false;
    }
}
