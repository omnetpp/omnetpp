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
package org.omnetpp.ned.editor.graph.model;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

import org.eclipse.gef.requests.CreationFactory;

/**
 * Used to create new example models. Used by the create wizard
 * @author rhornig
 *
 */
public class ModelFactory {

    Container root;

    protected static void connect(NedElement e1, String t1, NedElement e2, String t2) {
        Wire wire = new Wire();
        wire.setSource(e1);
        wire.setSourceGate(t1);
        wire.setTarget(e2);
        wire.setTargetGate(t2);
        wire.attachSource();
        wire.attachTarget();
    }

    public static Module createFullAdder() {
        final SimpleModule or;
        final Module circuit, circuit1, circuit2;

        circuit1 = createHalfAdder();
        circuit2 = createHalfAdder();
        circuit1.setLocation(new Point(2, 10));
        circuit2.setLocation(new Point(38, 90));

        circuit = new Module();
        circuit.setSize(new Dimension(120, 216));
        or = new SimpleModule();
        or.setLocation(new Point(22, 162));

        circuit.addChild(circuit1);
        circuit.addChild(circuit2);

        connect(circuit, Module.TERMINALS_OUT[0], circuit1, Module.TERMINALS_IN[0]);
        connect(circuit, Module.TERMINALS_OUT[2], circuit1, Module.TERMINALS_IN[3]);
        connect(circuit, Module.TERMINALS_OUT[3], circuit2, Module.TERMINALS_IN[3]);
        connect(circuit1, Module.TERMINALS_OUT[7], circuit2, Module.TERMINALS_IN[0]);

        circuit.addChild(or);
        connect(or, SimpleModule.TERMINAL_OUT, circuit, Module.TERMINALS_IN[4]);
        connect(circuit1, Module.TERMINALS_OUT[4], or, SimpleModule.TERMINAL_A);
        connect(circuit2, Module.TERMINALS_OUT[4], or, SimpleModule.TERMINAL_B);
        connect(circuit2, Module.TERMINALS_OUT[7], circuit, Module.TERMINALS_IN[7]);

        return circuit;
    }

    public static Module createHalfAdder() {
        SimpleModule and, xor;
        Module circuit;

        circuit = new Module();
        circuit.setSize(new Dimension(60, 70));
        and = new SimpleModule();
        and.setLocation(new Point(2, 12));
        xor = new SimpleModule();
        xor.setLocation(new Point(22, 12));

        circuit.addChild(xor);
        circuit.addChild(and);

        connect(circuit, Module.TERMINALS_OUT[0], and, SimpleModule.TERMINAL_A);
        connect(circuit, Module.TERMINALS_OUT[3], and, SimpleModule.TERMINAL_B);
        connect(circuit, Module.TERMINALS_OUT[0], xor, SimpleModule.TERMINAL_A);
        connect(circuit, Module.TERMINALS_OUT[3], xor, SimpleModule.TERMINAL_B);

        connect(and, SimpleModule.TERMINAL_OUT, circuit, Module.TERMINALS_IN[4]);
        connect(xor, SimpleModule.TERMINAL_OUT, circuit, Module.TERMINALS_IN[7]);
        return circuit;
    }

    public static Object createLargeModel() {
        Container root = new Container();

        final Module circuit1, circuit2, circuit3, circuit4;

        //
        circuit1 = createHalfAdder();
        circuit1.setSize(new Dimension(64, 216));
        circuit1.setLocation(new Point(455, 104));
        root.addChild(circuit1);
        //
        circuit2 = createFullAdder();
        circuit2.setLocation(new Point(305, 104));
        root.addChild(circuit2);
        connect(circuit1, Module.TERMINALS_OUT[4], circuit2, Module.TERMINALS_IN[3]);
        //
        circuit3 = createFullAdder();
        circuit3.setLocation(new Point(155, 104));
        root.addChild(circuit3);
        connect(circuit2, Module.TERMINALS_OUT[4], circuit3, Module.TERMINALS_IN[3]);
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
        root = new Container();
        return root;
    }

    static public Object createModel() {

        Container root = new Container();

        Module circuit1;

        circuit1 = createHalfAdder();
        circuit1.setLocation(new Point(50, 50));
        root.addChild(circuit1);
        return root;
    }

    public Object getRootElement() {
        if (root == null) createLargeModel();
        return root;
    }

}
