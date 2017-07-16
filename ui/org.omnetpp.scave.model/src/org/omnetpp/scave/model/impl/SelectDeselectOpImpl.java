/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import org.eclipse.emf.ecore.EClass;

import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SelectDeselectOp;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Select Deselect Op</b></em>'.
 * <!-- end-user-doc -->
 *
 * @generated
 */
public abstract class SelectDeselectOpImpl extends SetOperationImpl implements SelectDeselectOp {
    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected SelectDeselectOpImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.SELECT_DESELECT_OP;
    }

} //SelectDeselectOpImpl