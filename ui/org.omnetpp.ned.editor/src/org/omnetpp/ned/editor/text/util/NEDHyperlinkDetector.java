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
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

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
			String word = TextEditorUtil.get(textViewer, wordRegion);
			if (StringUtils.isEmpty(word))
				return null;

			// find hovered NED element
			//FIXME same code as in NedTextHover, factor out common parts!
			IFile file = ((FileEditorInput)editor.getEditorInput()).getFile();
			IDocument doc = textViewer.getDocument();
			int line = doc.getLineOfOffset(region.getOffset());
			int column = region.getOffset() - doc.getLineOffset(line);

			INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
			INEDElement hoveredElement = res.getNedElementAt(file, line, column);
			if (hoveredElement == null)
				return null;
			
			INedTypeElement typeElement = hoveredElement.getEnclosingTypeElement();
			INedTypeLookupContext context = typeElement instanceof CompoundModuleElementEx ? (CompoundModuleElementEx)typeElement : 
				typeElement!=null ? typeElement.getParentLookupContext() : 
					hoveredElement.getContainingNedFileElement();
			INEDTypeInfo nedTypeUnderCursor = NEDResourcesPlugin.getNEDResources().lookupNedType(word, context);
			if (nedTypeUnderCursor != null)
				return new IHyperlink[] {new NEDHyperlink(wordRegion, nedTypeUnderCursor.getNEDElement())};

			// if not found among the components, we do not create a hyperlink
			return null;

		} catch (BadLocationException e) {
			NEDResourcesPlugin.logError(e);
			return null;
		}
	}

}
