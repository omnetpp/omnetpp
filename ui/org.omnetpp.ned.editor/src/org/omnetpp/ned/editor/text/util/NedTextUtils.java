/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.util;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.editor.text.Keywords;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedSourceRegion;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.pojo.ParamElement;

/**
 * Misc functions for NED text editors.
 *
 * @author andras
 */
public class NedTextUtils {
    /**
     * Result of findNedContext()
     */
    public static class Info {
        public INedElement element; // the element being hovered/under cursor
        public IRegion regionToHighlight; // range to highlight as link (word)
        public INedElement referredElement;  // the jump target, usually the declaration of the gate/param/submodule/type

        public Info(INedElement element, IRegion regionToHighlight, INedElement referredElement) {
            this.element = element;
            this.regionToHighlight = regionToHighlight;
            this.referredElement = referredElement;
        }
    }

    public static Info getNedReferenceFromSource(ITextEditor textEditor, ITextViewer textViewer, IRegion region) {
        try {
            IRegion wordRegion = TextEditorUtil.detectWordRegion(textViewer, region.getOffset(), new SyntaxHighlightHelper.NedWordDetector());
            IRegion dottedWordRegion = TextEditorUtil.detectWordRegion(textViewer, region.getOffset(), new SyntaxHighlightHelper.NedDottedWordDetector());
            if (wordRegion.getLength() == 0)
                return null;

            String word = TextEditorUtil.get(textViewer, wordRegion);
            String dottedWord = TextEditorUtil.get(textViewer, dottedWordRegion);
            if (ArrayUtils.contains(Keywords.NED_RESERVED_WORDS, word))
                return null;

            // find which NED element was hovered
            IFile file = ((FileEditorInput)textEditor.getEditorInput()).getFile();
            IDocument doc = textViewer.getDocument();
            int line = doc.getLineOfOffset(region.getOffset());
            int column = region.getOffset() - doc.getLineOffset(line);

            INedTypeResolver res = NedResourcesPlugin.getNedResources();
            INedElement element = res.getNedElementAt(file, line+1, column);
            if (element == null)
                return null; // we don't know what's there

            if (element instanceof ImportElement) {
                INedElement declElement = lookupTypeElement(dottedWord, element.getContainingNedFileElement());
                return createInfo(element, dottedWordRegion, declElement);
            }
            if (element instanceof INedTypeElement) {
                if (word.equals(((INedTypeElement)element).getName()))
                    return createInfo(element, wordRegion, element);
            }
            if (element instanceof ExtendsElement || element instanceof InterfaceNameElement) {
                INedElement declElement = lookupTypeElement(dottedWord, element.getParent().getEnclosingLookupContext());
                return createInfo(element, dottedWordRegion, declElement);
            }
            if (element instanceof SubmoduleElementEx) {
                if (dottedWord.equals(((SubmoduleElementEx)element).getTypeOrLikeType())) {
                    INedElement declElement = lookupTypeElement(dottedWord, element.getEnclosingLookupContext());
                    return createInfo(element, dottedWordRegion, declElement);
                }
                if (dottedWord.equals(((SubmoduleElementEx)element).getName()))
                    return createInfo(element, dottedWordRegion, element);
                return null;
            }
            if (element instanceof ParamElement) {
                String paramName = ((ParamElement)element).getName();
                if (word.equals(paramName)) {
                    IHasParameters hasParameters = (IHasParameters) findParentWithClass(element, IHasParameters.class);
                    ParamElementEx declElement = hasParameters.getParamDeclarations().get(paramName);
                    return createInfo(element, wordRegion, declElement);
                }
                return null;
            }
            else if (element instanceof GateElement) {
                String gateName = ((GateElement)element).getName();
                if (word.equals(gateName)) {
                    IHasGates hasGates = (IHasGates) findParentWithClass(element, IHasGates.class);
                    GateElementEx declElement = hasGates.getGateDeclarations().get(gateName);
                    return createInfo(element, wordRegion, declElement);
                }
                return null;
            }
            if (element instanceof ConnectionElementEx) {
                ConnectionElementEx conn = (ConnectionElementEx)element;
                CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)element.getEnclosingTypeElement();

                if (word.equals(conn.getSrcModule()) || word.equals(conn.getDestModule()))
                    return createInfo(element, wordRegion, compoundModule.getNedTypeInfo().getSubmodules().get(word));

                // gate names are often the same on both side of the conn ("a.port++ <--> b.port++;"),
                // so we need to parse a bit where the user was hovering exactly
                IRegion connRegion = getElementRegion(doc, element);
                int wordEndOffset = wordRegion.getOffset() + wordRegion.getLength();
                int connEndOffset = connRegion.getOffset() + connRegion.getLength();
                String textBeforeWord = TextEditorUtil.get(textViewer, new Region(connRegion.getOffset(), wordRegion.getOffset() - connRegion.getOffset()));
                String textAfterWord = TextEditorUtil.get(textViewer, new Region(wordEndOffset, connEndOffset - wordEndOffset));

                boolean isInLeftGateRegion = !textBeforeWord.contains("--");
                boolean isInRightGateRegion = !textAfterWord.contains("--");
                boolean isInSrcGateRegion = (isInLeftGateRegion && conn.getIsForwardArrow()) || (isInRightGateRegion && !conn.getIsForwardArrow());
                boolean isInDestGateRegion = (isInRightGateRegion && conn.getIsForwardArrow()) || (isInLeftGateRegion && !conn.getIsForwardArrow());
                if (isInSrcGateRegion && word.equals(conn.getSrcGate()))
                    return createInfo(element, wordRegion, lookupGate(compoundModule, conn.getSrcModule(), conn.getSrcGate()));
                if (isInDestGateRegion && word.equals(conn.getDestGate()))
                    return createInfo(element, wordRegion, lookupGate(compoundModule, conn.getDestModule(), conn.getDestGate()));

                if (dottedWord.equals(conn.getTypeOrLikeType())) {
                    INedElement declElement = lookupTypeElement(dottedWord, element.getEnclosingLookupContext());
                    return createInfo(element, dottedWordRegion, declElement);
                }
                return null;

            }

            // If we are here, the selected word likely occurs within a comment (e.g. the type's NEDDOC comment)

            // Try to resolve as type name using the file's imports etc.
            INedTypeLookupContext context = element instanceof NedFileElementEx ? (NedFileElementEx)element : element.getEnclosingLookupContext();
            INedElement typeElement = lookupTypeElement(dottedWord, context);
            if (typeElement != null)
                return createInfo(element, dottedWordRegion, typeElement);

            // Try to resolve as toplevel NED type, assuming dottedWord is its fully qualified type name
            INedTypeInfo typeInfo = res.getToplevelNedType(dottedWord, file.getProject());
            if (typeInfo != null)
                return createInfo(element, dottedWordRegion, typeInfo.getNedElement());

            // Try to resolve by simple name (without package); If there are multiple matches, return the first one
            for (INedTypeInfo info : res.getToplevelNedTypesBySimpleName(word, file.getProject()))
                return createInfo(element, wordRegion, info.getNedElement());

            return null; // nothing
        }
        catch (BadLocationException e) {
            NedEditorPlugin.logError(e);
            return null;
        }
    }

    /**
     * Returns the region document region (offset+length) from the NED element, using its
     * source location info if present. If not, returns null.
     */
    public static IRegion getElementRegion(IDocument doc, INedElement element) throws BadLocationException {
        NedSourceRegion sourceRegion = element.getSourceRegion();
        if (sourceRegion == null)
            return null;
        // the "+column" bit assumes there're no tabs on that line. This is reasonable, because NED
        // code gets re-generated with spaces every now and then. "line-1" because doc is 0-based.
        int startOffset = doc.getLineOffset(sourceRegion.getStartLine()-1) + sourceRegion.getStartColumn();
        int endOffset = doc.getLineOffset(sourceRegion.getEndLine()-1) + sourceRegion.getEndColumn();
        return new Region(startOffset, endOffset - startOffset);
    }

    private static Info createInfo(INedElement element, IRegion wordRegion, INedElement declElement) {
        return declElement == null ? null : new Info(element, wordRegion, declElement);
    }

    private static INedElement lookupTypeElement(String dottedWord, INedTypeLookupContext context) {
        INedResources res = NedResourcesPlugin.getNedResources();
        INedTypeInfo nedType = res.lookupNedType(dottedWord, context);
        return nedType != null ? nedType.getNedElement() : null;
    }

    private static INedElement findParentWithClass(INedElement element, Class<?> clazz) {
        while (element != null && !clazz.isInstance(element))
            element = element.getParent();
        return element;
    }

    private static INedElement lookupGate(CompoundModuleElementEx compoundModule, String moduleName, String gateName) {
        IConnectableElement mod = StringUtils.isEmpty(moduleName) ? compoundModule : compoundModule.getNedTypeInfo().getSubmodules().get(moduleName);
        if (mod != null) {
            GateElementEx gate = mod.getGateSizes().get(gateName);
            if (gate == null)
                gate = mod.getGateDeclarations().get(gateName);
            return gate;
        }
        return null;
    }
}
