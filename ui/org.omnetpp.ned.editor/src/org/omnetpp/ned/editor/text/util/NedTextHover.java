package org.omnetpp.ned.editor.text.util;


import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextHoverExtension;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.information.IInformationProviderExtension2;
import org.eclipse.swt.graphics.Point;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

/**
 * NED text hover. Currently it displays fully qualified name and the documentation 
 * of the NED type name hovered. 
 *
 * @author rhornig, andras
 */
public class NedTextHover implements ITextHover, ITextHoverExtension, IInformationProviderExtension2 {
	private IEditorPart editor = null;

	public NedTextHover(IEditorPart editor) {
		this.editor = editor;
	}

	public String getHoverInfo(ITextViewer textViewer, IRegion hoverRegion) {
		try {
			String word = TextEditorUtil.getWordRegion(textViewer, hoverRegion.getOffset(), new NedSyntaxHighlightHelper.NedDottedWordDetector());
			if (StringUtils.isEmpty(word))
				return null;

			// if we find a NED component with that name, display its source code
			IFile file = ((FileEditorInput)editor.getEditorInput()).getFile();
			IDocument doc = textViewer.getDocument();
			int line = doc.getLineOfOffset(hoverRegion.getOffset());
			int column = hoverRegion.getOffset() - doc.getLineOffset(line);

			INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
			INEDElement hoveredElement = res.getNedElementAt(file, line, column);
			System.out.println("HOVERED:"+hoveredElement);
			if (hoveredElement == null)
				return null;

			INedTypeElement typeElement = hoveredElement.getEnclosingTypeElement();
			INedTypeLookupContext context = typeElement instanceof CompoundModuleElementEx ? (CompoundModuleElementEx)typeElement : 
				typeElement!=null ? typeElement.getParentLookupContext() : 
					hoveredElement.getContainingNedFileElement();
			INEDTypeInfo nedTypeHovered = res.lookupNedType(word, context);
			System.out.println("typeInfo="+nedTypeHovered+" on hovering "+word+" in "+hoveredElement+" ("+hoveredElement.getSourceRegion()+")");
			if (nedTypeHovered == null)
				return null;

			return makeHTMLHoverFor(nedTypeHovered);
		} 
		catch (BadLocationException e) {
			NedEditorPlugin.logError(e);
			return null;
		}
	}

	protected static String makeHTMLHoverFor(INEDTypeInfo typeInfo) {
		String text = "<b>" + typeInfo.getFullyQualifiedName() +  "</b><br/>\n";
		
		String comment = typeInfo.getNEDElement().getComment();
		if (StringUtils.isNotEmpty(comment))
				text += "<br/>" + StringUtils.makeHtmlDocu(comment);

		return HoverSupport.addHTMLStyleSheet(text);
	}

	public IRegion getHoverRegion(ITextViewer textViewer, int offset) {
		Point selection= textViewer.getSelectedRange();
		if (selection.x <= offset && offset < selection.x + selection.y)
			return new Region(selection.x, selection.y);
		return new Region(offset, 0);
	}
	
	/*
	 * @see org.eclipse.jface.text.ITextHoverExtension#getHoverControlCreator()
	 */
	public IInformationControlCreator getHoverControlCreator() {
		return HoverSupport.getHoverControlCreator();
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.text.information.IInformationProviderExtension2#getInformationPresenterControlCreator()
	 */
	public IInformationControlCreator getInformationPresenterControlCreator() {
		return HoverSupport.getInformationPresenterControlCreator();
	}
}
