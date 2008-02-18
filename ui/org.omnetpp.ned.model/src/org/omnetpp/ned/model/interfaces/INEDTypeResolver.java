package org.omnetpp.ned.model.interfaces;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
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
     * Returns the NED source folders for the given project. This is
     * the list of folders in the ".nedfolders" file, or the project
     * alone if ".nedfolders" does not exist or empty. An empty array
     * is returned if the project is not an OMNeT++ project (does not
     * have the omnetpp nature, or it is not enabled for some reason).
     */
	public IContainer[] getNedSourceFolders(IProject project);

    /**
	 * Returns the NED source folder for the given NED file. Returns null if the
	 * file is outside the project's NED source folders, or the project itself
	 * does not have the OMNeT++ nature, or that nature is disabled (see
	 * IProject.isNatureEnabled() on why a nature might be disabled.)
	 */
    public IContainer getNedSourceFolderFor(IFile file);

	/**
	 * Returns the expected package name for the given file. "" means the
	 * default package. Returns null for the toplevel "package.ned" file
	 * (which in fact is used to *define* the package, so it has no "expected"
	 * package name), and for files outside the NED source folders defined
	 * for the project (i.e. for which getNedSourceFolder() returns null.)
	 * Works for ANY file (not just for NED files)
	 */
    public String getExpectedPackageFor(IFile file);

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
	 * Returns all toplevel (non-inner) types from NED files in all projects,
	 * including duplicate types. This method is useful for implementing the
	 * Open NED Type dialog, and for not much else.
	 */
	public Collection<INEDTypeInfo> getNedTypesFromAllProjects();

    /**
	 * Returns all toplevel (non-inner) types in the NED files, excluding duplicate names,
	 * from the given project and its dependent projects.
	 */
	public Collection<INEDTypeInfo> getNedTypes(IProject context);

    /**
     * Returns all toplevel (non-inner) type names in the NED files, excluding
     * duplicate names, from the given project and its dependent projects.
     * Returned names are fully qualified.
     */
    public Set<String> getNedTypeQNames(IProject context);

    /**
     * Returns all toplevel (non-inner) type names in the NED files where
     * the predicate matches, excluding duplicate names, from the given project
     * and its dependent projects. Returned names are fully qualified.
     */
    public Set<String> getNedTypeQNames(IPredicate predicate, IProject context);

    /**
     * Returns ALL toplevel (non-inner) type names in the NED files, including
     * duplicate names, from the given project and its dependent projects.
     * This method can be used for generating unique type names. Returned names
     * are fully qualified.
     */
    public Set<String> getReservedQNames(IProject context);

    /**
     * Returns ALL toplevel (non-inner) type names in the given package, including
     * duplicate names, from the given project and its dependent projects.
     * This method can be used for generating unique type names. Returned names
     * are simple names (i.e. package name has been removed).
     */
    public Set<String> getReservedNames(IProject context, String packageName);
    
    /**
     * Return a NED type from its fully qualified name, from the given project
     * and its dependent projects. Inner types are NOT recognized.
     * Returns null if not found.
     */
    public INEDTypeInfo getToplevelNedType(String qualifiedName, IProject context);

	/**
     * Return a NED type from its fully qualified name, whether toplevel
     * or inner type, from the given project and its dependent projects.
     * Returns null if not found.
	 */
    public INEDTypeInfo getToplevelOrInnerNedType(String qualifiedName, IProject context);

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
	 * Returns all valid toplevel (non-inner) module names in the NED files,
	 * from the given project and its dependent projects.
	 * Returned names are fully qualified.
	 */
	public Set<String> getModuleQNames(IProject context);

    /**
	 * Returns all valid toplevel (non-inner) network names in the NED files,
	 * from the given project and its dependent projects.
	 * Returned names are fully qualified.
	 */
	public Set<String> getNetworkQNames(IProject context);

	/**
	 * Returns all valid toplevel (non-inner) channel names in the NED files,
	 * from the given project and its dependent projects.
	 * Returned names are fully qualified.
	 */
	public Set<String> getChannelQNames(IProject context);

	/**
	 * Returns all valid toplevel (non-inner) module interface names in the NED files,
	 * from the given project and its dependent projects.
	 * Returned names are fully qualified.
	 */
	public Set<String> getModuleInterfaceQNames(IProject context);

	/**
	 * Returns all valid toplevel (non-inner) channel interface names in the NED files,
	 * from the given project and its dependent projects.
	 * Returned names are fully qualified.
	 */
	public Set<String> getChannelInterfaceQNames(IProject context);
}

