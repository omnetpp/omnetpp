/**
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
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.ChartImpl <em>Chart</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.ChartImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getChart()
     * @generated
     */
    int CHART = 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHART__NAME = 0;

    /**
     * The feature id for the '<em><b>Input</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHART__INPUT = 1;

    /**
     * The feature id for the '<em><b>Properties</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHART__PROPERTIES = 2;

    /**
     * The number of structural features of the '<em>Chart</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHART_FEATURE_COUNT = 3;

    /**
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.PropertyImpl <em>Property</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.PropertyImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getProperty()
     * @generated
     */
    int PROPERTY = 1;

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
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.AnalysisImpl <em>Analysis</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.AnalysisImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getAnalysis()
     * @generated
     */
    int ANALYSIS = 2;

    /**
     * The feature id for the '<em><b>Inputs</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int ANALYSIS__INPUTS = 0;

    /**
     * The feature id for the '<em><b>Charts</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int ANALYSIS__CHARTS = 1;

    /**
     * The number of structural features of the '<em>Analysis</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int ANALYSIS_FEATURE_COUNT = 2;

    /**
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.InputsImpl <em>Inputs</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.InputsImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getInputs()
     * @generated
     */
    int INPUTS = 3;

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
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.InputFileImpl <em>Input File</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.InputFileImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getInputFile()
     * @generated
     */
    int INPUT_FILE = 4;

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
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.BarChartImpl <em>Bar Chart</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.BarChartImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getBarChart()
     * @generated
     */
    int BAR_CHART = 5;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int BAR_CHART__NAME = CHART__NAME;

    /**
     * The feature id for the '<em><b>Input</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int BAR_CHART__INPUT = CHART__INPUT;

    /**
     * The feature id for the '<em><b>Properties</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int BAR_CHART__PROPERTIES = CHART__PROPERTIES;

    /**
     * The feature id for the '<em><b>Group By Fields</b></em>' attribute list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int BAR_CHART__GROUP_BY_FIELDS = CHART_FEATURE_COUNT + 0;

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
     * The feature id for the '<em><b>Averaged Fields</b></em>' attribute list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int BAR_CHART__AVERAGED_FIELDS = CHART_FEATURE_COUNT + 4;

    /**
     * The number of structural features of the '<em>Bar Chart</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int BAR_CHART_FEATURE_COUNT = CHART_FEATURE_COUNT + 5;

    /**
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.LineChartImpl <em>Line Chart</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.LineChartImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getLineChart()
     * @generated
     */
    int LINE_CHART = 6;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int LINE_CHART__NAME = CHART__NAME;

    /**
     * The feature id for the '<em><b>Input</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int LINE_CHART__INPUT = CHART__INPUT;

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
    int HISTOGRAM_CHART = 7;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int HISTOGRAM_CHART__NAME = CHART__NAME;

    /**
     * The feature id for the '<em><b>Input</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int HISTOGRAM_CHART__INPUT = CHART__INPUT;

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
    int SCATTER_CHART = 8;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SCATTER_CHART__NAME = CHART__NAME;

    /**
     * The feature id for the '<em><b>Input</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SCATTER_CHART__INPUT = CHART__INPUT;

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
     * The meta object id for the '{@link org.omnetpp.scave.model.impl.ChartsImpl <em>Charts</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.impl.ChartsImpl
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getCharts()
     * @generated
     */
    int CHARTS = 9;

    /**
     * The feature id for the '<em><b>Charts</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHARTS__CHARTS = 0;

    /**
     * The number of structural features of the '<em>Charts</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHARTS_FEATURE_COUNT = 1;

    /**
     * The meta object id for the '{@link org.omnetpp.scave.model.ResultType <em>Result Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.scave.model.ResultType
     * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getResultType()
     * @generated
     */
    int RESULT_TYPE = 10;


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
     * Returns the meta object for the attribute '{@link org.omnetpp.scave.model.Chart#getInput <em>Input</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Input</em>'.
     * @see org.omnetpp.scave.model.Chart#getInput()
     * @see #getChart()
     * @generated
     */
    EAttribute getChart_Input();

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
     * Returns the meta object for the containment reference '{@link org.omnetpp.scave.model.Analysis#getCharts <em>Charts</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Charts</em>'.
     * @see org.omnetpp.scave.model.Analysis#getCharts()
     * @see #getAnalysis()
     * @generated
     */
    EReference getAnalysis_Charts();

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
     * Returns the meta object for class '{@link org.omnetpp.scave.model.BarChart <em>Bar Chart</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Bar Chart</em>'.
     * @see org.omnetpp.scave.model.BarChart
     * @generated
     */
    EClass getBarChart();

    /**
     * Returns the meta object for the attribute list '{@link org.omnetpp.scave.model.BarChart#getGroupByFields <em>Group By Fields</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute list '<em>Group By Fields</em>'.
     * @see org.omnetpp.scave.model.BarChart#getGroupByFields()
     * @see #getBarChart()
     * @generated
     */
    EAttribute getBarChart_GroupByFields();

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
     * Returns the meta object for the attribute list '{@link org.omnetpp.scave.model.BarChart#getAveragedFields <em>Averaged Fields</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute list '<em>Averaged Fields</em>'.
     * @see org.omnetpp.scave.model.BarChart#getAveragedFields()
     * @see #getBarChart()
     * @generated
     */
    EAttribute getBarChart_AveragedFields();

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
     * Returns the meta object for class '{@link org.omnetpp.scave.model.Charts <em>Charts</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Charts</em>'.
     * @see org.omnetpp.scave.model.Charts
     * @generated
     */
    EClass getCharts();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.scave.model.Charts#getCharts <em>Charts</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Charts</em>'.
     * @see org.omnetpp.scave.model.Charts#getCharts()
     * @see #getCharts()
     * @generated
     */
    EReference getCharts_Charts();

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
    interface Literals {
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
         * The meta object literal for the '<em><b>Input</b></em>' attribute feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EAttribute CHART__INPUT = eINSTANCE.getChart_Input();

        /**
         * The meta object literal for the '<em><b>Properties</b></em>' containment reference list feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EReference CHART__PROPERTIES = eINSTANCE.getChart_Properties();

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
         * The meta object literal for the '<em><b>Charts</b></em>' containment reference feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EReference ANALYSIS__CHARTS = eINSTANCE.getAnalysis_Charts();

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
         * The meta object literal for the '{@link org.omnetpp.scave.model.impl.BarChartImpl <em>Bar Chart</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see org.omnetpp.scave.model.impl.BarChartImpl
         * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getBarChart()
         * @generated
         */
        EClass BAR_CHART = eINSTANCE.getBarChart();

        /**
         * The meta object literal for the '<em><b>Group By Fields</b></em>' attribute list feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EAttribute BAR_CHART__GROUP_BY_FIELDS = eINSTANCE.getBarChart_GroupByFields();

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
         * The meta object literal for the '<em><b>Averaged Fields</b></em>' attribute list feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EAttribute BAR_CHART__AVERAGED_FIELDS = eINSTANCE.getBarChart_AveragedFields();

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
         * The meta object literal for the '{@link org.omnetpp.scave.model.impl.ChartsImpl <em>Charts</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see org.omnetpp.scave.model.impl.ChartsImpl
         * @see org.omnetpp.scave.model.impl.ScaveModelPackageImpl#getCharts()
         * @generated
         */
        EClass CHARTS = eINSTANCE.getCharts();

        /**
         * The meta object literal for the '<em><b>Charts</b></em>' containment reference list feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EReference CHARTS__CHARTS = eINSTANCE.getCharts_Charts();

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
