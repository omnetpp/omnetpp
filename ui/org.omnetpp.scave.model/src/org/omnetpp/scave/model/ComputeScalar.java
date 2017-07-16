/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Compute Scalar</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#getFilters <em>Filters</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#getScalarName <em>Scalar Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#getValueExpr <em>Value Expr</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#getModuleExpr <em>Module Expr</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#getGroupByExpr <em>Group By Expr</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#isAverageReplications <em>Average Replications</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#isComputeStddev <em>Compute Stddev</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#isComputeConfidenceInterval <em>Compute Confidence Interval</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#getConfidenceLevel <em>Confidence Level</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ComputeScalar#isComputeMinMax <em>Compute Min Max</em>}</li>
 * </ul>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar()
 * @model
 * @generated
 */
public interface ComputeScalar extends DatasetItem {
    /**
     * Returns the value of the '<em><b>Filters</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.SelectDeselectOp}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Filters</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Filters</em>' containment reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_Filters()
     * @model containment="true"
     * @generated
     */
    EList<SelectDeselectOp> getFilters();

    /**
     * Returns the value of the '<em><b>Scalar Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Scalar Name</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Scalar Name</em>' attribute.
     * @see #setScalarName(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ScalarName()
     * @model
     * @generated
     */
    String getScalarName();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#getScalarName <em>Scalar Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Scalar Name</em>' attribute.
     * @see #getScalarName()
     * @generated
     */
    void setScalarName(String value);

    /**
     * Returns the value of the '<em><b>Value Expr</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Value Expr</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Value Expr</em>' attribute.
     * @see #setValueExpr(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ValueExpr()
     * @model
     * @generated
     */
    String getValueExpr();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#getValueExpr <em>Value Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Value Expr</em>' attribute.
     * @see #getValueExpr()
     * @generated
     */
    void setValueExpr(String value);

    /**
     * Returns the value of the '<em><b>Module Expr</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Module Expr</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Module Expr</em>' attribute.
     * @see #setModuleExpr(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ModuleExpr()
     * @model
     * @generated
     */
    String getModuleExpr();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#getModuleExpr <em>Module Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Module Expr</em>' attribute.
     * @see #getModuleExpr()
     * @generated
     */
    void setModuleExpr(String value);

    /**
     * Returns the value of the '<em><b>Group By Expr</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Group By Expr</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Group By Expr</em>' attribute.
     * @see #setGroupByExpr(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_GroupByExpr()
     * @model
     * @generated
     */
    String getGroupByExpr();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#getGroupByExpr <em>Group By Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Group By Expr</em>' attribute.
     * @see #getGroupByExpr()
     * @generated
     */
    void setGroupByExpr(String value);

    /**
     * Returns the value of the '<em><b>Average Replications</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Average Replications</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Average Replications</em>' attribute.
     * @see #setAverageReplications(boolean)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_AverageReplications()
     * @model
     * @generated
     */
    boolean isAverageReplications();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#isAverageReplications <em>Average Replications</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Average Replications</em>' attribute.
     * @see #isAverageReplications()
     * @generated
     */
    void setAverageReplications(boolean value);

    /**
     * Returns the value of the '<em><b>Compute Stddev</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Compute Stddev</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Compute Stddev</em>' attribute.
     * @see #setComputeStddev(boolean)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ComputeStddev()
     * @model
     * @generated
     */
    boolean isComputeStddev();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#isComputeStddev <em>Compute Stddev</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Compute Stddev</em>' attribute.
     * @see #isComputeStddev()
     * @generated
     */
    void setComputeStddev(boolean value);

    /**
     * Returns the value of the '<em><b>Compute Confidence Interval</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Compute Confidence Interval</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Compute Confidence Interval</em>' attribute.
     * @see #setComputeConfidenceInterval(boolean)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ComputeConfidenceInterval()
     * @model
     * @generated
     */
    boolean isComputeConfidenceInterval();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#isComputeConfidenceInterval <em>Compute Confidence Interval</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Compute Confidence Interval</em>' attribute.
     * @see #isComputeConfidenceInterval()
     * @generated
     */
    void setComputeConfidenceInterval(boolean value);

    /**
     * Returns the value of the '<em><b>Confidence Level</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Confidence Level</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Confidence Level</em>' attribute.
     * @see #setConfidenceLevel(Double)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ConfidenceLevel()
     * @model
     * @generated
     */
    Double getConfidenceLevel();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#getConfidenceLevel <em>Confidence Level</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Confidence Level</em>' attribute.
     * @see #getConfidenceLevel()
     * @generated
     */
    void setConfidenceLevel(Double value);

    /**
     * Returns the value of the '<em><b>Compute Min Max</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Compute Min Max</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Compute Min Max</em>' attribute.
     * @see #setComputeMinMax(boolean)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getComputeScalar_ComputeMinMax()
     * @model
     * @generated
     */
    boolean isComputeMinMax();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ComputeScalar#isComputeMinMax <em>Compute Min Max</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Compute Min Max</em>' attribute.
     * @see #isComputeMinMax()
     * @generated
     */
    void setComputeMinMax(boolean value);

} // ComputeScalar
