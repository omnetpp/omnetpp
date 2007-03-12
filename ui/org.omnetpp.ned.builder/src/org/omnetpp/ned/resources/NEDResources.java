package org.omnetpp.ned.resources;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.ned.engine.NEDErrorStore;
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

/**
 * Parses all NED files in the workspace and makes them available
 * for other plugins for consistence checks among NED files etc.
 *  
 * It listens to workspace resource changes and modifies it content based on change
 * notifications
 * 
 * XXX should do full rebuild when Eclipse starts up!!!
 * XXX default installation should have "workspace auto refresh" enabled, and "Problems view" shown!!! 
 * XXX when something changes, we always rebuild INEDComponents. This is not needed -- rather, we
 *     should just call NEDComponent.componentsChanged()! (PERFORMANCE)
 *  
 * @author andras
 */
public class NEDResources implements INEDTypeResolver, IResourceChangeListener {

    private static final String NED_EXTENSION = "ned";

	// stores parsed contents of NED files
	private HashMap<IFile, NEDElement> nedFiles = new HashMap<IFile, NEDElement>();
    private ProblemMarkerJob markerJob = new ProblemMarkerJob("Updating problem markers");

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
		// create built-in channel type cIdealChannel
		ChannelNode nullChannel = (ChannelNode)NEDElementFactoryEx
                                    .getInstance().createNodeWithTag(NEDElementTags.NED_CHANNEL);
		nullChannel.setName("cIdealChannel");
		nullChannel.setIsWithcppclass(true);
		nullChannelType = new NEDComponent(nullChannel, null, this); 

		// create built-in channel ctype BasicChannel
		ChannelNode basicChannel = (ChannelNode) NEDElementFactoryEx
                                    .getInstance().createNodeWithTag(NEDElementTags.NED_CHANNEL);
		basicChannel.setName("cBasicChannel");
		basicChannel.setIsWithcppclass(true);
		ParametersNode params = (ParametersNode) NEDElementFactoryEx
		                    .getInstance().createNodeWithTag(NEDElementTags.NED_PARAMETERS, basicChannel);
		params.appendChild(createImplicitChannelParameter("delay", NEDElementUtil.NED_PARTYPE_DOUBLE));
        params.appendChild(createImplicitChannelParameter("error", NEDElementUtil.NED_PARTYPE_DOUBLE));
        params.appendChild(createImplicitChannelParameter("datarate", NEDElementUtil.NED_PARTYPE_DOUBLE));
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
                                .getInstance().createNodeWithTag(NEDElementTags.NED_GATES, bidirChannel);
		gates.appendChild(createGate("a", NEDElementUtil.NED_GATETYPE_INOUT));
        gates.appendChild(createGate("b", NEDElementUtil.NED_GATETYPE_INOUT));
		bidirChannelType = new NEDComponent(bidirChannel, null, this); 
		
		ModuleInterfaceNode unidirChannel = (ModuleInterfaceNode) NEDElementFactoryEx
                                .getInstance().createNodeWithTag(NEDElementTags.NED_MODULE_INTERFACE);
		unidirChannel.setName("IUnidirectionalChannel");
        GatesNode gates2 = (GatesNode) NEDElementFactoryEx
                                        .getInstance().createNodeWithTag(NEDElementTags.NED_GATES, unidirChannel);
		gates2.appendChild(createGate("i", NEDElementUtil.NED_GATETYPE_INPUT));
        gates2.appendChild(createGate("o", NEDElementUtil.NED_GATETYPE_OUTPUT));
		unidirChannelType = new NEDComponent(unidirChannel, null, this);
	}	

	/* utility method */
	protected NEDElement createGate(String name, int type) {
		GateNode g = (GateNode) NEDElementFactoryEx
                            .getInstance().createNodeWithTag(NEDElementTags.NED_GATE);
		g.setName(name);
		g.setType(type);
		return g;
	}

	/* utility method */
	protected NEDElement createImplicitChannelParameter(String name, int type) {
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
	
    public synchronized Set<IFile> getNEDFiles() {
		return nedFiles.keySet();
	}

	public synchronized NEDElement getNEDFileContents(IFile file) {
		if (needsRehash)
			rehash();
		return nedFiles.get(file);
	}

	public synchronized boolean containsNEDErrors(IFile file) {
        return markerJob.hasErrors(file);
	}

	public synchronized INEDTypeInfo getComponentAt(IFile file, int lineNumber) {
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

	public synchronized Collection<INEDTypeInfo> getAllComponents() {
		if (needsRehash)
			rehash();
		return components.values();
	}

	public synchronized Collection<INEDTypeInfo> getModules() {
		if (needsRehash)
			rehash();
		return modules.values();
	}

	public synchronized Collection<INEDTypeInfo> getChannels() {
		if (needsRehash)
			rehash();
		return channels.values();
	}

	public synchronized Collection<INEDTypeInfo> getModuleInterfaces() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.values();
	}

	public synchronized Collection<INEDTypeInfo> getChannelInterfaces() {
		if (needsRehash)
			rehash();
		return channelInterfaces.values();
	}

    public synchronized Set<String> getAllComponentNames() {
        if (needsRehash)
            rehash();
        return components.keySet();
    }

    public synchronized Set<String> getReservedNames() {
        if (needsRehash)
            rehash();
        return reservedNames;
    }
    
    public synchronized Set<String> getModuleNames() {
		if (needsRehash)
			rehash();
		return modules.keySet();
	}

	public synchronized Set<String> getChannelNames() {
		if (needsRehash)
			rehash();
		return channels.keySet();
	}

	public synchronized Set<String> getModuleInterfaceNames() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.keySet();
	}

	public synchronized Set<String> getChannelInterfaceNames() {
		if (needsRehash)
			rehash();
		return channelInterfaces.keySet();
	}

	public synchronized INEDTypeInfo getComponent(String name) {
		if (needsRehash)
			rehash();
		return components.get(name);
	}

	public synchronized INEDTypeInfo wrapNEDElement(NEDElement componentNode) {
		return new NEDComponent(componentNode, null, this); 
	}

	/**
	 * Determines if a resource is a NED file, that is, if it should be parsed.
	 */
	public static boolean isNEDFile(IResource resource) {
		// TODO should only regard files within a folder designated as "source folder" (persistent attribute!)
		return resource instanceof IFile && NED_EXTENSION.equalsIgnoreCase(((IFile) resource).getFileExtension());
	}

	/**
	 * NED editors should call this when they get opened.
	 */
	public synchronized void connect(IFile file) {
		if (connectCount.containsKey(file))
			connectCount.put(file, connectCount.get(file)+1);
		else {
			connectCount.put(file, 1);
		}
	}

	/**
	 * NED editors should call this when they get closed.
	 */
	public synchronized void disconnect(IFile file) {
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
    public synchronized void setNEDFileModel(IFile file, NEDElement tree) {
        if (tree==null)
            forgetNEDFile(file);  // XXX rather: it should never be called with tree==null!
        else
            storeNEDFileModel(file, tree);
        rehashIfNeeded();
    }
    
    /**
     * NED editors should call this when editor content changes.
     */
    public synchronized void setNEDFileText(IFile file, String text) {
        // parse the NED text and put it into the hash table
        NEDErrorStore errors = new NEDErrorStore();
        errors.setPrintToStderr(false);

        NEDElement tree = NEDTreeUtil.parseNedSource(text, errors, file.getLocation().toOSString());
        setNEDFileModel(file, tree);
        markerJob.setParseErrorStore(file, errors);
        // nedParseErrors.put(file, errors);
        // convertErrorsToMarkers(file, errors);
    }
    
	/**
	 * Gets called from incremental builder. 
	 */
	public synchronized void readNEDFile(IFile file) {
		// XXX for debugging
		// System.out.println(file.toString());
		
		// if this file is currently loaded in an editor, we don't read it from disk
		if (connectCount.containsKey(file))
			return;

		// parse the NED file and put it into the hash table
		String fileName = file.getLocation().toOSString();
		NEDErrorStore errors = new NEDErrorStore();
		NEDElement tree = NEDTreeUtil.loadNedSource(fileName, errors);
        markerJob.setParseErrorStore(file, errors);

		// only store it if there were no errors
		if (tree==null || !errors.empty())
			forgetNEDFile(file);
		else 
			storeNEDFileModel(file, tree);
	}

	public synchronized void forgetNEDFile(IFile file) {
        if (nedFiles.containsKey(file)) {
            nedFiles.remove(file);
            markerJob.removeStores(file);
            needsRehash = true;
        }
	}

	private synchronized void storeNEDFileModel(IFile file, NEDElement tree) {
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
	public synchronized void rehashIfNeeded() {
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
        // rehash done!
        needsRehash = false;

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
            // create a new ErrorStore for the consistency problems (the old one
            // will be deleted if exists)
            NEDErrorStore errorStore = new NEDErrorStore();
            markerJob.setConsistencyErrorStore(file, errorStore);

            // iterate on NED file contents, and register each componentt in our
            // hash tables
            NEDElement tree = nedFiles.get(file);
            for (NEDElement node : tree) {
                // find node's name and where it should be inserted
                String name = null;
                HashMap<String, INEDTypeInfo> map = null;
                if (node instanceof ChannelNode) {
                    name = ((ChannelNode) node).getName();
                    map = channels;
                } else if (node instanceof ChannelInterfaceNode) {
                    name = ((ChannelInterfaceNode) node).getName();
                    map = channelInterfaces;
                } else if (node instanceof SimpleModuleNode) {
                    name = ((SimpleModuleNode) node).getName();
                    map = modules;
                } else if (node instanceof CompoundModuleNode) {
                    name = ((CompoundModuleNode) node).getName();
                    map = modules;
                } else if (node instanceof ModuleInterfaceNode) {
                    name = ((ModuleInterfaceNode) node).getName();
                    map = moduleInterfaces;
                }

                // if node is a component (name!=null), check if duplicate and
                // store it if not
                if (name != null) {
                    if (components.containsKey(name)) {
                        // it is a duplicate: issue warning
                        IFile otherFile = components.get(name).getNEDFile();
                        if (otherFile == null) {
                            String message = node.getTagName()
                                    + " '"
                                    + name
                                    + "' is a built-in type and cannot be redefined";
                            errorStore
                                    .add(node.getSourceLocation(), 2, message);
                        } else {
                            // add it to the duplicate set so we can remove them
                            // before the end
                            duplicates.add(name);
                            String message = node.getTagName() + " '" + name
                                    + "' already defined in "
                                    + otherFile.getFullPath().toString();
                            errorStore
                                    .add(node.getSourceLocation(), 2, message);
                            // add the same error message to the other file too
                            String otherMessage = node.getTagName() + " '"
                                    + name + "' already defined in "
                                    + file.getFullPath().toString();
                            errorStore.add(components.get(name).getNEDElement()
                                    .getSourceLocation(), 2, otherMessage);
                        }
                    } else {
                        INEDTypeInfo component = new NEDComponent(node, file,
                                this);
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

        // run "semantic validator" for each file
        // FIXME this must be run in the background (it's a long running task)
        // top level element insertion is slow because of this validation
        for (IFile file : nedFiles.keySet()) {
            final NEDErrorStore consistencyErrors = markerJob
                    .getConsistencyErrorStore(file);
            INEDErrorStore errors = new INEDErrorStore() { // XXX make a better
                                                            // one
                public void add(NEDElement context, String message) {
                    consistencyErrors.add(context.getSourceLocation(), 2,
                            message);
                }
            };
            NEDFileValidator validator = new NEDFileValidator(this, errors);
            NedFileNode tree = (NedFileNode) nedFiles.get(file);
            validator.validate(tree);
        }

        markerJob.schedule();

        long dt = System.currentTimeMillis() - startMillis;
        System.out.println("rehash() took " + dt + "ms");
    }

    public synchronized void invalidate() {
        if (!needsRehash) {
            System.out.println("NEDResources invalidated");
            needsRehash = true;
        }
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
     * @return The inheritance chain has changed somewhere so the whole cache should be
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

// ************************************************************************************************    
    static int resourceChange = 0;
    
    public synchronized void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null) return;
//            System.out.println("resourceChange conter: "+resourceChange++);
//            System.out.println(((ResourceDelta)event.getDelta()).toDeepDebugString());
            
            event.getDelta().accept(            
                    new IResourceDeltaVisitor() {
                        public boolean visit(IResourceDelta delta) throws CoreException {
                            IResource resource = delta.getResource();
                            // continue visiting the children if it is not a NED file
                            if (!isNEDFile(resource))
                                return true;

                            switch (delta.getKind()) {
                            case IResourceDelta.REMOVED:
                                // handle removed resource
                                forgetNEDFile((IFile) resource);
                                invalidate();
                                break;
                            case IResourceDelta.ADDED:
                                readNEDFile((IFile) resource);
                                invalidate();
                                break;
                            case IResourceDelta.CHANGED:
                                // handle changed resource, but we are interested only in content change
                                // we don't care about marker and property changes
                                if ((delta.getFlags() & IResourceDelta.CONTENT)!=0) {
                                    readNEDFile((IFile) resource);
                                    invalidate();
                                }
                                break;
                            }
                            // ned files do not have children
                            return false;
                        }
                    }
            );
            
        } catch (CoreException e) {
            System.out.println("Error during workspace change notification: "+e);
        } finally {
            rehashIfNeeded();
        }
        
    }

}
