/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.scave.model.ComputeScalar;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SelectDeselectOp;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Compute Scalar</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#getFilters <em>Filters</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#getScalarName <em>Scalar Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#getValueExpr <em>Value Expr</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#getModuleExpr <em>Module Expr</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#getGroupByExpr <em>Group By Expr</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#isAverageReplications <em>Average Replications</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#isComputeStddev <em>Compute Stddev</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#isComputeConfidenceInterval <em>Compute Confidence Interval</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#getConfidenceLevel <em>Confidence Level</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ComputeScalarImpl#isComputeMinMax <em>Compute Min Max</em>}</li>
 * </ul>
 *
 * @generated
 */
public class ComputeScalarImpl extends DatasetItemImpl implements ComputeScalar {
    /**
     * The cached value of the '{@link #getFilters() <em>Filters</em>}' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getFilters()
     * @generated
     * @ordered
     */
    protected EList<SelectDeselectOp> filters;

    /**
     * The default value of the '{@link #getScalarName() <em>Scalar Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getScalarName()
     * @generated
     * @ordered
     */
    protected static final String SCALAR_NAME_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getScalarName() <em>Scalar Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getScalarName()
     * @generated
     * @ordered
     */
    protected String scalarName = SCALAR_NAME_EDEFAULT;

    /**
     * The default value of the '{@link #getValueExpr() <em>Value Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getValueExpr()
     * @generated
     * @ordered
     */
    protected static final String VALUE_EXPR_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getValueExpr() <em>Value Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getValueExpr()
     * @generated
     * @ordered
     */
    protected String valueExpr = VALUE_EXPR_EDEFAULT;

    /**
     * The default value of the '{@link #getModuleExpr() <em>Module Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getModuleExpr()
     * @generated
     * @ordered
     */
    protected static final String MODULE_EXPR_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getModuleExpr() <em>Module Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getModuleExpr()
     * @generated
     * @ordered
     */
    protected String moduleExpr = MODULE_EXPR_EDEFAULT;

    /**
     * The default value of the '{@link #getGroupByExpr() <em>Group By Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getGroupByExpr()
     * @generated
     * @ordered
     */
    protected static final String GROUP_BY_EXPR_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getGroupByExpr() <em>Group By Expr</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getGroupByExpr()
     * @generated
     * @ordered
     */
    protected String groupByExpr = GROUP_BY_EXPR_EDEFAULT;

    /**
     * The default value of the '{@link #isAverageReplications() <em>Average Replications</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isAverageReplications()
     * @generated
     * @ordered
     */
    protected static final boolean AVERAGE_REPLICATIONS_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isAverageReplications() <em>Average Replications</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isAverageReplications()
     * @generated
     * @ordered
     */
    protected boolean averageReplications = AVERAGE_REPLICATIONS_EDEFAULT;

    /**
     * The default value of the '{@link #isComputeStddev() <em>Compute Stddev</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isComputeStddev()
     * @generated
     * @ordered
     */
    protected static final boolean COMPUTE_STDDEV_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isComputeStddev() <em>Compute Stddev</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isComputeStddev()
     * @generated
     * @ordered
     */
    protected boolean computeStddev = COMPUTE_STDDEV_EDEFAULT;

    /**
     * The default value of the '{@link #isComputeConfidenceInterval() <em>Compute Confidence Interval</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isComputeConfidenceInterval()
     * @generated
     * @ordered
     */
    protected static final boolean COMPUTE_CONFIDENCE_INTERVAL_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isComputeConfidenceInterval() <em>Compute Confidence Interval</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isComputeConfidenceInterval()
     * @generated
     * @ordered
     */
    protected boolean computeConfidenceInterval = COMPUTE_CONFIDENCE_INTERVAL_EDEFAULT;

    /**
     * The default value of the '{@link #getConfidenceLevel() <em>Confidence Level</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getConfidenceLevel()
     * @generated
     * @ordered
     */
    protected static final Double CONFIDENCE_LEVEL_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getConfidenceLevel() <em>Confidence Level</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getConfidenceLevel()
     * @generated
     * @ordered
     */
    protected Double confidenceLevel = CONFIDENCE_LEVEL_EDEFAULT;

    /**
     * The default value of the '{@link #isComputeMinMax() <em>Compute Min Max</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isComputeMinMax()
     * @generated
     * @ordered
     */
    protected static final boolean COMPUTE_MIN_MAX_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isComputeMinMax() <em>Compute Min Max</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isComputeMinMax()
     * @generated
     * @ordered
     */
    protected boolean computeMinMax = COMPUTE_MIN_MAX_EDEFAULT;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ComputeScalarImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.COMPUTE_SCALAR;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList<SelectDeselectOp> getFilters() {
        if (filters == null) {
            filters = new EObjectContainmentEList<SelectDeselectOp>(SelectDeselectOp.class, this, ScaveModelPackage.COMPUTE_SCALAR__FILTERS);
        }
        return filters;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getScalarName() {
        return scalarName;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setScalarName(String newScalarName) {
        String oldScalarName = scalarName;
        scalarName = newScalarName;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME, oldScalarName, scalarName));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getValueExpr() {
        return valueExpr;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setValueExpr(String newValueExpr) {
        String oldValueExpr = valueExpr;
        valueExpr = newValueExpr;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR, oldValueExpr, valueExpr));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getModuleExpr() {
        return moduleExpr;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setModuleExpr(String newModuleExpr) {
        String oldModuleExpr = moduleExpr;
        moduleExpr = newModuleExpr;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR, oldModuleExpr, moduleExpr));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getGroupByExpr() {
        return groupByExpr;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setGroupByExpr(String newGroupByExpr) {
        String oldGroupByExpr = groupByExpr;
        groupByExpr = newGroupByExpr;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR, oldGroupByExpr, groupByExpr));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isAverageReplications() {
        return averageReplications;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setAverageReplications(boolean newAverageReplications) {
        boolean oldAverageReplications = averageReplications;
        averageReplications = newAverageReplications;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS, oldAverageReplications, averageReplications));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isComputeStddev() {
        return computeStddev;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setComputeStddev(boolean newComputeStddev) {
        boolean oldComputeStddev = computeStddev;
        computeStddev = newComputeStddev;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV, oldComputeStddev, computeStddev));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isComputeConfidenceInterval() {
        return computeConfidenceInterval;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setComputeConfidenceInterval(boolean newComputeConfidenceInterval) {
        boolean oldComputeConfidenceInterval = computeConfidenceInterval;
        computeConfidenceInterval = newComputeConfidenceInterval;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL, oldComputeConfidenceInterval, computeConfidenceInterval));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Double getConfidenceLevel() {
        return confidenceLevel;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setConfidenceLevel(Double newConfidenceLevel) {
        Double oldConfidenceLevel = confidenceLevel;
        confidenceLevel = newConfidenceLevel;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL, oldConfidenceLevel, confidenceLevel));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isComputeMinMax() {
        return computeMinMax;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setComputeMinMax(boolean newComputeMinMax) {
        boolean oldComputeMinMax = computeMinMax;
        computeMinMax = newComputeMinMax;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX, oldComputeMinMax, computeMinMax));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
        switch (featureID) {
            case ScaveModelPackage.COMPUTE_SCALAR__FILTERS:
                return ((InternalEList<?>)getFilters()).basicRemove(otherEnd, msgs);
        }
        return super.eInverseRemove(otherEnd, featureID, msgs);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object eGet(int featureID, boolean resolve, boolean coreType) {
        switch (featureID) {
            case ScaveModelPackage.COMPUTE_SCALAR__FILTERS:
                return getFilters();
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
                return getScalarName();
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
                return getValueExpr();
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
                return getModuleExpr();
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
                return getGroupByExpr();
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
                return isAverageReplications();
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
                return isComputeStddev();
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
                return isComputeConfidenceInterval();
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
                return getConfidenceLevel();
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                return isComputeMinMax();
        }
        return super.eGet(featureID, resolve, coreType);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @SuppressWarnings("unchecked")
    @Override
    public void eSet(int featureID, Object newValue) {
        switch (featureID) {
            case ScaveModelPackage.COMPUTE_SCALAR__FILTERS:
                getFilters().clear();
                getFilters().addAll((Collection<? extends SelectDeselectOp>)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
                setScalarName((String)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
                setValueExpr((String)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
                setModuleExpr((String)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
                setGroupByExpr((String)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
                setAverageReplications((Boolean)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
                setComputeStddev((Boolean)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
                setComputeConfidenceInterval((Boolean)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
                setConfidenceLevel((Double)newValue);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                setComputeMinMax((Boolean)newValue);
                return;
        }
        super.eSet(featureID, newValue);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public void eUnset(int featureID) {
        switch (featureID) {
            case ScaveModelPackage.COMPUTE_SCALAR__FILTERS:
                getFilters().clear();
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
                setScalarName(SCALAR_NAME_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
                setValueExpr(VALUE_EXPR_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
                setModuleExpr(MODULE_EXPR_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
                setGroupByExpr(GROUP_BY_EXPR_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
                setAverageReplications(AVERAGE_REPLICATIONS_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
                setComputeStddev(COMPUTE_STDDEV_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
                setComputeConfidenceInterval(COMPUTE_CONFIDENCE_INTERVAL_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
                setConfidenceLevel(CONFIDENCE_LEVEL_EDEFAULT);
                return;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                setComputeMinMax(COMPUTE_MIN_MAX_EDEFAULT);
                return;
        }
        super.eUnset(featureID);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public boolean eIsSet(int featureID) {
        switch (featureID) {
            case ScaveModelPackage.COMPUTE_SCALAR__FILTERS:
                return filters != null && !filters.isEmpty();
            case ScaveModelPackage.COMPUTE_SCALAR__SCALAR_NAME:
                return SCALAR_NAME_EDEFAULT == null ? scalarName != null : !SCALAR_NAME_EDEFAULT.equals(scalarName);
            case ScaveModelPackage.COMPUTE_SCALAR__VALUE_EXPR:
                return VALUE_EXPR_EDEFAULT == null ? valueExpr != null : !VALUE_EXPR_EDEFAULT.equals(valueExpr);
            case ScaveModelPackage.COMPUTE_SCALAR__MODULE_EXPR:
                return MODULE_EXPR_EDEFAULT == null ? moduleExpr != null : !MODULE_EXPR_EDEFAULT.equals(moduleExpr);
            case ScaveModelPackage.COMPUTE_SCALAR__GROUP_BY_EXPR:
                return GROUP_BY_EXPR_EDEFAULT == null ? groupByExpr != null : !GROUP_BY_EXPR_EDEFAULT.equals(groupByExpr);
            case ScaveModelPackage.COMPUTE_SCALAR__AVERAGE_REPLICATIONS:
                return averageReplications != AVERAGE_REPLICATIONS_EDEFAULT;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_STDDEV:
                return computeStddev != COMPUTE_STDDEV_EDEFAULT;
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_CONFIDENCE_INTERVAL:
                return computeConfidenceInterval != COMPUTE_CONFIDENCE_INTERVAL_EDEFAULT;
            case ScaveModelPackage.COMPUTE_SCALAR__CONFIDENCE_LEVEL:
                return CONFIDENCE_LEVEL_EDEFAULT == null ? confidenceLevel != null : !CONFIDENCE_LEVEL_EDEFAULT.equals(confidenceLevel);
            case ScaveModelPackage.COMPUTE_SCALAR__COMPUTE_MIN_MAX:
                return computeMinMax != COMPUTE_MIN_MAX_EDEFAULT;
        }
        return super.eIsSet(featureID);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public String toString() {
        if (eIsProxy()) return super.toString();

        StringBuffer result = new StringBuffer(super.toString());
        result.append(" (scalarName: ");
        result.append(scalarName);
        result.append(", valueExpr: ");
        result.append(valueExpr);
        result.append(", moduleExpr: ");
        result.append(moduleExpr);
        result.append(", groupByExpr: ");
        result.append(groupByExpr);
        result.append(", averageReplications: ");
        result.append(averageReplications);
        result.append(", computeStddev: ");
        result.append(computeStddev);
        result.append(", computeConfidenceInterval: ");
        result.append(computeConfidenceInterval);
        result.append(", confidenceLevel: ");
        result.append(confidenceLevel);
        result.append(", computeMinMax: ");
        result.append(computeMinMax);
        result.append(')');
        return result.toString();
    }

} //ComputeScalarImpl
