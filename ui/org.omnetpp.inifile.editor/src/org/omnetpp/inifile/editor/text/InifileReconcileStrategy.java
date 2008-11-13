package org.omnetpp.inifile.editor.text;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Position;
import org.eclipse.jface.text.reconciler.DirtyRegion;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.omnetpp.common.Debug;
import org.omnetpp.common.editor.text.FoldingRegionSynchronizer;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileDocument;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;

/**
 * This class has one instance per editor. It performs 
 * background parsing of the inifile, and keeps editor data 
 * (InifileContents) up to date.
 */
public class InifileReconcileStrategy implements IReconcilingStrategy {
	private InifileEditorData editorData = null;
	private FoldingRegionSynchronizer synchronizer = null;

	public InifileReconcileStrategy(InifileEditorData editorData) {
		this.editorData = editorData;
	}

	public void setDocument(IDocument document) {
	}

	public void reconcile(DirtyRegion dirtyRegion, IRegion subRegion) {
		// not called in our setup
	}

	public void reconcile(IRegion partition) {
		Debug.println("reconcile(IRegion) called");

		// force parsing and analyzing the file now (they are both lazy and 
		// wouldn't do that otherwise until a view, a tooltip or something
		// needs data from them)
		((InifileDocument)editorData.getInifileDocument()).parse();
		editorData.getInifileAnalyzer().analyze();

		// make inifile sections foldable
		updateFoldingRegions();
	}

	private void updateFoldingRegions() {
		//XXX in the ctor it's too early to call, editor input is not yet set there...?
		if (synchronizer == null)
			synchronizer = new FoldingRegionSynchronizer(editorData.getInifileEditor().getTextEditor());

		// collect positions
		final Map<String,Position> newAnnotationPositions = new HashMap<String,Position>();
		IDocument textDoc = editorData.getInifileEditor().getTextEditor().getDocument();
		IInifileDocument doc = editorData.getInifileDocument();
		for (String section : doc.getSectionNames()) {
			LineInfo lines = doc.getSectionLineDetails(section);
			addPosition(textDoc, lines.getLineNumber(), lines.getNumLines(), section+":"+lines.getLineNumber(), newAnnotationPositions);
		}

		// synchronize with the text editor
		synchronizer.updateFoldingRegions(newAnnotationPositions);
	}

	// like the above, but this version parses the document, because InifileDocument doesn't store
	// if there're multiple occurrences of a section name within the document. Question is if we 
	// really want to allow that?!?!
	//XXX needed..?
	private void updateFoldingRegions2() {
		if (synchronizer == null)
			synchronizer = new FoldingRegionSynchronizer(editorData.getInifileEditor().getTextEditor());

		// collect positions
		final Map<String,Position> newAnnotationPositions = new HashMap<String,Position>();

		try {
			final IDocument doc = editorData.getInifileEditor().getTextEditor().getDocument();
			Reader streamReader = new StringReader(doc.get());

			class Callback extends InifileParser.ParserAdapter {
				String key = null;
				int firstLine, lastLine;

				@Override
				public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {
					if (key != null)
						addPosition(doc, firstLine, lastLine, key, newAnnotationPositions);
					key = sectionName + ":" + lineNumber;
					firstLine = lineNumber;
					lastLine = firstLine + numLines - 1;
				}
				@Override
				public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment) {
					lastLine = lineNumber + numLines - 1;
				}

				public void done() {
					if (key != null)
						addPosition(doc, firstLine, lastLine, key, newAnnotationPositions);
				}
			}

			Callback callback = new Callback();
			new InifileParser().parse(streamReader, callback);
			callback.done();
		} 
		catch (IOException e) {
			// cannot happen with string input
		} 
		catch (ParseException e) {
		}

		// synchronize with the text editor
		synchronizer.updateFoldingRegions(newAnnotationPositions);
	}

	private static void addPosition(IDocument doc, int startLine, int numLines, String key, Map<String,Position> posList) {
		try {
			if (numLines > 1) {
				int startOffset = doc.getLineOffset(startLine - 1);
				int endOffset = doc.getLineOffset(startLine + numLines - 1);
				posList.put(key, new Position(startOffset, endOffset - startOffset));
			}
		} catch (BadLocationException e) {
		} 
	}
}
