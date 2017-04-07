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

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.scave.model.ScaveModelPackage
 * @generated
 */
public interface ScaveModelFactory extends EFactory {
    /**
     * The singleton instance of the factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    ScaveModelFactory eINSTANCE = org.omnetpp.scave.model.impl.ScaveModelFactoryImpl.init();

    /**
     * Returns a new object of class '<em>Dataset</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Dataset</em>'.
     * @generated
     */
    Dataset createDataset();

    /**
     * Returns a new object of class '<em>Add</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Add</em>'.
     * @generated
     */
    Add createAdd();

    /**
     * Returns a new object of class '<em>Apply</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Apply</em>'.
     * @generated
     */
    Apply createApply();

    /**
     * Returns a new object of class '<em>Except</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Except</em>'.
     * @generated
     */
    Except createExcept();

    /**
     * Returns a new object of class '<em>Property</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Property</em>'.
     * @generated
     */
    Property createProperty();

    /**
     * Returns a new object of class '<em>Group</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Group</em>'.
     * @generated
     */
    Group createGroup();

    /**
     * Returns a new object of class '<em>Discard</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Discard</em>'.
     * @generated
     */
    Discard createDiscard();

    /**
     * Returns a new object of class '<em>Param</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Param</em>'.
     * @generated
     */
    Param createParam();

    /**
     * Returns a new object of class '<em>Chart Sheet</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Chart Sheet</em>'.
     * @generated
     */
    ChartSheet createChartSheet();

    /**
     * Returns a new object of class '<em>Analysis</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Analysis</em>'.
     * @generated
     */
    Analysis createAnalysis();

    /**
     * Returns a new object of class '<em>Select</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Select</em>'.
     * @generated
     */
    Select createSelect();

    /**
     * Returns a new object of class '<em>Deselect</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Deselect</em>'.
     * @generated
     */
    Deselect createDeselect();

    /**
     * Returns a new object of class '<em>Inputs</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Inputs</em>'.
     * @generated
     */
    Inputs createInputs();

    /**
     * Returns a new object of class '<em>Chart Sheets</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Chart Sheets</em>'.
     * @generated
     */
    ChartSheets createChartSheets();

    /**
     * Returns a new object of class '<em>Datasets</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Datasets</em>'.
     * @generated
     */
    Datasets createDatasets();

    /**
     * Returns a new object of class '<em>Input File</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Input File</em>'.
     * @generated
     */
    InputFile createInputFile();

    /**
     * Returns a new object of class '<em>Compute</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Compute</em>'.
     * @generated
     */
    Compute createCompute();

    /**
     * Returns a new object of class '<em>Bar Chart</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Bar Chart</em>'.
     * @generated
     */
    BarChart createBarChart();

    /**
     * Returns a new object of class '<em>Line Chart</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Line Chart</em>'.
     * @generated
     */
    LineChart createLineChart();

    /**
     * Returns a new object of class '<em>Histogram Chart</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Histogram Chart</em>'.
     * @generated
     */
    HistogramChart createHistogramChart();

    /**
     * Returns a new object of class '<em>Scatter Chart</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Scatter Chart</em>'.
     * @generated
     */
    ScatterChart createScatterChart();

    /**
     * Returns a new object of class '<em>Compute Scalar</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Compute Scalar</em>'.
     * @generated
     */
    ComputeScalar createComputeScalar();

    /**
     * Returns the package supported by this factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the package supported by this factory.
     * @generated
     */
    ScaveModelPackage getScaveModelPackage();

} //ScaveModelFactory
