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
import org.eclipse.emf.ecore.impl.EObjectImpl;
import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Except</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ExceptImpl#getFilterPattern <em>Filter Pattern</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ExceptImpl extends EObjectImpl implements Except {
    /**
     * The default value of the '{@link #getFilterPattern() <em>Filter Pattern</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getFilterPattern()
     * @generated
     * @ordered
     */
    protected static final String FILTER_PATTERN_EDEFAULT = null;
    /**
     * The cached value of the '{@link #getFilterPattern() <em>Filter Pattern</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getFilterPattern()
     * @generated
     * @ordered
     */
    protected String filterPattern = FILTER_PATTERN_EDEFAULT;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ExceptImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.EXCEPT;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getFilterPattern() {
        return filterPattern;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setFilterPattern(String newFilterPattern) {
        String oldFilterPattern = filterPattern;
        filterPattern = newFilterPattern;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.EXCEPT__FILTER_PATTERN, oldFilterPattern, filterPattern));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object eGet(int featureID, boolean resolve, boolean coreType) {
        switch (featureID) {
            case ScaveModelPackage.EXCEPT__FILTER_PATTERN:
                return getFilterPattern();
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
            case ScaveModelPackage.EXCEPT__FILTER_PATTERN:
                setFilterPattern((String)newValue);
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
            case ScaveModelPackage.EXCEPT__FILTER_PATTERN:
                setFilterPattern(FILTER_PATTERN_EDEFAULT);
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
            case ScaveModelPackage.EXCEPT__FILTER_PATTERN:
                return FILTER_PATTERN_EDEFAULT == null ? filterPattern != null : !FILTER_PATTERN_EDEFAULT.equals(filterPattern);
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
        result.append(" (filterPattern: ");
        result.append(filterPattern);
        result.append(')');
        return result.toString();
    }

} //ExceptImpl