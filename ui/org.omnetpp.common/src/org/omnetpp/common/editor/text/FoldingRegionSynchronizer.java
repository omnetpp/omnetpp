package org.omnetpp.common.editor.text;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.source.IAnnotationModel;
import org.eclipse.jface.text.source.IAnnotationModelExtension;
import org.eclipse.jface.text.source.projection.ProjectionAnnotation;
import org.eclipse.jface.text.source.projection.ProjectionAnnotationModel;
import org.eclipse.ui.texteditor.ITextEditor;

/**
 * This class synchronizes folding regions on a text editor.
 *
 * @author andras
 */
public class FoldingRegionSynchronizer {
	private ITextEditor editor;

	// holds existing folding regions, because we need to keep synchronizing it with the model
	private Map<String,ProjectionAnnotation> oldAnnotations = new HashMap<String,ProjectionAnnotation>();

	/**
	 * Constructor accepts associated text editor.
	 */
	public FoldingRegionSynchronizer(ITextEditor editor) {
		this.editor = editor;
	}

	/**
	 * Adds/removes folding regions (ProjectionAnnotation) on the text editor, so
	 * that it's in sync with the positions in the given map. Folding region identity 
	 * is based on the string keys in the map (this is important so as not to lose 
	 * the expanded/collapsed state of the regions).
	 *
	 * Just plain deleting existing annotations and re-adding them would result 
	 * in their getting expanded all the time. 
	 */
	public void updateFoldingRegions(Map<String,Position> newAnnotationPositions) {

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
	}

	private static IAnnotationModel getAnnotationModel(ITextEditor editor) {
		return (IAnnotationModel) editor.getAdapter(ProjectionAnnotationModel.class);
	}
}
