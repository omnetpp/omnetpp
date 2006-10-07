package org.omnetpp.resources;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned2.model.NEDElement;

public interface INEDComponentResolver {

	/**
	 * Returns NED files in the workspace.
	 */
	public Set<IFile> getNEDFiles();

	/**
	 * Returns parsed contents of a NED file. Returns a potentially incomplete tree 
	 * if the file has parse errors; one can call containsNEDErrors() to find out 
	 * if that is the case.
	 *  
	 * @param file - must not be null
	 */
	public NEDElement getNEDFileContents(IFile file);

	/**
	 * Returns true if the given NED file has errors.
	 *  
	 * @param file - must not be null
	 */
	public boolean containsNEDErrors(IFile file);
	
	/**
	 * Returns a component declated at the given file/line. The line number should 
	 * point into the declaration of the component. Returns null if no such component 
	 * was found.
	 *  
	 * @param file - must not be null
	 */
	public INEDComponent getComponentAt(IFile file, int lineNumber);
	
	/**
	 * Returns all components in the NED files.
	 */
	public Collection<INEDComponent> getAllComponents();

	/**
	 * Returns all simple and compound modules in the NED files.
	 */
	public Collection<INEDComponent> getModules();

	/**
	 * Returns all channels in the NED files.
	 */
	public Collection<INEDComponent> getChannels();

	/**
	 * Returns all module interfaces in the NED files.
	 */
	public Collection<INEDComponent> getModuleInterfaces();

	/**
	 * Returns all channel interfaces in the NED files.
	 */
	public Collection<INEDComponent> getChannelInterfaces();

	/**
	 * Returns all module names in the NED files.
	 */
	public Set<String> getModuleNames();

	/**
	 * Returns all channel names in the NED files.
	 */
	public Set<String> getChannelNames();

	/**
	 * Returns all module interface names in the NED files.
	 */
	public Set<String> getModuleInterfaceNames();

	/**
	 * Returns all channel interface names in the NED files.
	 */
	public Set<String> getChannelInterfaceNames();

	/**
	 * Returns a component by name.
	 */
	public INEDComponent getComponent(String name);

	/**
	 * Creates and returns an INEDComponent for the given NEDElement representing
	 * a module type, channel type, etc. The component will NOT be registered
	 * in the resolver. This method was created for handling inner types.
	 */
	public INEDComponent wrapNEDElement(NEDElement componentNode);
	
}