package org.omnetpp.ned.editor.text;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.omnetpp.common.editor.text.FoldingRegionSynchronizer;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * This class has one instance per NED text editor. It performs background NED parsing, 
 * and refreshes folding regions in the text editor. 
 *
 * @author andras
 */
public class NEDReconcileStrategy implements IReconcilingStrategy {
	private TextualNedEditor editor;
	private FoldingRegionSynchronizer synchronizer;

	public NEDReconcileStrategy(TextualNedEditor editor) {
		this.editor = editor;
		this.synchronizer = new FoldingRegionSynchronizer(editor);
	}
	
	public void setDocument(IDocument document) {
	}

	public void reconcile(DirtyRegion dirtyRegion, IRegion subRegion) {
		// this method is not called, because reconciler is configured 
		// to be a non-incremental reconciler in NedSourceViewerConfiguration
		throw new IllegalStateException();
	}

	public void reconcile(IRegion partition) {
		editor.pushChangesIntoNEDResources(false);
		updateFoldingRegions();
	}

	private void updateFoldingRegions() {
		try {
			// collect positions
			IDocument doc = editor.getDocument();
			NedFileElementEx nedFileElement = NEDResourcesPlugin.getNEDResources().getNedFileElement(editor.getFile());
			Map<String,Position> newAnnotationPositions = new HashMap<String,Position>();
			for (INEDElement element : nedFileElement) {
				if (element instanceof INedTypeElement) {
					// NED types and their sections
					String key = ((INedTypeElement)element).getNEDTypeInfo().getFullyQualifiedName();
					addPosition(doc, element, key, newAnnotationPositions);
					addPosition(doc, element, NEDElementTags.NED_TYPES, key+"#t", newAnnotationPositions);
					addPosition(doc, element, NEDElementTags.NED_PARAMETERS, key+"#p", newAnnotationPositions);
					addPosition(doc, element, NEDElementTags.NED_GATES, key+"#g", newAnnotationPositions);
					addPosition(doc, element, NEDElementTags.NED_SUBMODULES, key+"#s", newAnnotationPositions);
					addPosition(doc, element, NEDElementTags.NED_CONNECTIONS, key+"#c", newAnnotationPositions);
				}
			}
		
			// synchronize with the text editor
			synchronizer.updateFoldingRegions(newAnnotationPositions);

		} catch (BadLocationException e) {
		}
	}

	private static void addPosition(IDocument doc, INEDElement parent, int tagCode, String key, Map<String,Position> posList) throws BadLocationException {
		addPosition(doc, parent.getFirstChildWithTag(tagCode), key, posList);
	}

	private static void addPosition(IDocument doc, INEDElement element, String key, Map<String,Position> posList) throws BadLocationException {
		if (element != null && element.getSourceRegion() != null) {
			int startLine = element.getSourceRegion().getStartLine();
			int endLine = element.getSourceRegion().getEndLine();
			if (startLine != endLine) {
				int startOffset = doc.getLineOffset(startLine - 1); 
				int endOffset = doc.getLineOffset(endLine - 1 + 1);
				posList.put(key, new Position(startOffset, endOffset - startOffset));
			}
		}
	}
}
