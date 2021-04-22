/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.highlight;


import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.EndOfLineRule;
import org.eclipse.jface.text.rules.IPredicateRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.RuleBasedPartitionScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.Token;
import org.omnetpp.inifile.editor.text.InifileTextEditorHelper;

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
        // Add rule for single-line comments. To prevent hashmarks inside string constants
        // from triggering the rule (and marking the rest of the string as comment), we
        // add a rule for string constants that fires BEFORE the comment rule.
        IToken commentToken = new Token(INI_COMMENT);
        IPredicateRule stringConstantRule = new SingleLineRule("\"", "\"", InifileTextEditorHelper.codeStringToken, '\\', true, true);
        IPredicateRule commentRule = new EndOfLineRule("#", commentToken, '\\', true);
        setPredicateRules(new IPredicateRule[] { stringConstantRule, commentRule });
    }
}
