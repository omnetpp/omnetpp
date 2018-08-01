/**
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
            public Adapter caseAnalysis(Analysis object) {
                return createAnalysisAdapter();
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
            public Adapter caseCharts(Charts object) {
                return createChartsAdapter();
            }
            @Override
            public Adapter caseAnalysisItem(AnalysisItem object) {
                return createAnalysisItemAdapter();
            }
            @Override
            public Adapter caseChart(Chart object) {
                return createChartAdapter();
            }
            @Override
            public Adapter caseProperty(Property object) {
                return createPropertyAdapter();
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
            public Adapter caseDataset(Dataset object) {
                return createDatasetAdapter();
            }
            @Override
            public Adapter caseChartSheet(ChartSheet object) {
                return createChartSheetAdapter();
            }
            @Override
            public Adapter caseFolder(Folder object) {
                return createFolderAdapter();
            }
            @Override
            public Adapter caseMatplotlibChart(MatplotlibChart object) {
                return createMatplotlibChartAdapter();
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
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Charts <em>Charts</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Charts
     * @generated
     */
    public Adapter createChartsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.AnalysisItem <em>Analysis Item</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.AnalysisItem
     * @generated
     */
    public Adapter createAnalysisItemAdapter() {
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
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.Folder <em>Folder</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.Folder
     * @generated
     */
    public Adapter createFolderAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.scave.model.MatplotlibChart <em>Matplotlib Chart</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.scave.model.MatplotlibChart
     * @generated
     */
    public Adapter createMatplotlibChartAdapter() {
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
