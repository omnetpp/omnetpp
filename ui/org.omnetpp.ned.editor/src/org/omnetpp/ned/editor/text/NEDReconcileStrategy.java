package org.omnetpp.ned.editor.text;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;

/**
 * This class has one instance per NED editor, and performs 
 * background NED parsing.
 *
 * @author rhornig
 */
public class NEDReconcileStrategy implements IReconcilingStrategy {
	private TextualNedEditor editor;
	
	public NEDReconcileStrategy(TextualNedEditor editor) {
		this.editor = editor;
	}
	
	public void setDocument(IDocument document) {
	}

	public void reconcile(DirtyRegion dirtyRegion, IRegion subRegion) {
		// this method is not called, because reconciler is configured 
		// to be a non-incremental reconciler in NedSourceViewerConfiguration

		// System.out.println("reconcile(DirtyRegion,IRegion) called");
		reconcile(subRegion);
	}

	public void reconcile(IRegion partition) {
		// System.out.println("reconcile(IRegion) called");
		editor.pushChangesIntoNEDResources(false);
	}
}
