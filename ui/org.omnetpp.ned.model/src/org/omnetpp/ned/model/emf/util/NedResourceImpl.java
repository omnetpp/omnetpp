/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Map;

import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.impl.ResourceImpl;
import org.omnetpp.ned.model.ModelUtil;
import org.omnetpp.ned.model.swig.NEDElement;
import org.omnetpp.ned.model.swig.NEDGenerator;
import org.omnetpp.ned.model.swig.NEDParser;

/**
 * <!-- begin-user-doc -->
 * The <b>Resource </b> associated with the package.
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned.model.emf.util.NedResourceFactoryImpl
 * @generated
 */
public class NedResourceImpl extends ResourceImpl {
    /**
     * Creates an instance of the resource.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param uri the URI of the new resource.
     * @generated
     */
    public NedResourceImpl(URI uri) {
        super(uri);
    }
    
    // TODO support load options and creat edefault values 
    
    @Override
    protected void doLoad(InputStream inputStream, Map options) throws IOException {
        // get the content of the stream
        InputStreamReader is = new InputStreamReader(inputStream);
        StringBuffer textualContent = new StringBuffer();
        char buff[] = new char[512]; 
        while(is.ready()) {
            int readedChars = is.read(buff);
            textualContent.append(String.valueOf(buff, 0, readedChars));
        }
        System.out.println(textualContent);
        
        NEDParser np = new NEDParser();
        // TODO should be a load option
        np.setParseExpressions(false);
        np.parseText(textualContent.toString());
        NEDElement treeRoot = np.getTree();

//        NEDGenerator ng = new NEDGenerator();
//        ng.setNewSyntax(false);
//        System.out.println(ng.generate(treeRoot, ""));
//        System.out.println(ModelUtil.printSwigElement(treeRoot));
        
        // convert SWIG to EMF tree
        EObject model = ModelUtil.swig2emf(treeRoot);
        System.out.println(model);
        // store the EMF model
        getContents().add(model);
        setLoaded(true);
        
    }

} //NedResourceImpl
