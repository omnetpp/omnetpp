/**
 */
package org.omnetpp.scave.model.impl;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Analysis</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.AnalysisImpl#getInputs <em>Inputs</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.AnalysisImpl#getCharts <em>Charts</em>}</li>
 * </ul>
 *
 * @generated
 */
public class AnalysisImpl extends EObjectImpl implements Analysis {
    /**
     * The cached value of the '{@link #getInputs() <em>Inputs</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getInputs()
     * @generated
     * @ordered
     */
    protected Inputs inputs;

    /**
     * The cached value of the '{@link #getCharts() <em>Charts</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getCharts()
     * @generated
     * @ordered
     */
    protected Charts charts;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected AnalysisImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.ANALYSIS;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Inputs getInputs() {
        return inputs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetInputs(Inputs newInputs, NotificationChain msgs) {
        Inputs oldInputs = inputs;
        inputs = newInputs;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__INPUTS, oldInputs, newInputs);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setInputs(Inputs newInputs) {
        if (newInputs != inputs) {
            NotificationChain msgs = null;
            if (inputs != null)
                msgs = ((InternalEObject)inputs).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__INPUTS, null, msgs);
            if (newInputs != null)
                msgs = ((InternalEObject)newInputs).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__INPUTS, null, msgs);
            msgs = basicSetInputs(newInputs, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__INPUTS, newInputs, newInputs));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Charts getCharts() {
        return charts;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetCharts(Charts newCharts, NotificationChain msgs) {
        Charts oldCharts = charts;
        charts = newCharts;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__CHARTS, oldCharts, newCharts);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setCharts(Charts newCharts) {
        if (newCharts != charts) {
            NotificationChain msgs = null;
            if (charts != null)
                msgs = ((InternalEObject)charts).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__CHARTS, null, msgs);
            if (newCharts != null)
                msgs = ((InternalEObject)newCharts).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__CHARTS, null, msgs);
            msgs = basicSetCharts(newCharts, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__CHARTS, newCharts, newCharts));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
        switch (featureID) {
            case ScaveModelPackage.ANALYSIS__INPUTS:
                return basicSetInputs(null, msgs);
            case ScaveModelPackage.ANALYSIS__CHARTS:
                return basicSetCharts(null, msgs);
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
            case ScaveModelPackage.ANALYSIS__INPUTS:
                return getInputs();
            case ScaveModelPackage.ANALYSIS__CHARTS:
                return getCharts();
        }
        return super.eGet(featureID, resolve, coreType);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public void eSet(int featureID, Object newValue) {
        switch (featureID) {
            case ScaveModelPackage.ANALYSIS__INPUTS:
                setInputs((Inputs)newValue);
                return;
            case ScaveModelPackage.ANALYSIS__CHARTS:
                setCharts((Charts)newValue);
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
            case ScaveModelPackage.ANALYSIS__INPUTS:
                setInputs((Inputs)null);
                return;
            case ScaveModelPackage.ANALYSIS__CHARTS:
                setCharts((Charts)null);
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
            case ScaveModelPackage.ANALYSIS__INPUTS:
                return inputs != null;
            case ScaveModelPackage.ANALYSIS__CHARTS:
                return charts != null;
        }
        return super.eIsSet(featureID);
    }

} //AnalysisImpl
