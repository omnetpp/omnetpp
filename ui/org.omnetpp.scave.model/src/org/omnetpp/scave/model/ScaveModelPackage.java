/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EEnum;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see org.omnetpp.scave.model.ScaveModelFactory
 * @model kind="package"
 * @generated
 */
public interface ScaveModelPackage extends EPackage {
	/**
	 * The package name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNAME = "model";

	/**
	 * The package namespace URI.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_URI = "http://www.omnetpp.org/omnetpp/scave";

	/**
	 * The package namespace name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_PREFIX = "scave";

	/**
	 * The singleton instance of the package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	ScaveModelPackage eINSTANCE = org.omnetpp.scave.model.impl.ScaveModelPackageImpl.init();

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.DatasetImpl <em>Dataset</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.DatasetImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDataset()
	 * @generated
	 */
	int DATASET = 0;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASET__NAME = 0;

	/**
	 * The feature id for the '<em><b>Items</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASET__ITEMS = 1;

	/**
	 * The feature id for the '<em><b>Based On</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASET__BASED_ON = 2;

	/**
	 * The number of structural features of the '<em>Dataset</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASET_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.DatasetItemImpl <em>Dataset Item</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.DatasetItemImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDatasetItem()
	 * @generated
	 */
	int DATASET_ITEM = 7;

	/**
	 * The number of structural features of the '<em>Dataset Item</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASET_ITEM_FEATURE_COUNT = 0;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ChartImpl <em>Chart</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ChartImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChart()
	 * @generated
	 */
	int CHART = 1;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART__NAME = DATASET_ITEM_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART__FILTERS = DATASET_ITEM_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Properties</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART__PROPERTIES = DATASET_ITEM_FEATURE_COUNT + 2;

	/**
	 * The number of structural features of the '<em>Chart</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART_FEATURE_COUNT = DATASET_ITEM_FEATURE_COUNT + 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.SetOperationImpl <em>Set Operation</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.SetOperationImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getSetOperation()
	 * @generated
	 */
	int SET_OPERATION = 6;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SET_OPERATION__SOURCE_DATASET = 0;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SET_OPERATION__FILTER_PATTERN = 1;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SET_OPERATION__TYPE = 2;

	/**
	 * The number of structural features of the '<em>Set Operation</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SET_OPERATION_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.AddDiscardOpImpl <em>Add Discard Op</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.AddDiscardOpImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAddDiscardOp()
	 * @generated
	 */
	int ADD_DISCARD_OP = 15;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD_DISCARD_OP__SOURCE_DATASET = SET_OPERATION__SOURCE_DATASET;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD_DISCARD_OP__FILTER_PATTERN = SET_OPERATION__FILTER_PATTERN;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD_DISCARD_OP__TYPE = SET_OPERATION__TYPE;

	/**
	 * The number of structural features of the '<em>Add Discard Op</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD_DISCARD_OP_FEATURE_COUNT = SET_OPERATION_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.AddImpl <em>Add</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.AddImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAdd()
	 * @generated
	 */
	int ADD = 2;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD__SOURCE_DATASET = ADD_DISCARD_OP__SOURCE_DATASET;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD__FILTER_PATTERN = ADD_DISCARD_OP__FILTER_PATTERN;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD__TYPE = ADD_DISCARD_OP__TYPE;

	/**
	 * The feature id for the '<em><b>Excepts</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD__EXCEPTS = ADD_DISCARD_OP_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Add</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADD_FEATURE_COUNT = ADD_DISCARD_OP_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ProcessingOpImpl <em>Processing Op</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ProcessingOpImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getProcessingOp()
	 * @generated
	 */
	int PROCESSING_OP = 4;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ApplyImpl <em>Apply</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ApplyImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getApply()
	 * @generated
	 */
	int APPLY = 21;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ExceptImpl <em>Except</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ExceptImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getExcept()
	 * @generated
	 */
	int EXCEPT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.PropertyImpl <em>Property</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.PropertyImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getProperty()
	 * @generated
	 */
	int PROPERTY = 9;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.GroupImpl <em>Group</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.GroupImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getGroup()
	 * @generated
	 */
	int GROUP = 8;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.DiscardImpl <em>Discard</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.DiscardImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDiscard()
	 * @generated
	 */
	int DISCARD = 3;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DISCARD__SOURCE_DATASET = ADD_DISCARD_OP__SOURCE_DATASET;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DISCARD__FILTER_PATTERN = ADD_DISCARD_OP__FILTER_PATTERN;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DISCARD__TYPE = ADD_DISCARD_OP__TYPE;

	/**
	 * The feature id for the '<em><b>Excepts</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DISCARD__EXCEPTS = ADD_DISCARD_OP_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Discard</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DISCARD_FEATURE_COUNT = ADD_DISCARD_OP_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Operation</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROCESSING_OP__OPERATION = DATASET_ITEM_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROCESSING_OP__FILTERS = DATASET_ITEM_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Params</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROCESSING_OP__PARAMS = DATASET_ITEM_FEATURE_COUNT + 2;

	/**
	 * The feature id for the '<em><b>Computed File</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROCESSING_OP__COMPUTED_FILE = DATASET_ITEM_FEATURE_COUNT + 3;

	/**
	 * The feature id for the '<em><b>Computation Hash</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROCESSING_OP__COMPUTATION_HASH = DATASET_ITEM_FEATURE_COUNT + 4;

	/**
	 * The number of structural features of the '<em>Processing Op</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROCESSING_OP_FEATURE_COUNT = DATASET_ITEM_FEATURE_COUNT + 5;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EXCEPT__FILTER_PATTERN = 0;

	/**
	 * The number of structural features of the '<em>Except</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EXCEPT_FEATURE_COUNT = 1;

	/**
	 * The feature id for the '<em><b>Items</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int GROUP__ITEMS = DATASET_ITEM_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int GROUP__NAME = DATASET_ITEM_FEATURE_COUNT + 1;

	/**
	 * The number of structural features of the '<em>Group</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int GROUP_FEATURE_COUNT = DATASET_ITEM_FEATURE_COUNT + 2;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROPERTY__NAME = 0;

	/**
	 * The feature id for the '<em><b>Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROPERTY__VALUE = 1;

	/**
	 * The number of structural features of the '<em>Property</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PROPERTY_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ParamImpl <em>Param</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ParamImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getParam()
	 * @generated
	 */
	int PARAM = 10;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PARAM__NAME = 0;

	/**
	 * The feature id for the '<em><b>Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PARAM__VALUE = 1;

	/**
	 * The number of structural features of the '<em>Param</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PARAM_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ChartSheetImpl <em>Chart Sheet</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ChartSheetImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChartSheet()
	 * @generated
	 */
	int CHART_SHEET = 11;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART_SHEET__NAME = 0;

	/**
	 * The feature id for the '<em><b>Charts</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART_SHEET__CHARTS = 1;

	/**
	 * The number of structural features of the '<em>Chart Sheet</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART_SHEET_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.AnalysisImpl <em>Analysis</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.AnalysisImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAnalysis()
	 * @generated
	 */
	int ANALYSIS = 12;

	/**
	 * The feature id for the '<em><b>Inputs</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ANALYSIS__INPUTS = 0;

	/**
	 * The feature id for the '<em><b>Datasets</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ANALYSIS__DATASETS = 1;

	/**
	 * The feature id for the '<em><b>Chart Sheets</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ANALYSIS__CHART_SHEETS = 2;

	/**
	 * The number of structural features of the '<em>Analysis</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ANALYSIS_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.SelectDeselectOpImpl <em>Select Deselect Op</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.SelectDeselectOpImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getSelectDeselectOp()
	 * @generated
	 */
	int SELECT_DESELECT_OP = 16;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT_DESELECT_OP__SOURCE_DATASET = SET_OPERATION__SOURCE_DATASET;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT_DESELECT_OP__FILTER_PATTERN = SET_OPERATION__FILTER_PATTERN;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT_DESELECT_OP__TYPE = SET_OPERATION__TYPE;

	/**
	 * The number of structural features of the '<em>Select Deselect Op</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT_DESELECT_OP_FEATURE_COUNT = SET_OPERATION_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.SelectImpl <em>Select</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.SelectImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getSelect()
	 * @generated
	 */
	int SELECT = 13;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT__SOURCE_DATASET = SELECT_DESELECT_OP__SOURCE_DATASET;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT__FILTER_PATTERN = SELECT_DESELECT_OP__FILTER_PATTERN;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT__TYPE = SELECT_DESELECT_OP__TYPE;

	/**
	 * The feature id for the '<em><b>Excepts</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT__EXCEPTS = SELECT_DESELECT_OP_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Select</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SELECT_FEATURE_COUNT = SELECT_DESELECT_OP_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.DeselectImpl <em>Deselect</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.DeselectImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDeselect()
	 * @generated
	 */
	int DESELECT = 14;

	/**
	 * The feature id for the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DESELECT__SOURCE_DATASET = SELECT_DESELECT_OP__SOURCE_DATASET;

	/**
	 * The feature id for the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DESELECT__FILTER_PATTERN = SELECT_DESELECT_OP__FILTER_PATTERN;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DESELECT__TYPE = SELECT_DESELECT_OP__TYPE;

	/**
	 * The feature id for the '<em><b>Excepts</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DESELECT__EXCEPTS = SELECT_DESELECT_OP_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Deselect</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DESELECT_FEATURE_COUNT = SELECT_DESELECT_OP_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.InputsImpl <em>Inputs</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.InputsImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getInputs()
	 * @generated
	 */
	int INPUTS = 17;

	/**
	 * The feature id for the '<em><b>Inputs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int INPUTS__INPUTS = 0;

	/**
	 * The number of structural features of the '<em>Inputs</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int INPUTS_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ChartSheetsImpl <em>Chart Sheets</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ChartSheetsImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChartSheets()
	 * @generated
	 */
	int CHART_SHEETS = 19;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.DatasetsImpl <em>Datasets</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.DatasetsImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDatasets()
	 * @generated
	 */
	int DATASETS = 20;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.InputFileImpl <em>Input File</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.InputFileImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getInputFile()
	 * @generated
	 */
	int INPUT_FILE = 18;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int INPUT_FILE__NAME = 0;

	/**
	 * The number of structural features of the '<em>Input File</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int INPUT_FILE_FEATURE_COUNT = 1;

	/**
	 * The feature id for the '<em><b>Chart Sheets</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART_SHEETS__CHART_SHEETS = 0;

	/**
	 * The number of structural features of the '<em>Chart Sheets</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CHART_SHEETS_FEATURE_COUNT = 1;

	/**
	 * The feature id for the '<em><b>Datasets</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASETS__DATASETS = 0;

	/**
	 * The number of structural features of the '<em>Datasets</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DATASETS_FEATURE_COUNT = 1;

	/**
	 * The feature id for the '<em><b>Operation</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int APPLY__OPERATION = PROCESSING_OP__OPERATION;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int APPLY__FILTERS = PROCESSING_OP__FILTERS;

	/**
	 * The feature id for the '<em><b>Params</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int APPLY__PARAMS = PROCESSING_OP__PARAMS;

	/**
	 * The feature id for the '<em><b>Computed File</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int APPLY__COMPUTED_FILE = PROCESSING_OP__COMPUTED_FILE;

	/**
	 * The feature id for the '<em><b>Computation Hash</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int APPLY__COMPUTATION_HASH = PROCESSING_OP__COMPUTATION_HASH;

	/**
	 * The number of structural features of the '<em>Apply</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int APPLY_FEATURE_COUNT = PROCESSING_OP_FEATURE_COUNT + 0;


	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ComputeImpl <em>Compute</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ComputeImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getCompute()
	 * @generated
	 */
	int COMPUTE = 22;

	/**
	 * The feature id for the '<em><b>Operation</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int COMPUTE__OPERATION = PROCESSING_OP__OPERATION;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int COMPUTE__FILTERS = PROCESSING_OP__FILTERS;

	/**
	 * The feature id for the '<em><b>Params</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int COMPUTE__PARAMS = PROCESSING_OP__PARAMS;

	/**
	 * The feature id for the '<em><b>Computed File</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int COMPUTE__COMPUTED_FILE = PROCESSING_OP__COMPUTED_FILE;

	/**
	 * The feature id for the '<em><b>Computation Hash</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int COMPUTE__COMPUTATION_HASH = PROCESSING_OP__COMPUTATION_HASH;

	/**
	 * The number of structural features of the '<em>Compute</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int COMPUTE_FEATURE_COUNT = PROCESSING_OP_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.BarChartImpl <em>Bar Chart</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.BarChartImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getBarChart()
	 * @generated
	 */
	int BAR_CHART = 23;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__NAME = CHART__NAME;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__FILTERS = CHART__FILTERS;

	/**
	 * The feature id for the '<em><b>Properties</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__PROPERTIES = CHART__PROPERTIES;

	/**
	 * The feature id for the '<em><b>Group By</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__GROUP_BY = CHART_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Group Name Format</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__GROUP_NAME_FORMAT = CHART_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Bar Name Format</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__BAR_NAME_FORMAT = CHART_FEATURE_COUNT + 2;

	/**
	 * The feature id for the '<em><b>Bar Fields</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART__BAR_FIELDS = CHART_FEATURE_COUNT + 3;

	/**
	 * The number of structural features of the '<em>Bar Chart</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int BAR_CHART_FEATURE_COUNT = CHART_FEATURE_COUNT + 4;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.LineChartImpl <em>Line Chart</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.LineChartImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getLineChart()
	 * @generated
	 */
	int LINE_CHART = 24;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int LINE_CHART__NAME = CHART__NAME;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int LINE_CHART__FILTERS = CHART__FILTERS;

	/**
	 * The feature id for the '<em><b>Properties</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int LINE_CHART__PROPERTIES = CHART__PROPERTIES;

	/**
	 * The feature id for the '<em><b>Line Name Format</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int LINE_CHART__LINE_NAME_FORMAT = CHART_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Line Chart</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int LINE_CHART_FEATURE_COUNT = CHART_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.HistogramChartImpl <em>Histogram Chart</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.HistogramChartImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getHistogramChart()
	 * @generated
	 */
	int HISTOGRAM_CHART = 25;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int HISTOGRAM_CHART__NAME = CHART__NAME;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int HISTOGRAM_CHART__FILTERS = CHART__FILTERS;

	/**
	 * The feature id for the '<em><b>Properties</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int HISTOGRAM_CHART__PROPERTIES = CHART__PROPERTIES;

	/**
	 * The number of structural features of the '<em>Histogram Chart</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int HISTOGRAM_CHART_FEATURE_COUNT = CHART_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.impl.ScatterChartImpl <em>Scatter Chart</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.impl.ScatterChartImpl
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getScatterChart()
	 * @generated
	 */
	int SCATTER_CHART = 26;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART__NAME = CHART__NAME;

	/**
	 * The feature id for the '<em><b>Filters</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART__FILTERS = CHART__FILTERS;

	/**
	 * The feature id for the '<em><b>Properties</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART__PROPERTIES = CHART__PROPERTIES;

	/**
	 * The feature id for the '<em><b>XData Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART__XDATA_PATTERN = CHART_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Iso Data Pattern</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART__ISO_DATA_PATTERN = CHART_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Average Replications</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART__AVERAGE_REPLICATIONS = CHART_FEATURE_COUNT + 2;

	/**
	 * The number of structural features of the '<em>Scatter Chart</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SCATTER_CHART_FEATURE_COUNT = CHART_FEATURE_COUNT + 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.scave.model.ResultType <em>Result Type</em>}' enum.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.scave.model.ResultType
	 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getResultType()
	 * @generated
	 */
	int RESULT_TYPE = 27;


	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Dataset <em>Dataset</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Dataset</em>'.
	 * @see org.omnetpp.scave.model.Dataset
	 * @generated
	 */
	EClass getDataset();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Dataset#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.Dataset#getName()
	 * @see #getDataset()
	 * @generated
	 */
	EAttribute getDataset_Name();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Dataset#getItems <em>Items</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Items</em>'.
	 * @see org.omnetpp.scave.model.Dataset#getItems()
	 * @see #getDataset()
	 * @generated
	 */
	EReference getDataset_Items();

	/**
	 * Returns the meta object for the reference '{@link org.omnetpp.scave.model.Dataset#getBasedOn <em>Based On</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Based On</em>'.
	 * @see org.omnetpp.scave.model.Dataset#getBasedOn()
	 * @see #getDataset()
	 * @generated
	 */
	EReference getDataset_BasedOn();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Chart <em>Chart</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Chart</em>'.
	 * @see org.omnetpp.scave.model.Chart
	 * @generated
	 */
	EClass getChart();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Chart#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.Chart#getName()
	 * @see #getChart()
	 * @generated
	 */
	EAttribute getChart_Name();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Chart#getFilters <em>Filters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Filters</em>'.
	 * @see org.omnetpp.scave.model.Chart#getFilters()
	 * @see #getChart()
	 * @generated
	 */
	EReference getChart_Filters();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Chart#getProperties <em>Properties</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Properties</em>'.
	 * @see org.omnetpp.scave.model.Chart#getProperties()
	 * @see #getChart()
	 * @generated
	 */
	EReference getChart_Properties();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Add <em>Add</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Add</em>'.
	 * @see org.omnetpp.scave.model.Add
	 * @generated
	 */
	EClass getAdd();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Add#getExcepts <em>Excepts</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Excepts</em>'.
	 * @see org.omnetpp.scave.model.Add#getExcepts()
	 * @see #getAdd()
	 * @generated
	 */
	EReference getAdd_Excepts();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Apply <em>Apply</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Apply</em>'.
	 * @see org.omnetpp.scave.model.Apply
	 * @generated
	 */
	EClass getApply();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Except <em>Except</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Except</em>'.
	 * @see org.omnetpp.scave.model.Except
	 * @generated
	 */
	EClass getExcept();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Except#getFilterPattern <em>Filter Pattern</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Filter Pattern</em>'.
	 * @see org.omnetpp.scave.model.Except#getFilterPattern()
	 * @see #getExcept()
	 * @generated
	 */
	EAttribute getExcept_FilterPattern();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Property <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Property</em>'.
	 * @see org.omnetpp.scave.model.Property
	 * @generated
	 */
	EClass getProperty();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Property#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.Property#getName()
	 * @see #getProperty()
	 * @generated
	 */
	EAttribute getProperty_Name();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Property#getValue <em>Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Value</em>'.
	 * @see org.omnetpp.scave.model.Property#getValue()
	 * @see #getProperty()
	 * @generated
	 */
	EAttribute getProperty_Value();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.SetOperation <em>Set Operation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Set Operation</em>'.
	 * @see org.omnetpp.scave.model.SetOperation
	 * @generated
	 */
	EClass getSetOperation();

	/**
	 * Returns the meta object for the reference '{@link org.omnetpp.scave.model.SetOperation#getSourceDataset <em>Source Dataset</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Source Dataset</em>'.
	 * @see org.omnetpp.scave.model.SetOperation#getSourceDataset()
	 * @see #getSetOperation()
	 * @generated
	 */
	EReference getSetOperation_SourceDataset();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.SetOperation#getFilterPattern <em>Filter Pattern</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Filter Pattern</em>'.
	 * @see org.omnetpp.scave.model.SetOperation#getFilterPattern()
	 * @see #getSetOperation()
	 * @generated
	 */
	EAttribute getSetOperation_FilterPattern();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.SetOperation#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see org.omnetpp.scave.model.SetOperation#getType()
	 * @see #getSetOperation()
	 * @generated
	 */
	EAttribute getSetOperation_Type();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Group <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Group</em>'.
	 * @see org.omnetpp.scave.model.Group
	 * @generated
	 */
	EClass getGroup();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Group#getItems <em>Items</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Items</em>'.
	 * @see org.omnetpp.scave.model.Group#getItems()
	 * @see #getGroup()
	 * @generated
	 */
	EReference getGroup_Items();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Group#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.Group#getName()
	 * @see #getGroup()
	 * @generated
	 */
	EAttribute getGroup_Name();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Discard <em>Discard</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Discard</em>'.
	 * @see org.omnetpp.scave.model.Discard
	 * @generated
	 */
	EClass getDiscard();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Discard#getExcepts <em>Excepts</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Excepts</em>'.
	 * @see org.omnetpp.scave.model.Discard#getExcepts()
	 * @see #getDiscard()
	 * @generated
	 */
	EReference getDiscard_Excepts();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.DatasetItem <em>Dataset Item</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Dataset Item</em>'.
	 * @see org.omnetpp.scave.model.DatasetItem
	 * @generated
	 */
	EClass getDatasetItem();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Param <em>Param</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Param</em>'.
	 * @see org.omnetpp.scave.model.Param
	 * @generated
	 */
	EClass getParam();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Param#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.Param#getName()
	 * @see #getParam()
	 * @generated
	 */
	EAttribute getParam_Name();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Param#getValue <em>Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Value</em>'.
	 * @see org.omnetpp.scave.model.Param#getValue()
	 * @see #getParam()
	 * @generated
	 */
	EAttribute getParam_Value();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.ChartSheet <em>Chart Sheet</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Chart Sheet</em>'.
	 * @see org.omnetpp.scave.model.ChartSheet
	 * @generated
	 */
	EClass getChartSheet();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.ChartSheet#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.ChartSheet#getName()
	 * @see #getChartSheet()
	 * @generated
	 */
	EAttribute getChartSheet_Name();

	/**
	 * Returns the meta object for the reference list '{@link org.omnetpp.scave.model.ChartSheet#getCharts <em>Charts</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Charts</em>'.
	 * @see org.omnetpp.scave.model.ChartSheet#getCharts()
	 * @see #getChartSheet()
	 * @generated
	 */
	EReference getChartSheet_Charts();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Analysis <em>Analysis</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Analysis</em>'.
	 * @see org.omnetpp.scave.model.Analysis
	 * @generated
	 */
	EClass getAnalysis();

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.scave.model.Analysis#getInputs <em>Inputs</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Inputs</em>'.
	 * @see org.omnetpp.scave.model.Analysis#getInputs()
	 * @see #getAnalysis()
	 * @generated
	 */
	EReference getAnalysis_Inputs();

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.scave.model.Analysis#getDatasets <em>Datasets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Datasets</em>'.
	 * @see org.omnetpp.scave.model.Analysis#getDatasets()
	 * @see #getAnalysis()
	 * @generated
	 */
	EReference getAnalysis_Datasets();

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.scave.model.Analysis#getChartSheets <em>Chart Sheets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Chart Sheets</em>'.
	 * @see org.omnetpp.scave.model.Analysis#getChartSheets()
	 * @see #getAnalysis()
	 * @generated
	 */
	EReference getAnalysis_ChartSheets();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Select <em>Select</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Select</em>'.
	 * @see org.omnetpp.scave.model.Select
	 * @generated
	 */
	EClass getSelect();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Select#getExcepts <em>Excepts</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Excepts</em>'.
	 * @see org.omnetpp.scave.model.Select#getExcepts()
	 * @see #getSelect()
	 * @generated
	 */
	EReference getSelect_Excepts();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Deselect <em>Deselect</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Deselect</em>'.
	 * @see org.omnetpp.scave.model.Deselect
	 * @generated
	 */
	EClass getDeselect();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Deselect#getExcepts <em>Excepts</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Excepts</em>'.
	 * @see org.omnetpp.scave.model.Deselect#getExcepts()
	 * @see #getDeselect()
	 * @generated
	 */
	EReference getDeselect_Excepts();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.AddDiscardOp <em>Add Discard Op</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Add Discard Op</em>'.
	 * @see org.omnetpp.scave.model.AddDiscardOp
	 * @generated
	 */
	EClass getAddDiscardOp();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.SelectDeselectOp <em>Select Deselect Op</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Select Deselect Op</em>'.
	 * @see org.omnetpp.scave.model.SelectDeselectOp
	 * @generated
	 */
	EClass getSelectDeselectOp();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Inputs <em>Inputs</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Inputs</em>'.
	 * @see org.omnetpp.scave.model.Inputs
	 * @generated
	 */
	EClass getInputs();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Inputs#getInputs <em>Inputs</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Inputs</em>'.
	 * @see org.omnetpp.scave.model.Inputs#getInputs()
	 * @see #getInputs()
	 * @generated
	 */
	EReference getInputs_Inputs();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.ChartSheets <em>Chart Sheets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Chart Sheets</em>'.
	 * @see org.omnetpp.scave.model.ChartSheets
	 * @generated
	 */
	EClass getChartSheets();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.ChartSheets#getChartSheets <em>Chart Sheets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Chart Sheets</em>'.
	 * @see org.omnetpp.scave.model.ChartSheets#getChartSheets()
	 * @see #getChartSheets()
	 * @generated
	 */
	EReference getChartSheets_ChartSheets();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Datasets <em>Datasets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Datasets</em>'.
	 * @see org.omnetpp.scave.model.Datasets
	 * @generated
	 */
	EClass getDatasets();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Datasets#getDatasets <em>Datasets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Datasets</em>'.
	 * @see org.omnetpp.scave.model.Datasets#getDatasets()
	 * @see #getDatasets()
	 * @generated
	 */
	EReference getDatasets_Datasets();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.InputFile <em>Input File</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Input File</em>'.
	 * @see org.omnetpp.scave.model.InputFile
	 * @generated
	 */
	EClass getInputFile();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.InputFile#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.scave.model.InputFile#getName()
	 * @see #getInputFile()
	 * @generated
	 */
	EAttribute getInputFile_Name();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.ProcessingOp <em>Processing Op</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Processing Op</em>'.
	 * @see org.omnetpp.scave.model.ProcessingOp
	 * @generated
	 */
	EClass getProcessingOp();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.ProcessingOp#getOperation <em>Operation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Operation</em>'.
	 * @see org.omnetpp.scave.model.ProcessingOp#getOperation()
	 * @see #getProcessingOp()
	 * @generated
	 */
	EAttribute getProcessingOp_Operation();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.ProcessingOp#getFilters <em>Filters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Filters</em>'.
	 * @see org.omnetpp.scave.model.ProcessingOp#getFilters()
	 * @see #getProcessingOp()
	 * @generated
	 */
	EReference getProcessingOp_Filters();

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.ProcessingOp#getParams <em>Params</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Params</em>'.
	 * @see org.omnetpp.scave.model.ProcessingOp#getParams()
	 * @see #getProcessingOp()
	 * @generated
	 */
	EReference getProcessingOp_Params();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.ProcessingOp#getComputedFile <em>Computed File</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Computed File</em>'.
	 * @see org.omnetpp.scave.model.ProcessingOp#getComputedFile()
	 * @see #getProcessingOp()
	 * @generated
	 */
	EAttribute getProcessingOp_ComputedFile();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.ProcessingOp#getComputationHash <em>Computation Hash</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Computation Hash</em>'.
	 * @see org.omnetpp.scave.model.ProcessingOp#getComputationHash()
	 * @see #getProcessingOp()
	 * @generated
	 */
	EAttribute getProcessingOp_ComputationHash();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.Compute <em>Compute</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Compute</em>'.
	 * @see org.omnetpp.scave.model.Compute
	 * @generated
	 */
	EClass getCompute();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.BarChart <em>Bar Chart</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Bar Chart</em>'.
	 * @see org.omnetpp.scave.model.BarChart
	 * @generated
	 */
	EClass getBarChart();

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.scave.model.BarChart#getGroupBy <em>Group By</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group By</em>'.
	 * @see org.omnetpp.scave.model.BarChart#getGroupBy()
	 * @see #getBarChart()
	 * @generated
	 */
	EAttribute getBarChart_GroupBy();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.BarChart#getGroupNameFormat <em>Group Name Format</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Group Name Format</em>'.
	 * @see org.omnetpp.scave.model.BarChart#getGroupNameFormat()
	 * @see #getBarChart()
	 * @generated
	 */
	EAttribute getBarChart_GroupNameFormat();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.BarChart#getBarNameFormat <em>Bar Name Format</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Bar Name Format</em>'.
	 * @see org.omnetpp.scave.model.BarChart#getBarNameFormat()
	 * @see #getBarChart()
	 * @generated
	 */
	EAttribute getBarChart_BarNameFormat();

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.scave.model.BarChart#getBarFields <em>Bar Fields</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Bar Fields</em>'.
	 * @see org.omnetpp.scave.model.BarChart#getBarFields()
	 * @see #getBarChart()
	 * @generated
	 */
	EAttribute getBarChart_BarFields();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.LineChart <em>Line Chart</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Line Chart</em>'.
	 * @see org.omnetpp.scave.model.LineChart
	 * @generated
	 */
	EClass getLineChart();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.LineChart#getLineNameFormat <em>Line Name Format</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Line Name Format</em>'.
	 * @see org.omnetpp.scave.model.LineChart#getLineNameFormat()
	 * @see #getLineChart()
	 * @generated
	 */
	EAttribute getLineChart_LineNameFormat();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.HistogramChart <em>Histogram Chart</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Histogram Chart</em>'.
	 * @see org.omnetpp.scave.model.HistogramChart
	 * @generated
	 */
	EClass getHistogramChart();

	/**
	 * Returns the meta object for class '{@link org.omnetpp.scave.model.ScatterChart <em>Scatter Chart</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Scatter Chart</em>'.
	 * @see org.omnetpp.scave.model.ScatterChart
	 * @generated
	 */
	EClass getScatterChart();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.ScatterChart#getXDataPattern <em>XData Pattern</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>XData Pattern</em>'.
	 * @see org.omnetpp.scave.model.ScatterChart#getXDataPattern()
	 * @see #getScatterChart()
	 * @generated
	 */
	EAttribute getScatterChart_XDataPattern();

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.scave.model.ScatterChart#getIsoDataPattern <em>Iso Data Pattern</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Iso Data Pattern</em>'.
	 * @see org.omnetpp.scave.model.ScatterChart#getIsoDataPattern()
	 * @see #getScatterChart()
	 * @generated
	 */
	EAttribute getScatterChart_IsoDataPattern();

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.ScatterChart#isAverageReplications <em>Average Replications</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Average Replications</em>'.
	 * @see org.omnetpp.scave.model.ScatterChart#isAverageReplications()
	 * @see #getScatterChart()
	 * @generated
	 */
	EAttribute getScatterChart_AverageReplications();

	/**
	 * Returns the meta object for enum '{@link org.omnetpp.scave.model.ResultType <em>Result Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for enum '<em>Result Type</em>'.
	 * @see org.omnetpp.scave.model.ResultType
	 * @generated
	 */
	EEnum getResultType();

	/**
	 * Returns the factory that creates the instances of the model.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the factory that creates the instances of the model.
	 * @generated
	 */
	ScaveModelFactory getScaveModelFactory();

	/**
	 * <!-- begin-user-doc -->
	 * Defines literals for the meta objects that represent
	 * <ul>
	 *   <li>each class,</li>
	 *   <li>each feature of each class,</li>
	 *   <li>each enum,</li>
	 *   <li>and each data type</li>
	 * </ul>
	 * <!-- end-user-doc -->
	 * @generated
	 */
	interface Literals  {
		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.DatasetImpl <em>Dataset</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.DatasetImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDataset()
		 * @generated
		 */
		EClass DATASET = eINSTANCE.getDataset();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute DATASET__NAME = eINSTANCE.getDataset_Name();

		/**
		 * The meta object literal for the '<em><b>Items</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference DATASET__ITEMS = eINSTANCE.getDataset_Items();

		/**
		 * The meta object literal for the '<em><b>Based On</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference DATASET__BASED_ON = eINSTANCE.getDataset_BasedOn();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ChartImpl <em>Chart</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ChartImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChart()
		 * @generated
		 */
		EClass CHART = eINSTANCE.getChart();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute CHART__NAME = eINSTANCE.getChart_Name();

		/**
		 * The meta object literal for the '<em><b>Filters</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CHART__FILTERS = eINSTANCE.getChart_Filters();

		/**
		 * The meta object literal for the '<em><b>Properties</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CHART__PROPERTIES = eINSTANCE.getChart_Properties();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.AddImpl <em>Add</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.AddImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAdd()
		 * @generated
		 */
		EClass ADD = eINSTANCE.getAdd();

		/**
		 * The meta object literal for the '<em><b>Excepts</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ADD__EXCEPTS = eINSTANCE.getAdd_Excepts();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ApplyImpl <em>Apply</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ApplyImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getApply()
		 * @generated
		 */
		EClass APPLY = eINSTANCE.getApply();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ExceptImpl <em>Except</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ExceptImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getExcept()
		 * @generated
		 */
		EClass EXCEPT = eINSTANCE.getExcept();

		/**
		 * The meta object literal for the '<em><b>Filter Pattern</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute EXCEPT__FILTER_PATTERN = eINSTANCE.getExcept_FilterPattern();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.PropertyImpl <em>Property</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.PropertyImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getProperty()
		 * @generated
		 */
		EClass PROPERTY = eINSTANCE.getProperty();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PROPERTY__NAME = eINSTANCE.getProperty_Name();

		/**
		 * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PROPERTY__VALUE = eINSTANCE.getProperty_Value();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.SetOperationImpl <em>Set Operation</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.SetOperationImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getSetOperation()
		 * @generated
		 */
		EClass SET_OPERATION = eINSTANCE.getSetOperation();

		/**
		 * The meta object literal for the '<em><b>Source Dataset</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference SET_OPERATION__SOURCE_DATASET = eINSTANCE.getSetOperation_SourceDataset();

		/**
		 * The meta object literal for the '<em><b>Filter Pattern</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute SET_OPERATION__FILTER_PATTERN = eINSTANCE.getSetOperation_FilterPattern();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute SET_OPERATION__TYPE = eINSTANCE.getSetOperation_Type();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.GroupImpl <em>Group</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.GroupImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getGroup()
		 * @generated
		 */
		EClass GROUP = eINSTANCE.getGroup();

		/**
		 * The meta object literal for the '<em><b>Items</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference GROUP__ITEMS = eINSTANCE.getGroup_Items();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute GROUP__NAME = eINSTANCE.getGroup_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.DiscardImpl <em>Discard</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.DiscardImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDiscard()
		 * @generated
		 */
		EClass DISCARD = eINSTANCE.getDiscard();

		/**
		 * The meta object literal for the '<em><b>Excepts</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference DISCARD__EXCEPTS = eINSTANCE.getDiscard_Excepts();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.DatasetItemImpl <em>Dataset Item</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.DatasetItemImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDatasetItem()
		 * @generated
		 */
		EClass DATASET_ITEM = eINSTANCE.getDatasetItem();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ParamImpl <em>Param</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ParamImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getParam()
		 * @generated
		 */
		EClass PARAM = eINSTANCE.getParam();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PARAM__NAME = eINSTANCE.getParam_Name();

		/**
		 * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PARAM__VALUE = eINSTANCE.getParam_Value();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ChartSheetImpl <em>Chart Sheet</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ChartSheetImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChartSheet()
		 * @generated
		 */
		EClass CHART_SHEET = eINSTANCE.getChartSheet();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute CHART_SHEET__NAME = eINSTANCE.getChartSheet_Name();

		/**
		 * The meta object literal for the '<em><b>Charts</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CHART_SHEET__CHARTS = eINSTANCE.getChartSheet_Charts();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.AnalysisImpl <em>Analysis</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.AnalysisImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAnalysis()
		 * @generated
		 */
		EClass ANALYSIS = eINSTANCE.getAnalysis();

		/**
		 * The meta object literal for the '<em><b>Inputs</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ANALYSIS__INPUTS = eINSTANCE.getAnalysis_Inputs();

		/**
		 * The meta object literal for the '<em><b>Datasets</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ANALYSIS__DATASETS = eINSTANCE.getAnalysis_Datasets();

		/**
		 * The meta object literal for the '<em><b>Chart Sheets</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ANALYSIS__CHART_SHEETS = eINSTANCE.getAnalysis_ChartSheets();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.SelectImpl <em>Select</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.SelectImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getSelect()
		 * @generated
		 */
		EClass SELECT = eINSTANCE.getSelect();

		/**
		 * The meta object literal for the '<em><b>Excepts</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference SELECT__EXCEPTS = eINSTANCE.getSelect_Excepts();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.DeselectImpl <em>Deselect</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.DeselectImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDeselect()
		 * @generated
		 */
		EClass DESELECT = eINSTANCE.getDeselect();

		/**
		 * The meta object literal for the '<em><b>Excepts</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference DESELECT__EXCEPTS = eINSTANCE.getDeselect_Excepts();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.AddDiscardOpImpl <em>Add Discard Op</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.AddDiscardOpImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAddDiscardOp()
		 * @generated
		 */
		EClass ADD_DISCARD_OP = eINSTANCE.getAddDiscardOp();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.SelectDeselectOpImpl <em>Select Deselect Op</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.SelectDeselectOpImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getSelectDeselectOp()
		 * @generated
		 */
		EClass SELECT_DESELECT_OP = eINSTANCE.getSelectDeselectOp();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.InputsImpl <em>Inputs</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.InputsImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getInputs()
		 * @generated
		 */
		EClass INPUTS = eINSTANCE.getInputs();

		/**
		 * The meta object literal for the '<em><b>Inputs</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference INPUTS__INPUTS = eINSTANCE.getInputs_Inputs();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ChartSheetsImpl <em>Chart Sheets</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ChartSheetsImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChartSheets()
		 * @generated
		 */
		EClass CHART_SHEETS = eINSTANCE.getChartSheets();

		/**
		 * The meta object literal for the '<em><b>Chart Sheets</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CHART_SHEETS__CHART_SHEETS = eINSTANCE.getChartSheets_ChartSheets();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.DatasetsImpl <em>Datasets</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.DatasetsImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getDatasets()
		 * @generated
		 */
		EClass DATASETS = eINSTANCE.getDatasets();

		/**
		 * The meta object literal for the '<em><b>Datasets</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference DATASETS__DATASETS = eINSTANCE.getDatasets_Datasets();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.InputFileImpl <em>Input File</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.InputFileImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getInputFile()
		 * @generated
		 */
		EClass INPUT_FILE = eINSTANCE.getInputFile();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute INPUT_FILE__NAME = eINSTANCE.getInputFile_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ProcessingOpImpl <em>Processing Op</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ProcessingOpImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getProcessingOp()
		 * @generated
		 */
		EClass PROCESSING_OP = eINSTANCE.getProcessingOp();

		/**
		 * The meta object literal for the '<em><b>Operation</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PROCESSING_OP__OPERATION = eINSTANCE.getProcessingOp_Operation();

		/**
		 * The meta object literal for the '<em><b>Filters</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PROCESSING_OP__FILTERS = eINSTANCE.getProcessingOp_Filters();

		/**
		 * The meta object literal for the '<em><b>Params</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PROCESSING_OP__PARAMS = eINSTANCE.getProcessingOp_Params();

		/**
		 * The meta object literal for the '<em><b>Computed File</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PROCESSING_OP__COMPUTED_FILE = eINSTANCE.getProcessingOp_ComputedFile();

		/**
		 * The meta object literal for the '<em><b>Computation Hash</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PROCESSING_OP__COMPUTATION_HASH = eINSTANCE.getProcessingOp_ComputationHash();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ComputeImpl <em>Compute</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ComputeImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getCompute()
		 * @generated
		 */
		EClass COMPUTE = eINSTANCE.getCompute();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.BarChartImpl <em>Bar Chart</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.BarChartImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getBarChart()
		 * @generated
		 */
		EClass BAR_CHART = eINSTANCE.getBarChart();

		/**
		 * The meta object literal for the '<em><b>Group By</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute BAR_CHART__GROUP_BY = eINSTANCE.getBarChart_GroupBy();

		/**
		 * The meta object literal for the '<em><b>Group Name Format</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute BAR_CHART__GROUP_NAME_FORMAT = eINSTANCE.getBarChart_GroupNameFormat();

		/**
		 * The meta object literal for the '<em><b>Bar Name Format</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute BAR_CHART__BAR_NAME_FORMAT = eINSTANCE.getBarChart_BarNameFormat();

		/**
		 * The meta object literal for the '<em><b>Bar Fields</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute BAR_CHART__BAR_FIELDS = eINSTANCE.getBarChart_BarFields();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.LineChartImpl <em>Line Chart</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.LineChartImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getLineChart()
		 * @generated
		 */
		EClass LINE_CHART = eINSTANCE.getLineChart();

		/**
		 * The meta object literal for the '<em><b>Line Name Format</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute LINE_CHART__LINE_NAME_FORMAT = eINSTANCE.getLineChart_LineNameFormat();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.HistogramChartImpl <em>Histogram Chart</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.HistogramChartImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getHistogramChart()
		 * @generated
		 */
		EClass HISTOGRAM_CHART = eINSTANCE.getHistogramChart();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ScatterChartImpl <em>Scatter Chart</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.impl.ScatterChartImpl
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getScatterChart()
		 * @generated
		 */
		EClass SCATTER_CHART = eINSTANCE.getScatterChart();

		/**
		 * The meta object literal for the '<em><b>XData Pattern</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute SCATTER_CHART__XDATA_PATTERN = eINSTANCE.getScatterChart_XDataPattern();

		/**
		 * The meta object literal for the '<em><b>Iso Data Pattern</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute SCATTER_CHART__ISO_DATA_PATTERN = eINSTANCE.getScatterChart_IsoDataPattern();

		/**
		 * The meta object literal for the '<em><b>Average Replications</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute SCATTER_CHART__AVERAGE_REPLICATIONS = eINSTANCE.getScatterChart_AverageReplications();

		/**
		 * The meta object literal for the '{@link org.omnetpp.scave.model.ResultType <em>Result Type</em>}' enum.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.scave.model.ResultType
		 * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getResultType()
		 * @generated
		 */
		EEnum RESULT_TYPE = eINSTANCE.getResultType();

	}

} //ScaveModelPackage
