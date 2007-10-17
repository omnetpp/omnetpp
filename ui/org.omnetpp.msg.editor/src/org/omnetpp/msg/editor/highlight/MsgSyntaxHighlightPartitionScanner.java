package org.omnetpp.msg.editor.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.*;

/**
 * This scanner recognizes the different partitions needed for different syntax highlighting areas.
 *
 * @author rhornig
 */
public class MsgSyntaxHighlightPartitionScanner extends RuleBasedPartitionScanner {

    public final static String PARTITIONING_ID = "__msg_syntaxhighlight_partitioning";   

    public final static String MSG_PRIVATE_DOC = "__msg_privatedoc"; 
    public final static String MSG_DOC = "__msg_doc"; 
	public final static String[] SUPPORTED_PARTITION_TYPES 
        = new String[] { IDocument.DEFAULT_CONTENT_TYPE, MSG_PRIVATE_DOC, MSG_DOC };

	/**
	 * Creates the partitioner and sets up the appropriate rules.
	 */
	public MsgSyntaxHighlightPartitionScanner() {
		super();

		IToken nedDocToken= new Token(MSG_DOC);
        IToken nedPrivateDocToken= new Token(MSG_PRIVATE_DOC);

		List<IRule> rules= new ArrayList<IRule>();

        // Add rule for single line private comments.
        rules.add(new EndOfLineRule("//#", nedPrivateDocToken)); 
		
        // Add rule for single line comments.
		rules.add(new EndOfLineRule("//", nedDocToken)); 

		setPredicateRules(rules.toArray(new IPredicateRule[]{}));
	}
}
