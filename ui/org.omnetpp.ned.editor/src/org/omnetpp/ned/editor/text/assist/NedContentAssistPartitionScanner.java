/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.assist;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.*;

/**
 * This scanner recognizes the different partitions needed for different content assists.
 *
 * @author rhornig
 */
public class NedContentAssistPartitionScanner extends RuleBasedPartitionScanner {

//    private static NedOutlinePartitionScanner fgInstance;

    public final static String PARTITIONING_ID = "__ned_contentassist_partitioning";

    public final static String NED_PRIVATE_DOC = "__ned_privatedoc";
    public final static String NED_DOC = "__ned_doc";
    public final static String NED_CODE = "__ned_code";
    public final static String NED_STRING = "__ned_string";
	public final static String[] SUPPORTED_PARTITION_TYPES
        = new String[] { IDocument.DEFAULT_CONTENT_TYPE, NED_CODE, NED_PRIVATE_DOC, NED_DOC , NED_STRING};

	/**
	 * Creates the partitioner and sets up the appropriate rules.
	 */
	public NedContentAssistPartitionScanner() {
		super();

		IToken nedDocToken= new Token(NED_DOC);
        IToken nedPrivateDocToken= new Token(NED_PRIVATE_DOC);
        //IToken nedCodeToken= new Token(NED_CODE);
        IToken nedStringToken= new Token(NED_STRING);

		List<IPredicateRule> rules= new ArrayList<IPredicateRule>();

        // Add rule for single line private comments.
        rules.add(new EndOfLineRule("//#", nedPrivateDocToken));

        // Add rule for single line comments.
		rules.add(new EndOfLineRule("//", nedDocToken));

		// Add rule for strings constants.
        // currently the escape char is not implemented in the parser
		// rules.add(new SingleLineRule("\"", "\"", nedStringToken, '\\'));
        rules.add(new SingleLineRule("\"", "\"", nedStringToken));

		IPredicateRule[] result= new IPredicateRule[rules.size()];
		rules.toArray(result);
		setPredicateRules(result);
	}
}
