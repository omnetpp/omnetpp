/**
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
     * Returns a new object of class '<em>Analysis</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Analysis</em>'.
     * @generated
     */
    Analysis createAnalysis();

    /**
     * Returns a new object of class '<em>Inputs</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Inputs</em>'.
     * @generated
     */
    Inputs createInputs();

    /**
     * Returns a new object of class '<em>Input File</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Input File</em>'.
     * @generated
     */
    InputFile createInputFile();

    /**
     * Returns a new object of class '<em>Charts</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Charts</em>'.
     * @generated
     */
    Charts createCharts();

    /**
     * Returns a new object of class '<em>Property</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Property</em>'.
     * @generated
     */
    Property createProperty();

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
     * Returns a new object of class '<em>Dataset</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Dataset</em>'.
     * @generated
     */
    Dataset createDataset();

    /**
     * Returns a new object of class '<em>Chart Sheet</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Chart Sheet</em>'.
     * @generated
     */
    ChartSheet createChartSheet();

    /**
     * Returns a new object of class '<em>Folder</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Folder</em>'.
     * @generated
     */
    Folder createFolder();

    /**
     * Returns the package supported by this factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the package supported by this factory.
     * @generated
     */
    ScaveModelPackage getScaveModelPackage();

} //ScaveModelFactory
