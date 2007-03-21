package org.omnetpp.inifile.editor.text;

import java.io.IOException;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.InifileContents;
import org.omnetpp.inifile.editor.model.ParseException;

/**
 * This class has one instance per editor. It performs 
 * background parsing of the inifile, and keeps editor data 
 * (InifileContents) up to date.
 */
public class NEDReconcileStrategy implements IReconcilingStrategy {

	private InifileEditorData editorData = null;
	private IDocument document = null;

	public NEDReconcileStrategy(InifileEditorData editorData) {
		this.editorData = editorData;
	}

	public void setDocument(IDocument document) {
		this.document = document;
	}

	public void reconcile(DirtyRegion dirtyRegion, IRegion subRegion) {
		// XXX this does not seem to get called, at least in our setup...
		System.out.println("reconcile(DirtyRegion,IRegion) called");
	}

	public void reconcile(IRegion partition) {
		System.out.println("reconcile(IRegion) called");
		String text = document.get();

		try {
			InifileContents ini = editorData.getInifileContents();
			ini.parse(text);
			ini.print(System.out); //XXX debug
		} 
		catch (IOException e) {
			// cannot happen with string input
		} 
		catch (ParseException e) {
			e.printStackTrace(); //XXX
		}
	}
}
