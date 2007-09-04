package org.omnetpp.ned.model.interfaces;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;

/*
 * For accessing NED types.
 *
 * @author Andras
 */
public interface INEDTypeResolver {

    public static final String NEDSYNTAXPROBLEM_MARKERID = "org.omnetpp.ned.core.nedsyntaxproblem";
    public static final String NEDCONSISTENCYPROBLEM_MARKERID = "org.omnetpp.ned.core.nedconsistencyproblem";

    /**
	 * Interface used by filtering methods.
	 */
	public interface IPredicate {
		public boolean matches(INEDTypeInfo typeInfo);
	}

	/**
	 * INTERNAL Factory method, to be called from INedTypeElement constructors.
	 */
	public INEDTypeInfo createTypeInfoFor(INedTypeElement node);

    /**
     * Recalculate everything if the state is invalid
     */
    public void rehashIfNeeded();

	/**
	 * Returns NED files in the workspace.
	 */
	public Set<IFile> getNedFiles();

	/**
	 * Returns parsed contents of a NED file. Returns a potentially incomplete tree
	 * if the file has parse errors; one can call containsNEDErrors() to find out
	 * if that is the case. Return value is never null. It is an error to invoke
	 * this method on a non-NED file.
	 *
	 * @param file - must not be null
	 */
	public NedFileElementEx getNedFileElement(IFile file);

    /**
     * XXX todo
     */
	public IFile getNedFile(NedFileElementEx nedFileElement);

    /**
     * Returns parsed and reformatted contents of a NED file. Returns a potentially incomplete text
     * if the file has parse errors; one can call containsNEDErrors() to find out
     * if that is the case.
     *
     * @param file - must not be null
     */
    public String getNedFileText(IFile file);

	/**
	 * Returns the markers for the given element and its subtree.
	 *
	 * IMPORTANT: Do NOT use this method to check whether an element actually
	 * contains errors! Markers are written out in a background job, and there's
	 * no guarantee that IFile already contains them. Use getMaxProblemSeverity()
	 * and the likes from INEDElement instead.
	 */
    public IMarker[] getMarkersForElement(INEDElement node);

    /**
     * Returns a INEDElement at the given file/line/column. Returns null if no
     * NED element was found at that position (that may be caused by 
     * missing source region info.)
     */
    public INEDElement getNedElementAt(IFile file, int line, int column);

    /**
     * Returns a INEDElement within the given parent at the given line/column. 
     * Returns null if no NED element was found at that position (that may be 
     * caused by missing source region info.)
     */
    public INEDElement getNedElementAt(INEDElement parent, int line, int column);
    
    /**
	 * Returns all components in the NED files.
	 */
	public Collection<INEDTypeInfo> getAllNedTypes();

    /**
     * Returns all VALID component names in the NED files.
	 * Returned names are fully qualified.
     */
    public Set<String> getAllNedTypeQNames();

    /**
     * Returns all VALID component names in the NED files where the predicate matches.
	 * Returned names are fully qualified.
     */
    public Set<String> getNedTypeQNames(IPredicate predicate);

    /**
     * Returns ALL component names in the NED files, including duplicates.
	 * Returned names are fully qualified.
     */
    public Set<String> getReservedQNames();

    /**
     * Return a NED type from its fully qualified name. Returns null if not found.
     */
    public INEDTypeInfo getNedType(String qualifiedName);

    /**
	 * Looks up the name in the given context, and returns the NED type info,
	 * or null if it does not exist. Considers imports, etc; inner types are
	 * also handled.
	 * @param name  May be a simple name or a qualified name; cannot be null
	 * @param context May not be null.
	 */
	public INEDTypeInfo lookupNedType(String name, INedTypeLookupContext context);

	/**
	 * Return all NED type names visible in the given context without
	 * fully qualifying them. Returned names are short names (NOT fully qualified).
	 */
	public Set<String> getVisibleTypeNames(INedTypeLookupContext context);

	/**
	 * Return all NED type names matching the predicate, and visible in the given
	 * context without fully qualifying them. Returned names are short names
	 * (NOT fully qualified).
	 */
	public Set<String> getVisibleTypeNames(INedTypeLookupContext context, IPredicate predicate);

    /**
	 * Returns all module names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getModuleQNames();

    /**
	 * Returns all network names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getNetworkQNames();

	/**
	 * Returns all channel names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getChannelQNames();

	/**
	 * Returns all module interface names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getModuleInterfaceQNames();

	/**
	 * Returns all channel interface names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getChannelInterfaceQNames();
}