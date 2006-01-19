/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.impl;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.omnetpp.ned.model.emf.CompoundModule;
import org.omnetpp.ned.model.emf.Connections;
import org.omnetpp.ned.model.emf.DisplayString;
import org.omnetpp.ned.model.emf.Gates;
import org.omnetpp.ned.model.emf.NedPackage;
import org.omnetpp.ned.model.emf.Params;
import org.omnetpp.ned.model.emf.Submodules;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Compound Module</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getParams <em>Params</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getGates <em>Gates</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getSubmodules <em>Submodules</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getConnections <em>Connections</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getDisplayString <em>Display String</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class CompoundModuleImpl extends EObjectImpl implements CompoundModule {
    /**
     * The cached value of the '{@link #getParams() <em>Params</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getParams()
     * @generated
     * @ordered
     */
    protected Params params = null;

    /**
     * The cached value of the '{@link #getGates() <em>Gates</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getGates()
     * @generated
     * @ordered
     */
    protected Gates gates = null;

    /**
     * The cached value of the '{@link #getSubmodules() <em>Submodules</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSubmodules()
     * @generated
     * @ordered
     */
    protected Submodules submodules = null;

    /**
     * The cached value of the '{@link #getConnections() <em>Connections</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getConnections()
     * @generated
     * @ordered
     */
    protected Connections connections = null;

    /**
     * The cached value of the '{@link #getDisplayString() <em>Display String</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDisplayString()
     * @generated
     * @ordered
     */
    protected DisplayString displayString = null;

    /**
     * The default value of the '{@link #getBannerComment() <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getBannerComment()
     * @generated
     * @ordered
     */
    protected static final String BANNER_COMMENT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getBannerComment() <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getBannerComment()
     * @generated
     * @ordered
     */
    protected String bannerComment = BANNER_COMMENT_EDEFAULT;

    /**
     * The default value of the '{@link #getName() <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getName()
     * @generated
     * @ordered
     */
    protected static final String NAME_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getName()
     * @generated
     * @ordered
     */
    protected String name = NAME_EDEFAULT;

    /**
     * The default value of the '{@link #getRightComment() <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getRightComment()
     * @generated
     * @ordered
     */
    protected static final String RIGHT_COMMENT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getRightComment() <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getRightComment()
     * @generated
     * @ordered
     */
    protected String rightComment = RIGHT_COMMENT_EDEFAULT;

    /**
     * The default value of the '{@link #getTrailingComment() <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTrailingComment()
     * @generated
     * @ordered
     */
    protected static final String TRAILING_COMMENT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getTrailingComment() <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTrailingComment()
     * @generated
     * @ordered
     */
    protected String trailingComment = TRAILING_COMMENT_EDEFAULT;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected CompoundModuleImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected EClass eStaticClass() {
        return NedPackage.eINSTANCE.getCompoundModule();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Params getParams() {
        return params;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetParams(Params newParams, NotificationChain msgs) {
        Params oldParams = params;
        params = newParams;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__PARAMS, oldParams, newParams);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setParams(Params newParams) {
        if (newParams != params) {
            NotificationChain msgs = null;
            if (params != null)
                msgs = ((InternalEObject)params).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__PARAMS, null, msgs);
            if (newParams != null)
                msgs = ((InternalEObject)newParams).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__PARAMS, null, msgs);
            msgs = basicSetParams(newParams, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__PARAMS, newParams, newParams));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Gates getGates() {
        return gates;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetGates(Gates newGates, NotificationChain msgs) {
        Gates oldGates = gates;
        gates = newGates;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__GATES, oldGates, newGates);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setGates(Gates newGates) {
        if (newGates != gates) {
            NotificationChain msgs = null;
            if (gates != null)
                msgs = ((InternalEObject)gates).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__GATES, null, msgs);
            if (newGates != null)
                msgs = ((InternalEObject)newGates).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__GATES, null, msgs);
            msgs = basicSetGates(newGates, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__GATES, newGates, newGates));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Submodules getSubmodules() {
        return submodules;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetSubmodules(Submodules newSubmodules, NotificationChain msgs) {
        Submodules oldSubmodules = submodules;
        submodules = newSubmodules;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__SUBMODULES, oldSubmodules, newSubmodules);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setSubmodules(Submodules newSubmodules) {
        if (newSubmodules != submodules) {
            NotificationChain msgs = null;
            if (submodules != null)
                msgs = ((InternalEObject)submodules).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__SUBMODULES, null, msgs);
            if (newSubmodules != null)
                msgs = ((InternalEObject)newSubmodules).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__SUBMODULES, null, msgs);
            msgs = basicSetSubmodules(newSubmodules, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__SUBMODULES, newSubmodules, newSubmodules));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Connections getConnections() {
        return connections;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetConnections(Connections newConnections, NotificationChain msgs) {
        Connections oldConnections = connections;
        connections = newConnections;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__CONNECTIONS, oldConnections, newConnections);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setConnections(Connections newConnections) {
        if (newConnections != connections) {
            NotificationChain msgs = null;
            if (connections != null)
                msgs = ((InternalEObject)connections).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__CONNECTIONS, null, msgs);
            if (newConnections != null)
                msgs = ((InternalEObject)newConnections).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__CONNECTIONS, null, msgs);
            msgs = basicSetConnections(newConnections, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__CONNECTIONS, newConnections, newConnections));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public DisplayString getDisplayString() {
        return displayString;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetDisplayString(DisplayString newDisplayString, NotificationChain msgs) {
        DisplayString oldDisplayString = displayString;
        displayString = newDisplayString;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__DISPLAY_STRING, oldDisplayString, newDisplayString);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDisplayString(DisplayString newDisplayString) {
        if (newDisplayString != displayString) {
            NotificationChain msgs = null;
            if (displayString != null)
                msgs = ((InternalEObject)displayString).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__DISPLAY_STRING, null, msgs);
            if (newDisplayString != null)
                msgs = ((InternalEObject)newDisplayString).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.COMPOUND_MODULE__DISPLAY_STRING, null, msgs);
            msgs = basicSetDisplayString(newDisplayString, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__DISPLAY_STRING, newDisplayString, newDisplayString));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getBannerComment() {
        return bannerComment;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setBannerComment(String newBannerComment) {
        String oldBannerComment = bannerComment;
        bannerComment = newBannerComment;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__BANNER_COMMENT, oldBannerComment, bannerComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getName() {
        return name;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setName(String newName) {
        String oldName = name;
        name = newName;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__NAME, oldName, name));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getRightComment() {
        return rightComment;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setRightComment(String newRightComment) {
        String oldRightComment = rightComment;
        rightComment = newRightComment;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__RIGHT_COMMENT, oldRightComment, rightComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getTrailingComment() {
        return trailingComment;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setTrailingComment(String newTrailingComment) {
        String oldTrailingComment = trailingComment;
        trailingComment = newTrailingComment;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.COMPOUND_MODULE__TRAILING_COMMENT, oldTrailingComment, trailingComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, Class baseClass, NotificationChain msgs) {
        if (featureID >= 0) {
            switch (eDerivedStructuralFeatureID(featureID, baseClass)) {
                case NedPackage.COMPOUND_MODULE__PARAMS:
                    return basicSetParams(null, msgs);
                case NedPackage.COMPOUND_MODULE__GATES:
                    return basicSetGates(null, msgs);
                case NedPackage.COMPOUND_MODULE__SUBMODULES:
                    return basicSetSubmodules(null, msgs);
                case NedPackage.COMPOUND_MODULE__CONNECTIONS:
                    return basicSetConnections(null, msgs);
                case NedPackage.COMPOUND_MODULE__DISPLAY_STRING:
                    return basicSetDisplayString(null, msgs);
                default:
                    return eDynamicInverseRemove(otherEnd, featureID, baseClass, msgs);
            }
        }
        return eBasicSetContainer(null, featureID, msgs);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Object eGet(EStructuralFeature eFeature, boolean resolve) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.COMPOUND_MODULE__PARAMS:
                return getParams();
            case NedPackage.COMPOUND_MODULE__GATES:
                return getGates();
            case NedPackage.COMPOUND_MODULE__SUBMODULES:
                return getSubmodules();
            case NedPackage.COMPOUND_MODULE__CONNECTIONS:
                return getConnections();
            case NedPackage.COMPOUND_MODULE__DISPLAY_STRING:
                return getDisplayString();
            case NedPackage.COMPOUND_MODULE__BANNER_COMMENT:
                return getBannerComment();
            case NedPackage.COMPOUND_MODULE__NAME:
                return getName();
            case NedPackage.COMPOUND_MODULE__RIGHT_COMMENT:
                return getRightComment();
            case NedPackage.COMPOUND_MODULE__TRAILING_COMMENT:
                return getTrailingComment();
        }
        return eDynamicGet(eFeature, resolve);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void eSet(EStructuralFeature eFeature, Object newValue) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.COMPOUND_MODULE__PARAMS:
                setParams((Params)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__GATES:
                setGates((Gates)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__SUBMODULES:
                setSubmodules((Submodules)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__CONNECTIONS:
                setConnections((Connections)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__DISPLAY_STRING:
                setDisplayString((DisplayString)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__BANNER_COMMENT:
                setBannerComment((String)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__NAME:
                setName((String)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__RIGHT_COMMENT:
                setRightComment((String)newValue);
                return;
            case NedPackage.COMPOUND_MODULE__TRAILING_COMMENT:
                setTrailingComment((String)newValue);
                return;
        }
        eDynamicSet(eFeature, newValue);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void eUnset(EStructuralFeature eFeature) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.COMPOUND_MODULE__PARAMS:
                setParams((Params)null);
                return;
            case NedPackage.COMPOUND_MODULE__GATES:
                setGates((Gates)null);
                return;
            case NedPackage.COMPOUND_MODULE__SUBMODULES:
                setSubmodules((Submodules)null);
                return;
            case NedPackage.COMPOUND_MODULE__CONNECTIONS:
                setConnections((Connections)null);
                return;
            case NedPackage.COMPOUND_MODULE__DISPLAY_STRING:
                setDisplayString((DisplayString)null);
                return;
            case NedPackage.COMPOUND_MODULE__BANNER_COMMENT:
                setBannerComment(BANNER_COMMENT_EDEFAULT);
                return;
            case NedPackage.COMPOUND_MODULE__NAME:
                setName(NAME_EDEFAULT);
                return;
            case NedPackage.COMPOUND_MODULE__RIGHT_COMMENT:
                setRightComment(RIGHT_COMMENT_EDEFAULT);
                return;
            case NedPackage.COMPOUND_MODULE__TRAILING_COMMENT:
                setTrailingComment(TRAILING_COMMENT_EDEFAULT);
                return;
        }
        eDynamicUnset(eFeature);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean eIsSet(EStructuralFeature eFeature) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.COMPOUND_MODULE__PARAMS:
                return params != null;
            case NedPackage.COMPOUND_MODULE__GATES:
                return gates != null;
            case NedPackage.COMPOUND_MODULE__SUBMODULES:
                return submodules != null;
            case NedPackage.COMPOUND_MODULE__CONNECTIONS:
                return connections != null;
            case NedPackage.COMPOUND_MODULE__DISPLAY_STRING:
                return displayString != null;
            case NedPackage.COMPOUND_MODULE__BANNER_COMMENT:
                return BANNER_COMMENT_EDEFAULT == null ? bannerComment != null : !BANNER_COMMENT_EDEFAULT.equals(bannerComment);
            case NedPackage.COMPOUND_MODULE__NAME:
                return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
            case NedPackage.COMPOUND_MODULE__RIGHT_COMMENT:
                return RIGHT_COMMENT_EDEFAULT == null ? rightComment != null : !RIGHT_COMMENT_EDEFAULT.equals(rightComment);
            case NedPackage.COMPOUND_MODULE__TRAILING_COMMENT:
                return TRAILING_COMMENT_EDEFAULT == null ? trailingComment != null : !TRAILING_COMMENT_EDEFAULT.equals(trailingComment);
        }
        return eDynamicIsSet(eFeature);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String toString() {
        if (eIsProxy()) return super.toString();

        StringBuffer result = new StringBuffer(super.toString());
        result.append(" (bannerComment: ");
        result.append(bannerComment);
        result.append(", name: ");
        result.append(name);
        result.append(", rightComment: ");
        result.append(rightComment);
        result.append(", trailingComment: ");
        result.append(trailingComment);
        result.append(')');
        return result.toString();
    }

} //CompoundModuleImpl
