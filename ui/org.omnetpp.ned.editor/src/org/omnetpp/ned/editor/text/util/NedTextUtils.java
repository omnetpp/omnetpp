package org.omnetpp.ned.editor.text.util;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.editor.text.NedKeywords;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;
import org.omnetpp.common.editor.text.TextEditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
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
    }
    
    public static Info getNedReferenceFromSource(ITextEditor textEditor, ITextViewer textViewer, IRegion region) {
        try {
            IRegion wordRegion = TextEditorUtil.detectWordRegion(textViewer, region.getOffset(), new NedSyntaxHighlightHelper.NedWordDetector());
            IRegion dottedWordRegion = TextEditorUtil.detectWordRegion(textViewer, region.getOffset(), new NedSyntaxHighlightHelper.NedDottedWordDetector());
            if (wordRegion.getLength() == 0)
                return null;

            String word = TextEditorUtil.get(textViewer, wordRegion);
            String dottedWord = TextEditorUtil.get(textViewer, dottedWordRegion);
            
            // find which NED element was hovered
            IFile file = ((FileEditorInput)textEditor.getEditorInput()).getFile();
            IDocument doc = textViewer.getDocument();
            int line = doc.getLineOfOffset(region.getOffset());
            int column = region.getOffset() - doc.getLineOffset(line);

            INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
            INEDElement element = res.getNedElementAt(file, line+1, column);
            if (element == null)
                return null; // we don't know what's there

            INEDElement declElement = NedTextUtils.findJumpTarget(element, dottedWord, word);
            if (declElement == null)
                return null;

            Info result = new Info();
            result.element = element;
            result.regionToHighlight = wordRegion; //FIXME or dottedWordRegion -- which one??
            result.referredElement = declElement;
            return result;
        } 
        catch (BadLocationException e) {
            NedEditorPlugin.logError(e);
            return null;
        }
        
    }
    
    /**
     * Implements functionality for "F3 Goto Declaration", "ctrl+click hyperlink", 
     * and hover information.
     */
    public static INEDElement findJumpTarget(INEDElement element, String dottedWord, String word) {
        if (ArrayUtils.contains(NedKeywords.RESERVED_WORDS, word))
            return null;
        
        if (element instanceof ImportElement) {
            return lookupTypeElement(dottedWord, element.getContainingNedFileElement());
        }
        if (element instanceof ExtendsElement || element instanceof InterfaceNameElement) {
            return lookupTypeElement(dottedWord, element.getParent().getEnclosingLookupContext());
        }
        if (element instanceof SubmoduleElementEx) {
            if (dottedWord.equals(((SubmoduleElementEx)element).getEffectiveType()))
                return lookupTypeElement(dottedWord, element.getEnclosingLookupContext());
            return null;
        }
        if (element instanceof ChannelSpecElement) {
            if (dottedWord.equals(((ConnectionElementEx)element.getParent()).getEffectiveType()))
                return lookupTypeElement(dottedWord, element.getEnclosingLookupContext());
            return null;
        }
        if (element instanceof ParamElement) {
            String paramName = ((ParamElement)element).getName();
            if (word.equals(paramName)) {
                IHasParameters hasParameters = (IHasParameters) findParentWithClass(element, IHasParameters.class);
                return hasParameters.getParamDeclarations().get(paramName);
            }
            return null;
        }
        else if (element instanceof GateElement) {
            String gateName = ((GateElement)element).getName();
            if (word.equals(gateName)) {
                IHasGates hasGates = (IHasGates) findParentWithClass(element, IHasGates.class);
                return hasGates.getGateDeclarations().get(gateName);
            }
            return null;
        }
        if (element instanceof ConnectionElementEx) {
            ConnectionElementEx conn = (ConnectionElementEx)element;
            CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)element.getEnclosingTypeElement();
            if (word.equals(conn.getSrcModule()) || word.equals(conn.getDestModule()))
                return compoundModule.getNEDTypeInfo().getSubmodules().get(word);

            if (word.equals(conn.getSrcGate()) && word.equals(conn.getDestGate()))
                System.out.println("We're in trouble");  //FIXME find out how to fix this
            if (word.equals(conn.getSrcGate()))
                return lookupGate(compoundModule, conn.getSrcModule(), conn.getSrcGate());
            if (word.equals(conn.getDestGate()))
                return lookupGate(compoundModule, conn.getDestModule(), conn.getDestGate());
        }
       
        return null; // nothing
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
