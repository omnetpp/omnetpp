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
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.pojo.ChannelSpecElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

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
			// find which NED element was hovered
			IFile file = ((FileEditorInput)editor.getEditorInput()).getFile();
			IDocument doc = textViewer.getDocument();
			int line = doc.getLineOfOffset(hoverRegion.getOffset());
			int column = hoverRegion.getOffset() - doc.getLineOffset(line);

			INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
			INEDElement hoveredElement = res.getNedElementAt(file, line+1, column);
			if (hoveredElement == null)
				return null; // we don't know what's there

			// get hover text for this
			String dottedWord = TextEditorUtil.getWordRegion(textViewer, hoverRegion.getOffset(), new NedSyntaxHighlightHelper.NedDottedWordDetector());
			String word = TextEditorUtil.getWordRegion(textViewer, hoverRegion.getOffset(), new NedSyntaxHighlightHelper.NedWordDetector());

			return getHoverText(hoveredElement, word, dottedWord);
		} 
		catch (BadLocationException e) {
			NedEditorPlugin.logError(e);
			return null;
		}
	}

	protected static String getHoverText(INEDElement hoveredElement, String hoveredWord, String hoveredDottedWord) {
		System.out.println("hovering "+hoveredDottedWord+" in "+hoveredElement+" ("+hoveredElement.getSourceRegion()+")");

		if (StringUtils.isEmpty(hoveredDottedWord))
			return null; // nothing interesting here

		INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
		
		// try to interpret the hovered word as a NED type name
		if (mayContainTypeName(hoveredElement)) {
			INedTypeElement typeElement = hoveredElement.getEnclosingTypeElement();
			INedTypeLookupContext context = typeElement instanceof CompoundModuleElementEx ? (CompoundModuleElementEx)typeElement : 
				typeElement!=null ? typeElement.getParentLookupContext() : 
					hoveredElement.getContainingNedFileElement();
			INEDTypeInfo typeInfo = res.lookupNedType(hoveredDottedWord, context);
			if (typeInfo != null)
				return getHoverTextFor(typeInfo);
		}
		
		if (hoveredElement instanceof ParamElementEx) {
			ParamElementEx paramElement = (ParamElementEx)hoveredElement;
			INEDTypeInfo declaringType;
			if (hoveredElement.getParentWithTag(NEDElementTags.NED_SUBMODULE) != null)
				declaringType = ((SubmoduleElementEx)hoveredElement.getParentWithTag(NEDElementTags.NED_SUBMODULE)).getNEDTypeInfo();
			else
				declaringType = hoveredElement.getEnclosingTypeElement().getNEDTypeInfo();
			if (declaringType != null) {
				ParamElementEx paramDecl = declaringType.getParamDeclarations().get(paramElement.getName());
				if (paramDecl != null) {
					//TODO...
				}
			}
		}
		//FIXME finish...
		return null;
	}

	protected static boolean mayContainTypeName(INEDElement element) {
		return element instanceof INedTypeElement || element instanceof ImportElement || 
				element instanceof ExtendsElement || element instanceof InterfaceNameElement ||
				element instanceof SubmoduleElement || element instanceof ChannelSpecElement;
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
