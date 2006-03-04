package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.pojo.NEDElementFactory;
import org.omnetpp.ned2.model.pojo.NedFileNode;
import org.omnetpp.ned2.model.swig.NED1Generator;
import org.omnetpp.ned2.model.swig.NED2Generator;
import org.omnetpp.ned2.model.swig.NEDElement;
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
		if (treeRoot instanceof NedFileNode && "1".equals(((NedFileNode)treeRoot).getVersion())) {
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
	public static org.omnetpp.ned2.model.NEDElement parseNedSource(String source) {
		NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(true); //XXX just for debugging
		NEDParser np = new NEDParser(errors);
		np.setParseExpressions(false);
		NEDElement treeRoot = np.parseNEDText(source); // TODO check NEDErrorStore for errors
        // TODO run validation code? DTDValidation, BasicValidation, etc...
		if (treeRoot == null || !errors.empty())
			return null;
		org.omnetpp.ned2.model.NEDElement tmpEl = swig2pojo(treeRoot, null);
		return tmpEl;
	}

	/**
	 * Load and parse NED file to a NEDElement tree.
	 * 
	 * @param fname file name
	 * @return null if there was an error during parsing.
	 */
	public static org.omnetpp.ned2.model.NEDElement loadNedSource(String fname) {
		NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(true); //XXX
		NEDParser np = new NEDParser(errors);
		np.setParseExpressions(false);
		NEDElement treeRoot = np.parseNEDFile(fname);
//		System.out.println(printSwigElementTree(treeRoot, ""));
		
		if (treeRoot == null)
			return null;
		org.omnetpp.ned2.model.NEDElement tmpEl = swig2pojo(treeRoot, null);
		return tmpEl;
	}

	/**
	 * Converts a native C++ (SWIG-wrapped) NEDElement tree to a plain java tree.  
	 * WARNING there are two differenet NEDElement types hadled in this function. 
	 */
	public static org.omnetpp.ned2.model.NEDElement swig2pojo(NEDElement swigNode, org.omnetpp.ned2.model.NEDElement parent) {
		org.omnetpp.ned2.model.NEDElement pojoNode = NEDElementFactory.getInstance() 
			.createNodeWithTag(swigNode.getTagCode(), parent);

		// set the attributes
		for (int i = 0; i < swigNode.getNumAttributes(); ++i) {
			pojoNode.setAttribute(i, swigNode.getAttribute(i));
		}

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
		if (pojoNode.getFirstChild() == null) {
			result += "/> \n";
		} else {
			result += "> \n";
			for (org.omnetpp.ned2.model.NEDElement child = pojoNode.getFirstChild(); child != null; child = child
					.getNextSibling())
				result += printPojoElementTree(child, indent + "  ");

			result += indent + "</" + pojoNode.getTagName() + ">\n";
		}
		return result;
	}

    // TODO special cases and exceptions should be handled correctly
	// GATE In/Out; import statement; for loop and variable
	// public static EObject swig2emf(NEDElement swigNode) {
	// EClass eClass = Swig2EmfMapper.getEClass(swigNode.getTagName());
	// System.out.println("Node: "+swigNode.getTagName()+" -> "+eClass);
	// if (eClass == null) return null;
	//        
	// EObject emfNode = NedFactory.eINSTANCE.create(eClass);
	//
	// // set the attributes
	// for(int i=0; i < swigNode.getNumAttributes(); ++i) {
	// EStructuralFeature eFeature =
	// Swig2EmfMapper.getEStructuralFeature(swigNode.getTagName(),
	// swigNode.getAttributeName(i));
	// System.out.println(" Attr: "+swigNode.getAttributeName(i)+" ->
	// "+eFeature);
	// if (eFeature == null) continue;
	//            
	// Object value = swigNode.getAttribute(i);
	// // convert the true/false string to booolean. all other attributes are
	// stored as strings
	// if (emfNode.eGet(eFeature) instanceof Boolean)
	// value = Boolean.valueOf((String)value);
	// emfNode.eSet(eFeature, value);
	// }
	//
	// // create child nodes
	// for(NEDElement child = swigNode.getFirstChild(); child != null; child =
	// child.getNextSibling()) {
	// EObject emfChild = swig2emf(child);
	// if (emfChild == null) continue;
	// EStructuralFeature eFeature =
	// Swig2EmfMapper.getEStructuralFeature(swigNode.getTagName(),
	// child.getTagName());
	// System.out.println(" Assigning:
	// "+swigNode.getTagName()+"."+child.getTagName()+" -> "+eFeature);
	// if (eFeature == null) continue;
	//            
	// // if more than one child is possible (ie. the feature is an EList -> add
	// the child to the EList)
	// // otherwise just set it as a usual attribute
	// Object childContainer = emfNode.eGet(eFeature);
	// if (childContainer instanceof EList)
	// ((EList)childContainer).add(emfChild);
	// else emfNode.eSet(eFeature, emfChild);
	// }
	//            
	// return emfNode;
	// }

}
