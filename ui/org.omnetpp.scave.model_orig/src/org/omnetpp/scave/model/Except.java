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

import org.eclipse.emf.ecore.EObject;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Except</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Except#getFilterPattern <em>Filter Pattern</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getExcept()
 * @model
 * @generated
 */
public interface Except extends EObject {

    /**
     * Returns the value of the '<em><b>Filter Pattern</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Filter Pattern</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Filter Pattern</em>' attribute.
     * @see #setFilterPattern(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getExcept_FilterPattern()
     * @model
     * @generated
     */
    String getFilterPattern();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Except#getFilterPattern <em>Filter Pattern</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Filter Pattern</em>' attribute.
     * @see #getFilterPattern()
     * @generated
     */
    void setFilterPattern(String value);
} // Except