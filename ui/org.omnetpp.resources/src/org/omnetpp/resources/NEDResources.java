package org.omnetpp.resources;

import java.util.Collection;
import java.util.Enumeration;
import java.util.Hashtable;

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
	private Hashtable<IFile, NEDElement> nedFiles = new Hashtable<IFile, NEDElement>();

	// stores textual contents of NED files
	private Hashtable<IFile, String> nedSources = new Hashtable<IFile, String>();

	// table of toplevel components (points into nedFiles trees)
	private Hashtable<String, NEDElement> components = new Hashtable<String, NEDElement>();

	// table of toplevel components, classified (points into nedFiles trees)
	private Hashtable<String, NEDElement> modules = new Hashtable<String, NEDElement>();
	private Hashtable<String, NEDElement> channels = new Hashtable<String, NEDElement>();
	private Hashtable<String, NEDElement> moduleInterfaces = new Hashtable<String, NEDElement>();
	private Hashtable<String, NEDElement> channelInterfaces = new Hashtable<String, NEDElement>();

	/**
	 * Returns NED files in the workspace.
	 */
	public Enumeration<IFile> getNEDFiles() {
		return nedSources.keys();
	}

	/**
	 * Returns parsed contents of a NED file. Returns null if file was not parseable.
	 */
	public NEDElement getNEDFileContents(IFile file) {
		return nedFiles.get(file);
	}

	/**
	 * Returns a textual contents of NED file
	 */
	public String getNEDFileSource(IFile file) {
		return nedSources.get(file);
	}

	/**
	 * Returns all components in the NED files.
	 */
	public Collection<NEDElement> getAllComponents() {
		return components.values();
	}

	/**
	 * Returns all simple and compound modules in the NED files.
	 */
	public Collection<NEDElement> getModules() {
		return modules.values();
	}

	/**
	 * Returns all channels in the NED files.
	 */
	public Collection<NEDElement> getChannels() {
		return channels.values();
	}

	/**
	 * Returns all module interfaces in the NED files.
	 */
	public Collection<NEDElement> getModuleInterfaces() {
		return moduleInterfaces.values();
	}

	/**
	 * Returns all channel interfaces in the NED files.
	 */
	public Collection<NEDElement> getChannelInterfaces() {
		return channelInterfaces.values();
	}

	/**
	 * Returns all module names in the NED files.
	 */
	public Enumeration<String> getModuleNames() {
		return modules.keys();
	}

	/**
	 * Returns all channel names in the NED files.
	 */
	public Enumeration<String> getChannelNames() {
		return channels.keys();
	}

	/**
	 * Returns all module interface names in the NED files.
	 */
	public Enumeration<String> getModuleInterfaceNames() {
		return moduleInterfaces.keys();
	}

	/**
	 * Returns all channel interface names in the NED files.
	 */
	public Enumeration<String> getChannelInterfaceNames() {
		return channelInterfaces.keys();
	}

	/**
	 * Returns a component by name.
	 */
	public NEDElement getComponent(String name) {
		return components.get(name);
	}
	
//	public void reread() {
//		nedFiles.clear();
//		components.clear();
//    	IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
//    	iterateResourceContainer(workspaceRoot);
//    }
//
//	private void iterateResourceContainer(IContainer parent) {
//		try {
//			for (IResource res : parent.members()) {
//				if (res instanceof IContainer) {
//					//System.out.println("container: "+res);
//					iterateResourceContainer((IContainer)res);
//				}
//				else if (res instanceof IFile) {
//					IFile file = (IFile)res;
//					if (NED_FILE_EXTENSION.equals(file.getFileExtension())) {
//						readNEDFile(file);
//					}
//				}
//			}
//		} catch (CoreException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//	}

	/**
	 * Gets called from incremental builder 
	 */
	public void readNEDFile(IFile file) {
		// read NED file textually, and store it
		//String text = readFile(file);
		
		// parse the NED file and put it into the hash table
		System.out.println("parsing nedfile: "+file);
		String fileName = file.getLocation().toFile().getPath();
		NEDElement tree = ModelUtil.loadNedSource(fileName);
		if (tree==null) {
			System.out.println(" ERROR");

			try {
				// XXX experimental: adds an entry to the Problems view
				file.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
				addMarker(file, IMarker.SEVERITY_ERROR, "cannot parse", 1); //XXX refine
			} catch (CoreException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else {
			System.out.println(" stored");
			try {
				file.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
				addMarker(file, IMarker.SEVERITY_INFO, "parsed fine!!!", 1); //XXX refine
			} catch (CoreException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			insertNEDFile(file, tree);
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

	private void insertNEDFile(IFile file, NEDElement tree) {
		// store NED file contents
		nedFiles.put(file, tree);
		
		// find toplevel components in the file, and put them into tables
		for (NEDElement node : tree) {
			String name = null;
			if (node instanceof ChannelNode) {
				name = ((ChannelNode)node).getName();
				channels.put(name, node);
			}
			else if (node instanceof ChannelInterfaceNode) {
				name = ((ChannelInterfaceNode)node).getName();
				channelInterfaces.put(name, node);
			}
			else if (node instanceof SimpleModuleNode) {
				name = ((SimpleModuleNode)node).getName();
				modules.put(name, node);
			}
			else if (node instanceof CompoundModuleNode) {
				name = ((CompoundModuleNode)node).getName();
				modules.put(name, node);
			}
			else if (node instanceof ModuleInterfaceNode) {
				name = ((ModuleInterfaceNode)node).getName();
				moduleInterfaces.put(name, node);
			}

			if (name!=null)
				components.put(name, node);
		}
	}
}
