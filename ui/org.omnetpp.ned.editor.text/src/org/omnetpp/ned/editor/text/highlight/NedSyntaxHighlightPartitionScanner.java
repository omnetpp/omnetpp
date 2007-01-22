package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.*;

/**
 * This scanner recognizes the different partitions needed for different syntax highlighting areas.
 */
public class NedSyntaxHighlightPartitionScanner extends RuleBasedPartitionScanner {

    public final static String PARTITIONING_ID = "__ned_syntaxhighlight_partitioning";   //$NON-NLS-1$

    public final static String NED_PRIVATE_DOC = "__ned_privatedoc"; //$NON-NLS-1$
    public final static String NED_DOC = "__ned_doc"; //$NON-NLS-1$
	public final static String[] SUPPORTED_PARTITION_TYPES 
        = new String[] { IDocument.DEFAULT_CONTENT_TYPE, NED_PRIVATE_DOC, NED_DOC };

	/**
	 * Creates the partitioner and sets up the appropriate rules.
	 */
	public NedSyntaxHighlightPartitionScanner() {
		super();

		IToken nedDocToken= new Token(NED_DOC);
        IToken nedPrivateDocToken= new Token(NED_PRIVATE_DOC);

		List<IRule> rules= new ArrayList<IRule>();

        // Add rule for single line private comments.
        rules.add(new EndOfLineRule("//#", nedPrivateDocToken)); //$NON-NLS-1$
		
        // Add rule for single line comments.
		rules.add(new EndOfLineRule("//", nedDocToken)); //$NON-NLS-1$

		IPredicateRule[] result= new IPredicateRule[rules.size()];
		rules.toArray(result);
		setPredicateRules(result);
	}
}
