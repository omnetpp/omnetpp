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
 * A representation of the model object '<em><b>Dataset</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Dataset#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Dataset#getItems <em>Items</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Dataset#getBasedOn <em>Based On</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getDataset()
 * @model
 * @generated
 */
public interface Dataset extends EObject {
    /**
     * Returns the value of the '<em><b>Name</b></em>' attribute.
     * The default value is <code>""</code>.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Name</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Name</em>' attribute.
     * @see #setName(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getDataset_Name()
     * @model default=""
     * @generated
     */
    String getName();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Dataset#getName <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Name</em>' attribute.
     * @see #getName()
     * @generated
     */
    void setName(String value);

    /**
     * Returns the value of the '<em><b>Items</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.DatasetItem}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Items</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Items</em>' containment reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getDataset_Items()
     * @model containment="true"
     * @generated
     */
    EList<DatasetItem> getItems();

    /**
     * Returns the value of the '<em><b>Based On</b></em>' reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Based On</em>' reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Based On</em>' reference.
     * @see #setBasedOn(Dataset)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getDataset_BasedOn()
     * @model
     * @generated
     */
    Dataset getBasedOn();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Dataset#getBasedOn <em>Based On</em>}' reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Based On</em>' reference.
     * @see #getBasedOn()
     * @generated
     */
    void setBasedOn(Dataset value);

} // Dataset