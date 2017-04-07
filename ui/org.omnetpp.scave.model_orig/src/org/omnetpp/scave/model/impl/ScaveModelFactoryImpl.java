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
package org.omnetpp.scave.model.impl;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.eclipse.emf.ecore.plugin.EcorePlugin;

import org.omnetpp.scave.model.*;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class ScaveModelFactoryImpl extends EFactoryImpl implements ScaveModelFactory {
    /**
     * Creates the default factory implementation.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static ScaveModelFactory init() {
        try {
            ScaveModelFactory theScaveModelFactory = (ScaveModelFactory)EPackage.Registry.INSTANCE.getEFactory(ScaveModelPackage.eNS_URI);
            if (theScaveModelFactory != null) {
                return theScaveModelFactory;
            }
        }
        catch (Exception exception) {
            EcorePlugin.INSTANCE.log(exception);
        }
        return new ScaveModelFactoryImpl();
    }

    /**
     * Creates an instance of the factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ScaveModelFactoryImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public EObject create(EClass eClass) {
        switch (eClass.getClassifierID()) {
            case ScaveModelPackage.DATASET: return createDataset();
            case ScaveModelPackage.ADD: return createAdd();
            case ScaveModelPackage.DISCARD: return createDiscard();
            case ScaveModelPackage.EXCEPT: return createExcept();
            case ScaveModelPackage.GROUP: return createGroup();
            case ScaveModelPackage.PROPERTY: return createProperty();
            case ScaveModelPackage.PARAM: return createParam();
            case ScaveModelPackage.CHART_SHEET: return createChartSheet();
            case ScaveModelPackage.ANALYSIS: return createAnalysis();
            case ScaveModelPackage.SELECT: return createSelect();
            case ScaveModelPackage.DESELECT: return createDeselect();
            case ScaveModelPackage.INPUTS: return createInputs();
            case ScaveModelPackage.INPUT_FILE: return createInputFile();
            case ScaveModelPackage.CHART_SHEETS: return createChartSheets();
            case ScaveModelPackage.DATASETS: return createDatasets();
            case ScaveModelPackage.APPLY: return createApply();
            case ScaveModelPackage.COMPUTE: return createCompute();
            case ScaveModelPackage.BAR_CHART: return createBarChart();
            case ScaveModelPackage.LINE_CHART: return createLineChart();
            case ScaveModelPackage.HISTOGRAM_CHART: return createHistogramChart();
            case ScaveModelPackage.SCATTER_CHART: return createScatterChart();
            case ScaveModelPackage.COMPUTE_SCALAR: return createComputeScalar();
            default:
                throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
        }
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object createFromString(EDataType eDataType, String initialValue) {
        switch (eDataType.getClassifierID()) {
            case ScaveModelPackage.RESULT_TYPE:
                return createResultTypeFromString(eDataType, initialValue);
            default:
                throw new IllegalArgumentException("The datatype '" + eDataType.getName() + "' is not a valid classifier");
        }
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public String convertToString(EDataType eDataType, Object instanceValue) {
        switch (eDataType.getClassifierID()) {
            case ScaveModelPackage.RESULT_TYPE:
                return convertResultTypeToString(eDataType, instanceValue);
            default:
                throw new IllegalArgumentException("The datatype '" + eDataType.getName() + "' is not a valid classifier");
        }
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Dataset createDataset() {
        DatasetImpl dataset = new DatasetImpl();
        return dataset;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Add createAdd() {
        AddImpl add = new AddImpl();
        return add;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Apply createApply() {
        ApplyImpl apply = new ApplyImpl();
        return apply;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Except createExcept() {
        ExceptImpl except = new ExceptImpl();
        return except;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Property createProperty() {
        PropertyImpl property = new PropertyImpl();
        return property;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Group createGroup() {
        GroupImpl group = new GroupImpl();
        return group;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Discard createDiscard() {
        DiscardImpl discard = new DiscardImpl();
        return discard;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Param createParam() {
        ParamImpl param = new ParamImpl();
        return param;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ChartSheet createChartSheet() {
        ChartSheetImpl chartSheet = new ChartSheetImpl();
        return chartSheet;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Analysis createAnalysis() {
        AnalysisImpl analysis = new AnalysisImpl();
        return analysis;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Select createSelect() {
        SelectImpl select = new SelectImpl();
        return select;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Deselect createDeselect() {
        DeselectImpl deselect = new DeselectImpl();
        return deselect;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Inputs createInputs() {
        InputsImpl inputs = new InputsImpl();
        return inputs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ChartSheets createChartSheets() {
        ChartSheetsImpl chartSheets = new ChartSheetsImpl();
        return chartSheets;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Datasets createDatasets() {
        DatasetsImpl datasets = new DatasetsImpl();
        return datasets;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public InputFile createInputFile() {
        InputFileImpl inputFile = new InputFileImpl();
        return inputFile;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Compute createCompute() {
        ComputeImpl compute = new ComputeImpl();
        return compute;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public BarChart createBarChart() {
        BarChartImpl barChart = new BarChartImpl();
        return barChart;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public LineChart createLineChart() {
        LineChartImpl lineChart = new LineChartImpl();
        return lineChart;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public HistogramChart createHistogramChart() {
        HistogramChartImpl histogramChart = new HistogramChartImpl();
        return histogramChart;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ScatterChart createScatterChart() {
        ScatterChartImpl scatterChart = new ScatterChartImpl();
        return scatterChart;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ComputeScalar createComputeScalar() {
        ComputeScalarImpl computeScalar = new ComputeScalarImpl();
        return computeScalar;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ResultType createResultTypeFromString(EDataType eDataType, String initialValue) {
        ResultType result = ResultType.get(initialValue);
        if (result == null) throw new IllegalArgumentException("The value '" + initialValue + "' is not a valid enumerator of '" + eDataType.getName() + "'");
        return result;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String convertResultTypeToString(EDataType eDataType, Object instanceValue) {
        return instanceValue == null ? null : instanceValue.toString();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ScaveModelPackage getScaveModelPackage() {
        return (ScaveModelPackage)getEPackage();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @deprecated
     * @generated
     */
    @Deprecated
    public static ScaveModelPackage getPackage() {
        return ScaveModelPackage.eINSTANCE;
    }

} //ScaveModelFactoryImpl
