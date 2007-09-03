package org.omnetpp.ned.editor.text.actions;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.ResourceBundle;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.TextEditorAction;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IHasType;

/**
 * Organize "import" lines in a NED file.
 *
 * @author andras
 */
public class OrganizeImportsAction extends TextEditorAction {
    public OrganizeImportsAction(ResourceBundle resourceBundle, String prefix, TextualNedEditor editor) {
        super(resourceBundle, prefix, editor);
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
        
        // update text editor
        ((TextualNedEditor)getTextEditor()).setText(nedFileElement.getNEDSource());
    }

	protected NedFileElementEx getNedFileElement() {
		IFile file = ((FileEditorInput)getTextEditor().getEditorInput()).getFile();
        return NEDResourcesPlugin.getNEDResources().getNedFileElement(file);
	}

    /**
     * Collect those names from the NED file that need imports.
     */
	protected Set<String> collectUnqualifiedTypeNames(NedFileElementEx nedFileElement) {
		final Set<String> result = new HashSet<String>(); 
		NEDElementUtilEx.visitNedTee(nedFileElement, new NEDElementUtilEx.INEDElementVisitor() {
			public void visit(INEDElement element) {
				if (element instanceof IHasType) {
					String effectiveType = ((IHasType)element).getEffectiveType();
					if (!effectiveType.contains("."))
						result.add(effectiveType);
				}
			}
		});
		return result;
	}

	/**
	 * Find the fully qualified type for the given simple name, and add it to the imports list.
	 */
	protected void resolveImport(String unqualifiedTypeName, String packagePrefix, List<String> oldImports, List<String> imports) {
		// find all potential types
        List<String> potentialMatches = new ArrayList<String>();
		for (String qualifiedName : NEDResourcesPlugin.getNEDResources().getAllNedTypeQNames())
			if (qualifiedName.endsWith("." + unqualifiedTypeName) || qualifiedName.equals(unqualifiedTypeName))
				potentialMatches.add(qualifiedName);

		// if one of the names is in the same package as this file, no need to add an import
        // FIXME dont use potential matches
		for (String potentialMatch : potentialMatches)
			if (potentialMatch.equals(packagePrefix + unqualifiedTypeName))
				return;

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
