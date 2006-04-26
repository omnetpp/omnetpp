package org.omnetpp.resources;

import java.util.Collection;
import java.util.HashMap;
import java.util.Set;
import java.util.StringTokenizer;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtil;
import org.omnetpp.ned2.model.NEDSourceRegion;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.ParamNode;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.swig.NEDErrorStore;

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
public class NEDResources implements INEDComponentResolver {

	// markers created during parsing
	public static final String NEDPROBLEM_MARKERID = "org.omnetpp.resources.nedproblem";

	// markers created in rehash()
	public static final String NEDCONSISTENCYPROBLEM_MARKERID = "org.omnetpp.resources.nedconsistencyproblem";
	
	// stores parsed contents of NED files
	private HashMap<IFile, NEDElement> nedFiles = new HashMap<IFile, NEDElement>();

	private HashMap<IFile, Integer> connectCount = new HashMap<IFile, Integer>();
	
	// table of toplevel components (points into nedFiles trees)
	private HashMap<String, INEDComponent> components = new HashMap<String, INEDComponent>();

	// tables of toplevel components, classified (points into nedFiles trees)
    private boolean needsRehash = false;  // if tables below need to be rebuilt
	private HashMap<String, INEDComponent> modules = new HashMap<String, INEDComponent>();
	private HashMap<String, INEDComponent> channels = new HashMap<String, INEDComponent>();
	private HashMap<String, INEDComponent> moduleInterfaces = new HashMap<String, INEDComponent>();
	private HashMap<String, INEDComponent> channelInterfaces = new HashMap<String, INEDComponent>();

	private INEDComponent defaultChannelType = null;

	/**
	 * Constructor.
	 */
	public NEDResources() {
		// create default channel type... 
		ChannelNode channel = new ChannelNode();
		channel.setName("BasicChannel");
		ParametersNode params = new ParametersNode(channel);
		createImplicitChannelParameter(params, "delay", NEDElementUtil.NED_PARTYPE_DOUBLE);
		createImplicitChannelParameter(params, "error", NEDElementUtil.NED_PARTYPE_DOUBLE);
		createImplicitChannelParameter(params, "datarate", NEDElementUtil.NED_PARTYPE_DOUBLE);
		defaultChannelType = new NEDComponent(channel, null, this); 
	}	

	/* utility method */
	protected NEDElement createImplicitChannelParameter(NEDElement parent, String name, int type) {
		ParamNode param = new ParamNode(parent);
		param.setName(name);
		param.setType(type);
		param.setIsFunction(false);
		param.setIsDefault(false);
		//TODO add default value of zero
		param.setSourceLocation("internal");
		return param;
	}
	
	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getNEDFiles()
	 */
	public Set<IFile> getNEDFiles() {
		return nedFiles.keySet();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getNEDFileContents(org.eclipse.core.resources.IFile)
	 */
	public NEDElement getNEDFileContents(IFile file) {
		if (needsRehash)
			rehash();
		return nedFiles.get(file);
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#containsNEDErrors(org.eclipse.core.resources.IFile)
	 */
	public boolean containsNEDErrors(IFile file) {
        return hasErrorMarker(file, NEDPROBLEM_MARKERID) || hasErrorMarker(file, NEDCONSISTENCYPROBLEM_MARKERID);
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getDefaultChannelType()
	 */
	public INEDComponent getDefaultChannelType() {
		return defaultChannelType;
	}
	
	/**
	 * True if the file has any marker of the given marker type id (or subclass) 
	 * with severity ERROR; or if an error occurred while checking the markers. 
	 */
	private boolean hasErrorMarker(IFile file, String markerType) {
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
	
	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getComponentAt(org.eclipse.core.resources.IFile, int)
	 */
	public INEDComponent getComponentAt(IFile file, int lineNumber) {
		if (needsRehash)
			rehash();
		for (INEDComponent component : components.values()) {
			if (file.equals(component.getNEDFile())) {
				NEDSourceRegion region = component.getNEDElement().getSourceRegion();
				if (region!=null && region.containsLine(lineNumber))
					return component;
			}
		}
		return null;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getAllComponents()
	 */
	public Collection<INEDComponent> getAllComponents() {
		if (needsRehash)
			rehash();
		return components.values();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getModules()
	 */
	public Collection<INEDComponent> getModules() {
		if (needsRehash)
			rehash();
		return modules.values();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getChannels()
	 */
	public Collection<INEDComponent> getChannels() {
		if (needsRehash)
			rehash();
		return channels.values();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getModuleInterfaces()
	 */
	public Collection<INEDComponent> getModuleInterfaces() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.values();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getChannelInterfaces()
	 */
	public Collection<INEDComponent> getChannelInterfaces() {
		if (needsRehash)
			rehash();
		return channelInterfaces.values();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getModuleNames()
	 */
	public Set<String> getModuleNames() {
		if (needsRehash)
			rehash();
		return modules.keySet();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getChannelNames()
	 */
	public Set<String> getChannelNames() {
		if (needsRehash)
			rehash();
		return channels.keySet();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getModuleInterfaceNames()
	 */
	public Set<String> getModuleInterfaceNames() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.keySet();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getChannelInterfaceNames()
	 */
	public Set<String> getChannelInterfaceNames() {
		if (needsRehash)
			rehash();
		return channelInterfaces.keySet();
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.resources.INEDComponentResolver#getComponent(java.lang.String)
	 */
	public INEDComponent getComponent(String name) {
		if (needsRehash)
			rehash();
		return components.get(name);
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
	public void setNEDFileContents(IFile file, String text) {
		// parse the NED text and put it into the hash table
		NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(false);
		NEDElement tree = ModelUtil.parseNedSource(text, errors);
		convertErrorsToMarkers(file, errors);

		// store it even if there were errors (needed by content assist)
		if (tree==null)
			forgetNEDFile(file);
		else 
			storeNEDFileContents(file, tree);
		rehashIfNeeded();
	}

	/**
	 * NED editors should call this when editor content changes.
	 */
	public void setNEDFileContents(IFile file, NEDElement tree) {
		if (tree==null)
			forgetNEDFile(file);  // XXX rather: it should never be called with tree==null!
		else
			storeNEDFileContents(file, tree);
		rehashIfNeeded();
	}

	/**
	 * Gets called from incremental builder. 
	 */
	public void readNEDFile(IFile file) {
		// if this file is currently loaded in an editor, we don't read it from disk
		if (connectCount.containsKey(file))
			return;

		// parse the NED file and put it into the hash table
		String fileName = file.getLocation().toFile().getPath();
		NEDErrorStore errors = new NEDErrorStore();
		NEDElement tree = ModelUtil.loadNedSource(fileName, errors);
		convertErrorsToMarkers(file, errors);

		// only store it if there were no errors
		if (tree==null || !errors.empty())
			forgetNEDFile(file);
		else 
			storeNEDFileContents(file, tree);
	}

	private void convertErrorsToMarkers(IFile file, NEDErrorStore errors) {
		try {
			file.deleteMarkers(NEDPROBLEM_MARKERID, true, IResource.DEPTH_ZERO);
			for (int i=0; i<errors.numErrors(); i++) {
                // XXX hack: parse out line number from string. NEDErrorStore should rather store line number as int...
				String loc = errors.errorLocation(i);
				int line = parseLineNumber(loc);
				addMarker(file, NEDPROBLEM_MARKERID, IMarker.SEVERITY_ERROR, errors.errorText(i), line);
			}
			//if (errors.numErrors()==0)
			//	addMarker(file, NEDPROBLEM_MARKERID, IMarker.SEVERITY_INFO, "parsed OK", 1); //XXX remove
		} catch (CoreException e) {
		}
	}

	private int parseLineNumber(String loc) {
		StringTokenizer t = new StringTokenizer(loc,":");
		while (t.hasMoreTokens()) loc = t.nextToken();
		int line = 1;
		try {line = Integer.parseInt(loc);} catch (Exception e) {}
		return line;
	}

	private void addMarker(IFile file, String type, int severity, String message, int line) throws CoreException {
		IMarker marker = file.createMarker(type);
		if (marker.exists()) {
			marker.setAttribute(IMarker.MESSAGE, message);
			marker.setAttribute(IMarker.SEVERITY, severity);
			marker.setAttribute(IMarker.LINE_NUMBER, line);
			//System.out.println("marker added: "+type+" on "+file+": "+message);
		}
	}

	public void forgetNEDFile(IFile file) {
		nedFiles.remove(file);
		needsRehash = true;
	}

	private void storeNEDFileContents(IFile file, NEDElement tree) {
		// store NED file contents
		Assert.isTrue(tree!=null);
		nedFiles.put(file, tree);
		needsRehash = true;
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
		//long startMillis = System.currentTimeMillis();
		if (!needsRehash)
			return;
		components.clear();
		channels.clear();
		channelInterfaces.clear();
		modules.clear();
		moduleInterfaces.clear();

		// find toplevel components in each file, and register them
		for (IFile file : nedFiles.keySet()) {
			// remove old consistency problem markers from file, before we proceed to add new ones
			try {
				file.deleteMarkers(NEDCONSISTENCYPROBLEM_MARKERID, true, IResource.DEPTH_ZERO);
				//System.out.println("markers removed: "+NEDCONSISTENCYPROBLEM_MARKERID+" from "+file);
			} catch (CoreException e) {
			}

			// iterate on NED file contents, and register each componentt in our hash tables
			NEDElement tree = nedFiles.get(file);
            for (NEDElement node : tree) {
            	// find node's name and where it should be inserted
            	String name = null;
            	HashMap<String, INEDComponent> map = null;
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
							String message = node.getTagName()+" '"+name+"' already defined in "+otherFile.getLocation().toOSString();
							int line = parseLineNumber(node.getSourceLocation());
							addMarker(file, NEDCONSISTENCYPROBLEM_MARKERID, IMarker.SEVERITY_WARNING, message, line); 
						} catch (CoreException e) {
						}
            		}
            		else {
            			INEDComponent component = new NEDComponent(node, file, this); 
            			map.put(name, component);
            			components.put(name, component);
            		}
            	}
 			}
		}

		// rehash done!
		needsRehash = false;

		// run "semantic validator" for each file
		for (IFile file : nedFiles.keySet()) {
			final IFile ifile = file;
			INEDErrorStore errors = new INEDErrorStore() {  // XXX make better one
				public void add(NEDElement context, String message) {
					int line = parseLineNumber(context.getSourceLocation());
					try {
						addMarker(ifile, NEDCONSISTENCYPROBLEM_MARKERID, IMarker.SEVERITY_ERROR, message, line);
					} catch (CoreException e) {
					} 
				}
			};
			NEDValidator validator = new NEDValidator(this, errors);
			NEDElement tree = nedFiles.get(file);
			validator.validate(tree);
		}

		
		//XXX temporary code, just testing:
		//for (INEDComponent c : components.values()) {
		//	c.getMemberNames();  // force resolution of inheritance
		//}

		//long dt = System.currentTimeMillis() - startMillis;
		//System.out.println("rehash() took "+dt+"ms");
		//System.out.println("memleak check: native NEDElements: "+org.omnetpp.ned2.model.swig.NEDElement.getNumExisting()+" / "+org.omnetpp.ned2.model.swig.NEDElement.getNumCreated());
	}
}
