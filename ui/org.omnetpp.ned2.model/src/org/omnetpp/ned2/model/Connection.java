/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Connection</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getExpression <em>Expression</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getParameters <em>Parameters</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getArrowDirection <em>Arrow Direction</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getChannelName <em>Channel Name</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getDestGate <em>Dest Gate</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getDestGateIndex <em>Dest Gate Index</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getDestGateSubg <em>Dest Gate Subg</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getDestModule <em>Dest Module</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getDestModuleIndex <em>Dest Module Index</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getSrcGate <em>Src Gate</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getSrcGateIndex <em>Src Gate Index</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getSrcGateSubg <em>Src Gate Subg</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getSrcModule <em>Src Module</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Connection#getSrcModuleIndex <em>Src Module Index</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection()
 * @model extendedMetaData="name='connection' kind='elementOnly'"
 * @generated
 */
public class Connection extends EObjectImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The cached value of the '{@link #getWhitespace() <em>Whitespace</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getWhitespace()
	 * @generated
	 * @ordered
	 */
	protected EList whitespace = null;

	/**
	 * The cached value of the '{@link #getExpression() <em>Expression</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExpression()
	 * @generated
	 * @ordered
	 */
	protected EList expression = null;

	/**
	 * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getParameters()
	 * @generated
	 * @ordered
	 */
	protected Parameters parameters = null;

	/**
	 * The default value of the '{@link #getArrowDirection() <em>Arrow Direction</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getArrowDirection()
	 * @generated
	 * @ordered
	 */
	protected static final String ARROW_DIRECTION_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getArrowDirection() <em>Arrow Direction</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getArrowDirection()
	 * @generated
	 * @ordered
	 */
	protected String arrowDirection = ARROW_DIRECTION_EDEFAULT;

	/**
	 * The default value of the '{@link #getChannelName() <em>Channel Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getChannelName()
	 * @generated
	 * @ordered
	 */
	protected static final String CHANNEL_NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getChannelName() <em>Channel Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getChannelName()
	 * @generated
	 * @ordered
	 */
	protected String channelName = CHANNEL_NAME_EDEFAULT;

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
	 * The default value of the '{@link #getDestGateSubg() <em>Dest Gate Subg</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDestGateSubg()
	 * @generated
	 * @ordered
	 */
	protected static final String DEST_GATE_SUBG_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getDestGateSubg() <em>Dest Gate Subg</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDestGateSubg()
	 * @generated
	 * @ordered
	 */
	protected String destGateSubg = DEST_GATE_SUBG_EDEFAULT;

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
	 * The default value of the '{@link #getSrcGateSubg() <em>Src Gate Subg</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSrcGateSubg()
	 * @generated
	 * @ordered
	 */
	protected static final String SRC_GATE_SUBG_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getSrcGateSubg() <em>Src Gate Subg</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSrcGateSubg()
	 * @generated
	 * @ordered
	 */
	protected String srcGateSubg = SRC_GATE_SUBG_EDEFAULT;

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
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Connection() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.CONNECTION;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.CONNECTION__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Expression</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Expression}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Expression</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_Expression()
	 * @model type="org.omnetpp.ned2.model.Expression" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='expression'"
	 * @generated
	 */
	public EList getExpression() {
		if (expression == null) {
			expression = new EObjectContainmentEList(Expression.class, this,
					Ned2Package.CONNECTION__EXPRESSION);
		}
		return expression;
	}

	/**
	 * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Parameters</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Parameters</em>' containment reference.
	 * @see #setParameters(Parameters)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_Parameters()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='parameters'"
	 * @generated
	 */
	public Parameters getParameters() {
		return parameters;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetParameters(Parameters newParameters,
			NotificationChain msgs) {
		Parameters oldParameters = parameters;
		parameters = newParameters;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.CONNECTION__PARAMETERS,
					oldParameters, newParameters);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getParameters <em>Parameters</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Parameters</em>' containment reference.
	 * @see #getParameters()
	 * @generated
	 */
	public void setParameters(Parameters newParameters) {
		if (newParameters != parameters) {
			NotificationChain msgs = null;
			if (parameters != null)
				msgs = ((InternalEObject) parameters).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.CONNECTION__PARAMETERS, null,
						msgs);
			if (newParameters != null)
				msgs = ((InternalEObject) newParameters).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.CONNECTION__PARAMETERS, null,
						msgs);
			msgs = basicSetParameters(newParameters, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__PARAMETERS, newParameters,
					newParameters));
	}

	/**
	 * Returns the value of the '<em><b>Arrow Direction</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Arrow Direction</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Arrow Direction</em>' attribute.
	 * @see #setArrowDirection(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_ArrowDirection()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
	 *        extendedMetaData="kind='attribute' name='arrow-direction'"
	 * @generated
	 */
	public String getArrowDirection() {
		return arrowDirection;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getArrowDirection <em>Arrow Direction</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Arrow Direction</em>' attribute.
	 * @see #getArrowDirection()
	 * @generated
	 */
	public void setArrowDirection(String newArrowDirection) {
		String oldArrowDirection = arrowDirection;
		arrowDirection = newArrowDirection;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__ARROW_DIRECTION, oldArrowDirection,
					arrowDirection));
	}

	/**
	 * Returns the value of the '<em><b>Channel Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Channel Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Channel Name</em>' attribute.
	 * @see #setChannelName(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_ChannelName()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
	 *        extendedMetaData="kind='attribute' name='channel-name'"
	 * @generated
	 */
	public String getChannelName() {
		return channelName;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getChannelName <em>Channel Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Channel Name</em>' attribute.
	 * @see #getChannelName()
	 * @generated
	 */
	public void setChannelName(String newChannelName) {
		String oldChannelName = channelName;
		channelName = newChannelName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__CHANNEL_NAME, oldChannelName,
					channelName));
	}

	/**
	 * Returns the value of the '<em><b>Dest Gate</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Dest Gate</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Dest Gate</em>' attribute.
	 * @see #setDestGate(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_DestGate()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='dest-gate'"
	 * @generated
	 */
	public String getDestGate() {
		return destGate;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getDestGate <em>Dest Gate</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Dest Gate</em>' attribute.
	 * @see #getDestGate()
	 * @generated
	 */
	public void setDestGate(String newDestGate) {
		String oldDestGate = destGate;
		destGate = newDestGate;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__DEST_GATE, oldDestGate, destGate));
	}

	/**
	 * Returns the value of the '<em><b>Dest Gate Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Dest Gate Index</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Dest Gate Index</em>' attribute.
	 * @see #setDestGateIndex(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_DestGateIndex()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='dest-gate-index'"
	 * @generated
	 */
	public String getDestGateIndex() {
		return destGateIndex;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getDestGateIndex <em>Dest Gate Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Dest Gate Index</em>' attribute.
	 * @see #getDestGateIndex()
	 * @generated
	 */
	public void setDestGateIndex(String newDestGateIndex) {
		String oldDestGateIndex = destGateIndex;
		destGateIndex = newDestGateIndex;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__DEST_GATE_INDEX, oldDestGateIndex,
					destGateIndex));
	}

	/**
	 * Returns the value of the '<em><b>Dest Gate Plusplus</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Dest Gate Plusplus</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Dest Gate Plusplus</em>' attribute.
	 * @see #isSetDestGatePlusplus()
	 * @see #unsetDestGatePlusplus()
	 * @see #setDestGatePlusplus(boolean)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_DestGatePlusplus()
	 * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='dest-gate-plusplus'"
	 * @generated
	 */
	public boolean isDestGatePlusplus() {
		return destGatePlusplus;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Dest Gate Plusplus</em>' attribute.
	 * @see #isSetDestGatePlusplus()
	 * @see #unsetDestGatePlusplus()
	 * @see #isDestGatePlusplus()
	 * @generated
	 */
	public void setDestGatePlusplus(boolean newDestGatePlusplus) {
		boolean oldDestGatePlusplus = destGatePlusplus;
		destGatePlusplus = newDestGatePlusplus;
		boolean oldDestGatePlusplusESet = destGatePlusplusESet;
		destGatePlusplusESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS,
					oldDestGatePlusplus, destGatePlusplus,
					!oldDestGatePlusplusESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.ned2.model.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetDestGatePlusplus()
	 * @see #isDestGatePlusplus()
	 * @see #setDestGatePlusplus(boolean)
	 * @generated
	 */
	public void unsetDestGatePlusplus() {
		boolean oldDestGatePlusplus = destGatePlusplus;
		boolean oldDestGatePlusplusESet = destGatePlusplusESet;
		destGatePlusplus = DEST_GATE_PLUSPLUS_EDEFAULT;
		destGatePlusplusESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS,
					oldDestGatePlusplus, DEST_GATE_PLUSPLUS_EDEFAULT,
					oldDestGatePlusplusESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.ned2.model.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Dest Gate Plusplus</em>' attribute is set.
	 * @see #unsetDestGatePlusplus()
	 * @see #isDestGatePlusplus()
	 * @see #setDestGatePlusplus(boolean)
	 * @generated
	 */
	public boolean isSetDestGatePlusplus() {
		return destGatePlusplusESet;
	}

	/**
	 * Returns the value of the '<em><b>Dest Gate Subg</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Dest Gate Subg</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Dest Gate Subg</em>' attribute.
	 * @see #setDestGateSubg(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_DestGateSubg()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='dest-gate-subg'"
	 * @generated
	 */
	public String getDestGateSubg() {
		return destGateSubg;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getDestGateSubg <em>Dest Gate Subg</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Dest Gate Subg</em>' attribute.
	 * @see #getDestGateSubg()
	 * @generated
	 */
	public void setDestGateSubg(String newDestGateSubg) {
		String oldDestGateSubg = destGateSubg;
		destGateSubg = newDestGateSubg;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__DEST_GATE_SUBG, oldDestGateSubg,
					destGateSubg));
	}

	/**
	 * Returns the value of the '<em><b>Dest Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Dest Module</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Dest Module</em>' attribute.
	 * @see #setDestModule(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_DestModule()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
	 *        extendedMetaData="kind='attribute' name='dest-module'"
	 * @generated
	 */
	public String getDestModule() {
		return destModule;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getDestModule <em>Dest Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Dest Module</em>' attribute.
	 * @see #getDestModule()
	 * @generated
	 */
	public void setDestModule(String newDestModule) {
		String oldDestModule = destModule;
		destModule = newDestModule;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__DEST_MODULE, oldDestModule,
					destModule));
	}

	/**
	 * Returns the value of the '<em><b>Dest Module Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Dest Module Index</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Dest Module Index</em>' attribute.
	 * @see #setDestModuleIndex(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_DestModuleIndex()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='dest-module-index'"
	 * @generated
	 */
	public String getDestModuleIndex() {
		return destModuleIndex;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getDestModuleIndex <em>Dest Module Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Dest Module Index</em>' attribute.
	 * @see #getDestModuleIndex()
	 * @generated
	 */
	public void setDestModuleIndex(String newDestModuleIndex) {
		String oldDestModuleIndex = destModuleIndex;
		destModuleIndex = newDestModuleIndex;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__DEST_MODULE_INDEX,
					oldDestModuleIndex, destModuleIndex));
	}

	/**
	 * Returns the value of the '<em><b>Src Gate</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src Gate</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src Gate</em>' attribute.
	 * @see #setSrcGate(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_SrcGate()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='src-gate'"
	 * @generated
	 */
	public String getSrcGate() {
		return srcGate;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getSrcGate <em>Src Gate</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src Gate</em>' attribute.
	 * @see #getSrcGate()
	 * @generated
	 */
	public void setSrcGate(String newSrcGate) {
		String oldSrcGate = srcGate;
		srcGate = newSrcGate;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__SRC_GATE, oldSrcGate, srcGate));
	}

	/**
	 * Returns the value of the '<em><b>Src Gate Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src Gate Index</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src Gate Index</em>' attribute.
	 * @see #setSrcGateIndex(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_SrcGateIndex()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='src-gate-index'"
	 * @generated
	 */
	public String getSrcGateIndex() {
		return srcGateIndex;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getSrcGateIndex <em>Src Gate Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src Gate Index</em>' attribute.
	 * @see #getSrcGateIndex()
	 * @generated
	 */
	public void setSrcGateIndex(String newSrcGateIndex) {
		String oldSrcGateIndex = srcGateIndex;
		srcGateIndex = newSrcGateIndex;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__SRC_GATE_INDEX, oldSrcGateIndex,
					srcGateIndex));
	}

	/**
	 * Returns the value of the '<em><b>Src Gate Plusplus</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src Gate Plusplus</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src Gate Plusplus</em>' attribute.
	 * @see #isSetSrcGatePlusplus()
	 * @see #unsetSrcGatePlusplus()
	 * @see #setSrcGatePlusplus(boolean)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_SrcGatePlusplus()
	 * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='src-gate-plusplus'"
	 * @generated
	 */
	public boolean isSrcGatePlusplus() {
		return srcGatePlusplus;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src Gate Plusplus</em>' attribute.
	 * @see #isSetSrcGatePlusplus()
	 * @see #unsetSrcGatePlusplus()
	 * @see #isSrcGatePlusplus()
	 * @generated
	 */
	public void setSrcGatePlusplus(boolean newSrcGatePlusplus) {
		boolean oldSrcGatePlusplus = srcGatePlusplus;
		srcGatePlusplus = newSrcGatePlusplus;
		boolean oldSrcGatePlusplusESet = srcGatePlusplusESet;
		srcGatePlusplusESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS,
					oldSrcGatePlusplus, srcGatePlusplus,
					!oldSrcGatePlusplusESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.ned2.model.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetSrcGatePlusplus()
	 * @see #isSrcGatePlusplus()
	 * @see #setSrcGatePlusplus(boolean)
	 * @generated
	 */
	public void unsetSrcGatePlusplus() {
		boolean oldSrcGatePlusplus = srcGatePlusplus;
		boolean oldSrcGatePlusplusESet = srcGatePlusplusESet;
		srcGatePlusplus = SRC_GATE_PLUSPLUS_EDEFAULT;
		srcGatePlusplusESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS,
					oldSrcGatePlusplus, SRC_GATE_PLUSPLUS_EDEFAULT,
					oldSrcGatePlusplusESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.ned2.model.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Src Gate Plusplus</em>' attribute is set.
	 * @see #unsetSrcGatePlusplus()
	 * @see #isSrcGatePlusplus()
	 * @see #setSrcGatePlusplus(boolean)
	 * @generated
	 */
	public boolean isSetSrcGatePlusplus() {
		return srcGatePlusplusESet;
	}

	/**
	 * Returns the value of the '<em><b>Src Gate Subg</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src Gate Subg</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src Gate Subg</em>' attribute.
	 * @see #setSrcGateSubg(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_SrcGateSubg()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='src-gate-subg'"
	 * @generated
	 */
	public String getSrcGateSubg() {
		return srcGateSubg;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getSrcGateSubg <em>Src Gate Subg</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src Gate Subg</em>' attribute.
	 * @see #getSrcGateSubg()
	 * @generated
	 */
	public void setSrcGateSubg(String newSrcGateSubg) {
		String oldSrcGateSubg = srcGateSubg;
		srcGateSubg = newSrcGateSubg;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__SRC_GATE_SUBG, oldSrcGateSubg,
					srcGateSubg));
	}

	/**
	 * Returns the value of the '<em><b>Src Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src Module</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src Module</em>' attribute.
	 * @see #setSrcModule(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_SrcModule()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
	 *        extendedMetaData="kind='attribute' name='src-module'"
	 * @generated
	 */
	public String getSrcModule() {
		return srcModule;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getSrcModule <em>Src Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src Module</em>' attribute.
	 * @see #getSrcModule()
	 * @generated
	 */
	public void setSrcModule(String newSrcModule) {
		String oldSrcModule = srcModule;
		srcModule = newSrcModule;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__SRC_MODULE, oldSrcModule, srcModule));
	}

	/**
	 * Returns the value of the '<em><b>Src Module Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src Module Index</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src Module Index</em>' attribute.
	 * @see #setSrcModuleIndex(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection_SrcModuleIndex()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='src-module-index'"
	 * @generated
	 */
	public String getSrcModuleIndex() {
		return srcModuleIndex;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Connection#getSrcModuleIndex <em>Src Module Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src Module Index</em>' attribute.
	 * @see #getSrcModuleIndex()
	 * @generated
	 */
	public void setSrcModuleIndex(String newSrcModuleIndex) {
		String oldSrcModuleIndex = srcModuleIndex;
		srcModuleIndex = newSrcModuleIndex;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTION__SRC_MODULE_INDEX,
					oldSrcModuleIndex, srcModuleIndex));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.CONNECTION__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTION__EXPRESSION:
			return ((InternalEList) getExpression())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTION__PARAMETERS:
			return basicSetParameters(null, msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
		case Ned2Package.CONNECTION__WHITESPACE:
			return getWhitespace();
		case Ned2Package.CONNECTION__EXPRESSION:
			return getExpression();
		case Ned2Package.CONNECTION__PARAMETERS:
			return getParameters();
		case Ned2Package.CONNECTION__ARROW_DIRECTION:
			return getArrowDirection();
		case Ned2Package.CONNECTION__CHANNEL_NAME:
			return getChannelName();
		case Ned2Package.CONNECTION__DEST_GATE:
			return getDestGate();
		case Ned2Package.CONNECTION__DEST_GATE_INDEX:
			return getDestGateIndex();
		case Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS:
			return isDestGatePlusplus() ? Boolean.TRUE : Boolean.FALSE;
		case Ned2Package.CONNECTION__DEST_GATE_SUBG:
			return getDestGateSubg();
		case Ned2Package.CONNECTION__DEST_MODULE:
			return getDestModule();
		case Ned2Package.CONNECTION__DEST_MODULE_INDEX:
			return getDestModuleIndex();
		case Ned2Package.CONNECTION__SRC_GATE:
			return getSrcGate();
		case Ned2Package.CONNECTION__SRC_GATE_INDEX:
			return getSrcGateIndex();
		case Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS:
			return isSrcGatePlusplus() ? Boolean.TRUE : Boolean.FALSE;
		case Ned2Package.CONNECTION__SRC_GATE_SUBG:
			return getSrcGateSubg();
		case Ned2Package.CONNECTION__SRC_MODULE:
			return getSrcModule();
		case Ned2Package.CONNECTION__SRC_MODULE_INDEX:
			return getSrcModuleIndex();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
		case Ned2Package.CONNECTION__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTION__EXPRESSION:
			getExpression().clear();
			getExpression().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTION__PARAMETERS:
			setParameters((Parameters) newValue);
			return;
		case Ned2Package.CONNECTION__ARROW_DIRECTION:
			setArrowDirection((String) newValue);
			return;
		case Ned2Package.CONNECTION__CHANNEL_NAME:
			setChannelName((String) newValue);
			return;
		case Ned2Package.CONNECTION__DEST_GATE:
			setDestGate((String) newValue);
			return;
		case Ned2Package.CONNECTION__DEST_GATE_INDEX:
			setDestGateIndex((String) newValue);
			return;
		case Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS:
			setDestGatePlusplus(((Boolean) newValue).booleanValue());
			return;
		case Ned2Package.CONNECTION__DEST_GATE_SUBG:
			setDestGateSubg((String) newValue);
			return;
		case Ned2Package.CONNECTION__DEST_MODULE:
			setDestModule((String) newValue);
			return;
		case Ned2Package.CONNECTION__DEST_MODULE_INDEX:
			setDestModuleIndex((String) newValue);
			return;
		case Ned2Package.CONNECTION__SRC_GATE:
			setSrcGate((String) newValue);
			return;
		case Ned2Package.CONNECTION__SRC_GATE_INDEX:
			setSrcGateIndex((String) newValue);
			return;
		case Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS:
			setSrcGatePlusplus(((Boolean) newValue).booleanValue());
			return;
		case Ned2Package.CONNECTION__SRC_GATE_SUBG:
			setSrcGateSubg((String) newValue);
			return;
		case Ned2Package.CONNECTION__SRC_MODULE:
			setSrcModule((String) newValue);
			return;
		case Ned2Package.CONNECTION__SRC_MODULE_INDEX:
			setSrcModuleIndex((String) newValue);
			return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eUnset(int featureID) {
		switch (featureID) {
		case Ned2Package.CONNECTION__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.CONNECTION__EXPRESSION:
			getExpression().clear();
			return;
		case Ned2Package.CONNECTION__PARAMETERS:
			setParameters((Parameters) null);
			return;
		case Ned2Package.CONNECTION__ARROW_DIRECTION:
			setArrowDirection(ARROW_DIRECTION_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__CHANNEL_NAME:
			setChannelName(CHANNEL_NAME_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__DEST_GATE:
			setDestGate(DEST_GATE_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__DEST_GATE_INDEX:
			setDestGateIndex(DEST_GATE_INDEX_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS:
			unsetDestGatePlusplus();
			return;
		case Ned2Package.CONNECTION__DEST_GATE_SUBG:
			setDestGateSubg(DEST_GATE_SUBG_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__DEST_MODULE:
			setDestModule(DEST_MODULE_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__DEST_MODULE_INDEX:
			setDestModuleIndex(DEST_MODULE_INDEX_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__SRC_GATE:
			setSrcGate(SRC_GATE_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__SRC_GATE_INDEX:
			setSrcGateIndex(SRC_GATE_INDEX_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS:
			unsetSrcGatePlusplus();
			return;
		case Ned2Package.CONNECTION__SRC_GATE_SUBG:
			setSrcGateSubg(SRC_GATE_SUBG_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__SRC_MODULE:
			setSrcModule(SRC_MODULE_EDEFAULT);
			return;
		case Ned2Package.CONNECTION__SRC_MODULE_INDEX:
			setSrcModuleIndex(SRC_MODULE_INDEX_EDEFAULT);
			return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean eIsSet(int featureID) {
		switch (featureID) {
		case Ned2Package.CONNECTION__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.CONNECTION__EXPRESSION:
			return expression != null && !expression.isEmpty();
		case Ned2Package.CONNECTION__PARAMETERS:
			return parameters != null;
		case Ned2Package.CONNECTION__ARROW_DIRECTION:
			return ARROW_DIRECTION_EDEFAULT == null ? arrowDirection != null
					: !ARROW_DIRECTION_EDEFAULT.equals(arrowDirection);
		case Ned2Package.CONNECTION__CHANNEL_NAME:
			return CHANNEL_NAME_EDEFAULT == null ? channelName != null
					: !CHANNEL_NAME_EDEFAULT.equals(channelName);
		case Ned2Package.CONNECTION__DEST_GATE:
			return DEST_GATE_EDEFAULT == null ? destGate != null
					: !DEST_GATE_EDEFAULT.equals(destGate);
		case Ned2Package.CONNECTION__DEST_GATE_INDEX:
			return DEST_GATE_INDEX_EDEFAULT == null ? destGateIndex != null
					: !DEST_GATE_INDEX_EDEFAULT.equals(destGateIndex);
		case Ned2Package.CONNECTION__DEST_GATE_PLUSPLUS:
			return isSetDestGatePlusplus();
		case Ned2Package.CONNECTION__DEST_GATE_SUBG:
			return DEST_GATE_SUBG_EDEFAULT == null ? destGateSubg != null
					: !DEST_GATE_SUBG_EDEFAULT.equals(destGateSubg);
		case Ned2Package.CONNECTION__DEST_MODULE:
			return DEST_MODULE_EDEFAULT == null ? destModule != null
					: !DEST_MODULE_EDEFAULT.equals(destModule);
		case Ned2Package.CONNECTION__DEST_MODULE_INDEX:
			return DEST_MODULE_INDEX_EDEFAULT == null ? destModuleIndex != null
					: !DEST_MODULE_INDEX_EDEFAULT.equals(destModuleIndex);
		case Ned2Package.CONNECTION__SRC_GATE:
			return SRC_GATE_EDEFAULT == null ? srcGate != null
					: !SRC_GATE_EDEFAULT.equals(srcGate);
		case Ned2Package.CONNECTION__SRC_GATE_INDEX:
			return SRC_GATE_INDEX_EDEFAULT == null ? srcGateIndex != null
					: !SRC_GATE_INDEX_EDEFAULT.equals(srcGateIndex);
		case Ned2Package.CONNECTION__SRC_GATE_PLUSPLUS:
			return isSetSrcGatePlusplus();
		case Ned2Package.CONNECTION__SRC_GATE_SUBG:
			return SRC_GATE_SUBG_EDEFAULT == null ? srcGateSubg != null
					: !SRC_GATE_SUBG_EDEFAULT.equals(srcGateSubg);
		case Ned2Package.CONNECTION__SRC_MODULE:
			return SRC_MODULE_EDEFAULT == null ? srcModule != null
					: !SRC_MODULE_EDEFAULT.equals(srcModule);
		case Ned2Package.CONNECTION__SRC_MODULE_INDEX:
			return SRC_MODULE_INDEX_EDEFAULT == null ? srcModuleIndex != null
					: !SRC_MODULE_INDEX_EDEFAULT.equals(srcModuleIndex);
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String toString() {
		if (eIsProxy())
			return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (arrowDirection: ");
		result.append(arrowDirection);
		result.append(", channelName: ");
		result.append(channelName);
		result.append(", destGate: ");
		result.append(destGate);
		result.append(", destGateIndex: ");
		result.append(destGateIndex);
		result.append(", destGatePlusplus: ");
		if (destGatePlusplusESet)
			result.append(destGatePlusplus);
		else
			result.append("<unset>");
		result.append(", destGateSubg: ");
		result.append(destGateSubg);
		result.append(", destModule: ");
		result.append(destModule);
		result.append(", destModuleIndex: ");
		result.append(destModuleIndex);
		result.append(", srcGate: ");
		result.append(srcGate);
		result.append(", srcGateIndex: ");
		result.append(srcGateIndex);
		result.append(", srcGatePlusplus: ");
		if (srcGatePlusplusESet)
			result.append(srcGatePlusplus);
		else
			result.append("<unset>");
		result.append(", srcGateSubg: ");
		result.append(srcGateSubg);
		result.append(", srcModule: ");
		result.append(srcModule);
		result.append(", srcModuleIndex: ");
		result.append(srcModuleIndex);
		result.append(')');
		return result.toString();
	}

} // Connection
