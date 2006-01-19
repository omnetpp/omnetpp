/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned.model.emf.ConnAttr;
import org.omnetpp.ned.model.emf.Connection;
import org.omnetpp.ned.model.emf.DisplayString;
import org.omnetpp.ned.model.emf.NedPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Connection</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getConnAttr <em>Conn Attr</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getDisplayString <em>Display String</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getCondition <em>Condition</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getDestGate <em>Dest Gate</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getDestGateIndex <em>Dest Gate Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#isDestGatePlusplus <em>Dest Gate Plusplus</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getDestModule <em>Dest Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getDestModuleIndex <em>Dest Module Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#isRightToLeft <em>Right To Left</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getSrcGate <em>Src Gate</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getSrcGateIndex <em>Src Gate Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#isSrcGatePlusplus <em>Src Gate Plusplus</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getSrcModule <em>Src Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getSrcModuleIndex <em>Src Module Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ConnectionImpl extends EObjectImpl implements Connection {
    /**
     * The cached value of the '{@link #getConnAttr() <em>Conn Attr</em>}' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getConnAttr()
     * @generated
     * @ordered
     */
    protected EList connAttr = null;

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
     * The default value of the '{@link #getCondition() <em>Condition</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getCondition()
     * @generated
     * @ordered
     */
    protected static final String CONDITION_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getCondition() <em>Condition</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getCondition()
     * @generated
     * @ordered
     */
    protected String condition = CONDITION_EDEFAULT;

    /**
     * The default value of the '{@link #getDestGate() <em>Dest Gate</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestGate()
     * @generated
     * @ordered
     */
    protected static final String DEST_GATE_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getDestGate() <em>Dest Gate</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestGate()
     * @generated
     * @ordered
     */
    protected String destGate = DEST_GATE_EDEFAULT;

    /**
     * The default value of the '{@link #getDestGateIndex() <em>Dest Gate Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestGateIndex()
     * @generated
     * @ordered
     */
    protected static final String DEST_GATE_INDEX_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getDestGateIndex() <em>Dest Gate Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestGateIndex()
     * @generated
     * @ordered
     */
    protected String destGateIndex = DEST_GATE_INDEX_EDEFAULT;

    /**
     * The default value of the '{@link #isDestGatePlusplus() <em>Dest Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isDestGatePlusplus()
     * @generated
     * @ordered
     */
    protected static final boolean DEST_GATE_PLUSPLUS_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isDestGatePlusplus() <em>Dest Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isDestGatePlusplus()
     * @generated
     * @ordered
     */
    protected boolean destGatePlusplus = DEST_GATE_PLUSPLUS_EDEFAULT;

    /**
     * This is true if the Dest Gate Plusplus attribute has been set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    protected boolean destGatePlusplusESet = false;

    /**
     * The default value of the '{@link #getDestModule() <em>Dest Module</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestModule()
     * @generated
     * @ordered
     */
    protected static final String DEST_MODULE_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getDestModule() <em>Dest Module</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestModule()
     * @generated
     * @ordered
     */
    protected String destModule = DEST_MODULE_EDEFAULT;

    /**
     * The default value of the '{@link #getDestModuleIndex() <em>Dest Module Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestModuleIndex()
     * @generated
     * @ordered
     */
    protected static final String DEST_MODULE_INDEX_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getDestModuleIndex() <em>Dest Module Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDestModuleIndex()
     * @generated
     * @ordered
     */
    protected String destModuleIndex = DEST_MODULE_INDEX_EDEFAULT;

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
     * The default value of the '{@link #isRightToLeft() <em>Right To Left</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isRightToLeft()
     * @generated
     * @ordered
     */
    protected static final boolean RIGHT_TO_LEFT_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isRightToLeft() <em>Right To Left</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isRightToLeft()
     * @generated
     * @ordered
     */
    protected boolean rightToLeft = RIGHT_TO_LEFT_EDEFAULT;

    /**
     * This is true if the Right To Left attribute has been set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    protected boolean rightToLeftESet = false;

    /**
     * The default value of the '{@link #getSrcGate() <em>Src Gate</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcGate()
     * @generated
     * @ordered
     */
    protected static final String SRC_GATE_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getSrcGate() <em>Src Gate</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcGate()
     * @generated
     * @ordered
     */
    protected String srcGate = SRC_GATE_EDEFAULT;

    /**
     * The default value of the '{@link #getSrcGateIndex() <em>Src Gate Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcGateIndex()
     * @generated
     * @ordered
     */
    protected static final String SRC_GATE_INDEX_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getSrcGateIndex() <em>Src Gate Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcGateIndex()
     * @generated
     * @ordered
     */
    protected String srcGateIndex = SRC_GATE_INDEX_EDEFAULT;

    /**
     * The default value of the '{@link #isSrcGatePlusplus() <em>Src Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isSrcGatePlusplus()
     * @generated
     * @ordered
     */
    protected static final boolean SRC_GATE_PLUSPLUS_EDEFAULT = false;

    /**
     * The cached value of the '{@link #isSrcGatePlusplus() <em>Src Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isSrcGatePlusplus()
     * @generated
     * @ordered
     */
    protected boolean srcGatePlusplus = SRC_GATE_PLUSPLUS_EDEFAULT;

    /**
     * This is true if the Src Gate Plusplus attribute has been set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    protected boolean srcGatePlusplusESet = false;

    /**
     * The default value of the '{@link #getSrcModule() <em>Src Module</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcModule()
     * @generated
     * @ordered
     */
    protected static final String SRC_MODULE_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getSrcModule() <em>Src Module</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcModule()
     * @generated
     * @ordered
     */
    protected String srcModule = SRC_MODULE_EDEFAULT;

    /**
     * The default value of the '{@link #getSrcModuleIndex() <em>Src Module Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcModuleIndex()
     * @generated
     * @ordered
     */
    protected static final String SRC_MODULE_INDEX_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getSrcModuleIndex() <em>Src Module Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSrcModuleIndex()
     * @generated
     * @ordered
     */
    protected String srcModuleIndex = SRC_MODULE_INDEX_EDEFAULT;

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
    protected ConnectionImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected EClass eStaticClass() {
        return NedPackage.eINSTANCE.getConnection();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getConnAttr() {
        if (connAttr == null) {
            connAttr = new EObjectContainmentEList(ConnAttr.class, this, NedPackage.CONNECTION__CONN_ATTR);
        }
        return connAttr;
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
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DISPLAY_STRING, oldDisplayString, newDisplayString);
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
                msgs = ((InternalEObject)displayString).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.CONNECTION__DISPLAY_STRING, null, msgs);
            if (newDisplayString != null)
                msgs = ((InternalEObject)newDisplayString).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.CONNECTION__DISPLAY_STRING, null, msgs);
            msgs = basicSetDisplayString(newDisplayString, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DISPLAY_STRING, newDisplayString, newDisplayString));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__BANNER_COMMENT, oldBannerComment, bannerComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getCondition() {
        return condition;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setCondition(String newCondition) {
        String oldCondition = condition;
        condition = newCondition;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__CONDITION, oldCondition, condition));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getDestGate() {
        return destGate;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDestGate(String newDestGate) {
        String oldDestGate = destGate;
        destGate = newDestGate;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DEST_GATE, oldDestGate, destGate));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getDestGateIndex() {
        return destGateIndex;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDestGateIndex(String newDestGateIndex) {
        String oldDestGateIndex = destGateIndex;
        destGateIndex = newDestGateIndex;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DEST_GATE_INDEX, oldDestGateIndex, destGateIndex));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isDestGatePlusplus() {
        return destGatePlusplus;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDestGatePlusplus(boolean newDestGatePlusplus) {
        boolean oldDestGatePlusplus = destGatePlusplus;
        destGatePlusplus = newDestGatePlusplus;
        boolean oldDestGatePlusplusESet = destGatePlusplusESet;
        destGatePlusplusESet = true;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DEST_GATE_PLUSPLUS, oldDestGatePlusplus, destGatePlusplus, !oldDestGatePlusplusESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void unsetDestGatePlusplus() {
        boolean oldDestGatePlusplus = destGatePlusplus;
        boolean oldDestGatePlusplusESet = destGatePlusplusESet;
        destGatePlusplus = DEST_GATE_PLUSPLUS_EDEFAULT;
        destGatePlusplusESet = false;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.UNSET, NedPackage.CONNECTION__DEST_GATE_PLUSPLUS, oldDestGatePlusplus, DEST_GATE_PLUSPLUS_EDEFAULT, oldDestGatePlusplusESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isSetDestGatePlusplus() {
        return destGatePlusplusESet;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getDestModule() {
        return destModule;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDestModule(String newDestModule) {
        String oldDestModule = destModule;
        destModule = newDestModule;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DEST_MODULE, oldDestModule, destModule));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getDestModuleIndex() {
        return destModuleIndex;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDestModuleIndex(String newDestModuleIndex) {
        String oldDestModuleIndex = destModuleIndex;
        destModuleIndex = newDestModuleIndex;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__DEST_MODULE_INDEX, oldDestModuleIndex, destModuleIndex));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__RIGHT_COMMENT, oldRightComment, rightComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isRightToLeft() {
        return rightToLeft;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setRightToLeft(boolean newRightToLeft) {
        boolean oldRightToLeft = rightToLeft;
        rightToLeft = newRightToLeft;
        boolean oldRightToLeftESet = rightToLeftESet;
        rightToLeftESet = true;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__RIGHT_TO_LEFT, oldRightToLeft, rightToLeft, !oldRightToLeftESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void unsetRightToLeft() {
        boolean oldRightToLeft = rightToLeft;
        boolean oldRightToLeftESet = rightToLeftESet;
        rightToLeft = RIGHT_TO_LEFT_EDEFAULT;
        rightToLeftESet = false;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.UNSET, NedPackage.CONNECTION__RIGHT_TO_LEFT, oldRightToLeft, RIGHT_TO_LEFT_EDEFAULT, oldRightToLeftESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isSetRightToLeft() {
        return rightToLeftESet;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getSrcGate() {
        return srcGate;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setSrcGate(String newSrcGate) {
        String oldSrcGate = srcGate;
        srcGate = newSrcGate;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__SRC_GATE, oldSrcGate, srcGate));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getSrcGateIndex() {
        return srcGateIndex;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setSrcGateIndex(String newSrcGateIndex) {
        String oldSrcGateIndex = srcGateIndex;
        srcGateIndex = newSrcGateIndex;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__SRC_GATE_INDEX, oldSrcGateIndex, srcGateIndex));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isSrcGatePlusplus() {
        return srcGatePlusplus;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setSrcGatePlusplus(boolean newSrcGatePlusplus) {
        boolean oldSrcGatePlusplus = srcGatePlusplus;
        srcGatePlusplus = newSrcGatePlusplus;
        boolean oldSrcGatePlusplusESet = srcGatePlusplusESet;
        srcGatePlusplusESet = true;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__SRC_GATE_PLUSPLUS, oldSrcGatePlusplus, srcGatePlusplus, !oldSrcGatePlusplusESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void unsetSrcGatePlusplus() {
        boolean oldSrcGatePlusplus = srcGatePlusplus;
        boolean oldSrcGatePlusplusESet = srcGatePlusplusESet;
        srcGatePlusplus = SRC_GATE_PLUSPLUS_EDEFAULT;
        srcGatePlusplusESet = false;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.UNSET, NedPackage.CONNECTION__SRC_GATE_PLUSPLUS, oldSrcGatePlusplus, SRC_GATE_PLUSPLUS_EDEFAULT, oldSrcGatePlusplusESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isSetSrcGatePlusplus() {
        return srcGatePlusplusESet;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getSrcModule() {
        return srcModule;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setSrcModule(String newSrcModule) {
        String oldSrcModule = srcModule;
        srcModule = newSrcModule;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__SRC_MODULE, oldSrcModule, srcModule));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getSrcModuleIndex() {
        return srcModuleIndex;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setSrcModuleIndex(String newSrcModuleIndex) {
        String oldSrcModuleIndex = srcModuleIndex;
        srcModuleIndex = newSrcModuleIndex;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__SRC_MODULE_INDEX, oldSrcModuleIndex, srcModuleIndex));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTION__TRAILING_COMMENT, oldTrailingComment, trailingComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, Class baseClass, NotificationChain msgs) {
        if (featureID >= 0) {
            switch (eDerivedStructuralFeatureID(featureID, baseClass)) {
                case NedPackage.CONNECTION__CONN_ATTR:
                    return ((InternalEList)getConnAttr()).basicRemove(otherEnd, msgs);
                case NedPackage.CONNECTION__DISPLAY_STRING:
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
            case NedPackage.CONNECTION__CONN_ATTR:
                return getConnAttr();
            case NedPackage.CONNECTION__DISPLAY_STRING:
                return getDisplayString();
            case NedPackage.CONNECTION__BANNER_COMMENT:
                return getBannerComment();
            case NedPackage.CONNECTION__CONDITION:
                return getCondition();
            case NedPackage.CONNECTION__DEST_GATE:
                return getDestGate();
            case NedPackage.CONNECTION__DEST_GATE_INDEX:
                return getDestGateIndex();
            case NedPackage.CONNECTION__DEST_GATE_PLUSPLUS:
                return isDestGatePlusplus() ? Boolean.TRUE : Boolean.FALSE;
            case NedPackage.CONNECTION__DEST_MODULE:
                return getDestModule();
            case NedPackage.CONNECTION__DEST_MODULE_INDEX:
                return getDestModuleIndex();
            case NedPackage.CONNECTION__RIGHT_COMMENT:
                return getRightComment();
            case NedPackage.CONNECTION__RIGHT_TO_LEFT:
                return isRightToLeft() ? Boolean.TRUE : Boolean.FALSE;
            case NedPackage.CONNECTION__SRC_GATE:
                return getSrcGate();
            case NedPackage.CONNECTION__SRC_GATE_INDEX:
                return getSrcGateIndex();
            case NedPackage.CONNECTION__SRC_GATE_PLUSPLUS:
                return isSrcGatePlusplus() ? Boolean.TRUE : Boolean.FALSE;
            case NedPackage.CONNECTION__SRC_MODULE:
                return getSrcModule();
            case NedPackage.CONNECTION__SRC_MODULE_INDEX:
                return getSrcModuleIndex();
            case NedPackage.CONNECTION__TRAILING_COMMENT:
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
            case NedPackage.CONNECTION__CONN_ATTR:
                getConnAttr().clear();
                getConnAttr().addAll((Collection)newValue);
                return;
            case NedPackage.CONNECTION__DISPLAY_STRING:
                setDisplayString((DisplayString)newValue);
                return;
            case NedPackage.CONNECTION__BANNER_COMMENT:
                setBannerComment((String)newValue);
                return;
            case NedPackage.CONNECTION__CONDITION:
                setCondition((String)newValue);
                return;
            case NedPackage.CONNECTION__DEST_GATE:
                setDestGate((String)newValue);
                return;
            case NedPackage.CONNECTION__DEST_GATE_INDEX:
                setDestGateIndex((String)newValue);
                return;
            case NedPackage.CONNECTION__DEST_GATE_PLUSPLUS:
                setDestGatePlusplus(((Boolean)newValue).booleanValue());
                return;
            case NedPackage.CONNECTION__DEST_MODULE:
                setDestModule((String)newValue);
                return;
            case NedPackage.CONNECTION__DEST_MODULE_INDEX:
                setDestModuleIndex((String)newValue);
                return;
            case NedPackage.CONNECTION__RIGHT_COMMENT:
                setRightComment((String)newValue);
                return;
            case NedPackage.CONNECTION__RIGHT_TO_LEFT:
                setRightToLeft(((Boolean)newValue).booleanValue());
                return;
            case NedPackage.CONNECTION__SRC_GATE:
                setSrcGate((String)newValue);
                return;
            case NedPackage.CONNECTION__SRC_GATE_INDEX:
                setSrcGateIndex((String)newValue);
                return;
            case NedPackage.CONNECTION__SRC_GATE_PLUSPLUS:
                setSrcGatePlusplus(((Boolean)newValue).booleanValue());
                return;
            case NedPackage.CONNECTION__SRC_MODULE:
                setSrcModule((String)newValue);
                return;
            case NedPackage.CONNECTION__SRC_MODULE_INDEX:
                setSrcModuleIndex((String)newValue);
                return;
            case NedPackage.CONNECTION__TRAILING_COMMENT:
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
            case NedPackage.CONNECTION__CONN_ATTR:
                getConnAttr().clear();
                return;
            case NedPackage.CONNECTION__DISPLAY_STRING:
                setDisplayString((DisplayString)null);
                return;
            case NedPackage.CONNECTION__BANNER_COMMENT:
                setBannerComment(BANNER_COMMENT_EDEFAULT);
                return;
            case NedPackage.CONNECTION__CONDITION:
                setCondition(CONDITION_EDEFAULT);
                return;
            case NedPackage.CONNECTION__DEST_GATE:
                setDestGate(DEST_GATE_EDEFAULT);
                return;
            case NedPackage.CONNECTION__DEST_GATE_INDEX:
                setDestGateIndex(DEST_GATE_INDEX_EDEFAULT);
                return;
            case NedPackage.CONNECTION__DEST_GATE_PLUSPLUS:
                unsetDestGatePlusplus();
                return;
            case NedPackage.CONNECTION__DEST_MODULE:
                setDestModule(DEST_MODULE_EDEFAULT);
                return;
            case NedPackage.CONNECTION__DEST_MODULE_INDEX:
                setDestModuleIndex(DEST_MODULE_INDEX_EDEFAULT);
                return;
            case NedPackage.CONNECTION__RIGHT_COMMENT:
                setRightComment(RIGHT_COMMENT_EDEFAULT);
                return;
            case NedPackage.CONNECTION__RIGHT_TO_LEFT:
                unsetRightToLeft();
                return;
            case NedPackage.CONNECTION__SRC_GATE:
                setSrcGate(SRC_GATE_EDEFAULT);
                return;
            case NedPackage.CONNECTION__SRC_GATE_INDEX:
                setSrcGateIndex(SRC_GATE_INDEX_EDEFAULT);
                return;
            case NedPackage.CONNECTION__SRC_GATE_PLUSPLUS:
                unsetSrcGatePlusplus();
                return;
            case NedPackage.CONNECTION__SRC_MODULE:
                setSrcModule(SRC_MODULE_EDEFAULT);
                return;
            case NedPackage.CONNECTION__SRC_MODULE_INDEX:
                setSrcModuleIndex(SRC_MODULE_INDEX_EDEFAULT);
                return;
            case NedPackage.CONNECTION__TRAILING_COMMENT:
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
            case NedPackage.CONNECTION__CONN_ATTR:
                return connAttr != null && !connAttr.isEmpty();
            case NedPackage.CONNECTION__DISPLAY_STRING:
                return displayString != null;
            case NedPackage.CONNECTION__BANNER_COMMENT:
                return BANNER_COMMENT_EDEFAULT == null ? bannerComment != null : !BANNER_COMMENT_EDEFAULT.equals(bannerComment);
            case NedPackage.CONNECTION__CONDITION:
                return CONDITION_EDEFAULT == null ? condition != null : !CONDITION_EDEFAULT.equals(condition);
            case NedPackage.CONNECTION__DEST_GATE:
                return DEST_GATE_EDEFAULT == null ? destGate != null : !DEST_GATE_EDEFAULT.equals(destGate);
            case NedPackage.CONNECTION__DEST_GATE_INDEX:
                return DEST_GATE_INDEX_EDEFAULT == null ? destGateIndex != null : !DEST_GATE_INDEX_EDEFAULT.equals(destGateIndex);
            case NedPackage.CONNECTION__DEST_GATE_PLUSPLUS:
                return isSetDestGatePlusplus();
            case NedPackage.CONNECTION__DEST_MODULE:
                return DEST_MODULE_EDEFAULT == null ? destModule != null : !DEST_MODULE_EDEFAULT.equals(destModule);
            case NedPackage.CONNECTION__DEST_MODULE_INDEX:
                return DEST_MODULE_INDEX_EDEFAULT == null ? destModuleIndex != null : !DEST_MODULE_INDEX_EDEFAULT.equals(destModuleIndex);
            case NedPackage.CONNECTION__RIGHT_COMMENT:
                return RIGHT_COMMENT_EDEFAULT == null ? rightComment != null : !RIGHT_COMMENT_EDEFAULT.equals(rightComment);
            case NedPackage.CONNECTION__RIGHT_TO_LEFT:
                return isSetRightToLeft();
            case NedPackage.CONNECTION__SRC_GATE:
                return SRC_GATE_EDEFAULT == null ? srcGate != null : !SRC_GATE_EDEFAULT.equals(srcGate);
            case NedPackage.CONNECTION__SRC_GATE_INDEX:
                return SRC_GATE_INDEX_EDEFAULT == null ? srcGateIndex != null : !SRC_GATE_INDEX_EDEFAULT.equals(srcGateIndex);
            case NedPackage.CONNECTION__SRC_GATE_PLUSPLUS:
                return isSetSrcGatePlusplus();
            case NedPackage.CONNECTION__SRC_MODULE:
                return SRC_MODULE_EDEFAULT == null ? srcModule != null : !SRC_MODULE_EDEFAULT.equals(srcModule);
            case NedPackage.CONNECTION__SRC_MODULE_INDEX:
                return SRC_MODULE_INDEX_EDEFAULT == null ? srcModuleIndex != null : !SRC_MODULE_INDEX_EDEFAULT.equals(srcModuleIndex);
            case NedPackage.CONNECTION__TRAILING_COMMENT:
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
        result.append(", condition: ");
        result.append(condition);
        result.append(", destGate: ");
        result.append(destGate);
        result.append(", destGateIndex: ");
        result.append(destGateIndex);
        result.append(", destGatePlusplus: ");
        if (destGatePlusplusESet) result.append(destGatePlusplus); else result.append("<unset>");
        result.append(", destModule: ");
        result.append(destModule);
        result.append(", destModuleIndex: ");
        result.append(destModuleIndex);
        result.append(", rightComment: ");
        result.append(rightComment);
        result.append(", rightToLeft: ");
        if (rightToLeftESet) result.append(rightToLeft); else result.append("<unset>");
        result.append(", srcGate: ");
        result.append(srcGate);
        result.append(", srcGateIndex: ");
        result.append(srcGateIndex);
        result.append(", srcGatePlusplus: ");
        if (srcGatePlusplusESet) result.append(srcGatePlusplus); else result.append("<unset>");
        result.append(", srcModule: ");
        result.append(srcModule);
        result.append(", srcModuleIndex: ");
        result.append(srcModuleIndex);
        result.append(", trailingComment: ");
        result.append(trailingComment);
        result.append(')');
        return result.toString();
    }

} //ConnectionImpl
