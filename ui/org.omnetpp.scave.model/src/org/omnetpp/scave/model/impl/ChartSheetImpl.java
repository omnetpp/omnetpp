/**
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
import org.eclipse.emf.ecore.util.EObjectResolvingEList;

import org.eclipse.emf.ecore.util.InternalEList;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Chart Sheet</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartSheetImpl#getCharts <em>Charts</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartSheetImpl#isTemporary <em>Temporary</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartSheetImpl#getProperties <em>Properties</em>}</li>
 * </ul>
 *
 * @generated
 */
public class ChartSheetImpl extends AnalysisItemImpl implements ChartSheet {
    /**
     * The cached value of the '{@link #getCharts() <em>Charts</em>}' reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getCharts()
     * @generated
     * @ordered
     */
    protected EList<Chart> charts;

    /**
     * The default value of the '{@link #isTemporary() <em>Temporary</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isTemporary()
     * @generated
     * @ordered
     */
    protected static final boolean TEMPORARY_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isTemporary() <em>Temporary</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isTemporary()
     * @generated
     * @ordered
     */
    protected boolean temporary = TEMPORARY_EDEFAULT;

    /**
     * The cached value of the '{@link #getProperties() <em>Properties</em>}' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getProperties()
     * @generated
     * @ordered
     */
    protected EList<Property> properties;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ChartSheetImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.CHART_SHEET;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList<Chart> getCharts() {
        if (charts == null) {
            charts = new EObjectResolvingEList<Chart>(Chart.class, this, ScaveModelPackage.CHART_SHEET__CHARTS);
        }
        return charts;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isTemporary() {
        return temporary;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setTemporary(boolean newTemporary) {
        boolean oldTemporary = temporary;
        temporary = newTemporary;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.CHART_SHEET__TEMPORARY, oldTemporary, temporary));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList<Property> getProperties() {
        if (properties == null) {
            properties = new EObjectContainmentEList<Property>(Property.class, this, ScaveModelPackage.CHART_SHEET__PROPERTIES);
        }
        return properties;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
        switch (featureID) {
            case ScaveModelPackage.CHART_SHEET__PROPERTIES:
                return ((InternalEList<?>)getProperties()).basicRemove(otherEnd, msgs);
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
            case ScaveModelPackage.CHART_SHEET__CHARTS:
                return getCharts();
            case ScaveModelPackage.CHART_SHEET__TEMPORARY:
                return isTemporary();
            case ScaveModelPackage.CHART_SHEET__PROPERTIES:
                return getProperties();
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
            case ScaveModelPackage.CHART_SHEET__CHARTS:
                getCharts().clear();
                getCharts().addAll((Collection<? extends Chart>)newValue);
                return;
            case ScaveModelPackage.CHART_SHEET__TEMPORARY:
                setTemporary((Boolean)newValue);
                return;
            case ScaveModelPackage.CHART_SHEET__PROPERTIES:
                getProperties().clear();
                getProperties().addAll((Collection<? extends Property>)newValue);
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
            case ScaveModelPackage.CHART_SHEET__CHARTS:
                getCharts().clear();
                return;
            case ScaveModelPackage.CHART_SHEET__TEMPORARY:
                setTemporary(TEMPORARY_EDEFAULT);
                return;
            case ScaveModelPackage.CHART_SHEET__PROPERTIES:
                getProperties().clear();
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
            case ScaveModelPackage.CHART_SHEET__CHARTS:
                return charts != null && !charts.isEmpty();
            case ScaveModelPackage.CHART_SHEET__TEMPORARY:
                return temporary != TEMPORARY_EDEFAULT;
            case ScaveModelPackage.CHART_SHEET__PROPERTIES:
                return properties != null && !properties.isEmpty();
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
        result.append(" (temporary: ");
        result.append(temporary);
        result.append(')');
        return result.toString();
    }

} //ChartSheetImpl
