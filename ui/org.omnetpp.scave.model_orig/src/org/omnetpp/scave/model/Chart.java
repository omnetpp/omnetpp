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
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Chart</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Chart#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Chart#getFilters <em>Filters</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Chart#getProperties <em>Properties</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getChart()
 * @model abstract="true"
 * @generated
 */
public interface Chart extends DatasetItem {
    /**
     * Returns the value of the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Name</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Name</em>' attribute.
     * @see #setName(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Name()
     * @model
     * @generated
     */
    String getName();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Chart#getName <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Name</em>' attribute.
     * @see #getName()
     * @generated
     */
    void setName(String value);

    /**
     * Returns the value of the '<em><b>Filters</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.SelectDeselectOp}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Filters</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Filters</em>' containment reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Filters()
     * @model containment="true"
     * @generated
     */
    EList<SelectDeselectOp> getFilters();

    /**
     * Returns the value of the '<em><b>Properties</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.Property}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Properties</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Properties</em>' containment reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Properties()
     * @model containment="true"
     * @generated
     */
    EList<Property> getProperties();

} // Chart