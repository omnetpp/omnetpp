/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.*;

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

        IToken nedDocToken= new Token(NED_DOC);
        IToken nedPrivateDocToken= new Token(NED_PRIVATE_DOC);

        List<IRule> rules= new ArrayList<IRule>();

        // Add rule for single line private comments.
        rules.add(new EndOfLineRule("//#", nedPrivateDocToken));

        // Add rule for single line comments.
        rules.add(new EndOfLineRule("//", nedDocToken));

        setPredicateRules(rules.toArray(new IPredicateRule[]{}));
    }
}
