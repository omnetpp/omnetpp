/**
 */
package org.omnetpp.scave.model.util;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.util.Switch;

import org.omnetpp.scave.model.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see org.omnetpp.scave.model.ScaveModelPackage
 * @generated
 */
public class ScaveModelSwitch<T> extends Switch<T> {
    /**
     * The cached model package
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected static ScaveModelPackage modelPackage;

    /**
     * Creates an instance of the switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ScaveModelSwitch() {
        if (modelPackage == null) {
            modelPackage = ScaveModelPackage.eINSTANCE;
        }
    }

    /**
     * Checks whether this is a switch for the given package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param ePackage the package in question.
     * @return whether this is a switch for the given package.
     * @generated
     */
    @Override
    protected boolean isSwitchFor(EPackage ePackage) {
        return ePackage == modelPackage;
    }

    /**
     * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the first non-null result returned by a <code>caseXXX</code> call.
     * @generated
     */
    @Override
    protected T doSwitch(int classifierID, EObject theEObject) {
        switch (classifierID) {
            case ScaveModelPackage.ANALYSIS: {
                Analysis analysis = (Analysis)theEObject;
                T result = caseAnalysis(analysis);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.INPUTS: {
                Inputs inputs = (Inputs)theEObject;
                T result = caseInputs(inputs);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.INPUT_FILE: {
                InputFile inputFile = (InputFile)theEObject;
                T result = caseInputFile(inputFile);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.CHARTS: {
                Charts charts = (Charts)theEObject;
                T result = caseCharts(charts);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.ANALYSIS_ITEM: {
                AnalysisItem analysisItem = (AnalysisItem)theEObject;
                T result = caseAnalysisItem(analysisItem);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.CHART: {
                Chart chart = (Chart)theEObject;
                T result = caseChart(chart);
                if (result == null) result = caseAnalysisItem(chart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.PROPERTY: {
                Property property = (Property)theEObject;
                T result = caseProperty(property);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.BAR_CHART: {
                BarChart barChart = (BarChart)theEObject;
                T result = caseBarChart(barChart);
                if (result == null) result = caseChart(barChart);
                if (result == null) result = caseAnalysisItem(barChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.LINE_CHART: {
                LineChart lineChart = (LineChart)theEObject;
                T result = caseLineChart(lineChart);
                if (result == null) result = caseChart(lineChart);
                if (result == null) result = caseAnalysisItem(lineChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.HISTOGRAM_CHART: {
                HistogramChart histogramChart = (HistogramChart)theEObject;
                T result = caseHistogramChart(histogramChart);
                if (result == null) result = caseChart(histogramChart);
                if (result == null) result = caseAnalysisItem(histogramChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.SCATTER_CHART: {
                ScatterChart scatterChart = (ScatterChart)theEObject;
                T result = caseScatterChart(scatterChart);
                if (result == null) result = caseChart(scatterChart);
                if (result == null) result = caseAnalysisItem(scatterChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.DATASET: {
                Dataset dataset = (Dataset)theEObject;
                T result = caseDataset(dataset);
                if (result == null) result = caseAnalysisItem(dataset);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.CHART_SHEET: {
                ChartSheet chartSheet = (ChartSheet)theEObject;
                T result = caseChartSheet(chartSheet);
                if (result == null) result = caseAnalysisItem(chartSheet);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.FOLDER: {
                Folder folder = (Folder)theEObject;
                T result = caseFolder(folder);
                if (result == null) result = caseAnalysisItem(folder);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            default: return defaultCase(theEObject);
        }
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Analysis</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Analysis</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseAnalysis(Analysis object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Inputs</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Inputs</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseInputs(Inputs object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Input File</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Input File</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseInputFile(InputFile object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Charts</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Charts</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseCharts(Charts object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Analysis Item</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Analysis Item</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseAnalysisItem(AnalysisItem object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Chart</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Chart</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseChart(Chart object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Property</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Property</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseProperty(Property object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Bar Chart</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Bar Chart</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseBarChart(BarChart object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Line Chart</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Line Chart</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseLineChart(LineChart object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Histogram Chart</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Histogram Chart</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseHistogramChart(HistogramChart object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Scatter Chart</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Scatter Chart</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseScatterChart(ScatterChart object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Dataset</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Dataset</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseDataset(Dataset object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Chart Sheet</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Chart Sheet</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseChartSheet(ChartSheet object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Folder</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Folder</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseFolder(Folder object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch, but this is the last case anyway.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject)
     * @generated
     */
    @Override
    public T defaultCase(EObject object) {
        return null;
    }

} //ScaveModelSwitch
