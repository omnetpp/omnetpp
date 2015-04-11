/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.Collection;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.INedErrorStore;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;

/**
 * For accessing NED types.
 *
 * @author Andras
 */
public interface INedTypeResolver {
    public static final String PACKAGE_NED_FILENAME = "package.ned";
    public static final String NED_EXTENSION = "ned";
    public static final String NAMESPACE_PROPERTY = "namespace";
    public static final String LICENSE_PROPERTY = "license";

    public static final String NEDSYNTAXPROBLEM_MARKERID = "org.omnetpp.ned.core.nedsyntaxproblem";
    public static final String NEDCONSISTENCYPROBLEM_MARKERID = "org.omnetpp.ned.core.nedconsistencyproblem";

    /**
     * Interface used by filtering methods.
     */
    public interface IPredicate {
        public boolean matches(INedTypeInfo typeInfo);
    }

    // some useful predicates
    public static final IPredicate ALL_FILTER = new IPredicate() {
        public boolean matches(INedTypeInfo component) {return true;}
    };
    public static class InstanceofPredicate implements IPredicate {
        private Class<? extends INedTypeElement> clazz;
        public InstanceofPredicate(Class<? extends INedTypeElement> clazz) {
            this.clazz = clazz;
        }
        public boolean matches(INedTypeInfo component) {
            return clazz.isInstance(component.getNedElement());
        }
    };
    public static final IPredicate MODULE_FILTER = new InstanceofPredicate(IModuleTypeElement.class);
    public static final IPredicate SIMPLE_MODULE_FILTER = new InstanceofPredicate(SimpleModuleElementEx.class);
    public static final IPredicate COMPOUND_MODULE_FILTER = new InstanceofPredicate(CompoundModuleElementEx.class);
    public static final IPredicate MODULEINTERFACE_FILTER = new InstanceofPredicate(ModuleInterfaceElementEx.class);
    public static final IPredicate CHANNEL_FILTER = new InstanceofPredicate(ChannelElementEx.class);
    public static final IPredicate CHANNELINTERFACE_FILTER = new InstanceofPredicate(ChannelInterfaceElementEx.class);
    public static final IPredicate NETWORK_FILTER = new IPredicate() {
        public boolean matches(INedTypeInfo component) {
            return component.getNedElement() instanceof IModuleTypeElement &&
                   ((IModuleTypeElement)component.getNedElement()).isNetwork();
        }
    };
    public static final IPredicate MODULE_EXCEPT_NETWORK_FILTER = new IPredicate() {
        public boolean matches(INedTypeInfo component) {
            return component.getNedElement() instanceof IModuleTypeElement &&
                   !((IModuleTypeElement)component.getNedElement()).isNetwork();
        }
    };

    /**
     * Returns the current value of a counter that gets incremented by every
     * NED change. Checking against this counter allows one to invalidate
     * cached NED data whenever they potentially become stale.
     */
    public long getLastChangeSerial();

    /**
     * INTERNAL Factory method, to be called from INedTypeElement constructors.
     */
    public INedTypeInfo createTypeInfoFor(INedTypeElement node);

    /**
     * Returns NED files in the workspace.
     */
    public Set<IFile> getNedFiles();

    /**
     * Returns the set of NED files in the given project.
     */
    public Set<IFile> getNedFiles(IProject project);

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
     * Determines if the given file is a NED file that has been loaded.
     */
    public boolean containsNedFileElement(IFile file);

    /**
     * Determines if a resource is a NED file. It checks the file extension
     * (".ned"), whether the file is in one of the NED source folders
     * designated for the project, and whether it is in an excluded folder.
     */
    public boolean isNedFile(IResource resource);

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
     *
     * Equivalent to getNedSourceFolderFor(file.getParent()).
     */
    public IContainer getNedSourceFolderFor(IFile file);

    /**
     * Returns the NED source folder for the given folder. Returns null if the
     * folder is outside the project's NED source folders, or the project itself
     * does not have the OMNeT++ nature, or that nature is disabled (see
     * IProject.isNatureEnabled() on why a nature might be disabled.)
     */
    public IContainer getNedSourceFolderFor(IContainer folder);

    /**
     * Returns the package name for the given folder ("" for the default package),
     * or null if the folder is outside all NED source folders. Ignores package
     * exclusions.
     */
    public String getPackageFor(IContainer folder);

    /**
     * Returns the expected package name for the given file. "" means the
     * default package. Returns null for the toplevel "package.ned" file
     * (which in fact is used to *define* the package, so it has no "expected"
     * package name), and for files outside the NED source folders defined
     * for the project (i.e. for which getNedSourceFolder() returns null.)
     * Works for any file, not just for NED files.
     */
    public String getExpectedPackageFor(IFile file);

    /**
     * Returns the names of the packages that are roots of disabled package trees.
     * That is, the returned packages and all packages under them are disabled.
     * NED files are not loaded from disabled packages.
     */
    public String[] getExcludedPackageRoots(IProject project);

    /**
     * Returns whether the given package is disabled in the given project.
     * A package is disabled when it is one of the disabled package roots
     * (see getExcludedPackageRoots()) or it is under one of the them.
     */
    public boolean isPackageEnabled(IProject project, String packageName);

    /**
     * Returns the folders that map to the given package in the given project.
     * Folders that do not exist (yet) are also returned. The number of elements
     * returned is less or equal to the number of NED source folders in the project
     * (i.e. getNedSourceFolders(project).length).
     */
    public IContainer[] getFoldersForPackage(IProject project, String packageName);

    /**
     * Returns the markers for the given element or and the given element and
     * its subtree, depending on the 'recursive' argument
     *
     * IMPORTANT: Do NOT use this method to check whether an element actually
     * contains errors! Markers are written out in a background job, and there's
     * no guarantee that IFile already contains them. Use getMaxProblemSeverity()
     * and the likes from INedElement instead. This method will return the maximum
     * of "limit" markers (to constraint the CPU usage for elements containing a lot of errors)
     */
    public IMarker[] getMarkersForElement(INedElement node, boolean recursive, int limit);

    /**
     * Returns a INedElement at the given file/line/column. The NedFileElement
     * is returned if no specific element was found. Returns null if no
     * NED element was found at that position (that may be caused by
     * missing source region info or when the line/column points outside
     * the file's range.)
     */
    public INedElement getNedElementAt(IFile file, int line, int column);

    /**
     * Returns a INedElement within the given parent at the given line/column.
     * (or the parent if no specific INedElement was found in the parent's range)
     * Returns null if no NED element was found at that position (that may be
     * caused by missing source region info.)
     */
    public INedElement getNedElementAt(INedElement parent, int line, int column);

    /**
     * Returns all toplevel (non-inner) types from NED files in all projects,
     * including duplicate types. This method is useful for implementing an
     * Open NED Type dialog, and for not much else.
     */
    public Collection<INedTypeInfo> getToplevelNedTypesFromAllProjects();

    /**
     * Like getToplevelNedTypesFromAllProjects(), but the result is filtered with the given predicate.
     */
    public Collection<INedTypeInfo> getToplevelNedTypesFromAllProjects(IPredicate predicate);

    /**
     * Returns the fully qualified names of all toplevel (non-inner) types
     * from NED files in all projects, including duplicate types. This method
     * is useful for implementing an Open NED Type dialog, and for not much else.
     */
    public Set<String> getToplevelNedTypeQNamesFromAllProjects();

    /**
     * Returns all toplevel (non-inner) NED types that have the given fully qualified name,
     * from all projects.
     */
    public Set<INedTypeInfo> getToplevelNedTypesFromAllProjects(String qualifiedName);

    /**
     * Returns all toplevel (non-inner) types in the NED files, excluding duplicate names,
     * from the given project and its dependent projects.
     */
    public Collection<INedTypeInfo> getToplevelNedTypes(IProject context);

    /**
     * Returns a filtered list of all toplevel (non-inner) types in the NED files, excluding
     * duplicate names, from the given project and its dependent projects.
     */
    public Collection<INedTypeInfo> getToplevelNedTypes(IPredicate predicate, IProject context);

    /**
     * Returns all toplevel (non-inner) types in the NED files that implement
     * the given interface, excluding duplicate names, from the given project
     * and its dependent projects.
     */
    public Collection<INedTypeInfo> getToplevelNedTypesThatImplement(INedTypeInfo interfaceType, IProject context);

    /**
     * Returns all toplevel (non-inner) types with the given simple name (unqualified name)
     * from the given project and its dependent projects.
     */
    public Collection<INedTypeInfo> getToplevelNedTypesBySimpleName(String simpleName, IProject context);

    /**
     * Returns all toplevel (non-inner) type names in the NED files, excluding
     * duplicate names, from the given project and its dependent projects.
     * Returned names are fully qualified.
     */
    public Set<String> getToplevelNedTypeQNames(IProject context);

    /**
     * Returns all toplevel (non-inner) type names in the NED files where
     * the predicate matches, excluding duplicate names, from the given project
     * and its dependent projects. Returned names are fully qualified.
     */
    public Set<String> getToplevelNedTypeQNames(IPredicate predicate, IProject context);

    /**
     * Returns all toplevel (non-inner) type names in the NED files, including
     * duplicate names, from the given project and its dependent projects.
     * This method can be used for generating unique type names. Returned names
     * are fully qualified.
     */
    public Set<String> getReservedQNames(IProject context);

    /**
     * Returns all toplevel (non-inner) type names in the given package, including
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
    public INedTypeInfo getToplevelNedType(String qualifiedName, IProject context);

    /**
     * Return a NED type from its fully qualified name, whether toplevel
     * or inner type, from the given project and its dependent projects.
     * Returns null if not found.
     *
     * If you are not interested in inner types, use getToplevelNedType().
     */
    public INedTypeInfo getToplevelOrInnerNedType(String qualifiedName, IProject context);

    /**
     * Looks up the name in the given context, and returns the NED type info,
     * or null if it does not exist. Considers imports, etc; inner types are
     * also handled.
     * @param name  May be a simple name or a qualified name; cannot be null
     * @param context May not be null.
     */
    public INedTypeInfo lookupNedType(String name, INedTypeLookupContext context);

    /**
     * Determines the actual type for a "like" submodule or channel, or null if
     * no such type was found. If name is a simple name (i.e. unqualified),
     * there must be exactly one NED type with that name that implements the
     * given interface. If the name is a fully qualified name, the corresponding
     * type will be returned even if it does not implement interfaceType. (If we
     * returned null, that would likely be interpreted by clients as
     * "no such type", which would be very misleading.) The caller should check
     * that the returned type actually implements interfaceType.
     */
    public INedTypeInfo lookupLikeType(String name, INedTypeInfo interfaceType, IProject context);

    /**
     * Return all NED type names matching the predicate, and defined in the given
     * context. For NedFileElement context this means all the top level type elements defined in
     * the given file, for CompoundModule context it returns all inner types (including
     * the inherited ones if any). Returned names are qualified
     */
    public Set<String> getLocalTypeNames(INedTypeLookupContext context, IPredicate predicate);

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

    /**
     * Convenience method. Calls getPropertyFor(nedFileElement,propertyName),
     * and if a property was found, returns its simple value (getSimpleValue());
     * otherwise returns null.
     */
    public String getSimplePropertyFor(NedFileElementEx nedFileElement, String propertyName);

    /**
     * Convenience method. Calls getPropertyFor(folder,propertyName),
     * and if a property was found, returns its simple value (getSimpleValue());
     * otherwise returns null.
     */
    public String getSimplePropertyFor(IContainer folder, String propertyName);

    /**
     * Searches for a file property with the given name in the given NED file, then
     * in the package.ned file in the same folder, and in package.ned files
     * of parent folders up to the root (i.e. the containing NED source folder).
     * Returns null if not found.
     */
    public PropertyElementEx getPropertyFor(NedFileElementEx nedFileElement, String propertyName);

    /**
     * Searches for a file property with the given name in the package.ned file
     * in the given folder, and in package.ned files of parent folders up to the root
     * (i.e. the containing NED source folder). Returns null if not found.
     */
    public PropertyElementEx getPropertyFor(IContainer folder, String propertyName);

    /**
     * Returns all built-in types' type info objects.
     */
    public Collection<INedTypeInfo> getBuiltInDeclarations();

    /**
     * Returns true if the given ned type info is a descriptor for a built-in type.
     */
    public boolean isBuiltInDeclaration(INedTypeInfo typeInfo);

    /**
     * Generate errors for duplicate types; exposed for NedValidationJob.
     */
    public void addErrorsForDuplicates(INedErrorStore errorStore);
}
