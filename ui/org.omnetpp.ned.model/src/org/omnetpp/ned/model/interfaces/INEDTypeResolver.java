package org.omnetpp.ned.model.interfaces;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.INEDElement;

/*
 * For accessing NED types.
 *
 * @author Andras
 */
public interface INEDTypeResolver {
	/**
	 * Interface used by filtering methods.
	 */
	interface IPredicate {
		public boolean matches(INEDTypeInfo node);
	}
	
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
	public INEDElement getNEDFileModel(IFile file);

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
	public boolean hasError(IFile file);

	/**
	 * Returns a component declared at the given file/line. The line number should
	 * point into the declaration of the component. Returns null if no such component
	 * was found.
	 *
	 * @param file - must not be null
	 */
	public INEDTypeInfo getComponentAt(IFile file, int lineNumber);

    /**
     * Returns a INEDElement at the given file/line/column. returns null if no ned element
     * found under the position
     *
     * @param file - must not be null
     */
    public INEDElement getNEDElementAt(IFile file, int line, int column);

    /**
	 * Returns all components in the NED files.
	 */
	public Collection<INEDTypeInfo> getAllComponents();

    /**
     * Returns the components in the NED files that are matched by the predicate
     */
    public Collection<INEDTypeInfo> getAllComponentsFilteredBy(IPredicate predicate);

	/**
	 * Returns all simple and compound modules in the NED files.
	 */
	public Collection<INEDTypeInfo> getModules();

	/**
	 * Returns all networks in the NED files.
	 */
	public Collection<INEDTypeInfo> getNetworks();

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
     * Returns all VALID component names in the NED files.
     */
    public Set<String> getAllComponentNames();

    /**
     * Returns all VALID component names in the NED files where the predicate matches
     */
    public Set<String> getAllComponentNamesFilteredBy(IPredicate predicate);

    /**
     * Returns ALL component names in the NED files, including duplicates.
     */
    public Set<String> getReservedComponentNames();

    /**
	 * Returns all module names in the NED files.
	 */
	public Set<String> getModuleNames();

    /**
	 * Returns all network names in the NED files.
	 */
	public Set<String> getNetworkNames();

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
	 * Creates and returns an INEDTypeInfo for the given INEDElement representing
	 * a module type, channel type, etc. The component will NOT be registered
	 * in the resolver. This method was created for handling inner types.
	 */
	//XXX probably to be revised... --Andras
	public INEDTypeInfo wrapNEDElement(INedTypeNode componentNode);

}