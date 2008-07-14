package org.omnetpp.msg.editor;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.engine.NED2Generator;
import org.omnetpp.ned.engine.NEDDTDValidator;
import org.omnetpp.ned.engine.NEDElement;
import org.omnetpp.ned.engine.NEDErrorStore;
import org.omnetpp.ned.engine.NEDParser;
import org.omnetpp.ned.engine.NEDSyntaxValidator;

/**
 * Parsing and manipulating MSG files
 * 
 * @author Andras
 */
public class MsgTreeUtil {

    public static boolean needsConversion(String string) {
        NEDElement node = parse(string);
        if (node == null)
            return false;
        boolean result = "1".equals(node.getAttribute("version"));
        node.delete();
        return result;
    }

    public static String convertToNewSyntax(String string) {
        NEDElement node = MsgTreeUtil.parse(string);
        if (node == null)
            throw new IllegalArgumentException();
        node.setAttribute("version", "2");
        String result = new NED2Generator(new NEDErrorStore()).generate(node, "");
        node.delete();
        return result;
    }

    public static NEDElement parse(String source) {
        NEDElement swigTree = null;
        try {
            // parse
            NEDErrorStore swigErrors = new NEDErrorStore();
            NEDParser np = new NEDParser(swigErrors);
            np.setParseExpressions(false);
            swigTree = np.parseMSGText(source, "buffer");
            if (swigTree == null)
                return null;
    
            if (!swigErrors.empty()) {
                swigTree.delete();
                return null;
            }
    
            // run DTD validation (once again)
            int numMessages = swigErrors.numMessages();
            NEDDTDValidator dtdvalidator = new NEDDTDValidator(swigErrors);
            dtdvalidator.validate(swigTree);
            Assert.isTrue(swigErrors.numMessages() == numMessages, "MSG tree produced by parser fails DTD validation");
    
            // additional syntax-related validation
            NEDSyntaxValidator syntaxValidator = new NEDSyntaxValidator(false, swigErrors);
            syntaxValidator.validate(swigTree);
            Assert.isTrue(swigErrors.numMessages() == numMessages, "MSG tree produced by parser fails syntax validation");
    
            return swigTree;
        }
        catch (Exception e) {
            if (swigTree != null)
                swigTree.delete();
            return null;
        }
    }

}
