package org.omnetpp.inifile.editor.text;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.InifileDocument;

/**
 * This class has one instance per editor. It performs 
 * background parsing of the inifile, and keeps editor data 
 * (InifileContents) up to date.
 */
public class InifileReconcileStrategy implements IReconcilingStrategy {
	private InifileEditorData editorData = null;
	//private IDocument document = null;

	public InifileReconcileStrategy(InifileEditorData editorData) {
		this.editorData = editorData;
	}

	public void setDocument(IDocument document) {
		//this.document = document;
	}

	public void reconcile(DirtyRegion dirtyRegion, IRegion subRegion) {
		// XXX this does not seem to get called, at least in our setup...
		System.out.println("reconcile(DirtyRegion,IRegion) called");
	}

	public void reconcile(IRegion partition) {
		System.out.println("reconcile(IRegion) called");
		
		// force parsing and analyzing the file now (they are both lazy and 
		// wouldn't do that otherwise until a view, a tooltip or something
		// needs data from them)
		((InifileDocument)editorData.getInifileDocument()).parse();
		editorData.getInifileAnalyzer().analyze();
	}
}
