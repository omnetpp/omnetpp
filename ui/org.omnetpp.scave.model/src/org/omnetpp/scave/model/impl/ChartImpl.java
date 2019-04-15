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
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Chart</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartImpl#getScript <em>Script</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartImpl#getProperties <em>Properties</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartImpl#isTemporary <em>Temporary</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartImpl#getForm <em>Form</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ChartImpl#getTemplateID <em>Template ID</em>}</li>
 * </ul>
 *
 * @generated
 */
public abstract class ChartImpl extends AnalysisItemImpl implements Chart {
    /**
     * The default value of the '{@link #getScript() <em>Script</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getScript()
     * @generated
     * @ordered
     */
    protected static final String SCRIPT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getScript() <em>Script</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getScript()
     * @generated
     * @ordered
     */
    protected String script = SCRIPT_EDEFAULT;

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
     * The default value of the '{@link #getForm() <em>Form</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getForm()
     * @generated
     * @ordered
     */
    protected static final String FORM_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getForm() <em>Form</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getForm()
     * @generated
     * @ordered
     */
    protected String form = FORM_EDEFAULT;

    /**
     * The default value of the '{@link #getTemplateID() <em>Template ID</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTemplateID()
     * @generated
     * @ordered
     */
    protected static final String TEMPLATE_ID_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getTemplateID() <em>Template ID</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTemplateID()
     * @generated
     * @ordered
     */
    protected String templateID = TEMPLATE_ID_EDEFAULT;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ChartImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ScaveModelPackage.Literals.CHART;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getScript() {
        return script;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setScript(String newScript) {
        String oldScript = script;
        script = newScript;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.CHART__SCRIPT, oldScript, script));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList<Property> getProperties() {
        if (properties == null) {
            properties = new EObjectContainmentEList<Property>(Property.class, this, ScaveModelPackage.CHART__PROPERTIES);
        }
        return properties;
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
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.CHART__TEMPORARY, oldTemporary, temporary));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getForm() {
        return form;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setForm(String newForm) {
        String oldForm = form;
        form = newForm;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.CHART__FORM, oldForm, form));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getTemplateID() {
        return templateID;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setTemplateID(String newTemplateID) {
        String oldTemplateID = templateID;
        templateID = newTemplateID;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.CHART__TEMPLATE_ID, oldTemplateID, templateID));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
        switch (featureID) {
            case ScaveModelPackage.CHART__PROPERTIES:
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
            case ScaveModelPackage.CHART__SCRIPT:
                return getScript();
            case ScaveModelPackage.CHART__PROPERTIES:
                return getProperties();
            case ScaveModelPackage.CHART__TEMPORARY:
                return isTemporary();
            case ScaveModelPackage.CHART__FORM:
                return getForm();
            case ScaveModelPackage.CHART__TEMPLATE_ID:
                return getTemplateID();
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
            case ScaveModelPackage.CHART__SCRIPT:
                setScript((String)newValue);
                return;
            case ScaveModelPackage.CHART__PROPERTIES:
                getProperties().clear();
                getProperties().addAll((Collection<? extends Property>)newValue);
                return;
            case ScaveModelPackage.CHART__TEMPORARY:
                setTemporary((Boolean)newValue);
                return;
            case ScaveModelPackage.CHART__FORM:
                setForm((String)newValue);
                return;
            case ScaveModelPackage.CHART__TEMPLATE_ID:
                setTemplateID((String)newValue);
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
            case ScaveModelPackage.CHART__SCRIPT:
                setScript(SCRIPT_EDEFAULT);
                return;
            case ScaveModelPackage.CHART__PROPERTIES:
                getProperties().clear();
                return;
            case ScaveModelPackage.CHART__TEMPORARY:
                setTemporary(TEMPORARY_EDEFAULT);
                return;
            case ScaveModelPackage.CHART__FORM:
                setForm(FORM_EDEFAULT);
                return;
            case ScaveModelPackage.CHART__TEMPLATE_ID:
                setTemplateID(TEMPLATE_ID_EDEFAULT);
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
            case ScaveModelPackage.CHART__SCRIPT:
                return SCRIPT_EDEFAULT == null ? script != null : !SCRIPT_EDEFAULT.equals(script);
            case ScaveModelPackage.CHART__PROPERTIES:
                return properties != null && !properties.isEmpty();
            case ScaveModelPackage.CHART__TEMPORARY:
                return temporary != TEMPORARY_EDEFAULT;
            case ScaveModelPackage.CHART__FORM:
                return FORM_EDEFAULT == null ? form != null : !FORM_EDEFAULT.equals(form);
            case ScaveModelPackage.CHART__TEMPLATE_ID:
                return TEMPLATE_ID_EDEFAULT == null ? templateID != null : !TEMPLATE_ID_EDEFAULT.equals(templateID);
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
        result.append(" (script: ");
        result.append(script);
        result.append(", temporary: ");
        result.append(temporary);
        result.append(", form: ");
        result.append(form);
        result.append(", templateID: ");
        result.append(templateID);
        result.append(')');
        return result.toString();
    }

} //ChartImpl
