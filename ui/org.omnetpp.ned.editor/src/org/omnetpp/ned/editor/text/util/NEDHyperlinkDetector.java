package org.omnetpp.ned.editor.text.util;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NEDHyperlink;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;

/**
 * TODO add documentation
 *
 * @author rhornig, andras
 */
public class NEDHyperlinkDetector implements IHyperlinkDetector {
	private TextEditor editor;

	public NEDHyperlinkDetector(TextEditor editor) {
		this.editor = editor;
	}

	public IHyperlink[] detectHyperlinks(ITextViewer textViewer, IRegion region, boolean canShowMultipleHyperlinks) {
		try {
			IRegion wordRegion = TextEditorUtil.detectWordRegion(textViewer, region.getOffset(), new NedSyntaxHighlightHelper.NedDottedWordDetector());
			String word1 = TextEditorUtil.get(textViewer, wordRegion);
			if (StringUtils.isEmpty(word1))
				return null;

            // find which NED element was hovered
            IFile file = ((FileEditorInput)editor.getEditorInput()).getFile();
            IDocument doc = textViewer.getDocument();
            int line = doc.getLineOfOffset(region.getOffset());
            int column = region.getOffset() - doc.getLineOffset(line);

            INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
            INEDElement element = res.getNedElementAt(file, line+1, column);
            if (element == null)
                return null; // we don't know what's there

            // get words under mouse
            String dottedWord = TextEditorUtil.getWordRegion(textViewer, region.getOffset(), new NedSyntaxHighlightHelper.NedDottedWordDetector());
            String word = TextEditorUtil.getWordRegion(textViewer, region.getOffset(), new NedSyntaxHighlightHelper.NedWordDetector());

            INEDElement declElement = NedTextUtils.findDeclaration(element, dottedWord, word);
            if (declElement == null)
                return null;
            return new IHyperlink[] {new NEDHyperlink(wordRegion, declElement)};

		} catch (BadLocationException e) {
			NEDResourcesPlugin.logError(e);
			return null;
		}
	}

}
