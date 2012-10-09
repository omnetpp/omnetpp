package freemarker.eclipse.editors;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.CompletionProposal;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContentAssistProcessor;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;

import freemarker.eclipse.FreemarkerPlugin;

/**
 * Provides completion proposals inside FreeMarker directives.
 *
 * @author <a href="mailto:andras@omnetpp.org">Andras Varga</a>
 */
public class FtlDirectiveCompletionProcessor implements IContentAssistProcessor {
    // directive names, copied out from FMParser.jj
    public static final String[] DIRECTIVES = {
        "assign", "attempt", "break", "case", "compress", "default", "else", "elseif",
        "escape", "fallback", "flush", "function", "global", "if", "import", "include",
        "list", "local", "lt", "macro", "nested", "noescape", "noparse", "nt", "recover",
        "recurse", "return", "rt", "setting", "stop", "switch", "t", "visit",
    };

    public static final String[] DIRECTIVE_ENDTAGS = {
        "assign", "attempt", "compress", "escape", "function", "global", "if",
        "list", "local", "macro", "noescape", "noparse", "setting", "switch",
    };

    public static final String[] DIRECTIVE_TEMPLATES = {
        "<#if {condition}> ... </#if>",
        "<#if {condition}> ... <#else> ... </#if>",
        "<#if {condition}> ... <#elseif {condition2}> ... <#else> ... </#if>",
        "<#switch {value}> <#case {refValue1}> ... <#break>  <#default> ... </#switch>",
        "<#list {sequence} as {item}> ... </#list>",
        "<#list {sequence} as {item}> ... <#if {condition}><#break></#if> ... </#list>",
        "<#include {path}>",
        "<#include {path} {options}>",
        "<#import {path} as {hash}>",
        "<#noparse> ... </#noparse>",
        "<#compress> ... </#compress>",
        "<#escape {identifier} as {expression}> ... </#escape>",
        "<#escape {identifier} as {expression}> ... <#noescape>...</#noescape> ... </#escape>",
        "<#assign {name}={value}>",
        "<#assign {name1}={value1}, {name2}={value2}>",
        "<#assign {name}> ... </#assign>",
        "<#assign {name}={value} in {namespacehash}>",
        "<#global {name}=value>",
        "<#global {name1}={value1}, {name2}={value2}>",
        "<#global {name}> ... </#assign>",
        "<#local {name}={value}>",
        "<#local {name1}={value1}, {name2}={value2}>",
        "<#local {name}> ... </#assign>",
        "<#setting {name}={value}>",
        "<#macro {name}> ... </#macro>",
        "<#macro {name} {param1} {param2}> ... <#nested {loopvar1}, {loopvar2}> ... <#return> ... </#macro>",
        "<#function {name} {param1} {param2}>  ...  <#return {value}> ... </#function>",
        "<#stop {message}>",
        "<#ftl {param1}={value1}>",
        "<#ftl {param1}={value1} {param2}={value2}>",
        "<#attempt> ... <#recover> ... </#attempt>",
        "<#visit {node}>",
        "<#visit {node} using {namespace}>",
        "<#recurse>",
        "<#recurse {node>}",
        "<#recurse {node} using {namespace}>",
    };

    private FtlExpressionCompletionProcessor expressionCompletion = new FtlExpressionCompletionProcessor();

    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();

        // get the current line, up to the cursor
        String linePrefix = "";
        try {
            IDocument doc = viewer.getDocument();
            int lineNumber = doc.getLineOfOffset(documentOffset);
            int lineStartOffset = doc.getLineOffset(lineNumber);
            linePrefix = doc.get(lineStartOffset, documentOffset - lineStartOffset);
        }
        catch (BadLocationException e) {
            FreemarkerPlugin.logError(e);
        }

        // offer names of directives after <#, [#, </# and [/#
        if (linePrefix.matches("^.*[<\\]](/?#).*")) {
            String prefix = linePrefix.replaceFirst("^.*[<\\]](/?#)", "$1");
            boolean startTag = !prefix.startsWith("/#");
            String [] directives = startTag ? DIRECTIVES : DIRECTIVE_ENDTAGS;
            prefix = prefix.replaceFirst("^/?#", "");

            for (String directive : directives)
                if (directive.startsWith(prefix))
                    result.add(new CompletionProposal(directive, documentOffset-prefix.length(), prefix.length(), directive.length()));

            // offer template proposals
            if (startTag) {
                for (String directive : DIRECTIVE_TEMPLATES) {
                    if (directive.startsWith("<#"+prefix)) {
                        // simplified solution; TODO use TemplateProposal
                        String offeredText = directive.replace("{", "").replace("}", "");
                        result.add(new CompletionProposal(offeredText, documentOffset-prefix.length()-2, prefix.length()+2, offeredText.length()));
                    }
                }
            }
        }

        // ask the expression completion class as well
        ICompletionProposal[] expressionProposals = expressionCompletion.computeCompletionProposals(viewer, documentOffset);
        if (expressionProposals != null)
            result.addAll(Arrays.asList(expressionProposals));

        return result.toArray(new ICompletionProposal[result.size()]);
    }

    public IContextInformation[] computeContextInformation(ITextViewer viewer, int offset) {
        return null;
    }

    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '#', '?' };
    }

    public char[] getContextInformationAutoActivationCharacters() {
        return null;
    }

    public IContextInformationValidator getContextInformationValidator() {
        return null;
    }

    public String getErrorMessage() {
        return null;
    }

}
