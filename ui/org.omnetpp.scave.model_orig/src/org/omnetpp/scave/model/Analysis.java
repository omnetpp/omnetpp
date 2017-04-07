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
 * A representation of the model object '<em><b>Analysis</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getInputs <em>Inputs</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getDatasets <em>Datasets</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getChartSheets <em>Chart Sheets</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis()
 * @model
 * @generated
 */
public interface Analysis extends EObject {
    /**
     * Returns the value of the '<em><b>Inputs</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Inputs</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Inputs</em>' containment reference.
     * @see #setInputs(Inputs)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_Inputs()
     * @model containment="true"
     * @generated
     */
    Inputs getInputs();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Analysis#getInputs <em>Inputs</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Inputs</em>' containment reference.
     * @see #getInputs()
     * @generated
     */
    void setInputs(Inputs value);

    /**
     * Returns the value of the '<em><b>Datasets</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Datasets</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Datasets</em>' containment reference.
     * @see #setDatasets(Datasets)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_Datasets()
     * @model containment="true"
     * @generated
     */
    Datasets getDatasets();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Analysis#getDatasets <em>Datasets</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Datasets</em>' containment reference.
     * @see #getDatasets()
     * @generated
     */
    void setDatasets(Datasets value);

    /**
     * Returns the value of the '<em><b>Chart Sheets</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Chart Sheets</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Chart Sheets</em>' containment reference.
     * @see #setChartSheets(ChartSheets)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_ChartSheets()
     * @model containment="true"
     * @generated
     */
    ChartSheets getChartSheets();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Analysis#getChartSheets <em>Chart Sheets</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Chart Sheets</em>' containment reference.
     * @see #getChartSheets()
     * @generated
     */
    void setChartSheets(ChartSheets value);

} // Analysis