package org.omnetpp.ned.core.refactoring;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.pojo.ChannelSpecElement;
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

        NEDResources res = NEDResourcesPlugin.getNEDResources();
        IFile file = res.getNedFile(nedFileElement);

        String expectedPackage = res.getExpectedPackageFor(file);

        if (expectedPackage != null && !StringUtils.equals(nedFileElement.getPackage(), expectedPackage))
            nedFileElement.setPackage(expectedPackage);
    }

    /**
     * Organizes imports in the file. If there's an ambiguity, a selection
     * dialog is presented to the user. 
     */
    //XXX factor out UI part (dialog), and pass it in as lambda?
    public static void organizeImports(NedFileElementEx nedFileElement) {
        if (nedFileElement.hasSyntaxError())
            return;

        NEDResources res = NEDResourcesPlugin.getNEDResources();
        IFile file = res.getNedFile(nedFileElement);
        IProject contextProject = file.getProject();

        // collect names to import
        Set<String> unqualifiedTypeNames = collectUnqualifiedTypeNames(nedFileElement);

        // resolve all imports
        List<String> oldImports = nedFileElement.getImports();
        List<String> imports = new ArrayList<String>();
        for (String typeName : unqualifiedTypeNames)
        	resolveImport(contextProject, typeName, nedFileElement.getQNameAsPrefix(), oldImports, imports);

        // update model if imports have changed
        Collections.sort(imports);
        Collections.sort(oldImports);
        if (!imports.equals(oldImports)) {
            nedFileElement.removeImports();
            for (String importSpec : imports)
                nedFileElement.addImport(importSpec);
            if (!imports.isEmpty())
                nedFileElement.getLastImportChild().appendChild(NEDElementUtilEx.createCommentElement("right", "\n\n\n"));
        }
    }

    /**
     * Collect those names from the NED file that need imports.
     */
	protected static Set<String> collectUnqualifiedTypeNames(NedFileElementEx nedFileElement) {
		final Set<String> result = new HashSet<String>();
		NEDElementUtilEx.visitNedTree(nedFileElement, new NEDElementUtilEx.INEDElementVisitor() {
			public void visit(INEDElement element) {
				if (element instanceof IHasType)
					collect(result, ((IHasType)element).getEffectiveType());
				else if (element instanceof ExtendsElement)
					collect(result, ((ExtendsElement)element).getName());
				else if (element instanceof InterfaceNameElement)
					collect(result, ((InterfaceNameElement)element).getName());
			}

			private void collect(Set<String> result, String typeName) {
				if (typeName != null && !typeName.contains("."))
					result.add(typeName);
			}
		});
		return result;
	}

	/**
	 * Find the fully qualified type for the given simple name, and add it to the imports list.
	 */
	protected static void resolveImport(IProject contextProject, String unqualifiedTypeName, String packagePrefix, List<String> oldImports, List<String> imports) {
		NEDResources res = NEDResourcesPlugin.getNEDResources();
		
		// name is in the same package as this file, no need to add an import
		if (res.getToplevelNedType(packagePrefix + unqualifiedTypeName, contextProject) != null)
			return;

		// find all potential types
        List<String> potentialMatches = new ArrayList<String>();
		for (String qualifiedName : res.getNedTypeQNames(contextProject))
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
					if (potentialMatch.matches(NEDElementUtilEx.importToRegex(oldImport))) {
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
    public static void cleanupTree(INEDElement element) {
        // filter the child nodes first
        for (INEDElement child : element)
            cleanupTree(child);

        // see if the current node can be filtered out

        // skip a channel spec if it does not contain any meaningful information
        if (element instanceof ChannelSpecElement) {
            ChannelSpecElement channelSpec = (ChannelSpecElement) element;
            if ((channelSpec.getType() == null || "".equals(channelSpec.getType()))
                && (channelSpec.getLikeType() == null || "".equals(channelSpec.getLikeType()))
                && !channelSpec.hasChildren()) {

                // remove it from the parent if it does not matter
                element.removeFromParent();
            }
        }
        
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

}
