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

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ChartSheets;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Chart Sheets</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartSheetsImpl#getChartSheets <em>Chart Sheets</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ChartSheetsImpl extends EObjectImpl implements ChartSheets {
    /**
     * The cached value of the '{@link #getChartSheets() <em>Chart Sheets</em>}' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getChartSheets()
     * @generated
     * @ordered
     */
    protected EList<ChartSheet> chartSheets;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ChartSheetsImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.CHART_SHEETS;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList<ChartSheet> getChartSheets() {
        if (chartSheets == null) {
            chartSheets = new EObjectContainmentEList<ChartSheet>(ChartSheet.class, this, ScaveModelPackage.CHART_SHEETS__CHART_SHEETS);
        }
        return chartSheets;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
        switch (featureID) {
            case ScaveModelPackage.CHART_SHEETS__CHART_SHEETS:
                return ((InternalEList<?>)getChartSheets()).basicRemove(otherEnd, msgs);
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
            case ScaveModelPackage.CHART_SHEETS__CHART_SHEETS:
                return getChartSheets();
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
            case ScaveModelPackage.CHART_SHEETS__CHART_SHEETS:
                getChartSheets().clear();
                getChartSheets().addAll((Collection<? extends ChartSheet>)newValue);
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
            case ScaveModelPackage.CHART_SHEETS__CHART_SHEETS:
                getChartSheets().clear();
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
            case ScaveModelPackage.CHART_SHEETS__CHART_SHEETS:
                return chartSheets != null && !chartSheets.isEmpty();
        }
        return super.eIsSet(featureID);
    }

} //ChartSheetsImpl