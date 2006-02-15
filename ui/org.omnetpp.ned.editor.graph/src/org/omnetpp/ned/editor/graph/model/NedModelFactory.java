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
public class NedModelFactory {

    NedFile root;

    protected static void connect(NedNode e1, String t1, NedNode e2, String t2) {
        Wire wire = new Wire();
        wire.setSource(e1);
        wire.setSourceGate(t1);
        wire.setTarget(e2);
        wire.setTargetGate(t2);
        wire.attachSource();
        wire.attachTarget();
    }

    public static CompoundModule createFullAdder() {
        final Submodule or;
        final CompoundModule circuit, circuit1, circuit2;

        circuit1 = createHalfAdder();
        circuit2 = createHalfAdder();
        circuit1.setLocation(new Point(2, 10));
        circuit2.setLocation(new Point(38, 90));

        circuit = new CompoundModule();
        circuit.setSize(new Dimension(120, 216));
        or = new Submodule();
        or.setLocation(new Point(22, 162));

        circuit.addChild(circuit1);
        circuit.addChild(circuit2);

        connect(circuit, CompoundModule.TERMINALS_OUT[0], circuit1, CompoundModule.TERMINALS_IN[0]);
        connect(circuit, CompoundModule.TERMINALS_OUT[2], circuit1, CompoundModule.TERMINALS_IN[3]);
        connect(circuit, CompoundModule.TERMINALS_OUT[3], circuit2, CompoundModule.TERMINALS_IN[3]);
        connect(circuit1, CompoundModule.TERMINALS_OUT[7], circuit2, CompoundModule.TERMINALS_IN[0]);

        circuit.addChild(or);
        connect(or, Submodule.TERMINAL_OUT, circuit, CompoundModule.TERMINALS_IN[4]);
        connect(circuit1, CompoundModule.TERMINALS_OUT[4], or, Submodule.TERMINAL_A);
        connect(circuit2, CompoundModule.TERMINALS_OUT[4], or, Submodule.TERMINAL_B);
        connect(circuit2, CompoundModule.TERMINALS_OUT[7], circuit, CompoundModule.TERMINALS_IN[7]);

        return circuit;
    }

    public static CompoundModule createHalfAdder() {
        Submodule and, xor;
        CompoundModule circuit;

        circuit = new CompoundModule();
        circuit.setSize(new Dimension(60, 70));
        and = new Submodule();
        and.setLocation(new Point(2, 12));
        xor = new Submodule();
        xor.setLocation(new Point(22, 12));

        circuit.addChild(xor);
        circuit.addChild(and);

        connect(circuit, CompoundModule.TERMINALS_OUT[0], and, Submodule.TERMINAL_A);
        connect(circuit, CompoundModule.TERMINALS_OUT[3], and, Submodule.TERMINAL_B);
        connect(circuit, CompoundModule.TERMINALS_OUT[0], xor, Submodule.TERMINAL_A);
        connect(circuit, CompoundModule.TERMINALS_OUT[3], xor, Submodule.TERMINAL_B);

        connect(and, Submodule.TERMINAL_OUT, circuit, CompoundModule.TERMINALS_IN[4]);
        connect(xor, Submodule.TERMINAL_OUT, circuit, CompoundModule.TERMINALS_IN[7]);
        return circuit;
    }

    public static Object createLargeModel() {
        NedFile root = new NedFile();

        final CompoundModule circuit1, circuit2, circuit3, circuit4;

        //
        circuit1 = createHalfAdder();
        circuit1.setSize(new Dimension(64, 216));
        circuit1.setLocation(new Point(455, 104));
        root.addChild(circuit1);
        //
        circuit2 = createFullAdder();
        circuit2.setLocation(new Point(305, 104));
        root.addChild(circuit2);
        connect(circuit1, CompoundModule.TERMINALS_OUT[4], circuit2, CompoundModule.TERMINALS_IN[3]);
        //
        circuit3 = createFullAdder();
        circuit3.setLocation(new Point(155, 104));
        root.addChild(circuit3);
        connect(circuit2, CompoundModule.TERMINALS_OUT[4], circuit3, CompoundModule.TERMINALS_IN[3]);
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
        root = new NedFile();
        return root;
    }

    static public Object createModel() {

        NedFile root = new NedFile();

        CompoundModule circuit1;

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
