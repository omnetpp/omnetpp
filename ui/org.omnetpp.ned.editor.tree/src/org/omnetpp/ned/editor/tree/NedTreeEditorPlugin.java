/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.editor.tree;

import org.eclipse.emf.common.EMFPlugin;
import org.eclipse.emf.common.util.ResourceLocator;
import org.omnetpp.ned.model.ModelUtil;
import org.omnetpp.ned.model.swig.NEDElement;
import org.omnetpp.ned.model.swig.NEDGenerator;
import org.omnetpp.ned.model.swig.NEDParser;

/**
 * This is the central singleton for the Ned editor plugin.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public final class NedTreeEditorPlugin extends EMFPlugin {
    /**
     * Keep track of the singleton.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static final NedTreeEditorPlugin INSTANCE = new NedTreeEditorPlugin();

    /**
     * Keep track of the singleton.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private static Implementation plugin;

    /**
     * Create the instance.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedTreeEditorPlugin() {
        super
          (new ResourceLocator [] {
           });
        
        // XXX just for testing. can be removed later
//        NEDParser np = new NEDParser();
//        np.setParseExpressions(false);
//        np.parseFile("c:/temp/tictoc.ned");
//        NEDElement treeRoot = np.getTree();
//        System.out.println(treeRoot.getTagName());
//        NEDGenerator ng = new NEDGenerator();
//        ng.setNewSyntax(false);
//        System.out.println(ng.generate(treeRoot, ""));
//        System.out.println(ModelUtil.printSwigElement(treeRoot));
//        
//        // generate EMF tree
//        System.out.println(ModelUtil.swig2emf(treeRoot));

    }

    /**
     * Returns the singleton instance of the Eclipse plugin.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the singleton instance.
     * @generated
     */
    public ResourceLocator getPluginResourceLocator() {
        return plugin;
    }

    /**
     * Returns the singleton instance of the Eclipse plugin.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the singleton instance.
     * @generated
     */
    public static Implementation getPlugin() {
        return plugin;
    }

    /**
     * The actual implementation of the Eclipse <b>Plugin</b>.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static class Implementation extends EclipsePlugin {
        /**
         * Creates an instance.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        public Implementation() {
            super();

            // Remember the static instance.
            //
            plugin = this;
        }
    }

}
