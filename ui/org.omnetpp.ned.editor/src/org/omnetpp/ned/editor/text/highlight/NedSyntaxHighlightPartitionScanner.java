/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.highlight;


import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.EndOfLineRule;
import org.eclipse.jface.text.rules.IPredicateRule;
import org.eclipse.jface.text.rules.RuleBasedPartitionScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.Token;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;

/**
 * This scanner recognizes the different partitions needed for different syntax highlighting areas.
 *
 * @author rhornig
 */
public class NedSyntaxHighlightPartitionScanner extends RuleBasedPartitionScanner {

    public final static String PARTITIONING_ID = "__ned_syntaxhighlight_partitioning";

    public final static String NED_PRIVATE_DOC = "__ned_privatedoc";
    public final static String NED_DOC = "__ned_doc";
    public final static String[] SUPPORTED_PARTITION_TYPES
        = new String[] { IDocument.DEFAULT_CONTENT_TYPE, NED_PRIVATE_DOC, NED_DOC };

    /**
     * Creates the partitioner and sets up the appropriate rules.
     */
    public NedSyntaxHighlightPartitionScanner() {
        super();

        // Add rule for single-line comments. To prevent "//" inside string constants
        // from triggering the rule (and marking the rest of the string as comment), we
        // add a rule for string constants that fires BEFORE the comment rule.
        IPredicateRule stringConstantRule = new SingleLineRule("\"", "\"", SyntaxHighlightHelper.codeStringToken, '\\');
        IPredicateRule privateCommentRule = new EndOfLineRule("//#", new Token(NED_PRIVATE_DOC));
        IPredicateRule commentRule = new EndOfLineRule("//", new Token(NED_DOC));

        setPredicateRules(new IPredicateRule[] { stringConstantRule, privateCommentRule, commentRule });
    }
}
