package org.omnetpp.ned2.model;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned2.model.pojo.NEDElementFactory;
import org.omnetpp.ned2.model.pojo.NedFileNode;
import org.omnetpp.ned2.model.swig.NED1Generator;
import org.omnetpp.ned2.model.swig.NED2Generator;
import org.omnetpp.ned2.model.swig.NEDDTDValidator;
import org.omnetpp.ned2.model.swig.NEDElement;
import org.omnetpp.ned2.model.swig.NEDErrorCategory;
import org.omnetpp.ned2.model.swig.NEDErrorStore;
import org.omnetpp.ned2.model.swig.NEDParser;

public class ModelUtil {
	// private static final String NED_EMF_MODEL_PACKAGE =
	// "org.omnetpp.ned.model.emf";

	/**
	 * Generate NED code from the given NEDElement tree. The root node
	 * does not have to be NedFileNode, any subtree can be converted
	 * to source form.
	 * 
	 * @param keepSyntax if set, sources parsed in old syntax (NED-1) will be generated in onld syntax as well 
	 */
	public static String generateNedSource(org.omnetpp.ned2.model.NEDElement treeRoot, boolean keepSyntax) {
		NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(true); //XXX just for debugging
		if (keepSyntax && treeRoot instanceof NedFileNode && "1".equals(((NedFileNode)treeRoot).getVersion())) {
			NED1Generator ng = new NED1Generator(errors);
			return ng.generate(pojo2swig(treeRoot), ""); // TODO check NEDErrorStore for conversion errors!! 
		}
		else {
			NED2Generator ng = new NED2Generator(errors);
			return ng.generate(pojo2swig(treeRoot), ""); // TODO check NEDErrorStore for errors!!
		}
	}

	/**
	 * Parse NED source and return it as a NEDElement tree.
	 * 
	 * @param source source code as string
	 * @return null if there was an error during parsing.
	 */
	public static org.omnetpp.ned2.model.NEDElement parseNedSource(String source, NEDErrorStore errors) {
        return parse(source, null, errors);
	}

	/**
	 * Load and parse NED file to a NEDElement tree.
	 * 
	 * @param filename file name
	 * @return null if there was an error during parsing.
	 */
	public static org.omnetpp.ned2.model.NEDElement loadNedSource(String filename, NEDErrorStore errors) {
        return parse(null, filename, errors);
	}

	/**
	 * Parse the given source or the given file.
	 */
	private static org.omnetpp.ned2.model.NEDElement parse(String source, String filename, NEDErrorStore errors) {
		Assert.isTrue((source==null)!=(filename==null)); // exactly one of them is given
		try {
			// parse
			NEDParser np = new NEDParser(errors);
			np.setParseExpressions(false);
			NEDElement swigTree = source!=null ? np.parseNEDText(source) : np.parseNEDFile(filename);
			if (swigTree == null || !errors.empty()) {
				if (swigTree!=null) swigTree.delete();
				return null;
			}

			// validate
			NEDDTDValidator dtdvalidator = new NEDDTDValidator(errors);
			dtdvalidator.validate(swigTree);
			if (!errors.empty()) {
				swigTree.delete();
				return null;
			}
			//NEDBasicValidator basicvalidator = new NEDBasicValidator(errors);
			//basicvalidator.validate(swigTree);

			// convert tree to pure Java objects
			org.omnetpp.ned2.model.NEDElement pojoTree = swig2pojo(swigTree, null);
			swigTree.delete();
			return pojoTree;
		} catch (RuntimeException e) {
			errors.add(NEDErrorCategory.ERRCAT_ERROR.ordinal(), "", "internal error: "+e);
			e.printStackTrace(); //XXX should go into the log
			return null;
		}
	}
	
	/**
	 * Converts a native C++ (SWIG-wrapped) NEDElement tree to a plain java tree.  
	 * WARNING there are two different NEDElement types hadled in this function. 
	 */
	public static org.omnetpp.ned2.model.NEDElement swig2pojo(NEDElement swigNode, org.omnetpp.ned2.model.NEDElement parent) {
		org.omnetpp.ned2.model.NEDElement pojoNode = NEDElementFactory.getInstance() 
			.createNodeWithTag(swigNode.getTagCode(), parent);

		// set the attributes
		for (int i = 0; i < swigNode.getNumAttributes(); ++i) {
			pojoNode.setAttribute(i, swigNode.getAttribute(i));
		}
		pojoNode.setSourceLocation(swigNode.getSourceLocation());

		// create child nodes
		for (NEDElement child = swigNode.getFirstChild(); child != null; child = child
				.getNextSibling()) {
			swig2pojo(child, pojoNode);
		}

		return pojoNode;
	}	

	/**
	 * Converts a plain java NEDElement tree to a native C++ (SWIG-wrapped) tree.  
	 * WARNING there are two differenet NEDElement types hadled in this function. 
	 */
	public static NEDElement pojo2swig(org.omnetpp.ned2.model.NEDElement pojoNode) {

		NEDElement swigNode = org.omnetpp.ned2.model.swig.NEDElementFactory.getInstance()
				.createNodeWithTag(pojoNode.getTagCode());

		// set the attributes
		for (int i = 0; i < pojoNode.getNumAttributes(); ++i) {
			String value = pojoNode.getAttribute(i);
			value = (value == null) ? "" : value;
			swigNode.setAttribute(i, value);
		}
		swigNode.setSourceLocation(pojoNode.getSourceLocation());

		// create child nodes
		for (org.omnetpp.ned2.model.NEDElement child = pojoNode.getFirstChild(); 
					child != null; child = child.getNextSibling()) {
			swigNode.appendChild(pojo2swig(child));
		}

		return swigNode;
	}	

	/**
	 * Converts a NEDElement tree to an XML-like textual format. Useful for debugging.
	 */
	public static String printSwigElementTree(NEDElement swigNode, String indent) {
		String result = indent;
		result += "<" + swigNode.getTagName();
		for (int i = 0; i < swigNode.getNumAttributes(); ++i)
			result += " " + swigNode.getAttributeName(i) + "=\""
					+ swigNode.getAttribute(i) + "\"";
		if (swigNode.getFirstChild() == null) {
			result += "/> \n";
		} else {
			result += "> \n";
			for (NEDElement child = swigNode.getFirstChild(); child != null; child = child
					.getNextSibling())
				result += printSwigElementTree(child, indent + "  ");

			result += indent + "</" + swigNode.getTagName() + ">\n";
		}
		return result;
	}

	public static String printPojoElementTree(org.omnetpp.ned2.model.NEDElement pojoNode, String indent) {
		String result = indent;
		result += "<" + pojoNode.getTagName();
		for (int i = 0; i < pojoNode.getNumAttributes(); ++i)
			result += " " + pojoNode.getAttributeName(i) + "=\""
					+ pojoNode.getAttribute(i) + "\"";
        
        String debugString = pojoNode.debugString();
        if (!"".equals(debugString))
                debugString = "<!-- "+debugString + " -->";
        
		if (pojoNode.getFirstChild() == null) {
			result += "/> " +  debugString + "\n";
		} else {
			result += "> " +  debugString + "\n";
			for (org.omnetpp.ned2.model.NEDElement child = pojoNode.getFirstChild(); child != null; child = child
					.getNextSibling())
				result += printPojoElementTree(child, indent + "  ");

			result += indent + "</" + pojoNode.getTagName() + ">\n";
		}
		return result;
	}
    
}
