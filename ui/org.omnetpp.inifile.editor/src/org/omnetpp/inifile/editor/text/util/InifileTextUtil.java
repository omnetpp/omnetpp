package org.omnetpp.inifile.editor.text.util;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Region;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

public class InifileTextUtil {

    /**
     * Get position of the key within the line.
     */
    static IRegion getKeyRegion(IDocument textDoc, int lineNumber) throws BadLocationException {
        int lineOffset = textDoc.getLineOffset(lineNumber-1);
        int nextLineOffset = textDoc.getLineOffset(lineNumber);
        String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
        int eqPos = lineContents.indexOf('=');
        Region region = new Region(lineOffset, eqPos);
        return trimRegion(textDoc, region);
    }

    /**
     * Get the pattern from the key at the offset position (excluding the [*] part)
     */
    static IRegion getKeyPartRegion(IDocument textDoc, IRegion keyRegion, int offset) throws BadLocationException {
        if (!contains(keyRegion, offset))
            return null;
        int start = offset;
        while (start >= keyRegion.getOffset() && isPatternChar(textDoc.getChar(start)))
            --start;
        int end = offset;
        while (end < keyRegion.getOffset() + keyRegion.getLength() && isPatternChar(textDoc.getChar(end)))
            ++end;
        return new Region(start+1, end-start-1);
    }
    
    /**
     * Get the position of key within the line.
     */
    static IRegion getValueRegion(IDocument textDoc, int lineNumber) throws BadLocationException {
        int lineOffset = textDoc.getLineOffset(lineNumber-1);
        int nextLineOffset = textDoc.getLineOffset(lineNumber);
        String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
        int eqPos = lineContents.indexOf('=');
        int hashmarkPos = lineContents.indexOf('#');
        Region region = new Region(lineOffset + eqPos + 1, hashmarkPos == -1 ? nextLineOffset-lineOffset-eqPos - 1 - 1 : hashmarkPos - eqPos -1);
        return trimRegion(textDoc, region);
    }

    static IRegion trimRegion(IDocument textDoc, IRegion r) throws BadLocationException {
        int begin = r.getOffset();
        int end = begin + r.getLength();
        while(Character.isWhitespace(textDoc.getChar(begin)) && begin < end)
            begin++;
        while(Character.isWhitespace(textDoc.getChar(end-1)) && begin < end)
            end--;
        return new Region(begin, end-begin);
    }
    
    static boolean contains(IRegion reg, int offset) {
        return offset >= reg.getOffset() && offset < reg.getOffset() + reg.getLength();
    }
    
    static boolean isPatternChar(char ch) {
        return ch != '.' && ch != '[' && ch != ']';
    }
    
    static INedElement findNamedElement(String pattern, ParamResolution resolution) {
        // try as parameter name
        ParamElementEx param = resolution.paramAssignment!=null ? resolution.paramAssignment : resolution.paramDeclaration;
        if (param != null && ParamUtil.matchesPattern(pattern, param.getName()))
            return param;
        // try as submodule name
        for (ISubmoduleOrConnection element : resolution.elementPath)
            if (element instanceof IHasName && ParamUtil.matchesPattern(pattern, ((IHasName)element).getName()))
                return element;
        // try as network name
        if (resolution.elementPath.length >= 2) {
            if (resolution.elementPath[1] != null && (resolution.elementPath[1].getCompoundModule() instanceof IHasName) &&
                    ParamUtil.matchesPattern(pattern, ((IHasName)resolution.elementPath[1].getCompoundModule()).getName()))
                return resolution.elementPath[1].getCompoundModule();
        }
        else if (param != null && param.getOwner() instanceof IHasName && ParamUtil.matchesPattern(pattern, ((IHasName)param.getOwner()).getName()))
            return param.getOwner();
        return null;
    }
    
    public static List<INedElement> findNamedElements(String pattern, ParamResolution[] resolutions) {
        Set<INedElement> elements = new LinkedHashSet<INedElement>();
        for (ParamResolution res : resolutions) {
            INedElement element = findNamedElement(pattern, res);
            if (element != null)
                elements.add(element);
        }
        return new ArrayList<INedElement>(elements);
    }

    /**
     * Returns the text displayed to disambiguate multiple target elements.
     */
    public static String getLinkText(INedElement element) {
        String text = null;
        INedTypeElement typeElement = null;
        
        if (element instanceof ParamElement) {
            ParamElement param = (ParamElement)element;
            typeElement = param.getEnclosingTypeElement();
            text = typeElement.getName() + "." + param.getName();
        }
        else if (element instanceof INedTypeElement) {
            typeElement = (INedTypeElement)element;
            text = typeElement.getName();
        }
        else if (element instanceof ISubmoduleOrConnection) {
            ISubmoduleOrConnection submodule = (ISubmoduleOrConnection)element;
            typeElement = submodule.getEnclosingTypeElement();
            if (submodule instanceof SubmoduleElement)
                text = typeElement.getName() + "." + ((SubmoduleElement)submodule).getName();
            else
                text = typeElement.getName();
        }
        else
            Assert.isTrue(false, "Element is: " + element);
        
        if (typeElement.getNedTypeInfo().isInnerType())
            text = typeElement.getEnclosingTypeElement().getName() + "." + text;
        
        return text;
    }
}
