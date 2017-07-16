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
package org.omnetpp.scave.model.util;

import java.util.List;

import org.eclipse.emf.ecore.EClass;
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
            case ScaveModelPackage.DATASET: {
                Dataset dataset = (Dataset)theEObject;
                T result = caseDataset(dataset);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.CHART: {
                Chart chart = (Chart)theEObject;
                T result = caseChart(chart);
                if (result == null) result = caseDatasetItem(chart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.ADD: {
                Add add = (Add)theEObject;
                T result = caseAdd(add);
                if (result == null) result = caseAddDiscardOp(add);
                if (result == null) result = caseSetOperation(add);
                if (result == null) result = caseDatasetItem(add);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.DISCARD: {
                Discard discard = (Discard)theEObject;
                T result = caseDiscard(discard);
                if (result == null) result = caseAddDiscardOp(discard);
                if (result == null) result = caseSetOperation(discard);
                if (result == null) result = caseDatasetItem(discard);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.PROCESSING_OP: {
                ProcessingOp processingOp = (ProcessingOp)theEObject;
                T result = caseProcessingOp(processingOp);
                if (result == null) result = caseDatasetItem(processingOp);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.EXCEPT: {
                Except except = (Except)theEObject;
                T result = caseExcept(except);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.SET_OPERATION: {
                SetOperation setOperation = (SetOperation)theEObject;
                T result = caseSetOperation(setOperation);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.DATASET_ITEM: {
                DatasetItem datasetItem = (DatasetItem)theEObject;
                T result = caseDatasetItem(datasetItem);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.GROUP: {
                Group group = (Group)theEObject;
                T result = caseGroup(group);
                if (result == null) result = caseDatasetItem(group);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.PROPERTY: {
                Property property = (Property)theEObject;
                T result = caseProperty(property);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.PARAM: {
                Param param = (Param)theEObject;
                T result = caseParam(param);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.CHART_SHEET: {
                ChartSheet chartSheet = (ChartSheet)theEObject;
                T result = caseChartSheet(chartSheet);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.ANALYSIS: {
                Analysis analysis = (Analysis)theEObject;
                T result = caseAnalysis(analysis);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.SELECT: {
                Select select = (Select)theEObject;
                T result = caseSelect(select);
                if (result == null) result = caseSelectDeselectOp(select);
                if (result == null) result = caseSetOperation(select);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.DESELECT: {
                Deselect deselect = (Deselect)theEObject;
                T result = caseDeselect(deselect);
                if (result == null) result = caseSelectDeselectOp(deselect);
                if (result == null) result = caseSetOperation(deselect);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.ADD_DISCARD_OP: {
                AddDiscardOp addDiscardOp = (AddDiscardOp)theEObject;
                T result = caseAddDiscardOp(addDiscardOp);
                if (result == null) result = caseSetOperation(addDiscardOp);
                if (result == null) result = caseDatasetItem(addDiscardOp);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.SELECT_DESELECT_OP: {
                SelectDeselectOp selectDeselectOp = (SelectDeselectOp)theEObject;
                T result = caseSelectDeselectOp(selectDeselectOp);
                if (result == null) result = caseSetOperation(selectDeselectOp);
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
            case ScaveModelPackage.CHART_SHEETS: {
                ChartSheets chartSheets = (ChartSheets)theEObject;
                T result = caseChartSheets(chartSheets);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.DATASETS: {
                Datasets datasets = (Datasets)theEObject;
                T result = caseDatasets(datasets);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.APPLY: {
                Apply apply = (Apply)theEObject;
                T result = caseApply(apply);
                if (result == null) result = caseProcessingOp(apply);
                if (result == null) result = caseDatasetItem(apply);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.COMPUTE: {
                Compute compute = (Compute)theEObject;
                T result = caseCompute(compute);
                if (result == null) result = caseProcessingOp(compute);
                if (result == null) result = caseDatasetItem(compute);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.BAR_CHART: {
                BarChart barChart = (BarChart)theEObject;
                T result = caseBarChart(barChart);
                if (result == null) result = caseChart(barChart);
                if (result == null) result = caseDatasetItem(barChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.LINE_CHART: {
                LineChart lineChart = (LineChart)theEObject;
                T result = caseLineChart(lineChart);
                if (result == null) result = caseChart(lineChart);
                if (result == null) result = caseDatasetItem(lineChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.HISTOGRAM_CHART: {
                HistogramChart histogramChart = (HistogramChart)theEObject;
                T result = caseHistogramChart(histogramChart);
                if (result == null) result = caseChart(histogramChart);
                if (result == null) result = caseDatasetItem(histogramChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.SCATTER_CHART: {
                ScatterChart scatterChart = (ScatterChart)theEObject;
                T result = caseScatterChart(scatterChart);
                if (result == null) result = caseChart(scatterChart);
                if (result == null) result = caseDatasetItem(scatterChart);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case ScaveModelPackage.COMPUTE_SCALAR: {
                ComputeScalar computeScalar = (ComputeScalar)theEObject;
                T result = caseComputeScalar(computeScalar);
                if (result == null) result = caseDatasetItem(computeScalar);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            default: return defaultCase(theEObject);
        }
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
     * Returns the result of interpreting the object as an instance of '<em>Add</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Add</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseAdd(Add object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Apply</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Apply</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseApply(Apply object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Except</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Except</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseExcept(Except object) {
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
     * Returns the result of interpreting the object as an instance of '<em>Set Operation</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Set Operation</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseSetOperation(SetOperation object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Group</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Group</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseGroup(Group object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Discard</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Discard</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseDiscard(Discard object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Dataset Item</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Dataset Item</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseDatasetItem(DatasetItem object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Param</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Param</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseParam(Param object) {
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
     * Returns the result of interpreting the object as an instance of '<em>Select</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Select</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseSelect(Select object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Deselect</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Deselect</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseDeselect(Deselect object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Add Discard Op</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Add Discard Op</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseAddDiscardOp(AddDiscardOp object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Select Deselect Op</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Select Deselect Op</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseSelectDeselectOp(SelectDeselectOp object) {
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
     * Returns the result of interpreting the object as an instance of '<em>Chart Sheets</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Chart Sheets</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseChartSheets(ChartSheets object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Datasets</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Datasets</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseDatasets(Datasets object) {
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
     * Returns the result of interpreting the object as an instance of '<em>Processing Op</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Processing Op</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseProcessingOp(ProcessingOp object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Compute</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Compute</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseCompute(Compute object) {
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
     * Returns the result of interpreting the object as an instance of '<em>Compute Scalar</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Compute Scalar</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseComputeScalar(ComputeScalar object) {
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
