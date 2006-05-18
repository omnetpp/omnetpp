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
	 * The switch the delegates to the <code>createXXX</code> methods.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ScaveModelSwitch modelSwitch =
		new ScaveModelSwitch() {
			public Object caseDataset(Dataset object) {
				return createDatasetAdapter();
			}
			public Object caseChart(Chart object) {
				return createChartAdapter();
			}
			public Object caseAdd(Add object) {
				return createAddAdapter();
			}
			public Object caseApply(Apply object) {
				return createApplyAdapter();
			}
			public Object caseExcept(Except object) {
				return createExceptAdapter();
			}
			public Object caseProperty(Property object) {
				return createPropertyAdapter();
			}
			public Object caseSetOperation(SetOperation object) {
				return createSetOperationAdapter();
			}
			public Object caseGroup(Group object) {
				return createGroupAdapter();
			}
			public Object caseDiscard(Discard object) {
				return createDiscardAdapter();
			}
			public Object caseDatasetItem(DatasetItem object) {
				return createDatasetItemAdapter();
			}
			public Object caseParam(Param object) {
				return createParamAdapter();
			}
			public Object caseChartSheet(ChartSheet object) {
				return createChartSheetAdapter();
			}
			public Object caseAnalysis(Analysis object) {
				return createAnalysisAdapter();
			}
			public Object caseSelect(Select object) {
				return createSelectAdapter();
			}
			public Object caseDeselect(Deselect object) {
				return createDeselectAdapter();
			}
			public Object caseAddDiscardOp(AddDiscardOp object) {
				return createAddDiscardOpAdapter();
			}
			public Object caseSelectDeselectOp(SelectDeselectOp object) {
				return createSelectDeselectOpAdapter();
			}
			public Object caseInputs(Inputs object) {
				return createInputsAdapter();
			}
			public Object caseChartSheets(ChartSheets object) {
				return createChartSheetsAdapter();
			}
			public Object caseDatasets(Datasets object) {
				return createDatasetsAdapter();
			}
			public Object caseInputFile(InputFile object) {
				return createInputFileAdapter();
			}
			public Object defaultCase(EObject object) {
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
	public Adapter createAdapter(Notifier target) {
		return (Adapter)modelSwitch.doSwitch((EObject)target);
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
