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

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

import org.omnetpp.scave.model.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.scave.model.ScaveModelPackage
 * @generated
 */
public class ScaveModelAdapterFactory extends AdapterFactoryImpl {
    /**
     * The cached model package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected static ScaveModelPackage modelPackage;

    /**
     * Creates an instance of the adapter factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ScaveModelAdapterFactory() {
        if (modelPackage == null) {
            modelPackage = ScaveModelPackage.eINSTANCE;
        }
    }

    /**
     * Returns whether this factory is applicable for the type of the object.
     * <!-- begin-user-doc -->
     * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
     * <!-- end-user-doc -->
     * @return whether this factory is applicable for the type of the object.
     * @generated
     */
    @Override
    public boolean isFactoryForType(Object object) {
        if (object == modelPackage) {
            return true;
        }
        if (object instanceof EObject) {
            return ((EObject)object).eClass().getEPackage() == modelPackage;
        }
        return false;
    }

    /**
     * The switch that delegates to the <code>createXXX</code> methods.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ScaveModelSwitch<Adapter> modelSwitch =
        new ScaveModelSwitch<Adapter>() {
            @Override
            public Adapter caseDataset(Dataset object) {
                return createDatasetAdapter();
            }
            @Override
            public Adapter caseChart(Chart object) {
                return createChartAdapter();
            }
            @Override
            public Adapter caseAdd(Add object) {
                return createAddAdapter();
            }
            @Override
            public Adapter caseDiscard(Discard object) {
                return createDiscardAdapter();
            }
            @Override
            public Adapter caseProcessingOp(ProcessingOp object) {
                return createProcessingOpAdapter();
            }
            @Override
            public Adapter caseExcept(Except object) {
                return createExceptAdapter();
            }
            @Override
            public Adapter caseSetOperation(SetOperation object) {
                return createSetOperationAdapter();
            }
            @Override
            public Adapter caseDatasetItem(DatasetItem object) {
                return createDatasetItemAdapter();
            }
            @Override
            public Adapter caseGroup(Group object) {
                return createGroupAdapter();
            }
            @Override
            public Adapter caseProperty(Property object) {
                return createPropertyAdapter();
            }
            @Override
            public Adapter caseParam(Param object) {
                return createParamAdapter();
            }
            @Override
            public Adapter caseChartSheet(ChartSheet object) {
                return createChartSheetAdapter();
            }
            @Override
            public Adapter caseAnalysis(Analysis object) {
                return createAnalysisAdapter();
            }
            @Override
            public Adapter caseSelect(Select object) {
                return createSelectAdapter();
            }
            @Override
            public Adapter caseDeselect(Deselect object) {
                return createDeselectAdapter();
            }
            @Override
            public Adapter caseAddDiscardOp(AddDiscardOp object) {
                return createAddDiscardOpAdapter();
            }
            @Override
            public Adapter caseSelectDeselectOp(SelectDeselectOp object) {
                return createSelectDeselectOpAdapter();
            }
            @Override
            public Adapter caseInputs(Inputs object) {
                return createInputsAdapter();
            }
            @Override
            public Adapter caseInputFile(InputFile object) {
                return createInputFileAdapter();
            }
            @Override
            public Adapter caseChartSheets(ChartSheets object) {
                return createChartSheetsAdapter();
            }
            @Override
            public Adapter caseDatasets(Datasets object) {
                return createDatasetsAdapter();
            }
            @Override
            public Adapter caseApply(Apply object) {
                return createApplyAdapter();
            }
            @Override
            public Adapter caseCompute(Compute object) {
                return createComputeAdapter();
            }
            @Override
            public Adapter caseBarChart(BarChart object) {
                return createBarChartAdapter();
            }
            @Override
            public Adapter caseLineChart(LineChart object) {
                return createLineChartAdapter();
            }
            @Override
            public Adapter caseHistogramChart(HistogramChart object) {
                return createHistogramChartAdapter();
            }
            @Override
            public Adapter caseScatterChart(ScatterChart object) {
                return createScatterChartAdapter();
            }
            @Override
            public Adapter caseComputeScalar(ComputeScalar object) {
                return createComputeScalarAdapter();
            }
            @Override
            public Adapter defaultCase(EObject object) {
                return createEObjectAdapter();
            }
        };

    /**
     * Creates an adapter for the <code>target</code>.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param target the object to adapt.
     * @return the adapter for the <code>target</code>.
     * @generated
     */
    @Override
    public Adapter createAdapter(Notifier target) {
        return modelSwitch.doSwitch((EObject)target);
    }


    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Dataset <em>Dataset</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Dataset
     * @generated
     */
    public Adapter createDatasetAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Chart <em>Chart</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Chart
     * @generated
     */
    public Adapter createChartAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Add <em>Add</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Add
     * @generated
     */
    public Adapter createAddAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Apply <em>Apply</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Apply
     * @generated
     */
    public Adapter createApplyAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Except <em>Except</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Except
     * @generated
     */
    public Adapter createExceptAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Property <em>Property</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Property
     * @generated
     */
    public Adapter createPropertyAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.SetOperation <em>Set Operation</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.SetOperation
     * @generated
     */
    public Adapter createSetOperationAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Group <em>Group</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Group
     * @generated
     */
    public Adapter createGroupAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Discard <em>Discard</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Discard
     * @generated
     */
    public Adapter createDiscardAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.DatasetItem <em>Dataset Item</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.DatasetItem
     * @generated
     */
    public Adapter createDatasetItemAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Param <em>Param</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Param
     * @generated
     */
    public Adapter createParamAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.ChartSheet <em>Chart Sheet</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.ChartSheet
     * @generated
     */
    public Adapter createChartSheetAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Analysis <em>Analysis</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Analysis
     * @generated
     */
    public Adapter createAnalysisAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Select <em>Select</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Select
     * @generated
     */
    public Adapter createSelectAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Deselect <em>Deselect</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Deselect
     * @generated
     */
    public Adapter createDeselectAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.AddDiscardOp <em>Add Discard Op</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.AddDiscardOp
     * @generated
     */
    public Adapter createAddDiscardOpAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.SelectDeselectOp <em>Select Deselect Op</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.SelectDeselectOp
     * @generated
     */
    public Adapter createSelectDeselectOpAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Inputs <em>Inputs</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Inputs
     * @generated
     */
    public Adapter createInputsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.ChartSheets <em>Chart Sheets</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.ChartSheets
     * @generated
     */
    public Adapter createChartSheetsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Datasets <em>Datasets</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Datasets
     * @generated
     */
    public Adapter createDatasetsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.InputFile <em>Input File</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.InputFile
     * @generated
     */
    public Adapter createInputFileAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.ProcessingOp <em>Processing Op</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.ProcessingOp
     * @generated
     */
    public Adapter createProcessingOpAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Compute <em>Compute</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Compute
     * @generated
     */
    public Adapter createComputeAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.BarChart <em>Bar Chart</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.BarChart
     * @generated
     */
    public Adapter createBarChartAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.LineChart <em>Line Chart</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.LineChart
     * @generated
     */
    public Adapter createLineChartAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.HistogramChart <em>Histogram Chart</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.HistogramChart
     * @generated
     */
    public Adapter createHistogramChartAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.ScatterChart <em>Scatter Chart</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.ScatterChart
     * @generated
     */
    public Adapter createScatterChartAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.ComputeScalar <em>Compute Scalar</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.ComputeScalar
     * @generated
     */
    public Adapter createComputeScalarAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for the default case.
     * <!-- begin-user-doc -->
     * This default implementation returns null.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @generated
     */
    public Adapter createEObjectAdapter() {
        return null;
    }

} //ScaveModelAdapterFactory
