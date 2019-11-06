/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.engine.ASTNode;
import org.omnetpp.ned.engine.ErrorStore;
import org.omnetpp.ned.engine.MsgAstNodeFactory;
import org.omnetpp.ned.engine.MsgDtdValidator;
import org.omnetpp.ned.engine.MsgGenerator;
import org.omnetpp.ned.engine.MsgParser;
import org.omnetpp.ned.engine.MsgTagCode;
import org.omnetpp.ned.engine.NedAstNodeFactory;
import org.omnetpp.ned.engine.NedDtdValidator;
import org.omnetpp.ned.engine.NedGenerator;
import org.omnetpp.ned.engine.NedParser;
import org.omnetpp.ned.engine.NedSyntaxValidator;
import org.omnetpp.ned.engine.NedTools;
import org.omnetpp.ned.engine.ProblemSeverity;
import org.omnetpp.ned.engine.SourceRegion;
import org.omnetpp.ned.engine.nedxml;
import org.omnetpp.ned.model.ex.MsgFileElementEx;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.pojo.ClassDeclElement;
import org.omnetpp.ned.model.pojo.ClassElement;
import org.omnetpp.ned.model.pojo.CplusplusElement;
import org.omnetpp.ned.model.pojo.EnumDeclElement;
import org.omnetpp.ned.model.pojo.EnumElement;
import org.omnetpp.ned.model.pojo.EnumFieldElement;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.MessageDeclElement;
import org.omnetpp.ned.model.pojo.MessageElement;
import org.omnetpp.ned.model.pojo.MsgFileElement;
import org.omnetpp.ned.model.pojo.NamespaceElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.PacketDeclElement;
import org.omnetpp.ned.model.pojo.PacketElement;
import org.omnetpp.ned.model.pojo.StructDeclElement;
import org.omnetpp.ned.model.pojo.StructElement;

/**
 * Utility functions working on NedElement trees. Conversions, serialization, dumping of trees.
 *
 * @author rhornig, andras
 */
public class NedTreeUtil {
    /**
     * Generate NED code from the given NedElement tree. The root node
     * does not have to be NedFileElement, any subtree can be converted
     * to source form.
     *
     * @param keepSyntax if set, sources parsed in old syntax (NED-1) will be generated in old syntax as well
     */
    public static String generateSource(INedElement treeRoot, boolean keepSyntax) {
        // Debug.println(generateXmlFromPojoElementTree(treeRoot,""));

        ErrorStore errors = new ErrorStore();
        errors.setPrintToStderr(false); // turn it on for debugging
        ASTNode swigTree = pojo2swig(treeRoot);
        String result = !isMsgElement(treeRoot)
                ? new NedGenerator().generate(swigTree, "")
                : new MsgGenerator().generate(swigTree, "");
        // TODO check ErrorStore for conversion errors
        swigTree.delete();
        return result;
    }

    protected static boolean isMsgElement(INedElement node) {
        return
                node instanceof MsgFileElement ||
                node instanceof ImportElement ||
                node instanceof NamespaceElement ||
                node instanceof CplusplusElement ||
                node instanceof StructDeclElement ||
                node instanceof ClassDeclElement ||
                node instanceof MessageDeclElement ||
                node instanceof PacketDeclElement ||
                node instanceof EnumDeclElement ||
                node instanceof EnumElement ||
                node instanceof EnumFieldElement ||
                node instanceof MessageElement ||
                node instanceof PacketElement ||
                node instanceof ClassElement ||
                node instanceof StructElement ||
                node instanceof FieldElement;
    }

    /**
     * Generate NEDXML from the given element tree.
     */
    public static String generateXML(INedElement tree, boolean srcLoc, int indentSize) {
        ASTNode swigTree = pojo2swig(tree);
        String result = nedxml.generateXML(swigTree, srcLoc, indentSize);
        swigTree.delete();
        return result;
    }

    /**
     * Load and parse NED file to a NedElement tree. Returns a non-null,
     * DTD-conforming (but possibly incomplete) tree even in case of parse errors.
     * The passed displayFileName will only be used to fill in the NedFileElement element
     * and source location attributes. Callers should check ErrorStore to determine
     * whether a parse error occurred. All errors produced here will be syntax errors
     * (see NEDSYNTAXPROBLEM_MARKERID).
     */
    public static synchronized NedFileElementEx parseNedFile(String filesystemFilename, INedErrorStore errors, String displayFilename, INedTypeResolver resolver) {
        return doParseNedSource(null, filesystemFilename, errors, displayFilename, resolver);
    }

    /**
     * Parse the NED source and return it as a NedElement tree. Returns a non-null,
     * DTD-conforming (but possibly incomplete) tree even in case of parse errors.
     * The passed displayFileName will only be used to fill in the NedFileElement element
     * and source location attributes. Callers should check INedErrorStore to determine
     * whether a parse error occurred. All errors produced here will be syntax errors
     * (see NEDSYNTAXPROBLEM_MARKERID).
     */
    public static synchronized NedFileElementEx parseNedText(String source, INedErrorStore errors, String displayFilename, INedTypeResolver resolver) {
        return doParseNedSource(source, null, errors, displayFilename, resolver);
    }

    private static synchronized NedFileElementEx doParseNedSource(String source, String filesystemFilename, INedErrorStore errors, String displayFilename, INedTypeResolver resolver) {
        Assert.isTrue(displayFilename != null);
        ASTNode swigTree = null;
        try {
            // Debug.println("Parsing NED file started: " + filesystemFilename);
            // parse
            ErrorStore swigErrors = new ErrorStore();
            NedParser np = new NedParser(swigErrors);
            swigTree = source!=null ? np.parseNedText(source, displayFilename) : np.parseNedFile(filesystemFilename, displayFilename);
            if (swigTree == null) {
                // return an empty NedFileElement if parsing totally failed
                NedFileElementEx fileNode = (NedFileElementEx)NedElementFactoryEx.getInstance().createElement(resolver, NedElementTags.NED_NED_FILE, null);
                fileNode.setFilename(displayFilename);
                copyGlobalErrors(swigErrors, fileNode, errors); // piggyback errors which came without context node onto the tree root
                return fileNode;
            }

            if (!swigErrors.empty()) {
                // There were parse errors, and the tree built may not be entirely correct.
                // Typical problems are "mandatory attribute missing" especially with connections,
                // due to parse errors before filling in the connection element was completed.
                // Here we try to check and repair the tree by discarding elements that cause
                // DTD validation error.
                NedTools.repairNedAST(swigTree);
            }

            // run DTD validation (once again)
            int numMessages = swigErrors.numMessages();
            NedDtdValidator dtdvalidator = new NedDtdValidator(swigErrors);
            dtdvalidator.validate(swigTree);
            dumpSwigErrors(swigErrors); //XXX remove -- debugging only

            Assert.isTrue(swigErrors.numMessages() == numMessages, "NED tree fails DTD validation, even after repairs");

            // additional syntax-related validation
            NedSyntaxValidator syntaxValidator = new NedSyntaxValidator(swigErrors);
            syntaxValidator.validate(swigTree);

            // convert tree to pure Java objects
            INedElement pojoTree = swig2pojo(swigTree, null, swigErrors, errors, resolver);
            Assert.isTrue(swigErrors.numMessages() == errors.getNumProblems(), "problems lost in translation");

            // Debug.println(generateXmlFromPojoElementTree(pojoTree, ""));

            return (NedFileElementEx)pojoTree;
        }
        finally {
            if (swigTree != null)
                swigTree.delete();

            // Debug.println("Parsing NED file finished: " + filesystemFilename);
        }
    }

    public static synchronized MsgFileElementEx parseMsgSource(String source, INedErrorStore errors, String filename) {
        Assert.isTrue(filename != null);
        ASTNode swigTree = null;
        try {
            // Debug.println("Parsing MSG file started: " + filename);
            // parse
            ErrorStore swigErrors = new ErrorStore();
            MsgParser np = new MsgParser(swigErrors);
            np.setMsgNewSyntaxFlag(true); //TODO configurable?
            swigTree = source!=null ? np.parseMsgText(source, filename) : np.parseMsgFile(filename);
            if (swigTree == null) {
                // return an empty MsgFileElement if parsing totally failed
                MsgFileElementEx fileNode = (MsgFileElementEx)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_MSG_FILE, null);
                fileNode.setFilename(filename);
                copyGlobalErrors(swigErrors, fileNode, errors); // piggyback errors which came without context node onto the tree root
                return fileNode;
            }

            // set the file name property in the nedFileElement
            if (swigTree.getTagCode() == MsgTagCode.MSG_MSG_FILE.swigValue())
                swigTree.setAttribute("filename", filename);

            if (!swigErrors.empty()) {
                // There were parse errors, and the tree built may not be entirely correct.
                // Typical problems are "mandatory attribute missing".
                // Here we try to check and repair the tree by discarding elements that cause
                // DTD validation error.
                NedTools.repairMsgAST(swigTree);
            }

            // run DTD validation (once again)
            int numMessages = swigErrors.numMessages();
            MsgDtdValidator dtdvalidator = new MsgDtdValidator(swigErrors);
            dtdvalidator.validate(swigTree);
            dumpSwigErrors(swigErrors); //XXX remove -- debugging only
            Assert.isTrue(swigErrors.numMessages() == numMessages, "MSG tree fails DTD validation, even after repairs");

            // convert tree to pure Java objects
            INedElement pojoTree = swig2pojo(swigTree, null, swigErrors, errors, null);
            Assert.isTrue(swigErrors.numMessages() == errors.getNumProblems(), "problems lost in translation");

            // Debug.println(generateXmlFromPojoElementTree(pojoTree, ""));

            return (MsgFileElementEx)pojoTree;
        }
        finally {
            if (swigTree != null)
                swigTree.delete();

            // Debug.println("Parsing MSG file finished: " + filename);
        }
    }

    protected static void dumpSwigErrors(ErrorStore swigErrors) {
        int n = swigErrors.numMessages();
        if (n > 0) {
            Debug.println(n + " errors:");
            for (int i=0 ; i<n ; i++) {
                ASTNode context = swigErrors.errorContext(i);
                Debug.println(
                        swigErrors.errorText(i)+
                        " loc: "+swigErrors.errorLocation(i) +
                        " context: " + (context==null ? "" : "<"+context.getTagName()+"> at "+context.getSourceLocation()));
                if (context!=null && context.getParent()!=null)
                    Debug.println(generateXmlFromSwigElementTree(context.getParent(), "  "));
                else if (context!=null)
                    Debug.println(generateXmlFromSwigElementTree(context, "  "));
            }
        }
    }

    /**
     * Converts a native C++ (SWIG-wrapped) NedElement tree to a plain java tree.
     * NOTE: There are two different NedElement types handled in this function.
     */
    public static INedElement swig2pojo(ASTNode swigNode, INedElement parent, ErrorStore swigErrors, INedErrorStore errors, INedTypeResolver resolver /*null for msgs*/) {
        // convert tree
        INedElement pojoTree = doSwig2pojo(swigNode, parent, swigErrors, errors, resolver);

        // piggyback errors which came without context node onto the tree root
        copyGlobalErrors(swigErrors, pojoTree, errors);

        return pojoTree;
    }

    protected static INedElement doSwig2pojo(ASTNode swigNode, INedElement parent, ErrorStore swigErrors, INedErrorStore errors, INedTypeResolver resolver /*null for msgs*/) {
        INedElement pojoNode = NedElementFactoryEx.getInstance().createElement(resolver, swigNode.getTagName(), parent); //TODO by code, not by name!

        // set the attributes
        int numAttributes = swigNode.getNumAttributes();
        for (int i = 0; i < numAttributes; ++i)
            pojoNode.setAttribute(i, swigNode.getAttribute(i));

        // copy source line number info
        pojoNode.setSourceLocation(swigNode.getSourceLocation());
        SourceRegion swigRegion = swigNode.getSourceRegion();
        if (swigRegion.getStartLine() != 0)
            pojoNode.setSourceRegion(new org.omnetpp.ned.model.NedSourceRegion(
                    swigRegion.getStartLine(), swigRegion.getStartColumn(),
                    swigRegion.getEndLine(), swigRegion.getEndColumn()));

        // take over error messages related to this node
        if (swigErrors != null && swigErrors.findFirstErrorFor(swigNode, 0) != -1) {
            int i = -1;
            while ((i = swigErrors.findFirstErrorFor(swigNode, i+1)) != -1) {
                int severity = getMarkerSeverityFor(ProblemSeverity.swigToEnum(swigErrors.errorSeverityCode(i)));
                errors.add(severity, pojoNode, getLineFrom(swigErrors.errorLocation(i)), swigErrors.errorText(i));
            }
        }

        // create child nodes
        for (ASTNode child = swigNode.getFirstChild(); child != null; child = child.getNextSibling())
            doSwig2pojo(child, pojoNode, swigErrors, errors, resolver);

        return pojoNode;
    }

    protected static void copyGlobalErrors(ErrorStore swigErrors, INedElement targetElement, INedErrorStore targetErrorStore) {
        // piggyback errors which came without context node onto the given element
        if (swigErrors != null && swigErrors.findFirstErrorFor(null, 0) != -1) {
            int i = -1;
            while ((i = swigErrors.findFirstErrorFor(null, i+1)) != -1) {
                int severity = getMarkerSeverityFor(ProblemSeverity.swigToEnum(swigErrors.errorSeverityCode(i)));
                targetErrorStore.add(severity, targetElement, getLineFrom(swigErrors.errorLocation(i)), swigErrors.errorText(i));
            }
        }
    }

    public static int getMarkerSeverityFor(ProblemSeverity severity) {
        switch (severity) {
            case SEVERITY_ERROR: return IMarker.SEVERITY_ERROR;
            case SEVERITY_WARNING: return IMarker.SEVERITY_WARNING;
            case SEVERITY_INFO: return IMarker.SEVERITY_INFO;
            default: throw new IllegalArgumentException();
        }
    }

    /**
     * Get line number from a string in the format "file:line"
     */
    public static int getLineFrom(String location) {
        String lineStr = StringUtils.substringAfterLast(location, ":");
        int line = 1;
        try {line = Integer.parseInt(lineStr);} catch (Exception e) {}
        return line;
    }

    private static NedAstNodeFactory nedElementFactory = new NedAstNodeFactory();
    private static MsgAstNodeFactory msgElementFactory = new MsgAstNodeFactory();

    /**
     * Converts a plain java NedElement tree to a native C++ (SWIG-wrapped) tree.
     * NOTE: There are two different NedElement types handled in this function.
     */
    public static ASTNode pojo2swig(INedElement pojoNode) {

        ASTNode swigNode = !isMsgElement(pojoNode)
                              ? nedElementFactory.createElementWithTag(pojoNode.getTagCode())
                              : msgElementFactory.createElementWithTag(pojoNode.getTagCode());

        // set the attributes
        for (int i = 0; i < pojoNode.getNumAttributes(); ++i) {
            String value = pojoNode.getAttribute(i);
            value = value == null ? "" : value;
            swigNode.setAttribute(i, value);
        }
        swigNode.setSourceLocation(pojoNode.getSourceLocation());

        // create child nodes
        for (INedElement child = pojoNode.getFirstChild(); child != null; child = child.getNextSibling()) {
            ASTNode convertedChild = pojo2swig(child);
            if (convertedChild != null)
                swigNode.appendChild(convertedChild);
        }

        return swigNode;
    }

    /**
     * Returns true if the given string can be parsed as a NED expression, and false otherwise.
     */
    public static boolean isExpressionValid(String expression) {
        Assert.isTrue(expression != null);
        ErrorStore errors = new ErrorStore();
        NedParser np = new NedParser(errors);
        return np.isValidNedExpression(expression);
    }

    /**
     * Converts a NedElement tree to an XML-like textual format. Useful for debugging.
     */
    public static String generateXmlFromSwigElementTree(ASTNode swigNode, String indent) {
        String result = indent;
        result += "<" + swigNode.getTagName();
        for (int i = 0; i < swigNode.getNumAttributes(); ++i)
            result += " " + swigNode.getAttributeName(i) + "=\""
                    + swigNode.getAttribute(i) + "\"";
        if (swigNode.getFirstChild() == null) {
            result += "/> \n";
        }
        else {
            result += "> \n";
            for (ASTNode child = swigNode.getFirstChild(); child != null; child = child
                    .getNextSibling())
                result += generateXmlFromSwigElementTree(child, indent + "  ");

            result += indent + "</" + swigNode.getTagName() + ">\n";
        }
        return result;
    }

    public static String generateXmlFromPojoElementTree(INedElement pojoNode, String indent, boolean printDebugString) {
        String result = indent;
        result += "<" + pojoNode.getTagName();
        for (int i = 0; i < pojoNode.getNumAttributes(); ++i)
            result += " " + pojoNode.getAttributeName(i) + "=\""
                    + pojoNode.getAttribute(i) + "\"";

        String debugString = !printDebugString ? "" : pojoNode.debugString();
        if (!"".equals(debugString))
                debugString = "<!-- "+debugString + " -->";

        if (pojoNode.getFirstChild() == null) {
            result += "/> " +  debugString + "\n";
        }
        else {
            result += "> " +  debugString + "\n";
            for (INedElement child = pojoNode.getFirstChild(); child != null; child = child.getNextSibling())
                result += generateXmlFromPojoElementTree(child, indent + "  ", printDebugString);

            result += indent + "</" + pojoNode.getTagName() + ">\n";
        }
        return result;
    }

    /**
     * Returns true if the two trees generate the same NED source code.
     * If either parameter is null, it returns false.
     */
    public static boolean isNedTreeEqual(INedElement tree1, INedElement tree2) {
        if (tree1 == tree2)
            return true;
        if (tree1==null || tree2==null)
            return false;
        return tree1.getNedSource().equals(tree2.getNedSource());
    }

}
