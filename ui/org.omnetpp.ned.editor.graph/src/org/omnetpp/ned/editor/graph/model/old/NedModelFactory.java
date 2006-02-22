/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.model.old;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.NedFileNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;

/**
 * Used to create new example models. Used by the create wizard
 * @author rhornig
 *
 */
public class NedModelFactory {

    NedFileModel root;

    protected static void connect(NedNodeModel e1, String t1, NedNodeModel e2, String t2) {
        WireModel wire = new WireModel();
        wire.setSource(e1);
        wire.setSourceGate(t1);
        wire.setTarget(e2);
        wire.setTargetGate(t2);
        wire.attachSource();
        wire.attachTarget();
    }

    public static CompoundModuleModel createFullAdder() {
        final SubmoduleModel or;
        final CompoundModuleModel circuit, circuit1, circuit2;

        circuit1 = createHalfAdder();
        circuit2 = createHalfAdder();
        circuit1.setLocation(new Point(2, 10));
        circuit2.setLocation(new Point(38, 90));

        circuit = new CompoundModuleModel();
        circuit.setSize(new Dimension(120, 216));
        or = new SubmoduleModel();
        or.setLocation(new Point(22, 162));

        circuit.addChild(circuit1);
        circuit.addChild(circuit2);

        connect(circuit, CompoundModuleModel.TERMINALS_OUT[0], circuit1, CompoundModuleModel.TERMINALS_IN[0]);
        connect(circuit, CompoundModuleModel.TERMINALS_OUT[2], circuit1, CompoundModuleModel.TERMINALS_IN[3]);
        connect(circuit, CompoundModuleModel.TERMINALS_OUT[3], circuit2, CompoundModuleModel.TERMINALS_IN[3]);
        connect(circuit1, CompoundModuleModel.TERMINALS_OUT[7], circuit2, CompoundModuleModel.TERMINALS_IN[0]);

        circuit.addChild(or);
        connect(or, SubmoduleModel.TERMINAL_OUT, circuit, CompoundModuleModel.TERMINALS_IN[4]);
        connect(circuit1, CompoundModuleModel.TERMINALS_OUT[4], or, SubmoduleModel.TERMINAL_A);
        connect(circuit2, CompoundModuleModel.TERMINALS_OUT[4], or, SubmoduleModel.TERMINAL_B);
        connect(circuit2, CompoundModuleModel.TERMINALS_OUT[7], circuit, CompoundModuleModel.TERMINALS_IN[7]);

        return circuit;
    }

    public static CompoundModuleModel createHalfAdder() {
        SubmoduleModel and, xor;
        CompoundModuleModel circuit;

        circuit = new CompoundModuleModel();
        circuit.setSize(new Dimension(60, 70));
        and = new SubmoduleModel();
        and.setLocation(new Point(2, 12));
        xor = new SubmoduleModel();
        xor.setLocation(new Point(22, 12));

        circuit.addChild(xor);
        circuit.addChild(and);

        connect(circuit, CompoundModuleModel.TERMINALS_OUT[0], and, SubmoduleModel.TERMINAL_A);
        connect(circuit, CompoundModuleModel.TERMINALS_OUT[3], and, SubmoduleModel.TERMINAL_B);
        connect(circuit, CompoundModuleModel.TERMINALS_OUT[0], xor, SubmoduleModel.TERMINAL_A);
        connect(circuit, CompoundModuleModel.TERMINALS_OUT[3], xor, SubmoduleModel.TERMINAL_B);

        connect(and, SubmoduleModel.TERMINAL_OUT, circuit, CompoundModuleModel.TERMINALS_IN[4]);
        connect(xor, SubmoduleModel.TERMINAL_OUT, circuit, CompoundModuleModel.TERMINALS_IN[7]);
        return circuit;
    }

    public static Object createLargeModel() {
        NedFileModel root = new NedFileModel();

        final CompoundModuleModel circuit1, circuit2, circuit3, circuit4;

        //
        circuit1 = createHalfAdder();
        circuit1.setSize(new Dimension(64, 216));
        circuit1.setLocation(new Point(455, 104));
        root.addChild(circuit1);
        //
        circuit2 = createFullAdder();
        circuit2.setLocation(new Point(305, 104));
        root.addChild(circuit2);
        connect(circuit1, CompoundModuleModel.TERMINALS_OUT[4], circuit2, CompoundModuleModel.TERMINALS_IN[3]);
        //
        circuit3 = createFullAdder();
        circuit3.setLocation(new Point(155, 104));
        root.addChild(circuit3);
        connect(circuit2, CompoundModuleModel.TERMINALS_OUT[4], circuit3, CompoundModuleModel.TERMINALS_IN[3]);
        //
        circuit4 = createFullAdder();
        circuit4.setLocation(new Point(5, 104));
        //

        return root;
    }

    public static CreationFactory getFullAdderFactory() {
        return new CreationFactory() {
            public Object getNewObject() {
                return createFullAdder();
            }

            public Object getObjectType() {
                return "Full Adder"; //$NON-NLS-1$
            }
        };
    }

    public static CreationFactory getHalfAdderFactory() {
        return new CreationFactory() {
            public Object getNewObject() {
                return createHalfAdder();
            }

            public Object getObjectType() {
                return "Half Adder"; //$NON-NLS-1$
            }
        };
    }

    public Object createEmptyModel() {
        root = new NedFileModel();
        return root;
    }

    static public Object createModel() {

        NedFileModel root = new NedFileModel();

        CompoundModuleModel circuit1;

        circuit1 = createHalfAdder();
        circuit1.setLocation(new Point(50, 50));
        root.addChild(circuit1);
        return root;
    }

    public Object getRootElement() {
        if (root == null) createLargeModel();
        return root;
    }
    
    public static NedFileModel pojo2nedFileModel(NedFileNode node) {
    	NedFileModel result = new NedFileModel();
    	
    	for(NEDElement ni : node) {
    		if (ni instanceof CompoundModuleNode) {
    	        result.addChild(pojo2compoundModuleModel((CompoundModuleNode)ni));
    		}
    	}
    	return result;
	}	
    	
    public static CompoundModuleModel pojo2compoundModuleModel(CompoundModuleNode node) {
		CompoundModuleModel result = new CompoundModuleModel();
        result.setSize(new Dimension(120, 216));
        result.setName(node.getName());

        // add submodules as children
    	for(NEDElement itCompoundModule : node) {
    		if (itCompoundModule instanceof SubmodulesNode) {
    			
    		}
    	}
        
        return result;
    }
}
