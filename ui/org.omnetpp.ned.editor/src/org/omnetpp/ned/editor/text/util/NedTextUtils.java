package org.omnetpp.ned.editor.text.util;

import org.apache.commons.lang.ArrayUtils;
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
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.pojo.ChannelSpecElement;
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
        public INEDElement element; // the element being hovered/under cursor
        public IRegion regionToHighlight; // range to highlight as link (word)
        public INEDElement referredElement;  // the jump target, usually the declaration of the gate/param/submodule/type

        public Info(INEDElement element, IRegion regionToHighlight, INEDElement referredElement) {
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

            INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
            INEDElement element = res.getNedElementAt(file, line+1, column);
            if (element == null)
                return null; // we don't know what's there

            if (element instanceof ImportElement) {
                INEDElement declElement = lookupTypeElement(dottedWord, element.getContainingNedFileElement());
                return createInfo(element, dottedWordRegion, declElement);
            }
            if (element instanceof INedTypeElement) {
                if (word.equals(((INedTypeElement)element).getName()))
                    return createInfo(element, wordRegion, element);
            }
            if (element instanceof ExtendsElement || element instanceof InterfaceNameElement) {
                INEDElement declElement = lookupTypeElement(dottedWord, element.getParent().getEnclosingLookupContext());
                return createInfo(element, dottedWordRegion, declElement);
            }
            if (element instanceof SubmoduleElementEx) {
                if (dottedWord.equals(((SubmoduleElementEx)element).getEffectiveType())) {
                    INEDElement declElement = lookupTypeElement(dottedWord, element.getEnclosingLookupContext());
                    return createInfo(element, dottedWordRegion, declElement);
                }
                if (dottedWord.equals(((SubmoduleElementEx)element).getName()))
                    return createInfo(element, dottedWordRegion, element);
                return null;
            }
            if (element instanceof ChannelSpecElement) {
                if (dottedWord.equals(((ConnectionElementEx)element.getParent()).getEffectiveType())) {
                    INEDElement declElement = lookupTypeElement(dottedWord, element.getEnclosingLookupContext());
                    return createInfo(element, dottedWordRegion, declElement);
                }
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
                    return createInfo(element, wordRegion, compoundModule.getNEDTypeInfo().getSubmodules().get(word));

                // gate names are often the same on both side of the conn ("a.port++ <--> b.port++;"),
                // so we need to parse a bit where the user was hovering exactly
                IRegion connRegion = getElementRegion(doc, element);
                String textBeforeWord = TextEditorUtil.get(textViewer, new Region(connRegion.getOffset(), wordRegion.getOffset() - connRegion.getOffset()));
                
                if (!textBeforeWord.contains("--") && word.equals(conn.getSrcGate()))
                    return createInfo(element, wordRegion, lookupGate(compoundModule, conn.getSrcModule(), conn.getSrcGate()));
                if (textBeforeWord.contains("--") && word.equals(conn.getDestGate()))
                    return createInfo(element, wordRegion, lookupGate(compoundModule, conn.getDestModule(), conn.getDestGate()));
            }

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
    public static IRegion getElementRegion(IDocument doc, INEDElement element) throws BadLocationException {
        NEDSourceRegion sourceRegion = element.getSourceRegion();
        if (sourceRegion == null)
            return null;
        // the "+column" bit assumes there're no tabs on that line. This is reasonable, because NED
        // code gets re-generated with spaces every now and then. "line-1" because doc is 0-based.
        int startOffset = doc.getLineOffset(sourceRegion.getStartLine()-1) + sourceRegion.getStartColumn();
        int endOffset = doc.getLineOffset(sourceRegion.getEndLine()-1) + sourceRegion.getEndColumn();
        return new Region(startOffset, endOffset - startOffset);
    }
    
    private static Info createInfo(INEDElement element, IRegion wordRegion, INEDElement declElement) {
        return declElement == null ? null : new Info(element, wordRegion, declElement);
    }

    private static INEDElement lookupTypeElement(String dottedWord, INedTypeLookupContext context) {
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        INEDTypeInfo nedType = res.lookupNedType(dottedWord, context);
        return nedType != null ? nedType.getNEDElement() : null;
    }

    private static INEDElement findParentWithClass(INEDElement element, Class<?> clazz) {
        while (element != null && !clazz.isInstance(element))
            element = element.getParent();
        return element;
    }

    private static INEDElement lookupGate(CompoundModuleElementEx compoundModule, String moduleName, String gateName) {
        IConnectableElement mod = StringUtils.isEmpty(moduleName) ? compoundModule : compoundModule.getNEDTypeInfo().getSubmodules().get(moduleName);
        if (mod != null) {
            GateElementEx gate = mod.getGateSizes().get(gateName);
            if (gate == null)
                gate = mod.getGateDeclarations().get(gateName);
            return gate;
        }
        return null;
    }
}
