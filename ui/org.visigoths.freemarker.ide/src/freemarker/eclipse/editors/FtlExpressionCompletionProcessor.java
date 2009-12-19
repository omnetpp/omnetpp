package freemarker.eclipse.editors;

import java.util.ArrayList;
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
 * Provides completion proposals inside FreeMarker expressions. 
 *
 * @author <a href="mailto:andras@omnetpp.org">Andras Varga</a>
 */
public class FtlExpressionCompletionProcessor implements IContentAssistProcessor {
    // copied out from freemarker.core.BuiltIn, because it has no public accessors  
    public static final String[] BUILTINS = {
            "ancestors", "byte", "c", "cap_first", "capitalize", "ceiling", "children",
            "chop_linebreak", "contains", "date", "datetime", "default", "double", "ends_with", "eval", "exists", "first",
            "float", "floor", "chunk", "has_content", "html", "if_exists", "index_of", "int", "interpret", "is_boolean",
            "is_collection", "is_date", "is_directive", "is_enumerable", "is_hash_ex", "is_hash", "is_indexable", "is_macro",
            "is_method", "is_node", "is_number", "is_sequence", "is_string", "is_transform", "j_string", "js_string", "keys",
            "last_index_of", "last", "left_pad", "length", "long", "lower_case", "namespace", "new", "node_name",
            "node_namespace", "node_type", "number", "parent", "replace", "reverse", "right_pad", "root", "round", "rtf",
            "seq_contains", "seq_index_of", "seq_last_index_of", "short", "size", "sort_by", "sort", "split", "starts_with",
            "string", "substring", "time", "trim", "uncap_first", "upper_case", "url", "values", "html", "word_list", "xhtml",
            "xml", "matches", "groups",
    };

    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {

        // get the current line, up to the cursor
        String linePrefix = "";
        try {
            IDocument doc = viewer.getDocument();
            int lineNumber = doc.getLineOfOffset(documentOffset);
            int lineStartOffset = doc.getLineOffset(lineNumber);
            linePrefix = doc.get(lineStartOffset, documentOffset - lineStartOffset);
        } catch (BadLocationException e) {
            FreemarkerPlugin.logError(e);
        }

        // offer names of builtins after a question mark
        int questionmarkPos = linePrefix.lastIndexOf('?');
        if (questionmarkPos == -1)
            return null;
        
        String prefix = linePrefix.substring(questionmarkPos+1);
        
        List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();
        
        for (String builtin : BUILTINS)
            if (builtin.startsWith(prefix))
                result.add(new CompletionProposal(builtin, documentOffset-prefix.length(), prefix.length(), builtin.length()));

        return result.toArray(new ICompletionProposal[result.size()]);
    }

    public IContextInformation[] computeContextInformation(ITextViewer viewer, int offset) {
        return null;
    }

    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '?' };
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
