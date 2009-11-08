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
        "assign", "attempt", "break", "call", "case", "comment", "compress", "default",
        "else", "elseif", "escape", "fallback", "flush", "foreach", "function", "global", "if", "import", "include", "list",
        "local", "lt", "macro", "nested", "noescape", "noparse", "nt", "recover", "recurse", "return", "rt", "setting",
        "stop", "switch", "t", "transform", "visit",
    };

    public static final String[] DIRECTIVE_ENDTAGS = {
        "assign", "attempt", "comment", "compress", "escape", "foreach", "function", "global", "if", "list",
        "local", "macro", "noescape", "noparse", "setting", "switch", "transform",
    };

    private FtlExpressionCompletionProcessor expressionCompletion = new FtlExpressionCompletionProcessor();

    @Override
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
            String [] directives = prefix.startsWith("/#") ? DIRECTIVE_ENDTAGS : DIRECTIVES;
            prefix = prefix.replaceFirst("^/?#", "");

            for (String directive : directives)
                if (directive.startsWith(prefix))
                    result.add(new CompletionProposal(directive, documentOffset - prefix.length(), prefix.length(), directive.length()));
        }

        // ask the expression completion class as well
        ICompletionProposal[] expressionProposals = expressionCompletion.computeCompletionProposals(viewer, documentOffset);
        if (expressionProposals != null)
            result.addAll(Arrays.asList(expressionProposals));
        
        return result.toArray(new ICompletionProposal[result.size()]);
    }

    @Override
    public IContextInformation[] computeContextInformation(ITextViewer viewer, int offset) {
        return null;
    }

    @Override
    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '#', '?' };
    }

    @Override
    public char[] getContextInformationAutoActivationCharacters() {
        return null;
    }

    @Override
    public IContextInformationValidator getContextInformationValidator() {
        return null;
    }

    @Override
    public String getErrorMessage() {
        return null;
    }

}
