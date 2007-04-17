package org.omnetpp.ned.model.interfaces;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.NEDElement;

public interface INEDTypeResolver {
     /**
     * 
     * @author rhornig
     */
    interface IFilter {
        /**
         * @param component
         * @return True if it shuld be included in the resuting collection
         */
        boolean filter(INEDTypeInfo component);
    };

    /**
     * Mark the whole resolver invalid
     */
    public void invalidate();
    
    /**
     * Recalculate everything if the state is invalid
     */
    public void rehashIfNeeded();
    
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
	public NEDElement getNEDFileModel(IFile file);

    /**
     * Returns parsed and reformatted contents of a NED file. Returns a potentially incomplete text 
     * if the file has parse errors; one can call containsNEDErrors() to find out 
     * if that is the case.
     *  
     * @param file - must not be null
     */
    public String getNEDFileText(IFile file);

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
	public INEDTypeInfo getComponentAt(IFile file, int lineNumber);
	
    /**
     * Returns a NEDElement at the given file/line/column. returns nul if no ned element
     * found under the position
     *  
     * @param file - must not be null
     */
    public NEDElement getNEDElementAt(IFile file, int line, int column);

    /**
	 * Returns all components in the NED files.
	 */
	public Collection<INEDTypeInfo> getAllComponents();

    /**
     * Returns all components in the NED files after applying the filter
     */
    public Collection<INEDTypeInfo> getAllComponentsFilteredBy(IFilter filter);
    
	/**
	 * Returns all simple and compound modules in the NED files.
	 */
	public Collection<INEDTypeInfo> getModules();

	/**
	 * Returns all channels in the NED files.
	 */
	public Collection<INEDTypeInfo> getChannels();

	/**
	 * Returns all module interfaces in the NED files.
	 */
	public Collection<INEDTypeInfo> getModuleInterfaces();

	/**
	 * Returns all channel interfaces in the NED files.
	 */
	public Collection<INEDTypeInfo> getChannelInterfaces();

    /**
     * Returns all VALID component name in the NED files.
     */
    public Set<String> getAllComponentNames();

    /**
     * Returns all VALID component name in the NED files after filtered by the filter
     */
    public Set<String> getAllComponentNamesFilteredBy(IFilter filter);

    /**
     * Returns ALL names reserved (used) in the NED files (including duplicates)
     */
    public Set<String> getReservedNames();
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
	 * Returns a component by name, or null if it does not exist.
	 */
	public INEDTypeInfo getComponent(String name);

	/**
	 * Creates and returns an INEDTypeInfo for the given NEDElement representing
	 * a module type, channel type, etc. The component will NOT be registered
	 * in the resolver. This method was created for handling inner types.
	 */
	public INEDTypeInfo wrapNEDElement(NEDElement componentNode);
	
}