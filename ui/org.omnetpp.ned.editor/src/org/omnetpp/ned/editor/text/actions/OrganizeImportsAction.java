package org.omnetpp.ned.editor.text.actions;

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
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;

/**
 * Organize "import" lines in a NED file.
 *
 * @author andras
 */
public class OrganizeImportsAction extends NedTextEditorAction {
    public static final String ID = "OrganizeImports";

    public OrganizeImportsAction(ITextEditor editor) {
        super(ID, editor);
    }

    @Override
	public void update() {
    	setEnabled(getTextEditor() != null && !getNedFileElement().hasSyntaxError());
	}

	@Override
    public void run() {
        NedFileElementEx nedFileElement = getNedFileElement();
        if (nedFileElement.hasSyntaxError())
        	return; // don't mess with the source while it has syntax error

        // fix the package as a bonus
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        String expectedPackage = res.getExpectedPackageFor(getFile());
		if (!StringUtils.equals(nedFileElement.getPackage(), expectedPackage))
			nedFileElement.setPackage(expectedPackage);  //XXX ask permission?
        
        // collect names to import
        Set<String> unqualifiedTypeNames = collectUnqualifiedTypeNames(nedFileElement);

        // resolve all imports
        List<String> oldImports = nedFileElement.getImports();
        List<String> imports = new ArrayList<String>();
        for (String typeName : unqualifiedTypeNames)
        	resolveImport(typeName, nedFileElement.getQNameAsPrefix(), oldImports, imports);

        // update model
        nedFileElement.removeImports();
        Collections.sort(imports);
        for (String importSpec : imports)
        	nedFileElement.addImport(importSpec);
        if (!imports.isEmpty())
        	nedFileElement.getLastImportChild().appendChild(NEDElementUtilEx.createCommentElement("right", "\n\n\n"));

        // update text editor
        ((TextualNedEditor)getTextEditor()).pullChangesFromNEDResources();
    }

	protected NedFileElementEx getNedFileElement() {
        return NEDResourcesPlugin.getNEDResources().getNedFileElement(getFile());
	}

	protected IFile getFile() {
		return ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
	}

    /**
     * Collect those names from the NED file that need imports.
     */
	protected Set<String> collectUnqualifiedTypeNames(NedFileElementEx nedFileElement) {
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
	protected void resolveImport(String unqualifiedTypeName, String packagePrefix, List<String> oldImports, List<String> imports) {
		NEDResources res = NEDResourcesPlugin.getNEDResources();
		IProject contextProject = res.getNedFile(getNedFileElement()).getProject();
		
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
		ListDialog dialog = new ListDialog(null);
		dialog.setInput(potentialMatches);
		dialog.setContentProvider(new ArrayContentProvider());
		dialog.setLabelProvider(new LabelProvider());
		dialog.setTitle("Select Import");
		dialog.setMessage("Choose type to import:");
		if (dialog.open() == Window.OK) {
			String selectedType = (String) dialog.getResult()[0];
			imports.add(selectedType);
		}
	}
}
