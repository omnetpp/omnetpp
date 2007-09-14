package org.omnetpp.ned.editor.text.util;


import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.ITextHoverExtension;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.information.IInformationProviderExtension2;
import org.eclipse.swt.graphics.Point;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.text.util.NedTextUtils.Info;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

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
	    Info info = NedTextUtils.getNedHoverContext((ITextEditor)editor, textViewer, hoverRegion);
	    if (info == null)
	        return null;

		if (info.referredElement instanceof INedTypeElement)
		    return getHoverTextFor(((INedTypeElement)info.referredElement).getNEDTypeInfo());

		return HoverSupport.addHTMLStyleSheet(info.referredElement.toString() + "<br/>" + "<pre>" + info.referredElement.getNEDSource() + "</pre>"); //FIXME refine!!! ie docu, etc
	}

	protected static String getHoverTextFor(INEDTypeInfo typeInfo) {
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
