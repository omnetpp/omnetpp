package org.omnetpp.ned.editor.text;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.eclipse.jface.text.source.IAnnotationModel;
import org.eclipse.jface.text.source.IAnnotationModelExtension;
import org.eclipse.jface.text.source.projection.ProjectionAnnotation;
import org.eclipse.jface.text.source.projection.ProjectionAnnotationModel;
import org.eclipse.ui.texteditor.ITextEditor;
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

	// holds existing folding regions, because we need to keep synchronizing it with the NED model
	private Map<String,ProjectionAnnotation> oldAnnotations = new HashMap<String,ProjectionAnnotation>();

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
		
		updateFoldingRegions();
	}

	private void updateFoldingRegions() {
		try {
			// collect positions
			IDocument doc = editor.getDocument();
			Map<String,Position> newAnnotationPositions = new HashMap<String,Position>();
			NedFileElementEx nedFileElement = NEDResourcesPlugin.getNEDResources().getNedFileElement(editor.getFile());
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

			// Unfortunately, we cannot just delete existing annotations and readd the 
			// current ones, because they'd lose their state (expanded/collapsed). 
			// We need to synchronize them with existing annotation model.
			IAnnotationModel annotationModel = getAnnotationModel(editor);
			IAnnotationModelExtension annotationModel2 = ((IAnnotationModelExtension)annotationModel);

			// first: remove extra annotations
			List<String> trash = new ArrayList<String>(); // to avoid ConcurrentModificationException
			for (String key : oldAnnotations.keySet())
				if (!newAnnotationPositions.containsKey(key))
					trash.add(key);
			for (String key : trash) {
				annotationModel.removeAnnotation(oldAnnotations.get(key));
				oldAnnotations.remove(key);
			}
			
			// second: add missing annotations and update positions
			for (String key : newAnnotationPositions.keySet()) {
				if (!oldAnnotations.containsKey(key)) {
					// missing: add it
					ProjectionAnnotation annotation = new ProjectionAnnotation();
					annotationModel.addAnnotation(annotation, newAnnotationPositions.get(key));
					oldAnnotations.put(key, annotation);
				}
				else {
					// update position
					ProjectionAnnotation annotation = oldAnnotations.get(key);
					Position position = annotationModel.getPosition(annotation);
					if (position == null)
						oldAnnotations.remove(key); // disappeared from the editor, forget it
					else if (!position.equals(newAnnotationPositions.get(key)))
						annotationModel2.modifyAnnotationPosition(annotation, newAnnotationPositions.get(key));
				}
			}
		} catch (BadLocationException e) {
		}
	}

	private static IAnnotationModel getAnnotationModel(ITextEditor editor) {
		return (IAnnotationModel) editor.getAdapter(ProjectionAnnotationModel.class);
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
