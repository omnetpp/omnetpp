package org.omnetpp.ned.editor.text.outline;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.*;

/**
 * This scanner recognizes the different partitions needed to display the outline view
 */
public class NedOutlinePartitionScanner extends RuleBasedPartitionScanner {

    public final static String PARTITIONING_ID = "__ned_outline_partitioning";   //$NON-NLS-1$

    public final static String NED_PRIVATE_DOC = "__ned_privatedoc"; //$NON-NLS-1$
    public final static String NED_DOC = "__ned_doc"; //$NON-NLS-1$
    public final static String NED_STRING = "__ned_string"; //$NON-NLS-1$
    public final static String NED_IMPORT = "__ned_import"; //$NON-NLS-1$
    public final static String NED_CHANNEL = "__ned_channel"; //$NON-NLS-1$
    public final static String NED_SIMPLE = "__ned_simple"; //$NON-NLS-1$
    public final static String NED_MODULE = "__ned_module"; //$NON-NLS-1$
    public final static String NED_NETWORK = "__ned_network"; //$NON-NLS-1$
	public final static String[] SUPPORTED_PARTITION_TYPES 
        = new String[] { IDocument.DEFAULT_CONTENT_TYPE, NED_IMPORT, NED_CHANNEL, NED_SIMPLE, NED_MODULE, NED_NETWORK };

	/**
	 * Creates the partitioner and sets up the appropriate rules.
	 */
	public NedOutlinePartitionScanner() {
		super();

        IToken nedDocToken= new Token(NED_DOC);
        IToken nedPrivateDocToken= new Token(NED_PRIVATE_DOC);
        IToken nedStringToken= new Token(NED_STRING);
        IToken nedImportToken = new Token(NED_IMPORT);
        IToken nedChannelToken = new Token(NED_CHANNEL);
        IToken nedSimpleToken = new Token(NED_SIMPLE);
        IToken nedModuleToken = new Token(NED_MODULE);
        IToken nedNetworkToken = new Token(NED_NETWORK);

		List rules= new ArrayList();

        rules.add(new EndOfLineRule("//#", nedPrivateDocToken)); 
        rules.add(new EndOfLineRule("//", nedDocToken));
        // rules.add(new SingleLineRule("\"", "\"", nedStringToken, '\\'));
        rules.add(new SingleLineRule("\"", "\"", nedStringToken));

        rules.add(new MultiLineRule("import",";", nedImportToken));
        rules.add(new MultiLineRule("channel","endchannel", nedChannelToken));
        rules.add(new MultiLineRule("simple","endsimple", nedSimpleToken));
        rules.add(new MultiLineRule("module","endmodule", nedModuleToken));
        rules.add(new MultiLineRule("network","endnetwork", nedNetworkToken));

        IPredicateRule[] result= new IPredicateRule[rules.size()];
		rules.toArray(result);
		setPredicateRules(result);
	}
}
