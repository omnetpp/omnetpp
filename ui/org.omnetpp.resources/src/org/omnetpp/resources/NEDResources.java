package org.omnetpp.resources;

import java.util.Collection;
import java.util.HashMap;
import java.util.Set;
import java.util.StringTokenizer;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.swig.NEDErrorStore;

/**
 * Parses all NED files in the workspace and makes them available
 * for other plugins for consistence checks among NED files etc.
 *  
 * Its readNEDFile(IFile file) method gets invoked by NEDBuilder.
 * 
 * XXX should use AbstractDocumentProvider for NED files editing? see DocumentProviderRegistry ? 
 * XXX display error markers in the Explorer view: see org.eclipse.jdt.ui.ProblemsLabelDecorator
 * XXX too many get() methods -- see which ones are actually needed...
 *
 * @author andras
 */
public class NEDResources {

	// stores parsed contents of NED files
	private HashMap<IFile, NEDElement> nedFiles = new HashMap<IFile, NEDElement>();

	private HashMap<IFile, Integer> connectCount = new HashMap<IFile, Integer>();
	
	// table of toplevel components (points into nedFiles trees)
	private HashMap<String, NEDElement> components = new HashMap<String, NEDElement>();
	private HashMap<String, IFile> componentFiles = new HashMap<String, IFile>();

	// tables of toplevel components, classified (points into nedFiles trees)
    boolean needsRehash = false;  // if tables below need to be rebuilt
	private HashMap<String, NEDElement> modules = new HashMap<String, NEDElement>();
	private HashMap<String, NEDElement> channels = new HashMap<String, NEDElement>();
	private HashMap<String, NEDElement> moduleInterfaces = new HashMap<String, NEDElement>();
	private HashMap<String, NEDElement> channelInterfaces = new HashMap<String, NEDElement>();

	/**
	 * Returns NED files in the workspace.
	 */
	public Set<IFile> getNEDFiles() {
		return nedFiles.keySet();
	}

	/**
	 * Returns parsed contents of a NED file. Returns null if file was not parseable.
	 */
	public NEDElement getNEDFileContents(IFile file) {
		return nedFiles.get(file);
	}

	/**
	 * Returns all components in the NED files.
	 */
	public Collection<NEDElement> getAllComponents() {
		if (needsRehash)
			rehash();
		return components.values();
	}

	/**
	 * Returns all simple and compound modules in the NED files.
	 */
	public Collection<NEDElement> getModules() {
		if (needsRehash)
			rehash();
		return modules.values();
	}

	/**
	 * Returns all channels in the NED files.
	 */
	public Collection<NEDElement> getChannels() {
		if (needsRehash)
			rehash();
		return channels.values();
	}

	/**
	 * Returns all module interfaces in the NED files.
	 */
	public Collection<NEDElement> getModuleInterfaces() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.values();
	}

	/**
	 * Returns all channel interfaces in the NED files.
	 */
	public Collection<NEDElement> getChannelInterfaces() {
		if (needsRehash)
			rehash();
		return channelInterfaces.values();
	}

	/**
	 * Returns all module names in the NED files.
	 */
	public Set<String> getModuleNames() {
		if (needsRehash)
			rehash();
		return modules.keySet();
	}

	/**
	 * Returns all channel names in the NED files.
	 */
	public Set<String> getChannelNames() {
		if (needsRehash)
			rehash();
		return channels.keySet();
	}

	/**
	 * Returns all module interface names in the NED files.
	 */
	public Set<String> getModuleInterfaceNames() {
		if (needsRehash)
			rehash();
		return moduleInterfaces.keySet();
	}

	/**
	 * Returns all channel interface names in the NED files.
	 */
	public Set<String> getChannelInterfaceNames() {
		if (needsRehash)
			rehash();
		return channelInterfaces.keySet();
	}

	/**
	 * Returns a component by name.
	 */
	public NEDElement getComponent(String name) {
		if (needsRehash)
			rehash();
		return components.get(name);
	}

	/**
	 *
	 */
	public boolean isNEDFile(Object element) {
		// XXX should only regard files within a folder designated as "source folder" (persistent attribute!)
		return element instanceof IFile && ((IFile) element).getFileExtension().equals("ned"); // XXX hardcoded constant
	}

	/**
	 * NED editors should call this when they get opened
	 */
	public void connect(IFile file) {
		if (connectCount.containsKey(file))
			connectCount.put(file, connectCount.get(file)+1);
		else
			connectCount.put(file, 1);
	}

	/**
	 * NED editors should call this when they get closed
	 */
	public void disconnect(IFile file) {
		int count = connectCount.get(file);  // must exist
		if (count<=1)
			connectCount.remove(file);
		else
			connectCount.put(file, count-1);
	}

	/**
	 * NED editors should call this when editor content changes
	 */
	public void setNEDFileContents(IFile file, String text) {
		// parse the NED text and put it into the hash table
		NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(false);
		NEDElement tree = ModelUtil.parseNedSource(text, errors);
		if (tree==null) {
            convertErrorsToMarkers(file, errors);
			forgetNEDFile(file);
		}
		else {
			try {
				file.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
				addMarker(file, IMarker.SEVERITY_INFO, "editor contents parsed OK", 1); //XXX remove
			} catch (CoreException e) {
				e.printStackTrace();
			}
			storeNEDFileContents(file, tree);
		}
	}

	/**
	 * NED editors should call this when editor content changes
	 */
	public void setNEDFileContents(IFile file, NEDElement tree) {
		storeNEDFileContents(file, tree);
	}

	/**
	 * Gets called from incremental builder 
	 */
	public void readNEDFile(IFile file) {
		// if this file is currently loaded in an editor, we don't read it from disk
		if (connectCount.containsKey(file))
			return;

		// parse the NED file and put it into the hash table
		String fileName = file.getLocation().toFile().getPath();
		NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(false);
		NEDElement tree = ModelUtil.loadNedSource(fileName, errors);
		if (tree==null) {
            convertErrorsToMarkers(file, errors);
			forgetNEDFile(file);
		}
		else {
			try {
				file.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
				addMarker(file, IMarker.SEVERITY_INFO, "file parsed OK", 1); //XXX remove
			} catch (CoreException e) {
				e.printStackTrace();
			}
			storeNEDFileContents(file, tree);
		}
	}

	private void convertErrorsToMarkers(IFile file, NEDErrorStore errors) {
		try {
			file.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
			for (int i=0; i<errors.numErrors(); i++) {
                // XXX hack: parse out line number from string. NEDErrorStore should rather store line number as int...
				String loc = errors.errorLocation(i);
				StringTokenizer t = new StringTokenizer(loc,":");
				while (t.hasMoreTokens()) loc = t.nextToken();
				int line = 1;
				try {line = Integer.parseInt(loc);} catch (Exception e) {}
				addMarker(file, IMarker.SEVERITY_ERROR, errors.errorText(i), line);
			}
		} catch (CoreException e) {
		}
	}

	private void addMarker(IFile file, int severity, String message, int line) throws CoreException {
		IMarker marker = file.createMarker(IMarker.PROBLEM);
		if (marker.exists()) {
			marker.setAttribute(IMarker.MESSAGE, message);
			marker.setAttribute(IMarker.SEVERITY, severity);
			marker.setAttribute(IMarker.LINE_NUMBER, line);
		}
	}

	private void forgetNEDFile(IFile file) {
		nedFiles.remove(file);
		needsRehash = true;
	}

	private void storeNEDFileContents(IFile file, NEDElement tree) {
		// store NED file contents
		nedFiles.put(file, tree);
		needsRehash = true;
	}
 
	protected void rehash() {
		// Rebuild hashtables after NED files change
		components.clear();
		componentFiles.clear();
		channels.clear();
		channelInterfaces.clear();
		modules.clear();
		moduleInterfaces.clear();

		// find toplevel components in each file, and register them
		for (IFile file : nedFiles.keySet()) {
            NEDElement tree = nedFiles.get(file);
            for (NEDElement node : tree) {
            	// find node's name and where it should be inserted
            	String name = null;
            	HashMap<String, NEDElement> map = null;
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

            	// if one of the above types (name!=null), check if duplicate and store it if not
            	if (name!=null) {
            		if (components.containsKey(name)) {
            			// duplicate: warning
            			try {
                			IFile otherFile = componentFiles.get(name);
							String message = node.getTagName()+" '"+name+"' already defined in "+otherFile.getLocation().toOSString(); 
							addMarker(file, IMarker.SEVERITY_WARNING, message, 1);
						} catch (CoreException e) {
						}
            		}
            		else {
            			map.put(name, node);
            			components.put(name, node);
            			componentFiles.put(name, file);
            		}
            	}
 			}
		}
		needsRehash = false;
	}
}
