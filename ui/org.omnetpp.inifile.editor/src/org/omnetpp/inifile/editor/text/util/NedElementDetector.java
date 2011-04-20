package org.omnetpp.inifile.editor.text.util;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.Region;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.Pair;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.ITimeout;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.KeyType;
import org.omnetpp.inifile.editor.model.ParamCollector;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.PropertyResolution;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * Utility for finding NED elements at a given position of the ini file.
 */
public class NedElementDetector {

    private final IDocument textDoc;
    private final IReadonlyInifileDocument doc;
    private final InifileAnalyzer analyzer;
    private final INedTypeResolver nedResolver;
    
    
    public NedElementDetector(IReadonlyInifileDocument doc, InifileAnalyzer analyzer, IDocument textDoc, INedTypeResolver nedResolver) {
        this.textDoc = textDoc;
        this.doc = doc;
        this.analyzer = analyzer;
        this.nedResolver = nedResolver;
    }

    public Pair<IRegion, Set<INedElement>> detectNedElementsAtOffset(int offset) {
        Pair<IRegion, Set<INedElement>> result = null;
        String section = null, key = null;

        // identify section+key at offset
        try {
            int lineNumber = textDoc.getLineOfOffset(offset)+1;
            section = doc.getSectionForLine(lineNumber);
            key = doc.getKeyForLine(lineNumber);
        } catch (BadLocationException e) { }
        
        if (section != null && key != null) {
            switch (KeyType.getKeyType(key)) {
                case CONFIG: result = detectNedElementsInConfigLine(offset, section, key); break;
                case PARAM: result = detectNedElementsInParameterLine(offset, section, key); break;
                case PER_OBJECT_CONFIG: result = detectNedElementsInPerObjectConfigLine(offset, section, key); break;
            }
        }
        return result;
    }
    
    /**
     * Detect NED elements in global config settings.
     * It resolves the referenced network in the "network=..." config lines.
     */
    protected Pair<IRegion, Set<INedElement>> detectNedElementsInConfigLine(int offset, String section, String key)
    {
        try {
            // network key value hover
            if (key.equals(ConfigRegistry.CFGID_NETWORK.getName())) {
                String networkName = doc.getValue(section, key);
                INedTypeInfo network = ParamCollector.resolveNetwork(doc, nedResolver, networkName);
                if (network != null) {
                    int lineNumber = textDoc.getLineOfOffset(offset)+1;
    
                    IRegion linkRegion = getValueRegion(textDoc, lineNumber);
                    if (contains(linkRegion, offset)) {
                        Set<INedElement> elements = new HashSet<INedElement>();
                        elements.add(network.getNedElement());
                        return Pair.pair(linkRegion, elements);
                    }
                }
            }
        } catch (BadLocationException e) {
        }
        
        return null;
    }
    
    /**
     * Detect NED elements in parameter setting lines.
     * It resolves module and parameter names in "<module>.<param> = ..." lines.
     */
    protected Pair<IRegion, Set<INedElement>> detectNedElementsInParameterLine(int offset, String section, String key)
    {
        try {
            int lineNumber = textDoc.getLineOfOffset(offset)+1;
    
            IRegion keyRegion = getKeyRegion(textDoc, lineNumber);
            IRegion linkRegion = getLinkRegion(textDoc, keyRegion, offset);
            String pattern = getFullNameToBeMatched(textDoc, keyRegion, offset);
            if (linkRegion != null && pattern != null) {
                ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key, new Timeout(InifileEditor.HYPERLINKDETECTOR_TIMEOUT));
                PatternMatcher fullNameMatcher = new PatternMatcher(pattern, true, true, true);
                Set<INedElement> elements = new HashSet<INedElement>();
                collectMatchingNedElementsFromParamResolutions(fullNameMatcher, resList, elements);
                if (elements.isEmpty())
                    collectMatchingModules(fullNameMatcher, section, elements);
                return elements.isEmpty() ? null : Pair.pair(linkRegion, elements);
            }
        } catch (BadLocationException e) { }
          catch (ParamResolutionDisabledException e) { /* no hyperlinks if param resolution is disabled */ }
          catch (ParamResolutionTimeoutException e) {  /* no hyperlinks if param resolution timed out */ }
        return null;
    }
    
    /**
     * Detect NED elements in per-object setting lines.
     * Resolves module, parameter and statistic names in the following lines:
     *   "<module>.<config> = ..."
     *   "<module>.<param>.<config> = ..."
     *   "<module>.<statistic>.<config> = ..."
     *   "<module>.<scalar>.<config> = ..."
     *   "<module>.<vector>.<config> = ..."
     */
    protected Pair<IRegion, Set<INedElement>> detectNedElementsInPerObjectConfigLine(int offset, String section, String key) {
        try {
            int lineNumber = textDoc.getLineOfOffset(offset)+1;
    
            IRegion keyRegion = getKeyRegion(textDoc, lineNumber);
            IRegion linkRegion = getLinkRegion(textDoc, keyRegion, offset);
            
            // cut off .<config> part
            int index = key.lastIndexOf('.');
            String pattern = key.substring(0, index);
    
            if (linkRegion != null) {
                ITimeout timeout = new Timeout(InifileEditor.HYPERLINKDETECTOR_TIMEOUT);
                PropertyResolution[] resList = analyzer.getPropertyResolutions("statistic", pattern, section, timeout);
                String fullNamePattern = getFullNameToBeMatched(textDoc, keyRegion, offset);
                PatternMatcher fullNameMatcher = new PatternMatcher(fullNamePattern, true, true, true);
                Set<INedElement> elements = new HashSet<INedElement>();
                collectMatchingNedElementsFromPropertyResolutions(fullNameMatcher, resList, elements);
                if (elements.isEmpty()) {
                    ParamResolution[] paramResolutions = analyzer.getParamResolutionsForKey(section, key, timeout);
                    collectMatchingNedElementsFromParamResolutions(fullNameMatcher, paramResolutions, elements);
                    if (elements.isEmpty())
                        collectMatchingModules(fullNameMatcher, section, elements);
                }
                return elements.isEmpty() ? null : Pair.pair(linkRegion, elements);
            }
        } catch (BadLocationException e) { }
          catch (ParamResolutionDisabledException e) { /* no hyperlinks if param resolution is disabled */ }
          catch (ParamResolutionTimeoutException e) {  /* no hyperlinks if param resolution timed out */ }
        return null;
    }
    
    private static void collectMatchingNedElementsFromParamResolutions(PatternMatcher fullNameMatcher, ParamResolution[] paramResolutions, Set<INedElement> result) {
        for (ParamResolution paramResolution : paramResolutions)
            collectMatchingNedElementsFromParamResolution(fullNameMatcher, paramResolution, result);
    }
    
    private static void collectMatchingNedElementsFromParamResolution(PatternMatcher fullNameMatcher, ParamResolution paramResolution, Set<INedElement> result) {
        String paramFullPath = paramResolution.fullPath + "." + paramResolution.paramDeclaration.getName();
        if (fullNameMatcher.matches(paramFullPath))
            result.add(paramResolution.paramDeclaration);
        INedElement network = getNetworkElement(paramResolution);
        if ((network instanceof IHasName) && fullNameMatcher.matches(((IHasName)network).getName()))
            result.add(network);
        collectMatchingNedElementsFromElementPath(fullNameMatcher, paramResolution.elementPath, paramResolution.fullPath, result);
    }
    
    private static void collectMatchingNedElementsFromPropertyResolutions(PatternMatcher fullNameMatcher, PropertyResolution[] propertyResolutions, Set<INedElement> result) {
        for (PropertyResolution propertyResolution : propertyResolutions)
            collectMatchingNedElementsFromPropertyResolution(fullNameMatcher, propertyResolution, result);
    }
    
    private static void collectMatchingNedElementsFromPropertyResolution(PatternMatcher fullNameMatcher, PropertyResolution propertyResolution, Set<INedElement> result) {
        if (fullNameMatcher.matches(propertyResolution.fullPath))
            result.add(propertyResolution.propertyDeclaration);
        INedElement network = getNetworkElement(propertyResolution);
        if ((network instanceof IHasName) && fullNameMatcher.matches(((IHasName)network).getName()))
            result.add(network);
        String moduleFullPath = propertyResolution.fullPath.substring(0, Math.max(propertyResolution.fullPath.lastIndexOf('.'), 0));
        collectMatchingNedElementsFromElementPath(fullNameMatcher, propertyResolution.elementPath, moduleFullPath, result);
    }
    
    private static void collectMatchingNedElementsFromElementPath(PatternMatcher fullNameMatcher, ISubmoduleOrConnection[] elementPath, String fullPath, Set<INedElement> result) {
        for (int i = elementPath.length - 1; i >= 0; --i) {
            ISubmoduleOrConnection element = elementPath[i];
            if ((element instanceof IHasName) && fullNameMatcher.matches(fullPath))
                result.add(element);
            fullPath = fullPath.substring(0, Math.max(fullPath.lastIndexOf('.'), 0));
        }
    }
    
    private void collectMatchingModules(PatternMatcher fullNameMatcher, String section, Set<INedElement> elements) {
        Map<String,ISubmoduleOrConnection> modules = ParamCollector.collectModules(doc, section, fullNameMatcher, nedResolver, null);
        elements.addAll(modules.values());
    }
    
    private static INedElement getNetworkElement(ParamResolution resolution) {
        // either the parent of the first element in elementPath
        if (resolution.elementPath.length >= 2 && 
            resolution.elementPath[1] != null &&
            resolution.elementPath[1].getCompoundModule() != null)
                return resolution.elementPath[1].getCompoundModule();
        // or the parent of the param element
        ParamElementEx param = resolution.paramAssignment!=null ? resolution.paramAssignment : resolution.paramDeclaration;
        return param.getOwner();
    }
    
    private static INedElement getNetworkElement(PropertyResolution resolution) {
        // either the parent of the first element in elementPath
        if (resolution.elementPath.length >= 2 && 
            resolution.elementPath[1] != null &&
            resolution.elementPath[1].getCompoundModule() != null)
                return resolution.elementPath[1].getCompoundModule();
        // or the parent of the property element
        return null; //resolution.propertyDeclaration.getOwner() ?
    }
    
    /**
     * Get position of the key within the line.
     */
    private static IRegion getKeyRegion(IDocument textDoc, int lineNumber) throws BadLocationException {
        int lineOffset = textDoc.getLineOffset(lineNumber-1);
        int nextLineOffset = textDoc.getLineOffset(lineNumber);
        String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
        int eqPos = lineContents.indexOf('=');
        Region region = new Region(lineOffset, eqPos);
        return trimRegion(textDoc, region);
    }

    /**
     * Returns the region which should be formatted as a link, when the cursor is at {@code offset}.
     */
    private static IRegion getLinkRegion(IDocument textDoc, IRegion keyRegion, int offset) throws BadLocationException {
        if (!contains(keyRegion, offset))
            return null;
        int start = offset;
        while (start >= keyRegion.getOffset() && textDoc.getChar(start) != '.')
            --start;
        int end = offset;
        while (end < keyRegion.getOffset() + keyRegion.getLength() && textDoc.getChar(end) != '.')
            ++end;
        return end > start ? new Region(start+1, end-start-1) : null;
    }
    
    /**
     * Returns that part of key which is to be matched when the cursor is at offset in textDoc.
     * Example:
     *   getOwnerFromKey("aaa.bbb.ccc", 9)  -> "aaa.bbb.ccc"
     *   getOwnerFromKey("aaa.bbb.ccc", 5)  -> "aaa.bbb"
     * 
     * @param textDoc the ini file document
     * @param keyRegion the region in the document containing the key
     * @param offset an offset inside the key
     */
    private static String getFullNameToBeMatched(IDocument textDoc, IRegion keyRegion, int offset) throws BadLocationException {
        if (!contains(keyRegion, offset))
            return null;
        int keyStart = keyRegion.getOffset();
        int keyEnd = keyStart + keyRegion.getLength();
        int end = offset;
        while (end < keyEnd && textDoc.getChar(end) != '.')
            ++end;
        return textDoc.get(keyStart, end-keyStart);
    }
    
    /**
     * Get the position of the value within the line.
     */
    private static IRegion getValueRegion(IDocument textDoc, int lineNumber) throws BadLocationException {
        int lineOffset = textDoc.getLineOffset(lineNumber-1);
        int nextLineOffset = textDoc.getLineOffset(lineNumber);
        String lineContents = textDoc.get(lineOffset, nextLineOffset - lineOffset);
        int eqPos = lineContents.indexOf('=');
        int hashmarkPos = lineContents.indexOf('#');
        Region region = new Region(lineOffset + eqPos + 1, hashmarkPos == -1 ? nextLineOffset-lineOffset-eqPos - 1 - 1 : hashmarkPos - eqPos -1);
        return trimRegion(textDoc, region);
    }

    private static IRegion trimRegion(IDocument textDoc, IRegion r) throws BadLocationException {
        int begin = r.getOffset();
        int end = begin + r.getLength();
        while(Character.isWhitespace(textDoc.getChar(begin)) && begin < end)
            begin++;
        while(Character.isWhitespace(textDoc.getChar(end-1)) && begin < end)
            end--;
        return new Region(begin, end-begin);
    }
    
    private static boolean contains(IRegion reg, int offset) {
        return offset >= reg.getOffset() && offset < reg.getOffset() + reg.getLength();
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
        else if (element instanceof PropertyElement) {
            PropertyElement prop = (PropertyElement)element;
            typeElement = prop.getEnclosingTypeElement();
            text = typeElement.getName() + "." + prop.getName();
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
