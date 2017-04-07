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

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Chart Sheet</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ChartSheet#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ChartSheet#getCharts <em>Charts</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ChartSheet#getProperties <em>Properties</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet()
 * @model
 * @generated
 */
public interface ChartSheet extends EObject {
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
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet_Name()
     * @model
     * @generated
     */
    String getName();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ChartSheet#getName <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Name</em>' attribute.
     * @see #getName()
     * @generated
     */
    void setName(String value);

    /**
     * Returns the value of the '<em><b>Charts</b></em>' reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.Chart}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Charts</em>' reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Charts</em>' reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet_Charts()
     * @model
     * @generated
     */
    EList<Chart> getCharts();

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
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet_Properties()
     * @model containment="true"
     * @generated
     */
    EList<Property> getProperties();

} // ChartSheet