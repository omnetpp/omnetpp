/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned.model.emf.NedFactory
 * @model kind="package"
 * @generated
 */
public interface NedPackage extends EPackage {
    /**
     * The package name.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    String eNAME = "emf";

    /**
     * The package namespace URI.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    String eNS_URI = "http://www.omnetpp.org/schema/ned";

    /**
     * The package namespace name.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    String eNS_PREFIX = "ned";

    /**
     * The singleton instance of the package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    NedPackage eINSTANCE = org.omnetpp.ned.model.emf.impl.NedPackageImpl.init();

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ChannelImpl <em>Channel</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ChannelImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getChannel()
     * @generated
     */
    int CHANNEL = 0;

    /**
     * The feature id for the '<em><b>Channel Attr</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL__CHANNEL_ATTR = 0;

    /**
     * The feature id for the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL__DISPLAY_STRING = 1;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL__BANNER_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL__NAME = 3;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL__RIGHT_COMMENT = 4;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL__TRAILING_COMMENT = 5;

    /**
     * The number of structural features of the the '<em>Channel</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_FEATURE_COUNT = 6;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ChannelAttrImpl <em>Channel Attr</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ChannelAttrImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getChannelAttr()
     * @generated
     */
    int CHANNEL_ATTR = 1;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_ATTR__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_ATTR__NAME = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_ATTR__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_ATTR__TRAILING_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_ATTR__VALUE = 4;

    /**
     * The number of structural features of the the '<em>Channel Attr</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CHANNEL_ATTR_FEATURE_COUNT = 5;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.CompoundModuleImpl <em>Compound Module</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.CompoundModuleImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getCompoundModule()
     * @generated
     */
    int COMPOUND_MODULE = 2;

    /**
     * The feature id for the '<em><b>Params</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__PARAMS = 0;

    /**
     * The feature id for the '<em><b>Gates</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__GATES = 1;

    /**
     * The feature id for the '<em><b>Submodules</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__SUBMODULES = 2;

    /**
     * The feature id for the '<em><b>Connections</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__CONNECTIONS = 3;

    /**
     * The feature id for the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__DISPLAY_STRING = 4;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__BANNER_COMMENT = 5;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__NAME = 6;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__RIGHT_COMMENT = 7;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE__TRAILING_COMMENT = 8;

    /**
     * The number of structural features of the the '<em>Compound Module</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int COMPOUND_MODULE_FEATURE_COUNT = 9;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ConnAttrImpl <em>Conn Attr</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ConnAttrImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getConnAttr()
     * @generated
     */
    int CONN_ATTR = 3;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONN_ATTR__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONN_ATTR__NAME = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONN_ATTR__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONN_ATTR__TRAILING_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONN_ATTR__VALUE = 4;

    /**
     * The number of structural features of the the '<em>Conn Attr</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONN_ATTR_FEATURE_COUNT = 5;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ConnectionImpl <em>Connection</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ConnectionImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getConnection()
     * @generated
     */
    int CONNECTION = 4;

    /**
     * The feature id for the '<em><b>Conn Attr</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__CONN_ATTR = 0;

    /**
     * The feature id for the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__DISPLAY_STRING = 1;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__BANNER_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Condition</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__CONDITION = 3;

    /**
     * The feature id for the '<em><b>Dest Gate</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__DEST_GATE = 4;

    /**
     * The feature id for the '<em><b>Dest Gate Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__DEST_GATE_INDEX = 5;

    /**
     * The feature id for the '<em><b>Dest Gate Plusplus</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__DEST_GATE_PLUSPLUS = 6;

    /**
     * The feature id for the '<em><b>Dest Module</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__DEST_MODULE = 7;

    /**
     * The feature id for the '<em><b>Dest Module Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__DEST_MODULE_INDEX = 8;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__RIGHT_COMMENT = 9;

    /**
     * The feature id for the '<em><b>Right To Left</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__RIGHT_TO_LEFT = 10;

    /**
     * The feature id for the '<em><b>Src Gate</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__SRC_GATE = 11;

    /**
     * The feature id for the '<em><b>Src Gate Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__SRC_GATE_INDEX = 12;

    /**
     * The feature id for the '<em><b>Src Gate Plusplus</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__SRC_GATE_PLUSPLUS = 13;

    /**
     * The feature id for the '<em><b>Src Module</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__SRC_MODULE = 14;

    /**
     * The feature id for the '<em><b>Src Module Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__SRC_MODULE_INDEX = 15;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION__TRAILING_COMMENT = 16;

    /**
     * The number of structural features of the the '<em>Connection</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTION_FEATURE_COUNT = 17;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl <em>Connections</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ConnectionsImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getConnections()
     * @generated
     */
    int CONNECTIONS = 5;

    /**
     * The feature id for the '<em><b>Group</b></em>' attribute list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__GROUP = 0;

    /**
     * The feature id for the '<em><b>Connection</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__CONNECTION = 1;

    /**
     * The feature id for the '<em><b>For Loop</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__FOR_LOOP = 2;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__BANNER_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Check Unconnected</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__CHECK_UNCONNECTED = 4;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__RIGHT_COMMENT = 5;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS__TRAILING_COMMENT = 6;

    /**
     * The number of structural features of the the '<em>Connections</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CONNECTIONS_FEATURE_COUNT = 7;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.DisplayStringImpl <em>Display String</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.DisplayStringImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getDisplayString()
     * @generated
     */
    int DISPLAY_STRING = 6;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int DISPLAY_STRING__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int DISPLAY_STRING__RIGHT_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int DISPLAY_STRING__TRAILING_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int DISPLAY_STRING__VALUE = 3;

    /**
     * The number of structural features of the the '<em>Display String</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int DISPLAY_STRING_FEATURE_COUNT = 4;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl <em>For Loop</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ForLoopImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getForLoop()
     * @generated
     */
    int FOR_LOOP = 7;

    /**
     * The feature id for the '<em><b>For Loop</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__FOR_LOOP = 0;

    /**
     * The feature id for the '<em><b>Connection</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__CONNECTION = 1;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__BANNER_COMMENT = 2;

    /**
     * The feature id for the '<em><b>From Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__FROM_VALUE = 3;

    /**
     * The feature id for the '<em><b>Param Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__PARAM_NAME = 4;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__RIGHT_COMMENT = 5;

    /**
     * The feature id for the '<em><b>To Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__TO_VALUE = 6;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP__TRAILING_COMMENT = 7;

    /**
     * The number of structural features of the the '<em>For Loop</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int FOR_LOOP_FEATURE_COUNT = 8;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.GateImpl <em>Gate</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.GateImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getGate()
     * @generated
     */
    int GATE = 8;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE__NAME = 1;

    /**
     * The feature id for the '<em><b>Output</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE__OUTPUT = 2;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE__RIGHT_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE__TRAILING_COMMENT = 4;

    /**
     * The feature id for the '<em><b>Vector</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE__VECTOR = 5;

    /**
     * The number of structural features of the the '<em>Gate</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATE_FEATURE_COUNT = 6;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.GatesImpl <em>Gates</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.GatesImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getGates()
     * @generated
     */
    int GATES = 9;

    /**
     * The feature id for the '<em><b>Gate</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATES__GATE = 0;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATES__BANNER_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATES__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATES__TRAILING_COMMENT = 3;

    /**
     * The number of structural features of the the '<em>Gates</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATES_FEATURE_COUNT = 4;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.GatesizeImpl <em>Gatesize</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.GatesizeImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getGatesize()
     * @generated
     */
    int GATESIZE = 10;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZE__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZE__NAME = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZE__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZE__TRAILING_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Vector Size</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZE__VECTOR_SIZE = 4;

    /**
     * The number of structural features of the the '<em>Gatesize</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZE_FEATURE_COUNT = 5;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.GatesizesImpl <em>Gatesizes</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.GatesizesImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getGatesizes()
     * @generated
     */
    int GATESIZES = 11;

    /**
     * The feature id for the '<em><b>Gatesize</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZES__GATESIZE = 0;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZES__BANNER_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Condition</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZES__CONDITION = 2;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZES__RIGHT_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZES__TRAILING_COMMENT = 4;

    /**
     * The number of structural features of the the '<em>Gatesizes</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GATESIZES_FEATURE_COUNT = 5;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ImportImpl <em>Import</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ImportImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getImport()
     * @generated
     */
    int IMPORT = 12;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int IMPORT__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Filename</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int IMPORT__FILENAME = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int IMPORT__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int IMPORT__TRAILING_COMMENT = 3;

    /**
     * The number of structural features of the the '<em>Import</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int IMPORT_FEATURE_COUNT = 4;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.NedFileImpl <em>File</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.NedFileImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getNedFile()
     * @generated
     */
    int NED_FILE = 13;

    /**
     * The feature id for the '<em><b>Group</b></em>' attribute list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__GROUP = 0;

    /**
     * The feature id for the '<em><b>Import</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__IMPORT = 1;

    /**
     * The feature id for the '<em><b>Channel</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__CHANNEL = 2;

    /**
     * The feature id for the '<em><b>Simple Module</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__SIMPLE_MODULE = 3;

    /**
     * The feature id for the '<em><b>Compound Module</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__COMPOUND_MODULE = 4;

    /**
     * The feature id for the '<em><b>Network</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__NETWORK = 5;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__BANNER_COMMENT = 6;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__RIGHT_COMMENT = 7;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE__TRAILING_COMMENT = 8;

    /**
     * The number of structural features of the the '<em>File</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NED_FILE_FEATURE_COUNT = 9;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.NetworkImpl <em>Network</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.NetworkImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getNetwork()
     * @generated
     */
    int NETWORK = 14;

    /**
     * The feature id for the '<em><b>Substparams</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK__SUBSTPARAMS = 0;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK__BANNER_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK__NAME = 2;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK__RIGHT_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK__TRAILING_COMMENT = 4;

    /**
     * The feature id for the '<em><b>Type Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK__TYPE_NAME = 5;

    /**
     * The number of structural features of the the '<em>Network</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int NETWORK_FEATURE_COUNT = 6;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ParamImpl <em>Param</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ParamImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getParam()
     * @generated
     */
    int PARAM = 15;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAM__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Data Type</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAM__DATA_TYPE = 1;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAM__NAME = 2;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAM__RIGHT_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAM__TRAILING_COMMENT = 4;

    /**
     * The number of structural features of the the '<em>Param</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAM_FEATURE_COUNT = 5;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.ParamsImpl <em>Params</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.ParamsImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getParams()
     * @generated
     */
    int PARAMS = 16;

    /**
     * The feature id for the '<em><b>Param</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAMS__PARAM = 0;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAMS__BANNER_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAMS__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAMS__TRAILING_COMMENT = 3;

    /**
     * The number of structural features of the the '<em>Params</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int PARAMS_FEATURE_COUNT = 4;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.SimpleModuleImpl <em>Simple Module</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.SimpleModuleImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getSimpleModule()
     * @generated
     */
    int SIMPLE_MODULE = 17;

    /**
     * The feature id for the '<em><b>Params</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__PARAMS = 0;

    /**
     * The feature id for the '<em><b>Gates</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__GATES = 1;

    /**
     * The feature id for the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__DISPLAY_STRING = 2;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__BANNER_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__NAME = 4;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__RIGHT_COMMENT = 5;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE__TRAILING_COMMENT = 6;

    /**
     * The number of structural features of the the '<em>Simple Module</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SIMPLE_MODULE_FEATURE_COUNT = 7;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl <em>Submodule</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.SubmoduleImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getSubmodule()
     * @generated
     */
    int SUBMODULE = 18;

    /**
     * The feature id for the '<em><b>Substparams</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__SUBSTPARAMS = 0;

    /**
     * The feature id for the '<em><b>Gatesizes</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__GATESIZES = 1;

    /**
     * The feature id for the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__DISPLAY_STRING = 2;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__BANNER_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Like Param</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__LIKE_PARAM = 4;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__NAME = 5;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__RIGHT_COMMENT = 6;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__TRAILING_COMMENT = 7;

    /**
     * The feature id for the '<em><b>Type Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__TYPE_NAME = 8;

    /**
     * The feature id for the '<em><b>Vector Size</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE__VECTOR_SIZE = 9;

    /**
     * The number of structural features of the the '<em>Submodule</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULE_FEATURE_COUNT = 10;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.SubmodulesImpl <em>Submodules</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.SubmodulesImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getSubmodules()
     * @generated
     */
    int SUBMODULES = 19;

    /**
     * The feature id for the '<em><b>Submodule</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULES__SUBMODULE = 0;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULES__BANNER_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULES__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULES__TRAILING_COMMENT = 3;

    /**
     * The number of structural features of the the '<em>Submodules</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBMODULES_FEATURE_COUNT = 4;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.SubstparamImpl <em>Substparam</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.SubstparamImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getSubstparam()
     * @generated
     */
    int SUBSTPARAM = 20;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAM__BANNER_COMMENT = 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAM__NAME = 1;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAM__RIGHT_COMMENT = 2;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAM__TRAILING_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAM__VALUE = 4;

    /**
     * The number of structural features of the the '<em>Substparam</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAM_FEATURE_COUNT = 5;

    /**
     * The meta object id for the '{@link org.omnetpp.ned.model.emf.impl.SubstparamsImpl <em>Substparams</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.omnetpp.ned.model.emf.impl.SubstparamsImpl
     * @see org.omnetpp.ned.model.emf.impl.NedPackageImpl#getSubstparams()
     * @generated
     */
    int SUBSTPARAMS = 21;

    /**
     * The feature id for the '<em><b>Substparam</b></em>' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAMS__SUBSTPARAM = 0;

    /**
     * The feature id for the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAMS__BANNER_COMMENT = 1;

    /**
     * The feature id for the '<em><b>Condition</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAMS__CONDITION = 2;

    /**
     * The feature id for the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAMS__RIGHT_COMMENT = 3;

    /**
     * The feature id for the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAMS__TRAILING_COMMENT = 4;

    /**
     * The number of structural features of the the '<em>Substparams</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int SUBSTPARAMS_FEATURE_COUNT = 5;


    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Channel <em>Channel</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Channel</em>'.
     * @see org.omnetpp.ned.model.emf.Channel
     * @generated
     */
    EClass getChannel();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Channel#getChannelAttr <em>Channel Attr</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Channel Attr</em>'.
     * @see org.omnetpp.ned.model.emf.Channel#getChannelAttr()
     * @see #getChannel()
     * @generated
     */
    EReference getChannel_ChannelAttr();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.Channel#getDisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Display String</em>'.
     * @see org.omnetpp.ned.model.emf.Channel#getDisplayString()
     * @see #getChannel()
     * @generated
     */
    EReference getChannel_DisplayString();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Channel#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Channel#getBannerComment()
     * @see #getChannel()
     * @generated
     */
    EAttribute getChannel_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Channel#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Channel#getName()
     * @see #getChannel()
     * @generated
     */
    EAttribute getChannel_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Channel#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Channel#getRightComment()
     * @see #getChannel()
     * @generated
     */
    EAttribute getChannel_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Channel#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Channel#getTrailingComment()
     * @see #getChannel()
     * @generated
     */
    EAttribute getChannel_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.ChannelAttr <em>Channel Attr</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Channel Attr</em>'.
     * @see org.omnetpp.ned.model.emf.ChannelAttr
     * @generated
     */
    EClass getChannelAttr();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ChannelAttr#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ChannelAttr#getBannerComment()
     * @see #getChannelAttr()
     * @generated
     */
    EAttribute getChannelAttr_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ChannelAttr#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.ChannelAttr#getName()
     * @see #getChannelAttr()
     * @generated
     */
    EAttribute getChannelAttr_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ChannelAttr#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ChannelAttr#getRightComment()
     * @see #getChannelAttr()
     * @generated
     */
    EAttribute getChannelAttr_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ChannelAttr#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ChannelAttr#getTrailingComment()
     * @see #getChannelAttr()
     * @generated
     */
    EAttribute getChannelAttr_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ChannelAttr#getValue <em>Value</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Value</em>'.
     * @see org.omnetpp.ned.model.emf.ChannelAttr#getValue()
     * @see #getChannelAttr()
     * @generated
     */
    EAttribute getChannelAttr_Value();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.CompoundModule <em>Compound Module</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Compound Module</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule
     * @generated
     */
    EClass getCompoundModule();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.CompoundModule#getParams <em>Params</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Params</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getParams()
     * @see #getCompoundModule()
     * @generated
     */
    EReference getCompoundModule_Params();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.CompoundModule#getGates <em>Gates</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Gates</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getGates()
     * @see #getCompoundModule()
     * @generated
     */
    EReference getCompoundModule_Gates();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.CompoundModule#getSubmodules <em>Submodules</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Submodules</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getSubmodules()
     * @see #getCompoundModule()
     * @generated
     */
    EReference getCompoundModule_Submodules();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.CompoundModule#getConnections <em>Connections</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Connections</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getConnections()
     * @see #getCompoundModule()
     * @generated
     */
    EReference getCompoundModule_Connections();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.CompoundModule#getDisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Display String</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getDisplayString()
     * @see #getCompoundModule()
     * @generated
     */
    EReference getCompoundModule_DisplayString();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.CompoundModule#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getBannerComment()
     * @see #getCompoundModule()
     * @generated
     */
    EAttribute getCompoundModule_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.CompoundModule#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getName()
     * @see #getCompoundModule()
     * @generated
     */
    EAttribute getCompoundModule_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.CompoundModule#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getRightComment()
     * @see #getCompoundModule()
     * @generated
     */
    EAttribute getCompoundModule_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.CompoundModule#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.CompoundModule#getTrailingComment()
     * @see #getCompoundModule()
     * @generated
     */
    EAttribute getCompoundModule_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.ConnAttr <em>Conn Attr</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Conn Attr</em>'.
     * @see org.omnetpp.ned.model.emf.ConnAttr
     * @generated
     */
    EClass getConnAttr();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ConnAttr#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ConnAttr#getBannerComment()
     * @see #getConnAttr()
     * @generated
     */
    EAttribute getConnAttr_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ConnAttr#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.ConnAttr#getName()
     * @see #getConnAttr()
     * @generated
     */
    EAttribute getConnAttr_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ConnAttr#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ConnAttr#getRightComment()
     * @see #getConnAttr()
     * @generated
     */
    EAttribute getConnAttr_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ConnAttr#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ConnAttr#getTrailingComment()
     * @see #getConnAttr()
     * @generated
     */
    EAttribute getConnAttr_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ConnAttr#getValue <em>Value</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Value</em>'.
     * @see org.omnetpp.ned.model.emf.ConnAttr#getValue()
     * @see #getConnAttr()
     * @generated
     */
    EAttribute getConnAttr_Value();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Connection <em>Connection</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Connection</em>'.
     * @see org.omnetpp.ned.model.emf.Connection
     * @generated
     */
    EClass getConnection();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Connection#getConnAttr <em>Conn Attr</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Conn Attr</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getConnAttr()
     * @see #getConnection()
     * @generated
     */
    EReference getConnection_ConnAttr();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.Connection#getDisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Display String</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getDisplayString()
     * @see #getConnection()
     * @generated
     */
    EReference getConnection_DisplayString();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getBannerComment()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getCondition <em>Condition</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Condition</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getCondition()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_Condition();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getDestGate <em>Dest Gate</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Dest Gate</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getDestGate()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_DestGate();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getDestGateIndex <em>Dest Gate Index</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Dest Gate Index</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getDestGateIndex()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_DestGateIndex();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Dest Gate Plusplus</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#isDestGatePlusplus()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_DestGatePlusplus();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getDestModule <em>Dest Module</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Dest Module</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getDestModule()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_DestModule();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getDestModuleIndex <em>Dest Module Index</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Dest Module Index</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getDestModuleIndex()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_DestModuleIndex();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getRightComment()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#isRightToLeft <em>Right To Left</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right To Left</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#isRightToLeft()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_RightToLeft();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getSrcGate <em>Src Gate</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Src Gate</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getSrcGate()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_SrcGate();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getSrcGateIndex <em>Src Gate Index</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Src Gate Index</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getSrcGateIndex()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_SrcGateIndex();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Src Gate Plusplus</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#isSrcGatePlusplus()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_SrcGatePlusplus();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getSrcModule <em>Src Module</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Src Module</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getSrcModule()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_SrcModule();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getSrcModuleIndex <em>Src Module Index</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Src Module Index</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getSrcModuleIndex()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_SrcModuleIndex();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connection#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Connection#getTrailingComment()
     * @see #getConnection()
     * @generated
     */
    EAttribute getConnection_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Connections <em>Connections</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Connections</em>'.
     * @see org.omnetpp.ned.model.emf.Connections
     * @generated
     */
    EClass getConnections();

    /**
     * Returns the meta object for the attribute list '{@link org.omnetpp.ned.model.emf.Connections#getGroup <em>Group</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute list '<em>Group</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#getGroup()
     * @see #getConnections()
     * @generated
     */
    EAttribute getConnections_Group();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Connections#getConnection <em>Connection</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Connection</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#getConnection()
     * @see #getConnections()
     * @generated
     */
    EReference getConnections_Connection();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Connections#getForLoop <em>For Loop</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>For Loop</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#getForLoop()
     * @see #getConnections()
     * @generated
     */
    EReference getConnections_ForLoop();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connections#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#getBannerComment()
     * @see #getConnections()
     * @generated
     */
    EAttribute getConnections_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Check Unconnected</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#isCheckUnconnected()
     * @see #getConnections()
     * @generated
     */
    EAttribute getConnections_CheckUnconnected();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connections#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#getRightComment()
     * @see #getConnections()
     * @generated
     */
    EAttribute getConnections_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Connections#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Connections#getTrailingComment()
     * @see #getConnections()
     * @generated
     */
    EAttribute getConnections_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.DisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Display String</em>'.
     * @see org.omnetpp.ned.model.emf.DisplayString
     * @generated
     */
    EClass getDisplayString();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.DisplayString#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.DisplayString#getBannerComment()
     * @see #getDisplayString()
     * @generated
     */
    EAttribute getDisplayString_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.DisplayString#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.DisplayString#getRightComment()
     * @see #getDisplayString()
     * @generated
     */
    EAttribute getDisplayString_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.DisplayString#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.DisplayString#getTrailingComment()
     * @see #getDisplayString()
     * @generated
     */
    EAttribute getDisplayString_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.DisplayString#getValue <em>Value</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Value</em>'.
     * @see org.omnetpp.ned.model.emf.DisplayString#getValue()
     * @see #getDisplayString()
     * @generated
     */
    EAttribute getDisplayString_Value();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.ForLoop <em>For Loop</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>For Loop</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop
     * @generated
     */
    EClass getForLoop();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.ForLoop#getForLoop <em>For Loop</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>For Loop</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getForLoop()
     * @see #getForLoop()
     * @generated
     */
    EReference getForLoop_ForLoop();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.ForLoop#getConnection <em>Connection</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Connection</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getConnection()
     * @see #getForLoop()
     * @generated
     */
    EReference getForLoop_Connection();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ForLoop#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getBannerComment()
     * @see #getForLoop()
     * @generated
     */
    EAttribute getForLoop_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ForLoop#getFromValue <em>From Value</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>From Value</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getFromValue()
     * @see #getForLoop()
     * @generated
     */
    EAttribute getForLoop_FromValue();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ForLoop#getParamName <em>Param Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Param Name</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getParamName()
     * @see #getForLoop()
     * @generated
     */
    EAttribute getForLoop_ParamName();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ForLoop#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getRightComment()
     * @see #getForLoop()
     * @generated
     */
    EAttribute getForLoop_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ForLoop#getToValue <em>To Value</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>To Value</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getToValue()
     * @see #getForLoop()
     * @generated
     */
    EAttribute getForLoop_ToValue();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.ForLoop#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.ForLoop#getTrailingComment()
     * @see #getForLoop()
     * @generated
     */
    EAttribute getForLoop_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Gate <em>Gate</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Gate</em>'.
     * @see org.omnetpp.ned.model.emf.Gate
     * @generated
     */
    EClass getGate();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gate#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gate#getBannerComment()
     * @see #getGate()
     * @generated
     */
    EAttribute getGate_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gate#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Gate#getName()
     * @see #getGate()
     * @generated
     */
    EAttribute getGate_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gate#isOutput <em>Output</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Output</em>'.
     * @see org.omnetpp.ned.model.emf.Gate#isOutput()
     * @see #getGate()
     * @generated
     */
    EAttribute getGate_Output();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gate#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gate#getRightComment()
     * @see #getGate()
     * @generated
     */
    EAttribute getGate_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gate#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gate#getTrailingComment()
     * @see #getGate()
     * @generated
     */
    EAttribute getGate_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gate#isVector <em>Vector</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Vector</em>'.
     * @see org.omnetpp.ned.model.emf.Gate#isVector()
     * @see #getGate()
     * @generated
     */
    EAttribute getGate_Vector();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Gates <em>Gates</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Gates</em>'.
     * @see org.omnetpp.ned.model.emf.Gates
     * @generated
     */
    EClass getGates();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Gates#getGate <em>Gate</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Gate</em>'.
     * @see org.omnetpp.ned.model.emf.Gates#getGate()
     * @see #getGates()
     * @generated
     */
    EReference getGates_Gate();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gates#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gates#getBannerComment()
     * @see #getGates()
     * @generated
     */
    EAttribute getGates_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gates#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gates#getRightComment()
     * @see #getGates()
     * @generated
     */
    EAttribute getGates_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gates#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gates#getTrailingComment()
     * @see #getGates()
     * @generated
     */
    EAttribute getGates_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Gatesize <em>Gatesize</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Gatesize</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesize
     * @generated
     */
    EClass getGatesize();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesize#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesize#getBannerComment()
     * @see #getGatesize()
     * @generated
     */
    EAttribute getGatesize_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesize#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesize#getName()
     * @see #getGatesize()
     * @generated
     */
    EAttribute getGatesize_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesize#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesize#getRightComment()
     * @see #getGatesize()
     * @generated
     */
    EAttribute getGatesize_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesize#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesize#getTrailingComment()
     * @see #getGatesize()
     * @generated
     */
    EAttribute getGatesize_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesize#getVectorSize <em>Vector Size</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Vector Size</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesize#getVectorSize()
     * @see #getGatesize()
     * @generated
     */
    EAttribute getGatesize_VectorSize();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Gatesizes <em>Gatesizes</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Gatesizes</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesizes
     * @generated
     */
    EClass getGatesizes();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Gatesizes#getGatesize <em>Gatesize</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Gatesize</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesizes#getGatesize()
     * @see #getGatesizes()
     * @generated
     */
    EReference getGatesizes_Gatesize();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesizes#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesizes#getBannerComment()
     * @see #getGatesizes()
     * @generated
     */
    EAttribute getGatesizes_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesizes#getCondition <em>Condition</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Condition</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesizes#getCondition()
     * @see #getGatesizes()
     * @generated
     */
    EAttribute getGatesizes_Condition();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesizes#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesizes#getRightComment()
     * @see #getGatesizes()
     * @generated
     */
    EAttribute getGatesizes_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Gatesizes#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Gatesizes#getTrailingComment()
     * @see #getGatesizes()
     * @generated
     */
    EAttribute getGatesizes_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Import <em>Import</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Import</em>'.
     * @see org.omnetpp.ned.model.emf.Import
     * @generated
     */
    EClass getImport();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Import#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Import#getBannerComment()
     * @see #getImport()
     * @generated
     */
    EAttribute getImport_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Import#getFilename <em>Filename</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Filename</em>'.
     * @see org.omnetpp.ned.model.emf.Import#getFilename()
     * @see #getImport()
     * @generated
     */
    EAttribute getImport_Filename();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Import#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Import#getRightComment()
     * @see #getImport()
     * @generated
     */
    EAttribute getImport_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Import#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Import#getTrailingComment()
     * @see #getImport()
     * @generated
     */
    EAttribute getImport_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.NedFile <em>File</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>File</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile
     * @generated
     */
    EClass getNedFile();

    /**
     * Returns the meta object for the attribute list '{@link org.omnetpp.ned.model.emf.NedFile#getGroup <em>Group</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute list '<em>Group</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getGroup()
     * @see #getNedFile()
     * @generated
     */
    EAttribute getNedFile_Group();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.NedFile#getImport <em>Import</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Import</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getImport()
     * @see #getNedFile()
     * @generated
     */
    EReference getNedFile_Import();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.NedFile#getChannel <em>Channel</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Channel</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getChannel()
     * @see #getNedFile()
     * @generated
     */
    EReference getNedFile_Channel();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.NedFile#getSimpleModule <em>Simple Module</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Simple Module</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getSimpleModule()
     * @see #getNedFile()
     * @generated
     */
    EReference getNedFile_SimpleModule();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.NedFile#getCompoundModule <em>Compound Module</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Compound Module</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getCompoundModule()
     * @see #getNedFile()
     * @generated
     */
    EReference getNedFile_CompoundModule();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.NedFile#getNetwork <em>Network</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Network</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getNetwork()
     * @see #getNedFile()
     * @generated
     */
    EReference getNedFile_Network();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.NedFile#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getBannerComment()
     * @see #getNedFile()
     * @generated
     */
    EAttribute getNedFile_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.NedFile#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getRightComment()
     * @see #getNedFile()
     * @generated
     */
    EAttribute getNedFile_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.NedFile#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.NedFile#getTrailingComment()
     * @see #getNedFile()
     * @generated
     */
    EAttribute getNedFile_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Network <em>Network</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Network</em>'.
     * @see org.omnetpp.ned.model.emf.Network
     * @generated
     */
    EClass getNetwork();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.Network#getSubstparams <em>Substparams</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Substparams</em>'.
     * @see org.omnetpp.ned.model.emf.Network#getSubstparams()
     * @see #getNetwork()
     * @generated
     */
    EReference getNetwork_Substparams();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Network#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Network#getBannerComment()
     * @see #getNetwork()
     * @generated
     */
    EAttribute getNetwork_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Network#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Network#getName()
     * @see #getNetwork()
     * @generated
     */
    EAttribute getNetwork_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Network#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Network#getRightComment()
     * @see #getNetwork()
     * @generated
     */
    EAttribute getNetwork_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Network#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Network#getTrailingComment()
     * @see #getNetwork()
     * @generated
     */
    EAttribute getNetwork_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Network#getTypeName <em>Type Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Type Name</em>'.
     * @see org.omnetpp.ned.model.emf.Network#getTypeName()
     * @see #getNetwork()
     * @generated
     */
    EAttribute getNetwork_TypeName();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Param <em>Param</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Param</em>'.
     * @see org.omnetpp.ned.model.emf.Param
     * @generated
     */
    EClass getParam();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Param#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Param#getBannerComment()
     * @see #getParam()
     * @generated
     */
    EAttribute getParam_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Param#getDataType <em>Data Type</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Data Type</em>'.
     * @see org.omnetpp.ned.model.emf.Param#getDataType()
     * @see #getParam()
     * @generated
     */
    EAttribute getParam_DataType();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Param#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Param#getName()
     * @see #getParam()
     * @generated
     */
    EAttribute getParam_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Param#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Param#getRightComment()
     * @see #getParam()
     * @generated
     */
    EAttribute getParam_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Param#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Param#getTrailingComment()
     * @see #getParam()
     * @generated
     */
    EAttribute getParam_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Params <em>Params</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Params</em>'.
     * @see org.omnetpp.ned.model.emf.Params
     * @generated
     */
    EClass getParams();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Params#getParam <em>Param</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Param</em>'.
     * @see org.omnetpp.ned.model.emf.Params#getParam()
     * @see #getParams()
     * @generated
     */
    EReference getParams_Param();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Params#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Params#getBannerComment()
     * @see #getParams()
     * @generated
     */
    EAttribute getParams_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Params#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Params#getRightComment()
     * @see #getParams()
     * @generated
     */
    EAttribute getParams_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Params#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Params#getTrailingComment()
     * @see #getParams()
     * @generated
     */
    EAttribute getParams_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.SimpleModule <em>Simple Module</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Simple Module</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule
     * @generated
     */
    EClass getSimpleModule();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.SimpleModule#getParams <em>Params</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Params</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getParams()
     * @see #getSimpleModule()
     * @generated
     */
    EReference getSimpleModule_Params();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.SimpleModule#getGates <em>Gates</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Gates</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getGates()
     * @see #getSimpleModule()
     * @generated
     */
    EReference getSimpleModule_Gates();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.SimpleModule#getDisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Display String</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getDisplayString()
     * @see #getSimpleModule()
     * @generated
     */
    EReference getSimpleModule_DisplayString();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.SimpleModule#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getBannerComment()
     * @see #getSimpleModule()
     * @generated
     */
    EAttribute getSimpleModule_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.SimpleModule#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getName()
     * @see #getSimpleModule()
     * @generated
     */
    EAttribute getSimpleModule_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.SimpleModule#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getRightComment()
     * @see #getSimpleModule()
     * @generated
     */
    EAttribute getSimpleModule_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.SimpleModule#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.SimpleModule#getTrailingComment()
     * @see #getSimpleModule()
     * @generated
     */
    EAttribute getSimpleModule_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Submodule <em>Submodule</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Submodule</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule
     * @generated
     */
    EClass getSubmodule();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Submodule#getSubstparams <em>Substparams</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Substparams</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getSubstparams()
     * @see #getSubmodule()
     * @generated
     */
    EReference getSubmodule_Substparams();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Submodule#getGatesizes <em>Gatesizes</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Gatesizes</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getGatesizes()
     * @see #getSubmodule()
     * @generated
     */
    EReference getSubmodule_Gatesizes();

    /**
     * Returns the meta object for the containment reference '{@link org.omnetpp.ned.model.emf.Submodule#getDisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Display String</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getDisplayString()
     * @see #getSubmodule()
     * @generated
     */
    EReference getSubmodule_DisplayString();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getBannerComment()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getLikeParam <em>Like Param</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Like Param</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getLikeParam()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_LikeParam();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getName()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getRightComment()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getTrailingComment()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getTypeName <em>Type Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Type Name</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getTypeName()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_TypeName();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodule#getVectorSize <em>Vector Size</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Vector Size</em>'.
     * @see org.omnetpp.ned.model.emf.Submodule#getVectorSize()
     * @see #getSubmodule()
     * @generated
     */
    EAttribute getSubmodule_VectorSize();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Submodules <em>Submodules</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Submodules</em>'.
     * @see org.omnetpp.ned.model.emf.Submodules
     * @generated
     */
    EClass getSubmodules();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Submodules#getSubmodule <em>Submodule</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Submodule</em>'.
     * @see org.omnetpp.ned.model.emf.Submodules#getSubmodule()
     * @see #getSubmodules()
     * @generated
     */
    EReference getSubmodules_Submodule();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodules#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Submodules#getBannerComment()
     * @see #getSubmodules()
     * @generated
     */
    EAttribute getSubmodules_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodules#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Submodules#getRightComment()
     * @see #getSubmodules()
     * @generated
     */
    EAttribute getSubmodules_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Submodules#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Submodules#getTrailingComment()
     * @see #getSubmodules()
     * @generated
     */
    EAttribute getSubmodules_TrailingComment();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Substparam <em>Substparam</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Substparam</em>'.
     * @see org.omnetpp.ned.model.emf.Substparam
     * @generated
     */
    EClass getSubstparam();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparam#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Substparam#getBannerComment()
     * @see #getSubstparam()
     * @generated
     */
    EAttribute getSubstparam_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparam#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see org.omnetpp.ned.model.emf.Substparam#getName()
     * @see #getSubstparam()
     * @generated
     */
    EAttribute getSubstparam_Name();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparam#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Substparam#getRightComment()
     * @see #getSubstparam()
     * @generated
     */
    EAttribute getSubstparam_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparam#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Substparam#getTrailingComment()
     * @see #getSubstparam()
     * @generated
     */
    EAttribute getSubstparam_TrailingComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparam#getValue <em>Value</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Value</em>'.
     * @see org.omnetpp.ned.model.emf.Substparam#getValue()
     * @see #getSubstparam()
     * @generated
     */
    EAttribute getSubstparam_Value();

    /**
     * Returns the meta object for class '{@link org.omnetpp.ned.model.emf.Substparams <em>Substparams</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Substparams</em>'.
     * @see org.omnetpp.ned.model.emf.Substparams
     * @generated
     */
    EClass getSubstparams();

    /**
     * Returns the meta object for the containment reference list '{@link org.omnetpp.ned.model.emf.Substparams#getSubstparam <em>Substparam</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference list '<em>Substparam</em>'.
     * @see org.omnetpp.ned.model.emf.Substparams#getSubstparam()
     * @see #getSubstparams()
     * @generated
     */
    EReference getSubstparams_Substparam();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparams#getBannerComment <em>Banner Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Banner Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Substparams#getBannerComment()
     * @see #getSubstparams()
     * @generated
     */
    EAttribute getSubstparams_BannerComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparams#getCondition <em>Condition</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Condition</em>'.
     * @see org.omnetpp.ned.model.emf.Substparams#getCondition()
     * @see #getSubstparams()
     * @generated
     */
    EAttribute getSubstparams_Condition();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparams#getRightComment <em>Right Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Right Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Substparams#getRightComment()
     * @see #getSubstparams()
     * @generated
     */
    EAttribute getSubstparams_RightComment();

    /**
     * Returns the meta object for the attribute '{@link org.omnetpp.ned.model.emf.Substparams#getTrailingComment <em>Trailing Comment</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Trailing Comment</em>'.
     * @see org.omnetpp.ned.model.emf.Substparams#getTrailingComment()
     * @see #getSubstparams()
     * @generated
     */
    EAttribute getSubstparams_TrailingComment();

    /**
     * Returns the factory that creates the instances of the model.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the factory that creates the instances of the model.
     * @generated
     */
    NedFactory getNedFactory();

} //NedPackage
