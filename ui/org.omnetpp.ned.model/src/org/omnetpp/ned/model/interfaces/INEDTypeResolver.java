package org.omnetpp.ned.model.interfaces;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
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
     * Returns the file this NED element is in.
     */
	public IFile getNedFile(NedFileElementEx nedFileElement);

 	/**
	 * Returns the NED source folder for the given NED file. Returns null if the 
	 * file is outside the project's NED source folders, or the project itself
	 * does not have the OMNeT++ nature, or that nature is disabled (see 
	 * IProject.isNatureEnabled() on why a nature might be disabled.)
	 */
    public IContainer getNedSourceFolderFor(IFile file);
    
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
	 * Returns all toplevel (non-inner) types in the NED files, excluding 
	 * duplicate names.
	 */
	public Collection<INEDTypeInfo> getAllNedTypes();

    /**
     * Returns all toplevel (non-inner) type names in the NED files, excluding 
     * duplicate names. Returned names are fully qualified.
     */
    public Set<String> getAllNedTypeQNames();

    /**
     * Returns all toplevel (non-inner) type names in the NED files where 
     * the predicate matches, excluding duplicate names.
	 * Returned names are fully qualified.
     */
    public Set<String> getNedTypeQNames(IPredicate predicate);

    /**
     * Returns ALL toplevel (non-inner) type names in the NED files, 
     * including duplicate names. This method can be used for generating
     * unique type names. Returned names are fully qualified.
     */
    public Set<String> getReservedQNames();

    /**
     * Return a NED type from its fully qualified name. Inner types are
     * NOT recognized. Returns null if not found.
     */
    public INEDTypeInfo getToplevelNedType(String qualifiedName);

	/**
     * Return a NED type from its fully qualified name, whether toplevel
     * or inner type. Returns null if not found.
	 */
    public INEDTypeInfo getToplevelOrInnerNedType(String qualifiedName);

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
	 * fully qualifying them, including inner types. 
	 * Returned names are short names (NOT qualified).
	 */
	public Set<String> getVisibleTypeNames(INedTypeLookupContext context);

	/**
	 * Return all NED type names matching the predicate, and visible in the given
	 * context without fully qualifying them, including inner types. 
	 * Returned names are short names (NOT qualified).
	 */
	public Set<String> getVisibleTypeNames(INedTypeLookupContext context, IPredicate predicate);

    /**
	 * Returns all valid toplevel (non-inner) module names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getModuleQNames();

    /**
	 * Returns all valid toplevel (non-inner) network names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getNetworkQNames();

	/**
	 * Returns all valid toplevel (non-inner) channel names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getChannelQNames();

	/**
	 * Returns all valid toplevel (non-inner) module interface names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getModuleInterfaceQNames();

	/**
	 * Returns all valid toplevel (non-inner) channel interface names in the NED files.
	 * Returned names are fully qualified.
	 */
	public Set<String> getChannelInterfaceQNames();
}