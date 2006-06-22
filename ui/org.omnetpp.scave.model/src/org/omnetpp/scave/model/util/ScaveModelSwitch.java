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
public class ScaveModelSwitch {
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
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	public Object doSwitch(EObject theEObject) {
		return doSwitch(theEObject.eClass(), theEObject);
	}

	/**
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	protected Object doSwitch(EClass theEClass, EObject theEObject) {
		if (theEClass.eContainer() == modelPackage) {
			return doSwitch(theEClass.getClassifierID(), theEObject);
		}
		else {
			List eSuperTypes = theEClass.getESuperTypes();
			return
				eSuperTypes.isEmpty() ?
					defaultCase(theEObject) :
					doSwitch((EClass)eSuperTypes.get(0), theEObject);
		}
	}

	/**
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	protected Object doSwitch(int classifierID, EObject theEObject) {
		switch (classifierID) {
			case ScaveModelPackage.DATASET: {
				Dataset dataset = (Dataset)theEObject;
				Object result = caseDataset(dataset);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.CHART: {
				Chart chart = (Chart)theEObject;
				Object result = caseChart(chart);
				if (result == null) result = caseDatasetItem(chart);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.ADD: {
				Add add = (Add)theEObject;
				Object result = caseAdd(add);
				if (result == null) result = caseAddDiscardOp(add);
				if (result == null) result = caseSetOperation(add);
				if (result == null) result = caseDatasetItem(add);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.APPLY: {
				Apply apply = (Apply)theEObject;
				Object result = caseApply(apply);
				if (result == null) result = caseProcessingOp(apply);
				if (result == null) result = caseDatasetItem(apply);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.EXCEPT: {
				Except except = (Except)theEObject;
				Object result = caseExcept(except);
				if (result == null) result = caseSetOperation(except);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.PROPERTY: {
				Property property = (Property)theEObject;
				Object result = caseProperty(property);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.SET_OPERATION: {
				SetOperation setOperation = (SetOperation)theEObject;
				Object result = caseSetOperation(setOperation);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.GROUP: {
				Group group = (Group)theEObject;
				Object result = caseGroup(group);
				if (result == null) result = caseDatasetItem(group);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.DISCARD: {
				Discard discard = (Discard)theEObject;
				Object result = caseDiscard(discard);
				if (result == null) result = caseAddDiscardOp(discard);
				if (result == null) result = caseSetOperation(discard);
				if (result == null) result = caseDatasetItem(discard);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.DATASET_ITEM: {
				DatasetItem datasetItem = (DatasetItem)theEObject;
				Object result = caseDatasetItem(datasetItem);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.PARAM: {
				Param param = (Param)theEObject;
				Object result = caseParam(param);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.CHART_SHEET: {
				ChartSheet chartSheet = (ChartSheet)theEObject;
				Object result = caseChartSheet(chartSheet);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.ANALYSIS: {
				Analysis analysis = (Analysis)theEObject;
				Object result = caseAnalysis(analysis);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.SELECT: {
				Select select = (Select)theEObject;
				Object result = caseSelect(select);
				if (result == null) result = caseSelectDeselectOp(select);
				if (result == null) result = caseSetOperation(select);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.DESELECT: {
				Deselect deselect = (Deselect)theEObject;
				Object result = caseDeselect(deselect);
				if (result == null) result = caseSelectDeselectOp(deselect);
				if (result == null) result = caseSetOperation(deselect);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.ADD_DISCARD_OP: {
				AddDiscardOp addDiscardOp = (AddDiscardOp)theEObject;
				Object result = caseAddDiscardOp(addDiscardOp);
				if (result == null) result = caseSetOperation(addDiscardOp);
				if (result == null) result = caseDatasetItem(addDiscardOp);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.SELECT_DESELECT_OP: {
				SelectDeselectOp selectDeselectOp = (SelectDeselectOp)theEObject;
				Object result = caseSelectDeselectOp(selectDeselectOp);
				if (result == null) result = caseSetOperation(selectDeselectOp);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.INPUTS: {
				Inputs inputs = (Inputs)theEObject;
				Object result = caseInputs(inputs);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.CHART_SHEETS: {
				ChartSheets chartSheets = (ChartSheets)theEObject;
				Object result = caseChartSheets(chartSheets);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.DATASETS: {
				Datasets datasets = (Datasets)theEObject;
				Object result = caseDatasets(datasets);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.INPUT_FILE: {
				InputFile inputFile = (InputFile)theEObject;
				Object result = caseInputFile(inputFile);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.PROCESSING_OP: {
				ProcessingOp processingOp = (ProcessingOp)theEObject;
				Object result = caseProcessingOp(processingOp);
				if (result == null) result = caseDatasetItem(processingOp);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ScaveModelPackage.COMPUTE: {
				Compute compute = (Compute)theEObject;
				Object result = caseCompute(compute);
				if (result == null) result = caseProcessingOp(compute);
				if (result == null) result = caseDatasetItem(compute);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			default: return defaultCase(theEObject);
		}
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Dataset</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Dataset</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseDataset(Dataset object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Chart</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Chart</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseChart(Chart object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Add</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Add</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseAdd(Add object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Apply</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Apply</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseApply(Apply object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Except</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Except</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseExcept(Except object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Property</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Property</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseProperty(Property object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Set Operation</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Set Operation</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseSetOperation(SetOperation object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Group</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Group</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseGroup(Group object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Discard</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Discard</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseDiscard(Discard object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Dataset Item</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Dataset Item</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseDatasetItem(DatasetItem object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Param</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Param</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseParam(Param object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Chart Sheet</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Chart Sheet</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseChartSheet(ChartSheet object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Analysis</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Analysis</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseAnalysis(Analysis object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Select</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Select</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseSelect(Select object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Deselect</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Deselect</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseDeselect(Deselect object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Add Discard Op</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Add Discard Op</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseAddDiscardOp(AddDiscardOp object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Select Deselect Op</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Select Deselect Op</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseSelectDeselectOp(SelectDeselectOp object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Inputs</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Inputs</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseInputs(Inputs object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Chart Sheets</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Chart Sheets</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseChartSheets(ChartSheets object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Datasets</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Datasets</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseDatasets(Datasets object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Input File</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Input File</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseInputFile(InputFile object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Processing Op</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Processing Op</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseProcessingOp(ProcessingOp object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Compute</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Compute</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseCompute(Compute object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>EObject</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch, but this is the last case anyway.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>EObject</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject)
	 * @generated
	 */
	public Object defaultCase(EObject object) {
		return null;
	}

} //ScaveModelSwitch
