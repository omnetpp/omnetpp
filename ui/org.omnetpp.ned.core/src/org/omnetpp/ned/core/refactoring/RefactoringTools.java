/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core.refactoring;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;

import org.apache.commons.collections.CollectionUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Implementation of various NED refactoring operations.
 *
 * @author andras
 */
public class RefactoringTools {

    /**
     * Replaces package declaration with the expected package.
     */
    public static void fixupPackageDeclaration(NedFileElementEx nedFileElement) {
        if (nedFileElement.hasSyntaxError())
            return;

        INedTypeResolver resolver = nedFileElement.getResolver();
        IFile file = resolver.getNedFile(nedFileElement);

        String expectedPackage = resolver.getExpectedPackageFor(file);

        if (expectedPackage != null && !StringUtils.equals(nedFileElement.getPackage(), expectedPackage))
            nedFileElement.setPackage(expectedPackage);
    }

    /**
     * Organizes imports in the file. If there's an ambiguity, a selection
     * dialog is presented to the user.
     */
    //XXX factor out UI part (dialog), and pass it in as lambda?
    public static void organizeImports(final NedFileElementEx nedFileElement) {
        if (nedFileElement.hasSyntaxError())
            return;

        final INedTypeResolver resolver = nedFileElement.getResolver();
        IFile file = resolver.getNedFile(nedFileElement);
        final IProject contextProject = file.getProject();

        // resolve all imports
        final List<String> oldImports = nedFileElement.getImports();
        final HashSet<String> imports = new HashSet<String>();
        NedElementUtilEx.visitNedTree(nedFileElement, new NedElementUtilEx.INedElementVisitor() {
            public void visit(INedElement element) {
                if (element instanceof ISubmoduleOrConnection)
                    collect(element.getEnclosingLookupContext(), NedElementUtilEx.getTypeOrLikeType((ISubmoduleOrConnection)element));
                else if (element instanceof ExtendsElement)
                    collect(element.getEnclosingLookupContext(), ((ExtendsElement)element).getName());
                else if (element instanceof InterfaceNameElement)
                    collect(element.getEnclosingLookupContext(), ((InterfaceNameElement)element).getName());
            }

            private void collect(INedTypeLookupContext lookupContext, String typeName) {
                if (typeName != null && !typeName.contains("."))
                    resolveImport(lookupContext, contextProject, typeName, nedFileElement.getQNameAsPrefix(), oldImports, imports);
            }
        });

        // update model if imports have changed
        List<String> newImports = org.omnetpp.common.util.CollectionUtils.toSorted(imports);
        Collections.sort(oldImports);
        if (!newImports.equals(oldImports)) {
            nedFileElement.removeImports();
            for (String importSpec : newImports)
                nedFileElement.addImport(importSpec);
        }
    }

    /**
     * Find the fully qualified type for the given simple name, and add it to the imports list.
     */
    protected static void resolveImport(INedTypeLookupContext lookupContext, IProject contextProject, final String unqualifiedTypeName, String packagePrefix, List<String> oldImports, HashSet<String> imports) {
        INedTypeResolver resolver = lookupContext.getResolver();
        // name is in the same package as this file, no need to add an import
        if (resolver.getToplevelNedType(packagePrefix + unqualifiedTypeName, contextProject) != null)
            return;

        List<String> potentialMatches = new ArrayList<String>();
        // find local types
        potentialMatches.addAll(resolver.getLocalTypeNames(lookupContext, new INedTypeResolver.IPredicate() {
            public boolean matches(INedTypeInfo typeInfo) {
                return typeInfo.getFullyQualifiedName().endsWith("." + unqualifiedTypeName);
            }
        }));
        // local types silently win over toplevel types, and we don't need to import any of them
        if (potentialMatches.size() == 1)
            return;
        else
            potentialMatches.clear();

        // find all potential types
        for (String qualifiedName : resolver.getToplevelNedTypeQNames(contextProject))
            if (qualifiedName.endsWith("." + unqualifiedTypeName) || qualifiedName.equals(unqualifiedTypeName))
                potentialMatches.add(qualifiedName);

        // if there's zero or one match, we're done
        if (potentialMatches.size() == 0)
            return; // not found, sorry
        if (potentialMatches.size() == 1) {
            imports.add(potentialMatches.get(0));
            return;
        }

        // if there's an import for one of them already, use that
        for (String potentialMatch : potentialMatches) {
            if (oldImports.contains(potentialMatch)) {
                imports.add(potentialMatch);
                return;
            }
            // oldImports may contain wildcards, so try with regex as well
            for (String oldImport : oldImports)
                if (oldImport.contains("*"))
                    if (potentialMatch.matches(NedElementUtilEx.importToRegex(oldImport))) {
                        imports.add(potentialMatch);
                        return;
                    }
        }

        // ambiguous import: let the user choose
        String selectedType = chooseImport(potentialMatches);
        if (selectedType != null)
            imports.add(selectedType);
    }

    /**
     * Dialog to prompt a user to choose one from the listed imports.
     */
    //XXX move it out of this class
    protected static String chooseImport(List<String> importsList) {
        IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        Shell shell = activeWorkbenchWindow == null ? null :activeWorkbenchWindow.getShell();
        ListDialog dialog = new ListDialog(shell);
        dialog.setInput(importsList);
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setLabelProvider(new LabelProvider());
        dialog.setTitle("Select Import");
        dialog.setMessage("Choose type to import:");
        if (dialog.open() == Window.OK)
            return (String) dialog.getResult()[0];
        return null;
    }

    /**
     * This method removes unnecessary elements from the NED tree, such as empty
     * channel spec, empty "parameters", "gates", "submodule" etc elements.
     */
    public static void cleanupTree(INedElement element) {
        // filter the child nodes first
        for (INedElement child : element)
            cleanupTree(child);

        // check for empty types, parameters, gates, submodules, connections node
        if ((element instanceof TypesElement
                || element instanceof ParametersElement
                || element instanceof GatesElement
                || element instanceof SubmodulesElement
                || element instanceof ConnectionsElement && !((ConnectionsElement)element).getAllowUnconnected())
                                && !element.hasChildren()) {
            element.removeFromParent();
        }
    }

    public static Collection<AddGateLabels> inferAllGateLabels(INedTypeElement element, boolean forward) {
        Collection<AddGateLabels> result = new HashSet<AddGateLabels>();

        // TODO: why do we need to copy it? some notification seems to cause a recalculation of gate declarations during inference
        for (GateElementEx gate : new ArrayList<GateElementEx>(element.getNedTypeInfo().getGateDeclarations().values()))
            inferGateLabels(gate, forward, result);

        return result;
    }

    public static void inferGateLabels(GateElementEx gate, boolean forward, Collection<AddGateLabels> result) {
        INedTypeElement typeElement = gate.getEnclosingTypeElement();

        for (INedTypeInfo typeInfo : NedResourcesPlugin.getNedResources().getToplevelNedTypesFromAllProjects()) {
            INedTypeElement element = typeInfo.getNedElement();

            if (element instanceof CompoundModuleElementEx) {
                CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)element;

                for (SubmoduleElementEx submoduleElement : compoundModule.getSubmodules()) {
                    if (submoduleElement.getEffectiveTypeRef() == typeElement) {
                        inferLabelsOnConnections(compoundModule.getSrcConnectionsFor(submoduleElement.getName()), gate, forward, result);
                        inferLabelsOnConnections(compoundModule.getDestConnectionsFor(submoduleElement.getName()), gate, forward, result);
                    }
                }

                inferLabelsOnConnections(compoundModule.getSrcConnections(), gate, forward, result);
                inferLabelsOnConnections(compoundModule.getDestConnections(), gate, forward, result);
            }
        }
    }

    private static void inferLabelsOnConnections(List<ConnectionElementEx> connections, GateElementEx gate1, boolean forward, Collection<AddGateLabels> result) {
        INedTypeInfo typeInfo = gate1.getEnclosingTypeElement().getNedTypeInfo();
        for (ConnectionElementEx connection : connections) {
            GateElementEx gate2 = null;
            INedTypeInfo srcTypeInfo = connection.getSrcModuleRef().getNedTypeInfo();
            INedTypeInfo destTypeInfo = connection.getDestModuleRef().getNedTypeInfo();

            if (connection.getDestGate().equals(gate1.getName()) && destTypeInfo == typeInfo)
                gate2 = srcTypeInfo.getGateDeclarations().get(connection.getSrcGate());
            else if (connection.getSrcGate().equals(gate1.getName()) && srcTypeInfo == typeInfo)
                gate2 = destTypeInfo.getGateDeclarations().get(connection.getDestGate());

            if (gate2 != null) {
                if (forward)
                    inferGateLabels(gate1, gate2, result);
                else
                    inferGateLabels(gate2, gate1, result);
            }
        }
    }

    @SuppressWarnings("unchecked")
    private static void inferGateLabels(GateElementEx fromGate, GateElementEx toGate, Collection<AddGateLabels> result) {
        ArrayList<String> fromLabels = NedElementUtilEx.getLabels(fromGate);
        ArrayList<String> toLabels = NedElementUtilEx.getLabels(toGate);
        Collection<String> addedLabels = CollectionUtils.subtract(fromLabels, toLabels);

        if (!addedLabels.isEmpty())
            result.add(new AddGateLabels(toGate, addedLabels));
    }

    public static class AddGateLabels implements Runnable {
        public GateElementEx gate;
        public Collection<String> labels;

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((gate == null) ? 0 : gate.hashCode());
            result = prime * result + ((labels == null) ? 0 : labels.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            AddGateLabels other = (AddGateLabels) obj;
            if (gate == null) {
                if (other.gate != null)
                    return false;
            }
            else if (!gate.equals(other.gate))
                return false;
            if (labels == null) {
                if (other.labels != null)
                    return false;
            }
            else if (!labels.equals(other.labels))
                return false;
            return true;
        }

        public AddGateLabels(GateElementEx gate, Collection<String> labels) {
            this.gate = gate;
            this.labels = labels;
        }

        public void run() {
            //Debug.println("*** Adding labels: " + labels + " to gate: " + gate.getEnclosingTypeElement().getName() + "." + gate.getName());
            NedElementUtilEx.addLabels(gate, labels);
        }
    }
}
