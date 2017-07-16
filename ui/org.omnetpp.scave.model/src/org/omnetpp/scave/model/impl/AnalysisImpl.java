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

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.ChartSheets;
import org.omnetpp.scave.model.Datasets;
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
 *   <li>{@link org.omnetpp.scave.model.impl.AnalysisImpl#getDatasets <em>Datasets</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.AnalysisImpl#getChartSheets <em>Chart Sheets</em>}</li>
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
     * The cached value of the '{@link #getDatasets() <em>Datasets</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDatasets()
     * @generated
     * @ordered
     */
    protected Datasets datasets;

    /**
     * The cached value of the '{@link #getChartSheets() <em>Chart Sheets</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getChartSheets()
     * @generated
     * @ordered
     */
    protected ChartSheets chartSheets;

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
    public Datasets getDatasets() {
        return datasets;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetDatasets(Datasets newDatasets, NotificationChain msgs) {
        Datasets oldDatasets = datasets;
        datasets = newDatasets;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__DATASETS, oldDatasets, newDatasets);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDatasets(Datasets newDatasets) {
        if (newDatasets != datasets) {
            NotificationChain msgs = null;
            if (datasets != null)
                msgs = ((InternalEObject)datasets).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__DATASETS, null, msgs);
            if (newDatasets != null)
                msgs = ((InternalEObject)newDatasets).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__DATASETS, null, msgs);
            msgs = basicSetDatasets(newDatasets, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__DATASETS, newDatasets, newDatasets));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ChartSheets getChartSheets() {
        return chartSheets;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetChartSheets(ChartSheets newChartSheets, NotificationChain msgs) {
        ChartSheets oldChartSheets = chartSheets;
        chartSheets = newChartSheets;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__CHART_SHEETS, oldChartSheets, newChartSheets);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setChartSheets(ChartSheets newChartSheets) {
        if (newChartSheets != chartSheets) {
            NotificationChain msgs = null;
            if (chartSheets != null)
                msgs = ((InternalEObject)chartSheets).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__CHART_SHEETS, null, msgs);
            if (newChartSheets != null)
                msgs = ((InternalEObject)newChartSheets).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - ScaveModelPackage.ANALYSIS__CHART_SHEETS, null, msgs);
            msgs = basicSetChartSheets(newChartSheets, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.ANALYSIS__CHART_SHEETS, newChartSheets, newChartSheets));
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
            case ScaveModelPackage.ANALYSIS__DATASETS:
                return basicSetDatasets(null, msgs);
            case ScaveModelPackage.ANALYSIS__CHART_SHEETS:
                return basicSetChartSheets(null, msgs);
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
            case ScaveModelPackage.ANALYSIS__DATASETS:
                return getDatasets();
            case ScaveModelPackage.ANALYSIS__CHART_SHEETS:
                return getChartSheets();
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
            case ScaveModelPackage.ANALYSIS__DATASETS:
                setDatasets((Datasets)newValue);
                return;
            case ScaveModelPackage.ANALYSIS__CHART_SHEETS:
                setChartSheets((ChartSheets)newValue);
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
            case ScaveModelPackage.ANALYSIS__DATASETS:
                setDatasets((Datasets)null);
                return;
            case ScaveModelPackage.ANALYSIS__CHART_SHEETS:
                setChartSheets((ChartSheets)null);
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
            case ScaveModelPackage.ANALYSIS__DATASETS:
                return datasets != null;
            case ScaveModelPackage.ANALYSIS__CHART_SHEETS:
                return chartSheets != null;
        }
        return super.eIsSet(featureID);
    }

} //AnalysisImpl