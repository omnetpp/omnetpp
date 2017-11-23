/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.msg.editor;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.engine.ASTNode;
import org.omnetpp.ned.engine.ErrorStore;
import org.omnetpp.ned.engine.MsgDtdValidator;
import org.omnetpp.ned.engine.MsgGenerator;
import org.omnetpp.ned.engine.MsgParser;

/**
 * Parsing and manipulating MSG files
 *
 * @author Andras
 */
public class MsgTreeUtil {

    public static boolean needsConversion(String string) {
        ASTNode node = parse(string);
        if (node == null)
            return false;
        boolean result = "1".equals(node.getAttribute("version"));
        node.delete();
        return result;
    }

    public static String convertToNewSyntax(String string) {
        ASTNode node = MsgTreeUtil.parse(string);
        if (node == null)
            throw new IllegalArgumentException();
        node.setAttribute("version", "2");
        String result = new MsgGenerator().generate(node, "");
        node.delete();
        return result;
    }

    public static ASTNode parse(String source) {
        ASTNode swigTree = null;
        try {
            // parse
            ErrorStore swigErrors = new ErrorStore();
            MsgParser np = new MsgParser(swigErrors);
            swigTree = np.parseMsgText(source, "buffer");
            if (swigTree == null)
                return null;

            if (!swigErrors.empty()) {
                swigTree.delete();
                return null;
            }

            // run DTD validation (once again)
            int numMessages = swigErrors.numMessages();
            MsgDtdValidator dtdvalidator = new MsgDtdValidator(swigErrors);
            dtdvalidator.validate(swigTree);
            Assert.isTrue(swigErrors.numMessages() == numMessages, "MSG tree produced by parser fails DTD validation");

            return swigTree;
        }
        catch (Exception e) {
            if (swigTree != null)
                swigTree.delete();
            return null;
        }
    }

}
