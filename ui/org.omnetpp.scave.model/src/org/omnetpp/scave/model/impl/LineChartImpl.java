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
import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Line Chart</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.LineChartImpl#getLineNameFormat <em>Line Name Format</em>}</li>
 * </ul>
 *
 * @generated
 */
public class LineChartImpl extends ChartImpl implements LineChart {
    /**
     * The default value of the '{@link #getLineNameFormat() <em>Line Name Format</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getLineNameFormat()
     * @generated
     * @ordered
     */
    protected static final String LINE_NAME_FORMAT_EDEFAULT = null;
    /**
     * The cached value of the '{@link #getLineNameFormat() <em>Line Name Format</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getLineNameFormat()
     * @generated
     * @ordered
     */
    protected String lineNameFormat = LINE_NAME_FORMAT_EDEFAULT;
    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected LineChartImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
                protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.LINE_CHART;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getLineNameFormat() {
        return lineNameFormat;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setLineNameFormat(String newLineNameFormat) {
        String oldLineNameFormat = lineNameFormat;
        lineNameFormat = newLineNameFormat;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT, oldLineNameFormat, lineNameFormat));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object eGet(int featureID, boolean resolve, boolean coreType) {
        switch (featureID) {
            case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
                return getLineNameFormat();
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
            case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
                setLineNameFormat((String)newValue);
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
            case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
                setLineNameFormat(LINE_NAME_FORMAT_EDEFAULT);
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
            case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
                return LINE_NAME_FORMAT_EDEFAULT == null ? lineNameFormat != null : !LINE_NAME_FORMAT_EDEFAULT.equals(lineNameFormat);
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
        result.append(" (lineNameFormat: ");
        result.append(lineNameFormat);
        result.append(')');
        return result.toString();
    }

} //LineChartImpl