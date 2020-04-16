/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.assist;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationPresenter;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.editor.text.Keywords;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver.IPredicate;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

// TODO completion within inner types
// TODO a better structure is needed for storing the completion proposals
// TODO context help can be supported, to show the documentation of the proposed keyword
// TODO F4 "Open Declaration"
// TODO completion for imports
/**
 * NED completion processor.
 *
 * @author andras
 */
public class AbstractNedCompletionProcessor extends NedTemplateCompletionProcessor {
    private static final String IDENT_REGEX = Keywords.NED_IDENT_REGEX;

    /**
     * Simple content assist tip closer. The tip is valid in a range
     * of 5 characters around its popup location.
     * TODO implement correctly the context information
     */
    protected static class Validator implements IContextInformationValidator, IContextInformationPresenter {
        protected int fInstallOffset;

        /*
         * @see IContextInformationValidator#isContextInformationValid(int)
         */
        public boolean isContextInformationValid(int offset) {
            return Math.abs(fInstallOffset - offset) < 5;
        }

        /*
         * @see IContextInformationValidator#install(IContextInformation, ITextViewer, int)
         */
        public void install(IContextInformation info, ITextViewer viewer, int offset) {
            fInstallOffset= offset;
        }

        /*
         * @see org.eclipse.jface.text.contentassist.IContextInformationPresenter#updatePresentation(int, TextPresentation)
         */
        public boolean updatePresentation(int documentPosition, TextPresentation presentation) {
            return false;
        }
    }

    // for CompletionInfo.sectionType
    protected static final int SECT_GLOBAL = 0;
    protected static final int SECT_PARAMETERS = 1;
    protected static final int SECT_GATES = 2;
    protected static final int SECT_TYPES = 3;
    protected static final int SECT_CONNECTIONS = 4;
    protected static final int SECT_CONNECTION_PARAMETERS = 5;
    protected static final int SECT_SUBMODULES = 6;
    protected static final int SECT_SUBMODULE_PARAMETERS = 7;
    protected static final int SECT_SUBMODULE_GATES = 8;

    protected static class CompletionInfo {
        public String linePrefix; // relevant line (lines) just before the insertion point
        public String linePrefixTrimmed; // like linePrefix, but last identifier (which the user is currently typing) chopped
        public String enclosingNedTypeName;
        public String nedTypeName;       // the type name
        public int sectionType; // SECT_xxx
        public String submoduleTypeName;
        public String connectionTypeName;
        public int parenthesisLevel;
    }

    protected IContextInformationValidator fValidator = new Validator();
    protected ITextEditor editor;

    public AbstractNedCompletionProcessor(ITextEditor editor) {
        this.editor = editor;
    }

    protected void addNedTypeProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result,
            IProject project, INedTypeInfo nedTypeInfoForInnerTypes, IPredicate predicate) {
        INedResources res = NedResourcesPlugin.getNedResources();
        // add inner types
        if (nedTypeInfoForInnerTypes != null) {
            Set<String> innerTypeNames = new HashSet<String>();
            for (INedTypeElement innerTypeElement : nedTypeInfoForInnerTypes.getInnerTypes().values()) {
                if (predicate.matches(innerTypeElement.getNedTypeInfo()))
                    innerTypeNames.add(innerTypeElement.getName());
            }
            addProposals(viewer, documentOffset, result, innerTypeNames, "inner type");
        }

        // add top level types
        // XXX offer "like" template too
        Set<String> qnames = res.getToplevelNedTypeQNames(predicate, project);
        String names[] = new String[qnames.size()];
        String descriptions[] = new String[qnames.size()];
        int i = 0;
        for (String qname : qnames) {
            INedTypeInfo topLevelTypeInfo = res.getToplevelNedType(qname, project);
            names[i] = topLevelTypeInfo.getName();
            String packageName = StringUtils.removeEnd(topLevelTypeInfo.getNamePrefix(), ".");
            packageName = StringUtils.isBlank(packageName) ? "" : packageName+" - ";
            descriptions[i] =  packageName + topLevelTypeInfo.getNedElement().getReadableTagName()+" type";
            i++;
        }
        addProposals(viewer, documentOffset, result, names, descriptions);
    }

    protected boolean containsOpenBracket(String line) {
        while (line.matches(".*\\[[^\\[\\]]*\\].*"))
            line = line.replaceAll("\\[[^\\[\\]]*\\]", "###");
        return line.contains("[");
    }

    protected String extractSubmoduleTypeName(String line, INedTypeInfo parentComponent) {
        // first, get rid of everything before any arrow(s), because it causes a problem for the next regexp
        line = line.replaceFirst("^.*(-->|<--|<-->)", "");
        // identifier followed by ".", potentially a submodule index ("[something]") in between
        Matcher matcher = Pattern.compile("(" + IDENT_REGEX + ") *(\\[[^\\[\\]]*\\])? *\\.$").matcher(line);
        if (matcher.find()) { // use find() because line may start with garbage
            String submoduleName = matcher.group(1);
            INedElement submodNode = parentComponent.getMembers().get(submoduleName);
            if (submodNode instanceof SubmoduleElement) {
                SubmoduleElement submod = (SubmoduleElement) submodNode;
                String submodTypeName = submod.getType();
                if (submodTypeName==null || submodTypeName.equals(""))
                    submodTypeName = submod.getLikeType();
                return submodTypeName;
            }
        }
        return null; // bad luck
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String[] descriptions) {
        result.addAll(createProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(), "", proposals, "", descriptions, null));
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String[] descriptions, Image[] images) {
        result.addAll(createProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(), "", proposals, "", descriptions, images));
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String description) {
        result.addAll(createProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(), "", proposals, "", description));
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
        result.addAll(createProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(), "", proposals.toArray(new String[] {}), "", description));
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String start, Set<String> proposals, String end, String description) {
        result.addAll(createProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(), start, proposals.toArray(new String[] {}), end, description));
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Template[] templates) {
        result.addAll(Arrays.asList(createTemplateProposals(viewer, documentOffset, new SyntaxHighlightHelper.NedWordDetector(), templates)));
    }

    protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Template[] templates, IWordDetector wordDetector) {
        result.addAll(Arrays.asList(createTemplateProposals(viewer, documentOffset, wordDetector, templates)));
    }

    protected CompletionInfo computeCompletionInfo(ITextViewer viewer, int documentOffset) {
        IDocument docu = viewer.getDocument();
        int offset = documentOffset;
        try {
            String source = docu.get(0,offset);
            // kill string literals
            source = source.replaceAll("\".*?\"", "\"###\"");  //FIXME but ignore embedded backslash+quote \" !!!
            // kill comments
            source = source.replaceAll("(?m)//.*", "");

            // completion prefix (linePrefix): stuff after last semicolon,
            // curly brace, "parameters:", "gates:", "connections:" etc.
            String prefix = source;
            // normalize whitespace
            prefix = prefix.replaceAll("(?s)\\s+", " ");
            // remove until the last brace
            prefix = prefix.replaceFirst(".*[\\{\\}]", "");
            // kill until the last good semicolon
            boolean quote = false;
            int parenthesisLevel = 0;
            int lastSemicolonIndex = -1;
            for (int i = 0; i < prefix.length(); i++) {
                char ch = prefix.charAt(i);
                if (ch == '"')
                    quote = !quote;
                else if (ch == '(')
                    parenthesisLevel++;
                else if (ch == ')')
                    parenthesisLevel--;
                else if (ch == ';' && !quote && parenthesisLevel == 0)
                    lastSemicolonIndex = i;
            }
            prefix = prefix.substring(lastSemicolonIndex + 1);
            // kill (...) regions
            while (prefix.matches(".*\\([^\\(\\)]*\\).*"))
                prefix = prefix.replaceAll("\\([^\\(\\)]*\\)", "###");
            prefix = prefix.replaceFirst(".*\\b(parameters|gates|types|submodules|connections|connections +[a-z]+) *:", "");
            String linePrefixTrimmed = prefix.replaceFirst("@?" + IDENT_REGEX + "$", "").trim(); // chop off last word

            // kill {...} regions (including bodies of inner types, etc)
            while (source.matches("(?s).*\\{[^\\{\\}]*\\}.*"))
                source = source.replaceAll("(?s)\\{[^\\{\\}]*\\}", "###");

            // detect if we are inside an inner type
            if (source.matches("(?s).*\\btypes\\b.*\\btypes\\b.*"))
                return null;      // inner types within inner types are not supported

            // handle inner types
            boolean insideInnertype = source.matches("(?s).*\\btypes\\b\\s*:[^:]*\\{.*");
            String enclosingNedTypeName = null;
            if (insideInnertype) {
                String sourceBeforeTypes = source.replaceFirst("(?s)^(.*)\\btypes\\s*:.*$", "$1");
                enclosingNedTypeName = sourceBeforeTypes.replaceFirst("(?s)^.*(simple|module|network|channel|interface|channelinterface)\\s+(" + IDENT_REGEX + ").*$", "$2");
                if (enclosingNedTypeName.equals(source))
                    enclosingNedTypeName = null;  // replace failed
                // use only the source after the type keyword (the inner type source)
                source = source.replaceFirst("(?s)^.*\\btypes\\s*:(.*)$", "$1");
            }

            // throw out the types section if it is closed (we are not in the types section)
            source = source.replaceAll("(?s)\\btypes\\b[^{]*?\\b(submodules|connections)\\b", "$1");

            // detect what section we are in
            int sectionType;
            if (source.matches("(?s).*\\bconnections\\b.*(-->|<--|<-->)[^;]*\\{.*"))
                sectionType = SECT_CONNECTION_PARAMETERS;
            else if (source.matches("(?s).*\\bconnections\\b.*"))
                sectionType = SECT_CONNECTIONS;
            else if (source.matches("(?s).*\\bsubmodules\\b.*\\bgates\\b.*"))
                sectionType = SECT_SUBMODULE_GATES;
            else if (source.matches("(?s).*\\bsubmodules\\b.*\\{.*"))
                sectionType = SECT_SUBMODULE_PARAMETERS;
            else if (source.matches("(?s).*\\bsubmodules\\b.*"))
                sectionType = SECT_SUBMODULES;
            else if (source.matches("(?s).*\\btypes\\b.*"))
                sectionType = SECT_TYPES;
            else if (source.matches("(?s).*\\bgates\\b.*"))
                sectionType = SECT_GATES;
            else if (source.matches("(?s).*\\{.*"))
                sectionType = SECT_PARAMETERS;
            else
                sectionType = SECT_GLOBAL;

            // detect module name: identifier after last "simple", "module", etc.
            String nedTypeName = source.replaceFirst("(?s)^.*\\b(simple|module|network|channel|interface|channelinterface)\\s+(" + IDENT_REGEX + ").*\\{.*$", "$2");
            if (nedTypeName.equals(source))
                nedTypeName = null;  // replace failed

            // detect submodule type: last occurrence of "identifier {"
            String lastTypeName = null;
            Matcher matcher = Pattern.compile("(?s).*[:\\s](" + IDENT_REGEX + ")\\s*\\{").matcher(source);
            if (matcher.lookingAt())
                lastTypeName = matcher.group(1);
            else
                lastTypeName = null;

//          Debug.println(">>>"+source+"<<<");
//          Debug.println("ENCLOSINGNEDTYPENAME:"+enclosingNedTypeName+"  NEDTYPENAME:"+nedTypeName+"  SECTIONTYPE:"+sectionType+"  SUBMODTYPENAME:"+submoduleTypeName);
//          Debug.println("PREFIX: >>"+prefix+"<<");
//          Debug.println("LINEPREFIXTRIMMED: >>"+linePrefixTrimmed+"<<");
//            Debug.println("inside inner type: "+insideInnertype);

            CompletionInfo ret = new CompletionInfo();
            ret.linePrefix = prefix;
            ret.linePrefixTrimmed = linePrefixTrimmed;
            ret.nedTypeName = nedTypeName;
            ret.enclosingNedTypeName = enclosingNedTypeName;
            ret.sectionType = sectionType;
            ret.submoduleTypeName = sectionType == SECT_SUBMODULE_GATES || sectionType == SECT_SUBMODULE_PARAMETERS ? lastTypeName : null;
            ret.connectionTypeName = sectionType == SECT_CONNECTION_PARAMETERS ? lastTypeName : null;
            ret.parenthesisLevel = parenthesisLevel;
            return ret;

        } catch (BadLocationException e) {
            return null;
        }
    }

    @Override
    public IContextInformation[] computeContextInformation(ITextViewer viewer, int documentOffset) {
        //XXX what the heck is this?
        //      IContextInformation[] result= new IContextInformation[5];
        //      for (int i= 0; i < result.length; i++)
        //          result[i]= new ContextInformation(
        //              MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.display.pattern"), new Object[] { new Integer(i), new Integer(documentOffset) }),
        //              MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.value.pattern"), new Object[] { new Integer(i), new Integer(documentOffset - 5), new Integer(documentOffset + 5)}));
        //      return result;
        return null;
    }

    @Override
    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '.', '@' };
    }

    @Override
    public char[] getContextInformationAutoActivationCharacters() {
        return new char[] { '(' };
    }

    @Override
    public IContextInformationValidator getContextInformationValidator() {
        return fValidator;
    }

    @Override
    public String getErrorMessage() {
        return null;
    }
}
