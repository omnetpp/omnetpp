package org.omnetpp.inifile.editor.text.highlight;


import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.EndOfLineRule;
import org.eclipse.jface.text.rules.IPredicateRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.RuleBasedPartitionScanner;
import org.eclipse.jface.text.rules.Token;

/**
 * This scanner recognizes the different partitions needed for different syntax highlighting areas.
 */
public class InifileSyntaxHighlightPartitionScanner extends RuleBasedPartitionScanner {

    public final static String PARTITIONING_ID = "__ini_syntaxhighlight_partitioning";

    public final static String INI_COMMENT = "__ini_comment";
	public final static String[] SUPPORTED_PARTITION_TYPES = new String[] { IDocument.DEFAULT_CONTENT_TYPE, INI_COMMENT };

	/**
	 * Creates the partitioner and sets up the appropriate rules.
	 */
	public InifileSyntaxHighlightPartitionScanner() {
		// Add rule for single-line comments.
		//XXX problem: this also fires inside string constants... 
		IToken nedDocToken = new Token(INI_COMMENT);
		IPredicateRule commentRule = new EndOfLineRule("#", nedDocToken);
		setPredicateRules(new IPredicateRule[] { commentRule });
	}
}
