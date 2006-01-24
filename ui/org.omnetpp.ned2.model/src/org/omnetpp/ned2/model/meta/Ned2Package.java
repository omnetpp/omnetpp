/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model.meta;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

import org.eclipse.emf.ecore.impl.EPackageImpl;

import org.eclipse.emf.ecore.xml.type.XMLTypePackage;

import org.omnetpp.ned2.model.Channel;
import org.omnetpp.ned2.model.ChannelInterface;
import org.omnetpp.ned2.model.CompoundModule;
import org.omnetpp.ned2.model.Condition;
import org.omnetpp.ned2.model.Connection;
import org.omnetpp.ned2.model.ConnectionGroup;
import org.omnetpp.ned2.model.Connections;
import org.omnetpp.ned2.model.Const;
import org.omnetpp.ned2.model.Expression;
import org.omnetpp.ned2.model.Extends;
import org.omnetpp.ned2.model.Files;
import org.omnetpp.ned2.model.Function;
import org.omnetpp.ned2.model.Gate;
import org.omnetpp.ned2.model.GateGroup;
import org.omnetpp.ned2.model.Gates;
import org.omnetpp.ned2.model.Import;
import org.omnetpp.ned2.model.InterfaceName;
import org.omnetpp.ned2.model.KeyValue;
import org.omnetpp.ned2.model.Loop;
import org.omnetpp.ned2.model.ModuleInterface;
import org.omnetpp.ned2.model.NedFile;
import org.omnetpp.ned2.model.Operator;
import org.omnetpp.ned2.model.Param;
import org.omnetpp.ned2.model.ParamGroup;
import org.omnetpp.ned2.model.Parameters;
import org.omnetpp.ned2.model.Property;
import org.omnetpp.ned2.model.Propertydef;
import org.omnetpp.ned2.model.Ref;
import org.omnetpp.ned2.model.SimpleModule;
import org.omnetpp.ned2.model.Submodule;
import org.omnetpp.ned2.model.Submodules;
import org.omnetpp.ned2.model.Types;
import org.omnetpp.ned2.model.Whitespace;

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
 * @see org.omnetpp.ned2.model.meta.Ned2Factory
 * @model kind="package"
 * @generated
 */
public class Ned2Package extends EPackageImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The package name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String eNAME = "ned2";

	/**
	 * The package namespace URI.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String eNS_URI = "http://omnetpp.org/schema/ned2";

	/**
	 * The package namespace name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String eNS_PREFIX = "ned2";

	/**
	 * The singleton instance of the package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final Ned2Package eINSTANCE = org.omnetpp.ned2.model.meta.Ned2Package
			.init();

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Channel <em>Channel</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Channel
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getChannel()
	 * @generated
	 */
	public static final int CHANNEL = 0;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Extends</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL__EXTENDS = 1;

	/**
	 * The feature id for the '<em><b>Interface Name</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL__INTERFACE_NAME = 2;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL__PARAMETERS = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL__NAME = 4;

	/**
	 * The number of structural features of the '<em>Channel</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.ChannelInterface <em>Channel Interface</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.ChannelInterface
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getChannelInterface()
	 * @generated
	 */
	public static final int CHANNEL_INTERFACE = 1;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL_INTERFACE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Extends</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL_INTERFACE__EXTENDS = 1;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL_INTERFACE__PARAMETERS = 2;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL_INTERFACE__NAME = 3;

	/**
	 * The number of structural features of the '<em>Channel Interface</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CHANNEL_INTERFACE_FEATURE_COUNT = 4;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.CompoundModule <em>Compound Module</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.CompoundModule
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getCompoundModule()
	 * @generated
	 */
	public static final int COMPOUND_MODULE = 2;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Extends</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__EXTENDS = 1;

	/**
	 * The feature id for the '<em><b>Interface Name</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__INTERFACE_NAME = 2;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__PARAMETERS = 3;

	/**
	 * The feature id for the '<em><b>Gates</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__GATES = 4;

	/**
	 * The feature id for the '<em><b>Types</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__TYPES = 5;

	/**
	 * The feature id for the '<em><b>Submodules</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__SUBMODULES = 6;

	/**
	 * The feature id for the '<em><b>Connections</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__CONNECTIONS = 7;

	/**
	 * The feature id for the '<em><b>Is Network</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__IS_NETWORK = 8;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE__NAME = 9;

	/**
	 * The number of structural features of the '<em>Compound Module</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int COMPOUND_MODULE_FEATURE_COUNT = 10;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Condition <em>Condition</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Condition
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getCondition()
	 * @generated
	 */
	public static final int CONDITION = 3;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONDITION__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONDITION__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>Condition</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONDITION__CONDITION = 2;

	/**
	 * The number of structural features of the '<em>Condition</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONDITION_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Connection <em>Connection</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Connection
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection()
	 * @generated
	 */
	public static final int CONNECTION = 4;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__PARAMETERS = 2;

	/**
	 * The feature id for the '<em><b>Arrow Direction</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__ARROW_DIRECTION = 3;

	/**
	 * The feature id for the '<em><b>Channel Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__CHANNEL_NAME = 4;

	/**
	 * The feature id for the '<em><b>Dest Gate</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__DEST_GATE = 5;

	/**
	 * The feature id for the '<em><b>Dest Gate Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__DEST_GATE_INDEX = 6;

	/**
	 * The feature id for the '<em><b>Dest Gate Plusplus</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__DEST_GATE_PLUSPLUS = 7;

	/**
	 * The feature id for the '<em><b>Dest Gate Subg</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__DEST_GATE_SUBG = 8;

	/**
	 * The feature id for the '<em><b>Dest Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__DEST_MODULE = 9;

	/**
	 * The feature id for the '<em><b>Dest Module Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__DEST_MODULE_INDEX = 10;

	/**
	 * The feature id for the '<em><b>Src Gate</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__SRC_GATE = 11;

	/**
	 * The feature id for the '<em><b>Src Gate Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__SRC_GATE_INDEX = 12;

	/**
	 * The feature id for the '<em><b>Src Gate Plusplus</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__SRC_GATE_PLUSPLUS = 13;

	/**
	 * The feature id for the '<em><b>Src Gate Subg</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__SRC_GATE_SUBG = 14;

	/**
	 * The feature id for the '<em><b>Src Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__SRC_MODULE = 15;

	/**
	 * The feature id for the '<em><b>Src Module Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION__SRC_MODULE_INDEX = 16;

	/**
	 * The number of structural features of the '<em>Connection</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_FEATURE_COUNT = 17;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.ConnectionGroup <em>Connection Group</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.ConnectionGroup
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup()
	 * @generated
	 */
	public static final int CONNECTION_GROUP = 5;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_GROUP__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_GROUP__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Loop</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_GROUP__LOOP = 2;

	/**
	 * The feature id for the '<em><b>Condition</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_GROUP__CONDITION = 3;

	/**
	 * The feature id for the '<em><b>Connection</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_GROUP__CONNECTION = 4;

	/**
	 * The number of structural features of the '<em>Connection Group</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTION_GROUP_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Connections <em>Connections</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Connections
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnections()
	 * @generated
	 */
	public static final int CONNECTIONS = 6;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTIONS__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTIONS__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Connection</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTIONS__CONNECTION = 2;

	/**
	 * The feature id for the '<em><b>Connection Group</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTIONS__CONNECTION_GROUP = 3;

	/**
	 * The feature id for the '<em><b>Check Unconnected</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTIONS__CHECK_UNCONNECTED = 4;

	/**
	 * The number of structural features of the '<em>Connections</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONNECTIONS_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Const <em>Const</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Const
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConst()
	 * @generated
	 */
	public static final int CONST = 7;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONST__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Text</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONST__TEXT = 1;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONST__TYPE = 2;

	/**
	 * The feature id for the '<em><b>Unit Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONST__UNIT_TYPE = 3;

	/**
	 * The feature id for the '<em><b>Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONST__VALUE = 4;

	/**
	 * The number of structural features of the '<em>Const</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int CONST_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Expression <em>Expression</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Expression
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getExpression()
	 * @generated
	 */
	public static final int EXPRESSION = 8;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Operator</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION__OPERATOR = 1;

	/**
	 * The feature id for the '<em><b>Function</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION__FUNCTION = 2;

	/**
	 * The feature id for the '<em><b>Ref</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION__REF = 3;

	/**
	 * The feature id for the '<em><b>Const</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION__CONST = 4;

	/**
	 * The feature id for the '<em><b>Target</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION__TARGET = 5;

	/**
	 * The number of structural features of the '<em>Expression</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXPRESSION_FEATURE_COUNT = 6;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Extends <em>Extends</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Extends
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getExtends()
	 * @generated
	 */
	public static final int EXTENDS = 9;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXTENDS__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXTENDS__NAME = 1;

	/**
	 * The number of structural features of the '<em>Extends</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int EXTENDS_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Files <em>Files</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Files
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getFiles()
	 * @generated
	 */
	public static final int FILES = 10;

	/**
	 * The feature id for the '<em><b>Ned File</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FILES__NED_FILE = 0;

	/**
	 * The number of structural features of the '<em>Files</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FILES_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Function <em>Function</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Function
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getFunction()
	 * @generated
	 */
	public static final int FUNCTION = 11;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Operator</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__OPERATOR = 2;

	/**
	 * The feature id for the '<em><b>Function</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__FUNCTION = 3;

	/**
	 * The feature id for the '<em><b>Ref</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__REF = 4;

	/**
	 * The feature id for the '<em><b>Const</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__CONST = 5;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION__NAME = 6;

	/**
	 * The number of structural features of the '<em>Function</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int FUNCTION_FEATURE_COUNT = 7;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Gate <em>Gate</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Gate
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getGate()
	 * @generated
	 */
	public static final int GATE = 12;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__PROPERTY = 2;

	/**
	 * The feature id for the '<em><b>Is Vector</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__IS_VECTOR = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__NAME = 4;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__TYPE = 5;

	/**
	 * The feature id for the '<em><b>Vector Size</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE__VECTOR_SIZE = 6;

	/**
	 * The number of structural features of the '<em>Gate</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE_FEATURE_COUNT = 7;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.GateGroup <em>Gate Group</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.GateGroup
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getGateGroup()
	 * @generated
	 */
	public static final int GATE_GROUP = 13;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE_GROUP__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Condition</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE_GROUP__CONDITION = 1;

	/**
	 * The feature id for the '<em><b>Gate</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE_GROUP__GATE = 2;

	/**
	 * The number of structural features of the '<em>Gate Group</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATE_GROUP_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Gates <em>Gates</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Gates
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getGates()
	 * @generated
	 */
	public static final int GATES = 14;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATES__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATES__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Gate</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATES__GATE = 2;

	/**
	 * The feature id for the '<em><b>Gate Group</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATES__GATE_GROUP = 3;

	/**
	 * The number of structural features of the '<em>Gates</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int GATES_FEATURE_COUNT = 4;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Import <em>Import</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Import
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getImport()
	 * @generated
	 */
	public static final int IMPORT = 15;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int IMPORT__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Filename</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int IMPORT__FILENAME = 1;

	/**
	 * The number of structural features of the '<em>Import</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int IMPORT_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.InterfaceName <em>Interface Name</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.InterfaceName
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getInterfaceName()
	 * @generated
	 */
	public static final int INTERFACE_NAME = 16;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int INTERFACE_NAME__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int INTERFACE_NAME__NAME = 1;

	/**
	 * The number of structural features of the '<em>Interface Name</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int INTERFACE_NAME_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.KeyValue <em>Key Value</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.KeyValue
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getKeyValue()
	 * @generated
	 */
	public static final int KEY_VALUE = 17;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int KEY_VALUE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Key</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int KEY_VALUE__KEY = 1;

	/**
	 * The feature id for the '<em><b>Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int KEY_VALUE__VALUE = 2;

	/**
	 * The number of structural features of the '<em>Key Value</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int KEY_VALUE_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Loop <em>Loop</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Loop
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop()
	 * @generated
	 */
	public static final int LOOP = 18;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int LOOP__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int LOOP__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>From Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int LOOP__FROM_VALUE = 2;

	/**
	 * The feature id for the '<em><b>Param Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int LOOP__PARAM_NAME = 3;

	/**
	 * The feature id for the '<em><b>To Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int LOOP__TO_VALUE = 4;

	/**
	 * The number of structural features of the '<em>Loop</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int LOOP_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.ModuleInterface <em>Module Interface</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.ModuleInterface
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getModuleInterface()
	 * @generated
	 */
	public static final int MODULE_INTERFACE = 19;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int MODULE_INTERFACE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Extends</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int MODULE_INTERFACE__EXTENDS = 1;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int MODULE_INTERFACE__PARAMETERS = 2;

	/**
	 * The feature id for the '<em><b>Gates</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int MODULE_INTERFACE__GATES = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int MODULE_INTERFACE__NAME = 4;

	/**
	 * The number of structural features of the '<em>Module Interface</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int MODULE_INTERFACE_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.NedFile <em>Ned File</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.NedFile
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getNedFile()
	 * @generated
	 */
	public static final int NED_FILE = 20;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Import</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__IMPORT = 2;

	/**
	 * The feature id for the '<em><b>Propertydef</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__PROPERTYDEF = 3;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__PROPERTY = 4;

	/**
	 * The feature id for the '<em><b>Channel</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__CHANNEL = 5;

	/**
	 * The feature id for the '<em><b>Channel Interface</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__CHANNEL_INTERFACE = 6;

	/**
	 * The feature id for the '<em><b>Simple Module</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__SIMPLE_MODULE = 7;

	/**
	 * The feature id for the '<em><b>Compound Module</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__COMPOUND_MODULE = 8;

	/**
	 * The feature id for the '<em><b>Module Interface</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__MODULE_INTERFACE = 9;

	/**
	 * The feature id for the '<em><b>Filename</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__FILENAME = 10;

	/**
	 * The feature id for the '<em><b>Package</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE__PACKAGE = 11;

	/**
	 * The number of structural features of the '<em>Ned File</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int NED_FILE_FEATURE_COUNT = 12;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Operator <em>Operator</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Operator
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getOperator()
	 * @generated
	 */
	public static final int OPERATOR = 21;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Operator</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__OPERATOR = 2;

	/**
	 * The feature id for the '<em><b>Function</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__FUNCTION = 3;

	/**
	 * The feature id for the '<em><b>Ref</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__REF = 4;

	/**
	 * The feature id for the '<em><b>Const</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__CONST = 5;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR__NAME = 6;

	/**
	 * The number of structural features of the '<em>Operator</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int OPERATOR_FEATURE_COUNT = 7;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Param <em>Param</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Param
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam()
	 * @generated
	 */
	public static final int PARAM = 22;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__PROPERTY = 2;

	/**
	 * The feature id for the '<em><b>Is Function</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__IS_FUNCTION = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__NAME = 4;

	/**
	 * The feature id for the '<em><b>Qualifier</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__QUALIFIER = 5;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__TYPE = 6;

	/**
	 * The feature id for the '<em><b>Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM__VALUE = 7;

	/**
	 * The number of structural features of the '<em>Param</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_FEATURE_COUNT = 8;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Parameters <em>Parameters</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Parameters
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters()
	 * @generated
	 */
	public static final int PARAMETERS = 23;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS__PROPERTY = 2;

	/**
	 * The feature id for the '<em><b>Param</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS__PARAM = 3;

	/**
	 * The feature id for the '<em><b>Param Group</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS__PARAM_GROUP = 4;

	/**
	 * The feature id for the '<em><b>Is Implicit</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS__IS_IMPLICIT = 5;

	/**
	 * The number of structural features of the '<em>Parameters</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAMETERS_FEATURE_COUNT = 6;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.ParamGroup <em>Param Group</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.ParamGroup
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParamGroup()
	 * @generated
	 */
	public static final int PARAM_GROUP = 24;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_GROUP__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Condition</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_GROUP__CONDITION = 1;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_GROUP__GROUP = 2;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_GROUP__PROPERTY = 3;

	/**
	 * The feature id for the '<em><b>Param</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_GROUP__PARAM = 4;

	/**
	 * The number of structural features of the '<em>Param Group</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PARAM_GROUP_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Property <em>Property</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Property
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getProperty()
	 * @generated
	 */
	public static final int PROPERTY = 25;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTY__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Key Value</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTY__KEY_VALUE = 1;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTY__PROPERTY = 2;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTY__NAME = 3;

	/**
	 * The feature id for the '<em><b>Qualifier</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTY__QUALIFIER = 4;

	/**
	 * The number of structural features of the '<em>Property</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTY_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Propertydef <em>Propertydef</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Propertydef
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getPropertydef()
	 * @generated
	 */
	public static final int PROPERTYDEF = 26;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTYDEF__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Key Value</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTYDEF__KEY_VALUE = 1;

	/**
	 * The feature id for the '<em><b>Property</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTYDEF__PROPERTY = 2;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTYDEF__NAME = 3;

	/**
	 * The number of structural features of the '<em>Propertydef</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int PROPERTYDEF_FEATURE_COUNT = 4;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Ref <em>Ref</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Ref
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef()
	 * @generated
	 */
	public static final int REF = 27;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int REF__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int REF__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int REF__MODULE = 2;

	/**
	 * The feature id for the '<em><b>Module Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int REF__MODULE_INDEX = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int REF__NAME = 4;

	/**
	 * The number of structural features of the '<em>Ref</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int REF_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.SimpleModule <em>Simple Module</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.SimpleModule
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule()
	 * @generated
	 */
	public static final int SIMPLE_MODULE = 28;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Extends</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE__EXTENDS = 1;

	/**
	 * The feature id for the '<em><b>Interface Name</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE__INTERFACE_NAME = 2;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE__PARAMETERS = 3;

	/**
	 * The feature id for the '<em><b>Gates</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE__GATES = 4;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE__NAME = 5;

	/**
	 * The number of structural features of the '<em>Simple Module</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SIMPLE_MODULE_FEATURE_COUNT = 6;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Submodule <em>Submodule</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Submodule
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule()
	 * @generated
	 */
	public static final int SUBMODULE = 29;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Expression</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__EXPRESSION = 1;

	/**
	 * The feature id for the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__PARAMETERS = 2;

	/**
	 * The feature id for the '<em><b>Gates</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__GATES = 3;

	/**
	 * The feature id for the '<em><b>Like Param</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__LIKE_PARAM = 4;

	/**
	 * The feature id for the '<em><b>Like Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__LIKE_TYPE = 5;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__NAME = 6;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__TYPE = 7;

	/**
	 * The feature id for the '<em><b>Vector Size</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE__VECTOR_SIZE = 8;

	/**
	 * The number of structural features of the '<em>Submodule</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULE_FEATURE_COUNT = 9;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Submodules <em>Submodules</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Submodules
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodules()
	 * @generated
	 */
	public static final int SUBMODULES = 30;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULES__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Submodule</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULES__SUBMODULE = 1;

	/**
	 * The number of structural features of the '<em>Submodules</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int SUBMODULES_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Types <em>Types</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Types
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getTypes()
	 * @generated
	 */
	public static final int TYPES = 31;

	/**
	 * The feature id for the '<em><b>Whitespace</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__WHITESPACE = 0;

	/**
	 * The feature id for the '<em><b>Group</b></em>' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__GROUP = 1;

	/**
	 * The feature id for the '<em><b>Channel</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__CHANNEL = 2;

	/**
	 * The feature id for the '<em><b>Channel Interface</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__CHANNEL_INTERFACE = 3;

	/**
	 * The feature id for the '<em><b>Simple Module</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__SIMPLE_MODULE = 4;

	/**
	 * The feature id for the '<em><b>Compound Module</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__COMPOUND_MODULE = 5;

	/**
	 * The feature id for the '<em><b>Module Interface</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES__MODULE_INTERFACE = 6;

	/**
	 * The number of structural features of the '<em>Types</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int TYPES_FEATURE_COUNT = 7;

	/**
	 * The meta object id for the '{@link org.omnetpp.ned2.model.Whitespace <em>Whitespace</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.omnetpp.ned2.model.Whitespace
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getWhitespace()
	 * @generated
	 */
	public static final int WHITESPACE = 32;

	/**
	 * The feature id for the '<em><b>Content</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int WHITESPACE__CONTENT = 0;

	/**
	 * The feature id for the '<em><b>Locid</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int WHITESPACE__LOCID = 1;

	/**
	 * The number of structural features of the '<em>Whitespace</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	public static final int WHITESPACE_FEATURE_COUNT = 2;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass channelEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass channelInterfaceEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass compoundModuleEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass conditionEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass connectionEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass connectionGroupEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass connectionsEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass constEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass expressionEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass extendsEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass filesEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass functionEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass gateEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass gateGroupEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass gatesEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass importEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass interfaceNameEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass keyValueEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass loopEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass moduleInterfaceEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass nedFileEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass operatorEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass paramEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass parametersEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass paramGroupEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass propertyEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass propertydefEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass refEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass simpleModuleEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass submoduleEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass submodulesEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass typesEClass = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private EClass whitespaceEClass = null;

	/**
	 * Creates an instance of the model <b>Package</b>, registered with
	 * {@link org.eclipse.emf.ecore.EPackage.Registry EPackage.Registry} by the package
	 * package URI value.
	 * <p>Note: the correct way to create the package is via the static
	 * factory method {@link #init init()}, which also performs
	 * initialization of the package, or returns the registered package,
	 * if one already exists.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see org.eclipse.emf.ecore.EPackage.Registry
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#eNS_URI
	 * @see #init()
	 * @generated
	 */
	private Ned2Package() {
		super(eNS_URI, Ned2Factory.eINSTANCE);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private static boolean isInited = false;

	/**
	 * Creates, registers, and initializes the <b>Package</b> for this
	 * model, and for any others upon which it depends.  Simple
	 * dependencies are satisfied by calling this method on all
	 * dependent packages before doing anything else.  This method drives
	 * initialization for interdependent packages directly, in parallel
	 * with this package, itself.
	 * <p>Of this package and its interdependencies, all packages which
	 * have not yet been registered by their URI values are first created
	 * and registered.  The packages are then initialized in two steps:
	 * meta-model objects for all of the packages are created before any
	 * are initialized, since one package's meta-model objects may refer to
	 * those of another.
	 * <p>Invocation of this method will not affect any packages that have
	 * already been initialized.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #eNS_URI
	 * @see #createPackageContents()
	 * @see #initializePackageContents()
	 * @generated
	 */
	public static Ned2Package init() {
		if (isInited)
			return (Ned2Package) EPackage.Registry.INSTANCE
					.getEPackage(Ned2Package.eNS_URI);

		// Obtain or create and register package
		Ned2Package theNed2Package = (Ned2Package) (EPackage.Registry.INSTANCE
				.getEPackage(eNS_URI) instanceof Ned2Package ? EPackage.Registry.INSTANCE
				.getEPackage(eNS_URI)
				: new Ned2Package());

		isInited = true;

		// Initialize simple dependencies
		XMLTypePackage.eINSTANCE.eClass();

		// Create package meta-data objects
		theNed2Package.createPackageContents();

		// Initialize created meta-data
		theNed2Package.initializePackageContents();

		// Mark meta-data to indicate it can't be changed
		theNed2Package.freeze();

		return theNed2Package;
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Channel <em>Channel</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Channel</em>'.
	 * @see org.omnetpp.ned2.model.Channel
	 * @generated
	 */
	public EClass getChannel() {
		return channelEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Channel#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Channel#getWhitespace()
	 * @see #getChannel()
	 * @generated
	 */
	public EReference getChannel_Whitespace() {
		return (EReference) channelEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Channel#getExtends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Extends</em>'.
	 * @see org.omnetpp.ned2.model.Channel#getExtends()
	 * @see #getChannel()
	 * @generated
	 */
	public EReference getChannel_Extends() {
		return (EReference) channelEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Channel#getInterfaceName <em>Interface Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Interface Name</em>'.
	 * @see org.omnetpp.ned2.model.Channel#getInterfaceName()
	 * @see #getChannel()
	 * @generated
	 */
	public EReference getChannel_InterfaceName() {
		return (EReference) channelEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Channel#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.Channel#getParameters()
	 * @see #getChannel()
	 * @generated
	 */
	public EReference getChannel_Parameters() {
		return (EReference) channelEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Channel#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Channel#getName()
	 * @see #getChannel()
	 * @generated
	 */
	public EAttribute getChannel_Name() {
		return (EAttribute) channelEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.ChannelInterface <em>Channel Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Channel Interface</em>'.
	 * @see org.omnetpp.ned2.model.ChannelInterface
	 * @generated
	 */
	public EClass getChannelInterface() {
		return channelInterfaceEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ChannelInterface#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.ChannelInterface#getWhitespace()
	 * @see #getChannelInterface()
	 * @generated
	 */
	public EReference getChannelInterface_Whitespace() {
		return (EReference) channelInterfaceEClass.getEStructuralFeatures()
				.get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ChannelInterface#getExtends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Extends</em>'.
	 * @see org.omnetpp.ned2.model.ChannelInterface#getExtends()
	 * @see #getChannelInterface()
	 * @generated
	 */
	public EReference getChannelInterface_Extends() {
		return (EReference) channelInterfaceEClass.getEStructuralFeatures()
				.get(1);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.ChannelInterface#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.ChannelInterface#getParameters()
	 * @see #getChannelInterface()
	 * @generated
	 */
	public EReference getChannelInterface_Parameters() {
		return (EReference) channelInterfaceEClass.getEStructuralFeatures()
				.get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.ChannelInterface#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.ChannelInterface#getName()
	 * @see #getChannelInterface()
	 * @generated
	 */
	public EAttribute getChannelInterface_Name() {
		return (EAttribute) channelInterfaceEClass.getEStructuralFeatures()
				.get(3);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.CompoundModule <em>Compound Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Compound Module</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule
	 * @generated
	 */
	public EClass getCompoundModule() {
		return compoundModuleEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.CompoundModule#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getWhitespace()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Whitespace() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.CompoundModule#getExtends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Extends</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getExtends()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Extends() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.CompoundModule#getInterfaceName <em>Interface Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Interface Name</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getInterfaceName()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_InterfaceName() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(2);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.CompoundModule#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getParameters()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Parameters() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(3);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.CompoundModule#getGates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Gates</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getGates()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Gates() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(4);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.CompoundModule#getTypes <em>Types</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Types</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getTypes()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Types() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(5);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.CompoundModule#getSubmodules <em>Submodules</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Submodules</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getSubmodules()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Submodules() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(6);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.CompoundModule#getConnections <em>Connections</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Connections</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getConnections()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EReference getCompoundModule_Connections() {
		return (EReference) compoundModuleEClass.getEStructuralFeatures()
				.get(7);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.CompoundModule#isIsNetwork <em>Is Network</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Is Network</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#isIsNetwork()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EAttribute getCompoundModule_IsNetwork() {
		return (EAttribute) compoundModuleEClass.getEStructuralFeatures()
				.get(8);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.CompoundModule#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.CompoundModule#getName()
	 * @see #getCompoundModule()
	 * @generated
	 */
	public EAttribute getCompoundModule_Name() {
		return (EAttribute) compoundModuleEClass.getEStructuralFeatures()
				.get(9);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Condition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Condition</em>'.
	 * @see org.omnetpp.ned2.model.Condition
	 * @generated
	 */
	public EClass getCondition() {
		return conditionEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Condition#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Condition#getWhitespace()
	 * @see #getCondition()
	 * @generated
	 */
	public EReference getCondition_Whitespace() {
		return (EReference) conditionEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Condition#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Condition#getExpression()
	 * @see #getCondition()
	 * @generated
	 */
	public EReference getCondition_Expression() {
		return (EReference) conditionEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Condition#getCondition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Condition</em>'.
	 * @see org.omnetpp.ned2.model.Condition#getCondition()
	 * @see #getCondition()
	 * @generated
	 */
	public EAttribute getCondition_Condition() {
		return (EAttribute) conditionEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Connection <em>Connection</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Connection</em>'.
	 * @see org.omnetpp.ned2.model.Connection
	 * @generated
	 */
	public EClass getConnection() {
		return connectionEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Connection#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getWhitespace()
	 * @see #getConnection()
	 * @generated
	 */
	public EReference getConnection_Whitespace() {
		return (EReference) connectionEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Connection#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getExpression()
	 * @see #getConnection()
	 * @generated
	 */
	public EReference getConnection_Expression() {
		return (EReference) connectionEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Connection#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getParameters()
	 * @see #getConnection()
	 * @generated
	 */
	public EReference getConnection_Parameters() {
		return (EReference) connectionEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getArrowDirection <em>Arrow Direction</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Arrow Direction</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getArrowDirection()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_ArrowDirection() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getChannelName <em>Channel Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Channel Name</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getChannelName()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_ChannelName() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getDestGate <em>Dest Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Dest Gate</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getDestGate()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_DestGate() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getDestGateIndex <em>Dest Gate Index</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Dest Gate Index</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getDestGateIndex()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_DestGateIndex() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Dest Gate Plusplus</em>'.
	 * @see org.omnetpp.ned2.model.Connection#isDestGatePlusplus()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_DestGatePlusplus() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(7);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getDestGateSubg <em>Dest Gate Subg</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Dest Gate Subg</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getDestGateSubg()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_DestGateSubg() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(8);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getDestModule <em>Dest Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Dest Module</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getDestModule()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_DestModule() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(9);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getDestModuleIndex <em>Dest Module Index</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Dest Module Index</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getDestModuleIndex()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_DestModuleIndex() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(10);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getSrcGate <em>Src Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Src Gate</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getSrcGate()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_SrcGate() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(11);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getSrcGateIndex <em>Src Gate Index</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Src Gate Index</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getSrcGateIndex()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_SrcGateIndex() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(12);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Src Gate Plusplus</em>'.
	 * @see org.omnetpp.ned2.model.Connection#isSrcGatePlusplus()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_SrcGatePlusplus() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(13);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getSrcGateSubg <em>Src Gate Subg</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Src Gate Subg</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getSrcGateSubg()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_SrcGateSubg() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(14);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getSrcModule <em>Src Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Src Module</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getSrcModule()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_SrcModule() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(15);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connection#getSrcModuleIndex <em>Src Module Index</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Src Module Index</em>'.
	 * @see org.omnetpp.ned2.model.Connection#getSrcModuleIndex()
	 * @see #getConnection()
	 * @generated
	 */
	public EAttribute getConnection_SrcModuleIndex() {
		return (EAttribute) connectionEClass.getEStructuralFeatures().get(16);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.ConnectionGroup <em>Connection Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Connection Group</em>'.
	 * @see org.omnetpp.ned2.model.ConnectionGroup
	 * @generated
	 */
	public EClass getConnectionGroup() {
		return connectionGroupEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ConnectionGroup#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.ConnectionGroup#getWhitespace()
	 * @see #getConnectionGroup()
	 * @generated
	 */
	public EReference getConnectionGroup_Whitespace() {
		return (EReference) connectionGroupEClass.getEStructuralFeatures().get(
				0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.ConnectionGroup#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.ConnectionGroup#getGroup()
	 * @see #getConnectionGroup()
	 * @generated
	 */
	public EAttribute getConnectionGroup_Group() {
		return (EAttribute) connectionGroupEClass.getEStructuralFeatures().get(
				1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ConnectionGroup#getLoop <em>Loop</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Loop</em>'.
	 * @see org.omnetpp.ned2.model.ConnectionGroup#getLoop()
	 * @see #getConnectionGroup()
	 * @generated
	 */
	public EReference getConnectionGroup_Loop() {
		return (EReference) connectionGroupEClass.getEStructuralFeatures().get(
				2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ConnectionGroup#getCondition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Condition</em>'.
	 * @see org.omnetpp.ned2.model.ConnectionGroup#getCondition()
	 * @see #getConnectionGroup()
	 * @generated
	 */
	public EReference getConnectionGroup_Condition() {
		return (EReference) connectionGroupEClass.getEStructuralFeatures().get(
				3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ConnectionGroup#getConnection <em>Connection</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Connection</em>'.
	 * @see org.omnetpp.ned2.model.ConnectionGroup#getConnection()
	 * @see #getConnectionGroup()
	 * @generated
	 */
	public EReference getConnectionGroup_Connection() {
		return (EReference) connectionGroupEClass.getEStructuralFeatures().get(
				4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Connections <em>Connections</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Connections</em>'.
	 * @see org.omnetpp.ned2.model.Connections
	 * @generated
	 */
	public EClass getConnections() {
		return connectionsEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Connections#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Connections#getWhitespace()
	 * @see #getConnections()
	 * @generated
	 */
	public EReference getConnections_Whitespace() {
		return (EReference) connectionsEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.Connections#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.Connections#getGroup()
	 * @see #getConnections()
	 * @generated
	 */
	public EAttribute getConnections_Group() {
		return (EAttribute) connectionsEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Connections#getConnection <em>Connection</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Connection</em>'.
	 * @see org.omnetpp.ned2.model.Connections#getConnection()
	 * @see #getConnections()
	 * @generated
	 */
	public EReference getConnections_Connection() {
		return (EReference) connectionsEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Connections#getConnectionGroup <em>Connection Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Connection Group</em>'.
	 * @see org.omnetpp.ned2.model.Connections#getConnectionGroup()
	 * @see #getConnections()
	 * @generated
	 */
	public EReference getConnections_ConnectionGroup() {
		return (EReference) connectionsEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Connections#isCheckUnconnected <em>Check Unconnected</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Check Unconnected</em>'.
	 * @see org.omnetpp.ned2.model.Connections#isCheckUnconnected()
	 * @see #getConnections()
	 * @generated
	 */
	public EAttribute getConnections_CheckUnconnected() {
		return (EAttribute) connectionsEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Const <em>Const</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Const</em>'.
	 * @see org.omnetpp.ned2.model.Const
	 * @generated
	 */
	public EClass getConst() {
		return constEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Const#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Const#getWhitespace()
	 * @see #getConst()
	 * @generated
	 */
	public EReference getConst_Whitespace() {
		return (EReference) constEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Const#getText <em>Text</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Text</em>'.
	 * @see org.omnetpp.ned2.model.Const#getText()
	 * @see #getConst()
	 * @generated
	 */
	public EAttribute getConst_Text() {
		return (EAttribute) constEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Const#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see org.omnetpp.ned2.model.Const#getType()
	 * @see #getConst()
	 * @generated
	 */
	public EAttribute getConst_Type() {
		return (EAttribute) constEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Const#getUnitType <em>Unit Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Unit Type</em>'.
	 * @see org.omnetpp.ned2.model.Const#getUnitType()
	 * @see #getConst()
	 * @generated
	 */
	public EAttribute getConst_UnitType() {
		return (EAttribute) constEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Const#getValue <em>Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Value</em>'.
	 * @see org.omnetpp.ned2.model.Const#getValue()
	 * @see #getConst()
	 * @generated
	 */
	public EAttribute getConst_Value() {
		return (EAttribute) constEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Expression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Expression
	 * @generated
	 */
	public EClass getExpression() {
		return expressionEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Expression#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Expression#getWhitespace()
	 * @see #getExpression()
	 * @generated
	 */
	public EReference getExpression_Whitespace() {
		return (EReference) expressionEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Expression#getOperator <em>Operator</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Operator</em>'.
	 * @see org.omnetpp.ned2.model.Expression#getOperator()
	 * @see #getExpression()
	 * @generated
	 */
	public EReference getExpression_Operator() {
		return (EReference) expressionEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Expression#getFunction <em>Function</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Function</em>'.
	 * @see org.omnetpp.ned2.model.Expression#getFunction()
	 * @see #getExpression()
	 * @generated
	 */
	public EReference getExpression_Function() {
		return (EReference) expressionEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Expression#getRef <em>Ref</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Ref</em>'.
	 * @see org.omnetpp.ned2.model.Expression#getRef()
	 * @see #getExpression()
	 * @generated
	 */
	public EReference getExpression_Ref() {
		return (EReference) expressionEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Expression#getConst <em>Const</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Const</em>'.
	 * @see org.omnetpp.ned2.model.Expression#getConst()
	 * @see #getExpression()
	 * @generated
	 */
	public EReference getExpression_Const() {
		return (EReference) expressionEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Expression#getTarget <em>Target</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Target</em>'.
	 * @see org.omnetpp.ned2.model.Expression#getTarget()
	 * @see #getExpression()
	 * @generated
	 */
	public EAttribute getExpression_Target() {
		return (EAttribute) expressionEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Extends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Extends</em>'.
	 * @see org.omnetpp.ned2.model.Extends
	 * @generated
	 */
	public EClass getExtends() {
		return extendsEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Extends#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Extends#getWhitespace()
	 * @see #getExtends()
	 * @generated
	 */
	public EReference getExtends_Whitespace() {
		return (EReference) extendsEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Extends#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Extends#getName()
	 * @see #getExtends()
	 * @generated
	 */
	public EAttribute getExtends_Name() {
		return (EAttribute) extendsEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Files <em>Files</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Files</em>'.
	 * @see org.omnetpp.ned2.model.Files
	 * @generated
	 */
	public EClass getFiles() {
		return filesEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Files#getNedFile <em>Ned File</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Ned File</em>'.
	 * @see org.omnetpp.ned2.model.Files#getNedFile()
	 * @see #getFiles()
	 * @generated
	 */
	public EReference getFiles_NedFile() {
		return (EReference) filesEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Function <em>Function</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Function</em>'.
	 * @see org.omnetpp.ned2.model.Function
	 * @generated
	 */
	public EClass getFunction() {
		return functionEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Function#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Function#getWhitespace()
	 * @see #getFunction()
	 * @generated
	 */
	public EReference getFunction_Whitespace() {
		return (EReference) functionEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.Function#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.Function#getGroup()
	 * @see #getFunction()
	 * @generated
	 */
	public EAttribute getFunction_Group() {
		return (EAttribute) functionEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Function#getOperator <em>Operator</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Operator</em>'.
	 * @see org.omnetpp.ned2.model.Function#getOperator()
	 * @see #getFunction()
	 * @generated
	 */
	public EReference getFunction_Operator() {
		return (EReference) functionEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Function#getFunction <em>Function</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Function</em>'.
	 * @see org.omnetpp.ned2.model.Function#getFunction()
	 * @see #getFunction()
	 * @generated
	 */
	public EReference getFunction_Function() {
		return (EReference) functionEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Function#getRef <em>Ref</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Ref</em>'.
	 * @see org.omnetpp.ned2.model.Function#getRef()
	 * @see #getFunction()
	 * @generated
	 */
	public EReference getFunction_Ref() {
		return (EReference) functionEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Function#getConst <em>Const</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Const</em>'.
	 * @see org.omnetpp.ned2.model.Function#getConst()
	 * @see #getFunction()
	 * @generated
	 */
	public EReference getFunction_Const() {
		return (EReference) functionEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Function#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Function#getName()
	 * @see #getFunction()
	 * @generated
	 */
	public EAttribute getFunction_Name() {
		return (EAttribute) functionEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Gate <em>Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Gate</em>'.
	 * @see org.omnetpp.ned2.model.Gate
	 * @generated
	 */
	public EClass getGate() {
		return gateEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Gate#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Gate#getWhitespace()
	 * @see #getGate()
	 * @generated
	 */
	public EReference getGate_Whitespace() {
		return (EReference) gateEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Gate#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Gate#getExpression()
	 * @see #getGate()
	 * @generated
	 */
	public EReference getGate_Expression() {
		return (EReference) gateEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Gate#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.Gate#getProperty()
	 * @see #getGate()
	 * @generated
	 */
	public EReference getGate_Property() {
		return (EReference) gateEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Gate#isIsVector <em>Is Vector</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Is Vector</em>'.
	 * @see org.omnetpp.ned2.model.Gate#isIsVector()
	 * @see #getGate()
	 * @generated
	 */
	public EAttribute getGate_IsVector() {
		return (EAttribute) gateEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Gate#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Gate#getName()
	 * @see #getGate()
	 * @generated
	 */
	public EAttribute getGate_Name() {
		return (EAttribute) gateEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Gate#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see org.omnetpp.ned2.model.Gate#getType()
	 * @see #getGate()
	 * @generated
	 */
	public EAttribute getGate_Type() {
		return (EAttribute) gateEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Gate#getVectorSize <em>Vector Size</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Vector Size</em>'.
	 * @see org.omnetpp.ned2.model.Gate#getVectorSize()
	 * @see #getGate()
	 * @generated
	 */
	public EAttribute getGate_VectorSize() {
		return (EAttribute) gateEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.GateGroup <em>Gate Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Gate Group</em>'.
	 * @see org.omnetpp.ned2.model.GateGroup
	 * @generated
	 */
	public EClass getGateGroup() {
		return gateGroupEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.GateGroup#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.GateGroup#getWhitespace()
	 * @see #getGateGroup()
	 * @generated
	 */
	public EReference getGateGroup_Whitespace() {
		return (EReference) gateGroupEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.GateGroup#getCondition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Condition</em>'.
	 * @see org.omnetpp.ned2.model.GateGroup#getCondition()
	 * @see #getGateGroup()
	 * @generated
	 */
	public EReference getGateGroup_Condition() {
		return (EReference) gateGroupEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.GateGroup#getGate <em>Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Gate</em>'.
	 * @see org.omnetpp.ned2.model.GateGroup#getGate()
	 * @see #getGateGroup()
	 * @generated
	 */
	public EReference getGateGroup_Gate() {
		return (EReference) gateGroupEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Gates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Gates</em>'.
	 * @see org.omnetpp.ned2.model.Gates
	 * @generated
	 */
	public EClass getGates() {
		return gatesEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Gates#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Gates#getWhitespace()
	 * @see #getGates()
	 * @generated
	 */
	public EReference getGates_Whitespace() {
		return (EReference) gatesEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.Gates#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.Gates#getGroup()
	 * @see #getGates()
	 * @generated
	 */
	public EAttribute getGates_Group() {
		return (EAttribute) gatesEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Gates#getGate <em>Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Gate</em>'.
	 * @see org.omnetpp.ned2.model.Gates#getGate()
	 * @see #getGates()
	 * @generated
	 */
	public EReference getGates_Gate() {
		return (EReference) gatesEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Gates#getGateGroup <em>Gate Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Gate Group</em>'.
	 * @see org.omnetpp.ned2.model.Gates#getGateGroup()
	 * @see #getGates()
	 * @generated
	 */
	public EReference getGates_GateGroup() {
		return (EReference) gatesEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Import <em>Import</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Import</em>'.
	 * @see org.omnetpp.ned2.model.Import
	 * @generated
	 */
	public EClass getImport() {
		return importEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Import#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Import#getWhitespace()
	 * @see #getImport()
	 * @generated
	 */
	public EReference getImport_Whitespace() {
		return (EReference) importEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Import#getFilename <em>Filename</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Filename</em>'.
	 * @see org.omnetpp.ned2.model.Import#getFilename()
	 * @see #getImport()
	 * @generated
	 */
	public EAttribute getImport_Filename() {
		return (EAttribute) importEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.InterfaceName <em>Interface Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Interface Name</em>'.
	 * @see org.omnetpp.ned2.model.InterfaceName
	 * @generated
	 */
	public EClass getInterfaceName() {
		return interfaceNameEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.InterfaceName#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.InterfaceName#getWhitespace()
	 * @see #getInterfaceName()
	 * @generated
	 */
	public EReference getInterfaceName_Whitespace() {
		return (EReference) interfaceNameEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.InterfaceName#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.InterfaceName#getName()
	 * @see #getInterfaceName()
	 * @generated
	 */
	public EAttribute getInterfaceName_Name() {
		return (EAttribute) interfaceNameEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.KeyValue <em>Key Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Key Value</em>'.
	 * @see org.omnetpp.ned2.model.KeyValue
	 * @generated
	 */
	public EClass getKeyValue() {
		return keyValueEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.KeyValue#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.KeyValue#getWhitespace()
	 * @see #getKeyValue()
	 * @generated
	 */
	public EReference getKeyValue_Whitespace() {
		return (EReference) keyValueEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.KeyValue#getKey <em>Key</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Key</em>'.
	 * @see org.omnetpp.ned2.model.KeyValue#getKey()
	 * @see #getKeyValue()
	 * @generated
	 */
	public EAttribute getKeyValue_Key() {
		return (EAttribute) keyValueEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.KeyValue#getValue <em>Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Value</em>'.
	 * @see org.omnetpp.ned2.model.KeyValue#getValue()
	 * @see #getKeyValue()
	 * @generated
	 */
	public EAttribute getKeyValue_Value() {
		return (EAttribute) keyValueEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Loop <em>Loop</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Loop</em>'.
	 * @see org.omnetpp.ned2.model.Loop
	 * @generated
	 */
	public EClass getLoop() {
		return loopEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Loop#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Loop#getWhitespace()
	 * @see #getLoop()
	 * @generated
	 */
	public EReference getLoop_Whitespace() {
		return (EReference) loopEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Loop#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Loop#getExpression()
	 * @see #getLoop()
	 * @generated
	 */
	public EReference getLoop_Expression() {
		return (EReference) loopEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Loop#getFromValue <em>From Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>From Value</em>'.
	 * @see org.omnetpp.ned2.model.Loop#getFromValue()
	 * @see #getLoop()
	 * @generated
	 */
	public EAttribute getLoop_FromValue() {
		return (EAttribute) loopEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Loop#getParamName <em>Param Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Param Name</em>'.
	 * @see org.omnetpp.ned2.model.Loop#getParamName()
	 * @see #getLoop()
	 * @generated
	 */
	public EAttribute getLoop_ParamName() {
		return (EAttribute) loopEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Loop#getToValue <em>To Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>To Value</em>'.
	 * @see org.omnetpp.ned2.model.Loop#getToValue()
	 * @see #getLoop()
	 * @generated
	 */
	public EAttribute getLoop_ToValue() {
		return (EAttribute) loopEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.ModuleInterface <em>Module Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Module Interface</em>'.
	 * @see org.omnetpp.ned2.model.ModuleInterface
	 * @generated
	 */
	public EClass getModuleInterface() {
		return moduleInterfaceEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ModuleInterface#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.ModuleInterface#getWhitespace()
	 * @see #getModuleInterface()
	 * @generated
	 */
	public EReference getModuleInterface_Whitespace() {
		return (EReference) moduleInterfaceEClass.getEStructuralFeatures().get(
				0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ModuleInterface#getExtends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Extends</em>'.
	 * @see org.omnetpp.ned2.model.ModuleInterface#getExtends()
	 * @see #getModuleInterface()
	 * @generated
	 */
	public EReference getModuleInterface_Extends() {
		return (EReference) moduleInterfaceEClass.getEStructuralFeatures().get(
				1);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.ModuleInterface#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.ModuleInterface#getParameters()
	 * @see #getModuleInterface()
	 * @generated
	 */
	public EReference getModuleInterface_Parameters() {
		return (EReference) moduleInterfaceEClass.getEStructuralFeatures().get(
				2);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.ModuleInterface#getGates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Gates</em>'.
	 * @see org.omnetpp.ned2.model.ModuleInterface#getGates()
	 * @see #getModuleInterface()
	 * @generated
	 */
	public EReference getModuleInterface_Gates() {
		return (EReference) moduleInterfaceEClass.getEStructuralFeatures().get(
				3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.ModuleInterface#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.ModuleInterface#getName()
	 * @see #getModuleInterface()
	 * @generated
	 */
	public EAttribute getModuleInterface_Name() {
		return (EAttribute) moduleInterfaceEClass.getEStructuralFeatures().get(
				4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.NedFile <em>Ned File</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Ned File</em>'.
	 * @see org.omnetpp.ned2.model.NedFile
	 * @generated
	 */
	public EClass getNedFile() {
		return nedFileEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getWhitespace()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_Whitespace() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.NedFile#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getGroup()
	 * @see #getNedFile()
	 * @generated
	 */
	public EAttribute getNedFile_Group() {
		return (EAttribute) nedFileEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getImport <em>Import</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Import</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getImport()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_Import() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getPropertydef <em>Propertydef</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Propertydef</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getPropertydef()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_Propertydef() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getProperty()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_Property() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getChannel <em>Channel</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Channel</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getChannel()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_Channel() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getChannelInterface <em>Channel Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Channel Interface</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getChannelInterface()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_ChannelInterface() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getSimpleModule <em>Simple Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Simple Module</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getSimpleModule()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_SimpleModule() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(7);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getCompoundModule <em>Compound Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Compound Module</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getCompoundModule()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_CompoundModule() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(8);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.NedFile#getModuleInterface <em>Module Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Module Interface</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getModuleInterface()
	 * @see #getNedFile()
	 * @generated
	 */
	public EReference getNedFile_ModuleInterface() {
		return (EReference) nedFileEClass.getEStructuralFeatures().get(9);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.NedFile#getFilename <em>Filename</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Filename</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getFilename()
	 * @see #getNedFile()
	 * @generated
	 */
	public EAttribute getNedFile_Filename() {
		return (EAttribute) nedFileEClass.getEStructuralFeatures().get(10);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.NedFile#getPackage <em>Package</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Package</em>'.
	 * @see org.omnetpp.ned2.model.NedFile#getPackage()
	 * @see #getNedFile()
	 * @generated
	 */
	public EAttribute getNedFile_Package() {
		return (EAttribute) nedFileEClass.getEStructuralFeatures().get(11);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Operator <em>Operator</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Operator</em>'.
	 * @see org.omnetpp.ned2.model.Operator
	 * @generated
	 */
	public EClass getOperator() {
		return operatorEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Operator#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getWhitespace()
	 * @see #getOperator()
	 * @generated
	 */
	public EReference getOperator_Whitespace() {
		return (EReference) operatorEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.Operator#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getGroup()
	 * @see #getOperator()
	 * @generated
	 */
	public EAttribute getOperator_Group() {
		return (EAttribute) operatorEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Operator#getOperator <em>Operator</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Operator</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getOperator()
	 * @see #getOperator()
	 * @generated
	 */
	public EReference getOperator_Operator() {
		return (EReference) operatorEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Operator#getFunction <em>Function</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Function</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getFunction()
	 * @see #getOperator()
	 * @generated
	 */
	public EReference getOperator_Function() {
		return (EReference) operatorEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Operator#getRef <em>Ref</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Ref</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getRef()
	 * @see #getOperator()
	 * @generated
	 */
	public EReference getOperator_Ref() {
		return (EReference) operatorEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Operator#getConst <em>Const</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Const</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getConst()
	 * @see #getOperator()
	 * @generated
	 */
	public EReference getOperator_Const() {
		return (EReference) operatorEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Operator#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Operator#getName()
	 * @see #getOperator()
	 * @generated
	 */
	public EAttribute getOperator_Name() {
		return (EAttribute) operatorEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Param <em>Param</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Param</em>'.
	 * @see org.omnetpp.ned2.model.Param
	 * @generated
	 */
	public EClass getParam() {
		return paramEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Param#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Param#getWhitespace()
	 * @see #getParam()
	 * @generated
	 */
	public EReference getParam_Whitespace() {
		return (EReference) paramEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Param#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Param#getExpression()
	 * @see #getParam()
	 * @generated
	 */
	public EReference getParam_Expression() {
		return (EReference) paramEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Param#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.Param#getProperty()
	 * @see #getParam()
	 * @generated
	 */
	public EReference getParam_Property() {
		return (EReference) paramEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Param#isIsFunction <em>Is Function</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Is Function</em>'.
	 * @see org.omnetpp.ned2.model.Param#isIsFunction()
	 * @see #getParam()
	 * @generated
	 */
	public EAttribute getParam_IsFunction() {
		return (EAttribute) paramEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Param#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Param#getName()
	 * @see #getParam()
	 * @generated
	 */
	public EAttribute getParam_Name() {
		return (EAttribute) paramEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Param#getQualifier <em>Qualifier</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Qualifier</em>'.
	 * @see org.omnetpp.ned2.model.Param#getQualifier()
	 * @see #getParam()
	 * @generated
	 */
	public EAttribute getParam_Qualifier() {
		return (EAttribute) paramEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Param#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see org.omnetpp.ned2.model.Param#getType()
	 * @see #getParam()
	 * @generated
	 */
	public EAttribute getParam_Type() {
		return (EAttribute) paramEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Param#getValue <em>Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Value</em>'.
	 * @see org.omnetpp.ned2.model.Param#getValue()
	 * @see #getParam()
	 * @generated
	 */
	public EAttribute getParam_Value() {
		return (EAttribute) paramEClass.getEStructuralFeatures().get(7);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Parameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.Parameters
	 * @generated
	 */
	public EClass getParameters() {
		return parametersEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Parameters#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Parameters#getWhitespace()
	 * @see #getParameters()
	 * @generated
	 */
	public EReference getParameters_Whitespace() {
		return (EReference) parametersEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.Parameters#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.Parameters#getGroup()
	 * @see #getParameters()
	 * @generated
	 */
	public EAttribute getParameters_Group() {
		return (EAttribute) parametersEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Parameters#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.Parameters#getProperty()
	 * @see #getParameters()
	 * @generated
	 */
	public EReference getParameters_Property() {
		return (EReference) parametersEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Parameters#getParam <em>Param</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Param</em>'.
	 * @see org.omnetpp.ned2.model.Parameters#getParam()
	 * @see #getParameters()
	 * @generated
	 */
	public EReference getParameters_Param() {
		return (EReference) parametersEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Parameters#getParamGroup <em>Param Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Param Group</em>'.
	 * @see org.omnetpp.ned2.model.Parameters#getParamGroup()
	 * @see #getParameters()
	 * @generated
	 */
	public EReference getParameters_ParamGroup() {
		return (EReference) parametersEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Parameters#isIsImplicit <em>Is Implicit</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Is Implicit</em>'.
	 * @see org.omnetpp.ned2.model.Parameters#isIsImplicit()
	 * @see #getParameters()
	 * @generated
	 */
	public EAttribute getParameters_IsImplicit() {
		return (EAttribute) parametersEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.ParamGroup <em>Param Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Param Group</em>'.
	 * @see org.omnetpp.ned2.model.ParamGroup
	 * @generated
	 */
	public EClass getParamGroup() {
		return paramGroupEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ParamGroup#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.ParamGroup#getWhitespace()
	 * @see #getParamGroup()
	 * @generated
	 */
	public EReference getParamGroup_Whitespace() {
		return (EReference) paramGroupEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.ParamGroup#getCondition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Condition</em>'.
	 * @see org.omnetpp.ned2.model.ParamGroup#getCondition()
	 * @see #getParamGroup()
	 * @generated
	 */
	public EReference getParamGroup_Condition() {
		return (EReference) paramGroupEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.ParamGroup#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.ParamGroup#getGroup()
	 * @see #getParamGroup()
	 * @generated
	 */
	public EAttribute getParamGroup_Group() {
		return (EAttribute) paramGroupEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ParamGroup#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.ParamGroup#getProperty()
	 * @see #getParamGroup()
	 * @generated
	 */
	public EReference getParamGroup_Property() {
		return (EReference) paramGroupEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.ParamGroup#getParam <em>Param</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Param</em>'.
	 * @see org.omnetpp.ned2.model.ParamGroup#getParam()
	 * @see #getParamGroup()
	 * @generated
	 */
	public EReference getParamGroup_Param() {
		return (EReference) paramGroupEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Property <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.Property
	 * @generated
	 */
	public EClass getProperty() {
		return propertyEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Property#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Property#getWhitespace()
	 * @see #getProperty()
	 * @generated
	 */
	public EReference getProperty_Whitespace() {
		return (EReference) propertyEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Property#getKeyValue <em>Key Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Key Value</em>'.
	 * @see org.omnetpp.ned2.model.Property#getKeyValue()
	 * @see #getProperty()
	 * @generated
	 */
	public EReference getProperty_KeyValue() {
		return (EReference) propertyEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Property#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.Property#getProperty()
	 * @see #getProperty()
	 * @generated
	 */
	public EReference getProperty_Property() {
		return (EReference) propertyEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Property#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Property#getName()
	 * @see #getProperty()
	 * @generated
	 */
	public EAttribute getProperty_Name() {
		return (EAttribute) propertyEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Property#getQualifier <em>Qualifier</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Qualifier</em>'.
	 * @see org.omnetpp.ned2.model.Property#getQualifier()
	 * @see #getProperty()
	 * @generated
	 */
	public EAttribute getProperty_Qualifier() {
		return (EAttribute) propertyEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Propertydef <em>Propertydef</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Propertydef</em>'.
	 * @see org.omnetpp.ned2.model.Propertydef
	 * @generated
	 */
	public EClass getPropertydef() {
		return propertydefEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Propertydef#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Propertydef#getWhitespace()
	 * @see #getPropertydef()
	 * @generated
	 */
	public EReference getPropertydef_Whitespace() {
		return (EReference) propertydefEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Propertydef#getKeyValue <em>Key Value</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Key Value</em>'.
	 * @see org.omnetpp.ned2.model.Propertydef#getKeyValue()
	 * @see #getPropertydef()
	 * @generated
	 */
	public EReference getPropertydef_KeyValue() {
		return (EReference) propertydefEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Propertydef#getProperty <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Property</em>'.
	 * @see org.omnetpp.ned2.model.Propertydef#getProperty()
	 * @see #getPropertydef()
	 * @generated
	 */
	public EReference getPropertydef_Property() {
		return (EReference) propertydefEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Propertydef#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Propertydef#getName()
	 * @see #getPropertydef()
	 * @generated
	 */
	public EAttribute getPropertydef_Name() {
		return (EAttribute) propertydefEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Ref <em>Ref</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Ref</em>'.
	 * @see org.omnetpp.ned2.model.Ref
	 * @generated
	 */
	public EClass getRef() {
		return refEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Ref#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Ref#getWhitespace()
	 * @see #getRef()
	 * @generated
	 */
	public EReference getRef_Whitespace() {
		return (EReference) refEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Ref#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Ref#getExpression()
	 * @see #getRef()
	 * @generated
	 */
	public EReference getRef_Expression() {
		return (EReference) refEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Ref#getModule <em>Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Module</em>'.
	 * @see org.omnetpp.ned2.model.Ref#getModule()
	 * @see #getRef()
	 * @generated
	 */
	public EAttribute getRef_Module() {
		return (EAttribute) refEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Ref#getModuleIndex <em>Module Index</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Module Index</em>'.
	 * @see org.omnetpp.ned2.model.Ref#getModuleIndex()
	 * @see #getRef()
	 * @generated
	 */
	public EAttribute getRef_ModuleIndex() {
		return (EAttribute) refEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Ref#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Ref#getName()
	 * @see #getRef()
	 * @generated
	 */
	public EAttribute getRef_Name() {
		return (EAttribute) refEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.SimpleModule <em>Simple Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Simple Module</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule
	 * @generated
	 */
	public EClass getSimpleModule() {
		return simpleModuleEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.SimpleModule#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule#getWhitespace()
	 * @see #getSimpleModule()
	 * @generated
	 */
	public EReference getSimpleModule_Whitespace() {
		return (EReference) simpleModuleEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.SimpleModule#getExtends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Extends</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule#getExtends()
	 * @see #getSimpleModule()
	 * @generated
	 */
	public EReference getSimpleModule_Extends() {
		return (EReference) simpleModuleEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.SimpleModule#getInterfaceName <em>Interface Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Interface Name</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule#getInterfaceName()
	 * @see #getSimpleModule()
	 * @generated
	 */
	public EReference getSimpleModule_InterfaceName() {
		return (EReference) simpleModuleEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.SimpleModule#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule#getParameters()
	 * @see #getSimpleModule()
	 * @generated
	 */
	public EReference getSimpleModule_Parameters() {
		return (EReference) simpleModuleEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.SimpleModule#getGates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Gates</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule#getGates()
	 * @see #getSimpleModule()
	 * @generated
	 */
	public EReference getSimpleModule_Gates() {
		return (EReference) simpleModuleEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.SimpleModule#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.SimpleModule#getName()
	 * @see #getSimpleModule()
	 * @generated
	 */
	public EAttribute getSimpleModule_Name() {
		return (EAttribute) simpleModuleEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Submodule <em>Submodule</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Submodule</em>'.
	 * @see org.omnetpp.ned2.model.Submodule
	 * @generated
	 */
	public EClass getSubmodule() {
		return submoduleEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Submodule#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getWhitespace()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EReference getSubmodule_Whitespace() {
		return (EReference) submoduleEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Submodule#getExpression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Expression</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getExpression()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EReference getSubmodule_Expression() {
		return (EReference) submoduleEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Submodule#getParameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Parameters</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getParameters()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EReference getSubmodule_Parameters() {
		return (EReference) submoduleEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference '{@link org.omnetpp.ned2.model.Submodule#getGates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Gates</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getGates()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EReference getSubmodule_Gates() {
		return (EReference) submoduleEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Submodule#getLikeParam <em>Like Param</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Like Param</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getLikeParam()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EAttribute getSubmodule_LikeParam() {
		return (EAttribute) submoduleEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Submodule#getLikeType <em>Like Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Like Type</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getLikeType()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EAttribute getSubmodule_LikeType() {
		return (EAttribute) submoduleEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Submodule#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getName()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EAttribute getSubmodule_Name() {
		return (EAttribute) submoduleEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Submodule#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getType()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EAttribute getSubmodule_Type() {
		return (EAttribute) submoduleEClass.getEStructuralFeatures().get(7);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Submodule#getVectorSize <em>Vector Size</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Vector Size</em>'.
	 * @see org.omnetpp.ned2.model.Submodule#getVectorSize()
	 * @see #getSubmodule()
	 * @generated
	 */
	public EAttribute getSubmodule_VectorSize() {
		return (EAttribute) submoduleEClass.getEStructuralFeatures().get(8);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Submodules <em>Submodules</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Submodules</em>'.
	 * @see org.omnetpp.ned2.model.Submodules
	 * @generated
	 */
	public EClass getSubmodules() {
		return submodulesEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Submodules#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Submodules#getWhitespace()
	 * @see #getSubmodules()
	 * @generated
	 */
	public EReference getSubmodules_Whitespace() {
		return (EReference) submodulesEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Submodules#getSubmodule <em>Submodule</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Submodule</em>'.
	 * @see org.omnetpp.ned2.model.Submodules#getSubmodule()
	 * @see #getSubmodules()
	 * @generated
	 */
	public EReference getSubmodules_Submodule() {
		return (EReference) submodulesEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Types <em>Types</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Types</em>'.
	 * @see org.omnetpp.ned2.model.Types
	 * @generated
	 */
	public EClass getTypes() {
		return typesEClass;
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Types#getWhitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Types#getWhitespace()
	 * @see #getTypes()
	 * @generated
	 */
	public EReference getTypes_Whitespace() {
		return (EReference) typesEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute list '{@link org.omnetpp.ned2.model.Types#getGroup <em>Group</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute list '<em>Group</em>'.
	 * @see org.omnetpp.ned2.model.Types#getGroup()
	 * @see #getTypes()
	 * @generated
	 */
	public EAttribute getTypes_Group() {
		return (EAttribute) typesEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Types#getChannel <em>Channel</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Channel</em>'.
	 * @see org.omnetpp.ned2.model.Types#getChannel()
	 * @see #getTypes()
	 * @generated
	 */
	public EReference getTypes_Channel() {
		return (EReference) typesEClass.getEStructuralFeatures().get(2);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Types#getChannelInterface <em>Channel Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Channel Interface</em>'.
	 * @see org.omnetpp.ned2.model.Types#getChannelInterface()
	 * @see #getTypes()
	 * @generated
	 */
	public EReference getTypes_ChannelInterface() {
		return (EReference) typesEClass.getEStructuralFeatures().get(3);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Types#getSimpleModule <em>Simple Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Simple Module</em>'.
	 * @see org.omnetpp.ned2.model.Types#getSimpleModule()
	 * @see #getTypes()
	 * @generated
	 */
	public EReference getTypes_SimpleModule() {
		return (EReference) typesEClass.getEStructuralFeatures().get(4);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Types#getCompoundModule <em>Compound Module</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Compound Module</em>'.
	 * @see org.omnetpp.ned2.model.Types#getCompoundModule()
	 * @see #getTypes()
	 * @generated
	 */
	public EReference getTypes_CompoundModule() {
		return (EReference) typesEClass.getEStructuralFeatures().get(5);
	}

	/**
	 * Returns the meta object for the containment reference list '{@link org.omnetpp.ned2.model.Types#getModuleInterface <em>Module Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Module Interface</em>'.
	 * @see org.omnetpp.ned2.model.Types#getModuleInterface()
	 * @see #getTypes()
	 * @generated
	 */
	public EReference getTypes_ModuleInterface() {
		return (EReference) typesEClass.getEStructuralFeatures().get(6);
	}

	/**
	 * Returns the meta object for class '{@link org.omnetpp.ned2.model.Whitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Whitespace</em>'.
	 * @see org.omnetpp.ned2.model.Whitespace
	 * @generated
	 */
	public EClass getWhitespace() {
		return whitespaceEClass;
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Whitespace#getContent <em>Content</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Content</em>'.
	 * @see org.omnetpp.ned2.model.Whitespace#getContent()
	 * @see #getWhitespace()
	 * @generated
	 */
	public EAttribute getWhitespace_Content() {
		return (EAttribute) whitespaceEClass.getEStructuralFeatures().get(0);
	}

	/**
	 * Returns the meta object for the attribute '{@link org.omnetpp.ned2.model.Whitespace#getLocid <em>Locid</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Locid</em>'.
	 * @see org.omnetpp.ned2.model.Whitespace#getLocid()
	 * @see #getWhitespace()
	 * @generated
	 */
	public EAttribute getWhitespace_Locid() {
		return (EAttribute) whitespaceEClass.getEStructuralFeatures().get(1);
	}

	/**
	 * Returns the factory that creates the instances of the model.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the factory that creates the instances of the model.
	 * @generated
	 */
	public Ned2Factory getNed2Factory() {
		return (Ned2Factory) getEFactoryInstance();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private boolean isCreated = false;

	/**
	 * Creates the meta-model objects for the package.  This method is
	 * guarded to have no affect on any invocation but its first.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void createPackageContents() {
		if (isCreated)
			return;
		isCreated = true;

		// Create classes and their features
		channelEClass = createEClass(CHANNEL);
		createEReference(channelEClass, CHANNEL__WHITESPACE);
		createEReference(channelEClass, CHANNEL__EXTENDS);
		createEReference(channelEClass, CHANNEL__INTERFACE_NAME);
		createEReference(channelEClass, CHANNEL__PARAMETERS);
		createEAttribute(channelEClass, CHANNEL__NAME);

		channelInterfaceEClass = createEClass(CHANNEL_INTERFACE);
		createEReference(channelInterfaceEClass, CHANNEL_INTERFACE__WHITESPACE);
		createEReference(channelInterfaceEClass, CHANNEL_INTERFACE__EXTENDS);
		createEReference(channelInterfaceEClass, CHANNEL_INTERFACE__PARAMETERS);
		createEAttribute(channelInterfaceEClass, CHANNEL_INTERFACE__NAME);

		compoundModuleEClass = createEClass(COMPOUND_MODULE);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__WHITESPACE);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__EXTENDS);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__INTERFACE_NAME);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__PARAMETERS);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__GATES);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__TYPES);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__SUBMODULES);
		createEReference(compoundModuleEClass, COMPOUND_MODULE__CONNECTIONS);
		createEAttribute(compoundModuleEClass, COMPOUND_MODULE__IS_NETWORK);
		createEAttribute(compoundModuleEClass, COMPOUND_MODULE__NAME);

		conditionEClass = createEClass(CONDITION);
		createEReference(conditionEClass, CONDITION__WHITESPACE);
		createEReference(conditionEClass, CONDITION__EXPRESSION);
		createEAttribute(conditionEClass, CONDITION__CONDITION);

		connectionEClass = createEClass(CONNECTION);
		createEReference(connectionEClass, CONNECTION__WHITESPACE);
		createEReference(connectionEClass, CONNECTION__EXPRESSION);
		createEReference(connectionEClass, CONNECTION__PARAMETERS);
		createEAttribute(connectionEClass, CONNECTION__ARROW_DIRECTION);
		createEAttribute(connectionEClass, CONNECTION__CHANNEL_NAME);
		createEAttribute(connectionEClass, CONNECTION__DEST_GATE);
		createEAttribute(connectionEClass, CONNECTION__DEST_GATE_INDEX);
		createEAttribute(connectionEClass, CONNECTION__DEST_GATE_PLUSPLUS);
		createEAttribute(connectionEClass, CONNECTION__DEST_GATE_SUBG);
		createEAttribute(connectionEClass, CONNECTION__DEST_MODULE);
		createEAttribute(connectionEClass, CONNECTION__DEST_MODULE_INDEX);
		createEAttribute(connectionEClass, CONNECTION__SRC_GATE);
		createEAttribute(connectionEClass, CONNECTION__SRC_GATE_INDEX);
		createEAttribute(connectionEClass, CONNECTION__SRC_GATE_PLUSPLUS);
		createEAttribute(connectionEClass, CONNECTION__SRC_GATE_SUBG);
		createEAttribute(connectionEClass, CONNECTION__SRC_MODULE);
		createEAttribute(connectionEClass, CONNECTION__SRC_MODULE_INDEX);

		connectionGroupEClass = createEClass(CONNECTION_GROUP);
		createEReference(connectionGroupEClass, CONNECTION_GROUP__WHITESPACE);
		createEAttribute(connectionGroupEClass, CONNECTION_GROUP__GROUP);
		createEReference(connectionGroupEClass, CONNECTION_GROUP__LOOP);
		createEReference(connectionGroupEClass, CONNECTION_GROUP__CONDITION);
		createEReference(connectionGroupEClass, CONNECTION_GROUP__CONNECTION);

		connectionsEClass = createEClass(CONNECTIONS);
		createEReference(connectionsEClass, CONNECTIONS__WHITESPACE);
		createEAttribute(connectionsEClass, CONNECTIONS__GROUP);
		createEReference(connectionsEClass, CONNECTIONS__CONNECTION);
		createEReference(connectionsEClass, CONNECTIONS__CONNECTION_GROUP);
		createEAttribute(connectionsEClass, CONNECTIONS__CHECK_UNCONNECTED);

		constEClass = createEClass(CONST);
		createEReference(constEClass, CONST__WHITESPACE);
		createEAttribute(constEClass, CONST__TEXT);
		createEAttribute(constEClass, CONST__TYPE);
		createEAttribute(constEClass, CONST__UNIT_TYPE);
		createEAttribute(constEClass, CONST__VALUE);

		expressionEClass = createEClass(EXPRESSION);
		createEReference(expressionEClass, EXPRESSION__WHITESPACE);
		createEReference(expressionEClass, EXPRESSION__OPERATOR);
		createEReference(expressionEClass, EXPRESSION__FUNCTION);
		createEReference(expressionEClass, EXPRESSION__REF);
		createEReference(expressionEClass, EXPRESSION__CONST);
		createEAttribute(expressionEClass, EXPRESSION__TARGET);

		extendsEClass = createEClass(EXTENDS);
		createEReference(extendsEClass, EXTENDS__WHITESPACE);
		createEAttribute(extendsEClass, EXTENDS__NAME);

		filesEClass = createEClass(FILES);
		createEReference(filesEClass, FILES__NED_FILE);

		functionEClass = createEClass(FUNCTION);
		createEReference(functionEClass, FUNCTION__WHITESPACE);
		createEAttribute(functionEClass, FUNCTION__GROUP);
		createEReference(functionEClass, FUNCTION__OPERATOR);
		createEReference(functionEClass, FUNCTION__FUNCTION);
		createEReference(functionEClass, FUNCTION__REF);
		createEReference(functionEClass, FUNCTION__CONST);
		createEAttribute(functionEClass, FUNCTION__NAME);

		gateEClass = createEClass(GATE);
		createEReference(gateEClass, GATE__WHITESPACE);
		createEReference(gateEClass, GATE__EXPRESSION);
		createEReference(gateEClass, GATE__PROPERTY);
		createEAttribute(gateEClass, GATE__IS_VECTOR);
		createEAttribute(gateEClass, GATE__NAME);
		createEAttribute(gateEClass, GATE__TYPE);
		createEAttribute(gateEClass, GATE__VECTOR_SIZE);

		gateGroupEClass = createEClass(GATE_GROUP);
		createEReference(gateGroupEClass, GATE_GROUP__WHITESPACE);
		createEReference(gateGroupEClass, GATE_GROUP__CONDITION);
		createEReference(gateGroupEClass, GATE_GROUP__GATE);

		gatesEClass = createEClass(GATES);
		createEReference(gatesEClass, GATES__WHITESPACE);
		createEAttribute(gatesEClass, GATES__GROUP);
		createEReference(gatesEClass, GATES__GATE);
		createEReference(gatesEClass, GATES__GATE_GROUP);

		importEClass = createEClass(IMPORT);
		createEReference(importEClass, IMPORT__WHITESPACE);
		createEAttribute(importEClass, IMPORT__FILENAME);

		interfaceNameEClass = createEClass(INTERFACE_NAME);
		createEReference(interfaceNameEClass, INTERFACE_NAME__WHITESPACE);
		createEAttribute(interfaceNameEClass, INTERFACE_NAME__NAME);

		keyValueEClass = createEClass(KEY_VALUE);
		createEReference(keyValueEClass, KEY_VALUE__WHITESPACE);
		createEAttribute(keyValueEClass, KEY_VALUE__KEY);
		createEAttribute(keyValueEClass, KEY_VALUE__VALUE);

		loopEClass = createEClass(LOOP);
		createEReference(loopEClass, LOOP__WHITESPACE);
		createEReference(loopEClass, LOOP__EXPRESSION);
		createEAttribute(loopEClass, LOOP__FROM_VALUE);
		createEAttribute(loopEClass, LOOP__PARAM_NAME);
		createEAttribute(loopEClass, LOOP__TO_VALUE);

		moduleInterfaceEClass = createEClass(MODULE_INTERFACE);
		createEReference(moduleInterfaceEClass, MODULE_INTERFACE__WHITESPACE);
		createEReference(moduleInterfaceEClass, MODULE_INTERFACE__EXTENDS);
		createEReference(moduleInterfaceEClass, MODULE_INTERFACE__PARAMETERS);
		createEReference(moduleInterfaceEClass, MODULE_INTERFACE__GATES);
		createEAttribute(moduleInterfaceEClass, MODULE_INTERFACE__NAME);

		nedFileEClass = createEClass(NED_FILE);
		createEReference(nedFileEClass, NED_FILE__WHITESPACE);
		createEAttribute(nedFileEClass, NED_FILE__GROUP);
		createEReference(nedFileEClass, NED_FILE__IMPORT);
		createEReference(nedFileEClass, NED_FILE__PROPERTYDEF);
		createEReference(nedFileEClass, NED_FILE__PROPERTY);
		createEReference(nedFileEClass, NED_FILE__CHANNEL);
		createEReference(nedFileEClass, NED_FILE__CHANNEL_INTERFACE);
		createEReference(nedFileEClass, NED_FILE__SIMPLE_MODULE);
		createEReference(nedFileEClass, NED_FILE__COMPOUND_MODULE);
		createEReference(nedFileEClass, NED_FILE__MODULE_INTERFACE);
		createEAttribute(nedFileEClass, NED_FILE__FILENAME);
		createEAttribute(nedFileEClass, NED_FILE__PACKAGE);

		operatorEClass = createEClass(OPERATOR);
		createEReference(operatorEClass, OPERATOR__WHITESPACE);
		createEAttribute(operatorEClass, OPERATOR__GROUP);
		createEReference(operatorEClass, OPERATOR__OPERATOR);
		createEReference(operatorEClass, OPERATOR__FUNCTION);
		createEReference(operatorEClass, OPERATOR__REF);
		createEReference(operatorEClass, OPERATOR__CONST);
		createEAttribute(operatorEClass, OPERATOR__NAME);

		paramEClass = createEClass(PARAM);
		createEReference(paramEClass, PARAM__WHITESPACE);
		createEReference(paramEClass, PARAM__EXPRESSION);
		createEReference(paramEClass, PARAM__PROPERTY);
		createEAttribute(paramEClass, PARAM__IS_FUNCTION);
		createEAttribute(paramEClass, PARAM__NAME);
		createEAttribute(paramEClass, PARAM__QUALIFIER);
		createEAttribute(paramEClass, PARAM__TYPE);
		createEAttribute(paramEClass, PARAM__VALUE);

		parametersEClass = createEClass(PARAMETERS);
		createEReference(parametersEClass, PARAMETERS__WHITESPACE);
		createEAttribute(parametersEClass, PARAMETERS__GROUP);
		createEReference(parametersEClass, PARAMETERS__PROPERTY);
		createEReference(parametersEClass, PARAMETERS__PARAM);
		createEReference(parametersEClass, PARAMETERS__PARAM_GROUP);
		createEAttribute(parametersEClass, PARAMETERS__IS_IMPLICIT);

		paramGroupEClass = createEClass(PARAM_GROUP);
		createEReference(paramGroupEClass, PARAM_GROUP__WHITESPACE);
		createEReference(paramGroupEClass, PARAM_GROUP__CONDITION);
		createEAttribute(paramGroupEClass, PARAM_GROUP__GROUP);
		createEReference(paramGroupEClass, PARAM_GROUP__PROPERTY);
		createEReference(paramGroupEClass, PARAM_GROUP__PARAM);

		propertyEClass = createEClass(PROPERTY);
		createEReference(propertyEClass, PROPERTY__WHITESPACE);
		createEReference(propertyEClass, PROPERTY__KEY_VALUE);
		createEReference(propertyEClass, PROPERTY__PROPERTY);
		createEAttribute(propertyEClass, PROPERTY__NAME);
		createEAttribute(propertyEClass, PROPERTY__QUALIFIER);

		propertydefEClass = createEClass(PROPERTYDEF);
		createEReference(propertydefEClass, PROPERTYDEF__WHITESPACE);
		createEReference(propertydefEClass, PROPERTYDEF__KEY_VALUE);
		createEReference(propertydefEClass, PROPERTYDEF__PROPERTY);
		createEAttribute(propertydefEClass, PROPERTYDEF__NAME);

		refEClass = createEClass(REF);
		createEReference(refEClass, REF__WHITESPACE);
		createEReference(refEClass, REF__EXPRESSION);
		createEAttribute(refEClass, REF__MODULE);
		createEAttribute(refEClass, REF__MODULE_INDEX);
		createEAttribute(refEClass, REF__NAME);

		simpleModuleEClass = createEClass(SIMPLE_MODULE);
		createEReference(simpleModuleEClass, SIMPLE_MODULE__WHITESPACE);
		createEReference(simpleModuleEClass, SIMPLE_MODULE__EXTENDS);
		createEReference(simpleModuleEClass, SIMPLE_MODULE__INTERFACE_NAME);
		createEReference(simpleModuleEClass, SIMPLE_MODULE__PARAMETERS);
		createEReference(simpleModuleEClass, SIMPLE_MODULE__GATES);
		createEAttribute(simpleModuleEClass, SIMPLE_MODULE__NAME);

		submoduleEClass = createEClass(SUBMODULE);
		createEReference(submoduleEClass, SUBMODULE__WHITESPACE);
		createEReference(submoduleEClass, SUBMODULE__EXPRESSION);
		createEReference(submoduleEClass, SUBMODULE__PARAMETERS);
		createEReference(submoduleEClass, SUBMODULE__GATES);
		createEAttribute(submoduleEClass, SUBMODULE__LIKE_PARAM);
		createEAttribute(submoduleEClass, SUBMODULE__LIKE_TYPE);
		createEAttribute(submoduleEClass, SUBMODULE__NAME);
		createEAttribute(submoduleEClass, SUBMODULE__TYPE);
		createEAttribute(submoduleEClass, SUBMODULE__VECTOR_SIZE);

		submodulesEClass = createEClass(SUBMODULES);
		createEReference(submodulesEClass, SUBMODULES__WHITESPACE);
		createEReference(submodulesEClass, SUBMODULES__SUBMODULE);

		typesEClass = createEClass(TYPES);
		createEReference(typesEClass, TYPES__WHITESPACE);
		createEAttribute(typesEClass, TYPES__GROUP);
		createEReference(typesEClass, TYPES__CHANNEL);
		createEReference(typesEClass, TYPES__CHANNEL_INTERFACE);
		createEReference(typesEClass, TYPES__SIMPLE_MODULE);
		createEReference(typesEClass, TYPES__COMPOUND_MODULE);
		createEReference(typesEClass, TYPES__MODULE_INTERFACE);

		whitespaceEClass = createEClass(WHITESPACE);
		createEAttribute(whitespaceEClass, WHITESPACE__CONTENT);
		createEAttribute(whitespaceEClass, WHITESPACE__LOCID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private boolean isInitialized = false;

	/**
	 * Complete the initialization of the package and its meta-model.  This
	 * method is guarded to have no affect on any invocation but its first.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void initializePackageContents() {
		if (isInitialized)
			return;
		isInitialized = true;

		// Initialize package
		setName(eNAME);
		setNsPrefix(eNS_PREFIX);
		setNsURI(eNS_URI);

		// Obtain other dependent packages
		XMLTypePackage theXMLTypePackage = (XMLTypePackage) EPackage.Registry.INSTANCE
				.getEPackage(XMLTypePackage.eNS_URI);

		// Add supertypes to classes

		// Initialize classes and features; add operations and parameters
		initEClass(channelEClass, Channel.class, "Channel", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getChannel_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Channel.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getChannel_Extends(), this.getExtends(), null,
				"extends", null, 0, 1, Channel.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getChannel_InterfaceName(), this.getInterfaceName(),
				null, "interfaceName", null, 0, -1, Channel.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getChannel_Parameters(), this.getParameters(), null,
				"parameters", null, 0, 1, Channel.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getChannel_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(channelInterfaceEClass, ChannelInterface.class,
				"ChannelInterface", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getChannelInterface_Whitespace(), this.getWhitespace(),
				null, "whitespace", null, 0, -1, ChannelInterface.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getChannelInterface_Extends(), this.getExtends(), null,
				"extends", null, 0, -1, ChannelInterface.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getChannelInterface_Parameters(), this.getParameters(),
				null, "parameters", null, 0, 1, ChannelInterface.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getChannelInterface_Name(), theXMLTypePackage
				.getNMTOKEN(), "name", null, 1, 1, ChannelInterface.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(compoundModuleEClass, CompoundModule.class,
				"CompoundModule", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getCompoundModule_Whitespace(), this.getWhitespace(),
				null, "whitespace", null, 0, -1, CompoundModule.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getCompoundModule_Extends(), this.getExtends(), null,
				"extends", null, 0, 1, CompoundModule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getCompoundModule_InterfaceName(), this
				.getInterfaceName(), null, "interfaceName", null, 0, -1,
				CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getCompoundModule_Parameters(), this.getParameters(),
				null, "parameters", null, 0, 1, CompoundModule.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getCompoundModule_Gates(), this.getGates(), null,
				"gates", null, 0, 1, CompoundModule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getCompoundModule_Types(), this.getTypes(), null,
				"types", null, 0, 1, CompoundModule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getCompoundModule_Submodules(), this.getSubmodules(),
				null, "submodules", null, 0, 1, CompoundModule.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getCompoundModule_Connections(), this.getConnections(),
				null, "connections", null, 0, 1, CompoundModule.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getCompoundModule_IsNetwork(), theXMLTypePackage
				.getBoolean(), "isNetwork", "false", 0, 1,
				CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getCompoundModule_Name(),
				theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1,
				CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(conditionEClass, Condition.class, "Condition", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getCondition_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Condition.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getCondition_Expression(), this.getExpression(), null,
				"expression", null, 0, 1, Condition.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getCondition_Condition(), theXMLTypePackage.getString(),
				"condition", null, 0, 1, Condition.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(connectionEClass, Connection.class, "Connection",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getConnection_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Connection.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getConnection_Expression(), this.getExpression(), null,
				"expression", null, 0, -1, Connection.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getConnection_Parameters(), this.getParameters(), null,
				"parameters", null, 0, 1, Connection.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_ArrowDirection(), theXMLTypePackage
				.getString(), "arrowDirection", null, 1, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_ChannelName(), theXMLTypePackage
				.getNMTOKEN(), "channelName", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_DestGate(),
				theXMLTypePackage.getNMTOKEN(), "destGate", null, 1, 1,
				Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				!IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_DestGateIndex(), theXMLTypePackage
				.getString(), "destGateIndex", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_DestGatePlusplus(), theXMLTypePackage
				.getBoolean(), "destGatePlusplus", "false", 0, 1,
				Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_DestGateSubg(), theXMLTypePackage
				.getString(), "destGateSubg", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_DestModule(), theXMLTypePackage
				.getNMTOKEN(), "destModule", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_DestModuleIndex(), theXMLTypePackage
				.getString(), "destModuleIndex", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_SrcGate(), theXMLTypePackage.getNMTOKEN(),
				"srcGate", null, 1, 1, Connection.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_SrcGateIndex(), theXMLTypePackage
				.getString(), "srcGateIndex", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_SrcGatePlusplus(), theXMLTypePackage
				.getBoolean(), "srcGatePlusplus", "false", 0, 1,
				Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_SrcGateSubg(), theXMLTypePackage
				.getString(), "srcGateSubg", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_SrcModule(), theXMLTypePackage
				.getNMTOKEN(), "srcModule", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnection_SrcModuleIndex(), theXMLTypePackage
				.getString(), "srcModuleIndex", null, 0, 1, Connection.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(connectionGroupEClass, ConnectionGroup.class,
				"ConnectionGroup", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getConnectionGroup_Whitespace(), this.getWhitespace(),
				null, "whitespace", null, 0, -1, ConnectionGroup.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getConnectionGroup_Group(), ecorePackage
				.getEFeatureMapEntry(), "group", null, 0, -1,
				ConnectionGroup.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getConnectionGroup_Loop(), this.getLoop(), null, "loop",
				null, 0, -1, ConnectionGroup.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getConnectionGroup_Condition(), this.getCondition(),
				null, "condition", null, 0, -1, ConnectionGroup.class,
				IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED,
				IS_ORDERED);
		initEReference(getConnectionGroup_Connection(), this.getConnection(),
				null, "connection", null, 0, -1, ConnectionGroup.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(connectionsEClass, Connections.class, "Connections",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getConnections_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Connections.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnections_Group(), ecorePackage
				.getEFeatureMapEntry(), "group", null, 0, -1,
				Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				!IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getConnections_Connection(), this.getConnection(), null,
				"connection", null, 0, -1, Connections.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getConnections_ConnectionGroup(), this
				.getConnectionGroup(), null, "connectionGroup", null, 0, -1,
				Connections.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE,
				IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE,
				IS_DERIVED, IS_ORDERED);
		initEAttribute(getConnections_CheckUnconnected(), theXMLTypePackage
				.getBoolean(), "checkUnconnected", "true", 0, 1,
				Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(constEClass, Const.class, "Const", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getConst_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Const.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConst_Text(), theXMLTypePackage.getString(), "text",
				null, 0, 1, Const.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getConst_Type(), theXMLTypePackage.getString(), "type",
				null, 1, 1, Const.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getConst_UnitType(), theXMLTypePackage.getString(),
				"unitType", null, 0, 1, Const.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getConst_Value(), theXMLTypePackage.getString(),
				"value", null, 0, 1, Const.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(expressionEClass, Expression.class, "Expression",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getExpression_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Expression.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getExpression_Operator(), this.getOperator(), null,
				"operator", null, 0, 1, Expression.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getExpression_Function(), this.getFunction(), null,
				"function", null, 0, 1, Expression.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getExpression_Ref(), this.getRef(), null, "ref", null,
				0, 1, Expression.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getExpression_Const(), this.getConst(), null, "const",
				null, 0, 1, Expression.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getExpression_Target(), theXMLTypePackage.getString(),
				"target", null, 0, 1, Expression.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(extendsEClass, Extends.class, "Extends", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getExtends_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Extends.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getExtends_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, Extends.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(filesEClass, Files.class, "Files", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getFiles_NedFile(), this.getNedFile(), null, "nedFile",
				null, 0, -1, Files.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(functionEClass, Function.class, "Function", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getFunction_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Function.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getFunction_Group(), ecorePackage.getEFeatureMapEntry(),
				"group", null, 0, -1, Function.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getFunction_Operator(), this.getOperator(), null,
				"operator", null, 0, -1, Function.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getFunction_Function(), this.getFunction(), null,
				"function", null, 0, -1, Function.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getFunction_Ref(), this.getRef(), null, "ref", null, 0,
				-1, Function.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE,
				IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE,
				IS_DERIVED, IS_ORDERED);
		initEReference(getFunction_Const(), this.getConst(), null, "const",
				null, 0, -1, Function.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEAttribute(getFunction_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, Function.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(gateEClass, Gate.class, "Gate", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getGate_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Gate.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getGate_Expression(), this.getExpression(), null,
				"expression", null, 0, 1, Gate.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getGate_Property(), this.getProperty(), null,
				"property", null, 0, -1, Gate.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getGate_IsVector(), theXMLTypePackage.getBoolean(),
				"isVector", "false", 0, 1, Gate.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE,
				!IS_DERIVED, IS_ORDERED);
		initEAttribute(getGate_Name(), theXMLTypePackage.getNMTOKEN(), "name",
				null, 1, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getGate_Type(), theXMLTypePackage.getString(), "type",
				null, 1, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getGate_VectorSize(), theXMLTypePackage.getString(),
				"vectorSize", null, 0, 1, Gate.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(gateGroupEClass, GateGroup.class, "GateGroup", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getGateGroup_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, GateGroup.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getGateGroup_Condition(), this.getCondition(), null,
				"condition", null, 1, 1, GateGroup.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getGateGroup_Gate(), this.getGate(), null, "gate", null,
				0, -1, GateGroup.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(gatesEClass, Gates.class, "Gates", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getGates_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Gates.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getGates_Group(), ecorePackage.getEFeatureMapEntry(),
				"group", null, 0, -1, Gates.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getGates_Gate(), this.getGate(), null, "gate", null, 0,
				-1, Gates.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE,
				IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE,
				IS_DERIVED, IS_ORDERED);
		initEReference(getGates_GateGroup(), this.getGateGroup(), null,
				"gateGroup", null, 0, -1, Gates.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);

		initEClass(importEClass, Import.class, "Import", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getImport_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Import.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getImport_Filename(), theXMLTypePackage.getString(),
				"filename", null, 1, 1, Import.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(interfaceNameEClass, InterfaceName.class, "InterfaceName",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getInterfaceName_Whitespace(), this.getWhitespace(),
				null, "whitespace", null, 0, -1, InterfaceName.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getInterfaceName_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, InterfaceName.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(keyValueEClass, KeyValue.class, "KeyValue", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getKeyValue_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, KeyValue.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getKeyValue_Key(), theXMLTypePackage.getString(), "key",
				null, 0, 1, KeyValue.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getKeyValue_Value(), theXMLTypePackage.getString(),
				"value", null, 0, 1, KeyValue.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(loopEClass, Loop.class, "Loop", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getLoop_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Loop.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getLoop_Expression(), this.getExpression(), null,
				"expression", null, 0, -1, Loop.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getLoop_FromValue(), theXMLTypePackage.getString(),
				"fromValue", null, 0, 1, Loop.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getLoop_ParamName(), theXMLTypePackage.getNMTOKEN(),
				"paramName", null, 1, 1, Loop.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getLoop_ToValue(), theXMLTypePackage.getString(),
				"toValue", null, 0, 1, Loop.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(moduleInterfaceEClass, ModuleInterface.class,
				"ModuleInterface", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getModuleInterface_Whitespace(), this.getWhitespace(),
				null, "whitespace", null, 0, -1, ModuleInterface.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getModuleInterface_Extends(), this.getExtends(), null,
				"extends", null, 0, -1, ModuleInterface.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getModuleInterface_Parameters(), this.getParameters(),
				null, "parameters", null, 0, 1, ModuleInterface.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getModuleInterface_Gates(), this.getGates(), null,
				"gates", null, 0, 1, ModuleInterface.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getModuleInterface_Name(), theXMLTypePackage
				.getNMTOKEN(), "name", null, 1, 1, ModuleInterface.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(nedFileEClass, NedFile.class, "NedFile", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getNedFile_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, NedFile.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getNedFile_Group(), ecorePackage.getEFeatureMapEntry(),
				"group", null, 0, -1, NedFile.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_Import(), this.getImport(), null, "import",
				null, 0, -1, NedFile.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_Propertydef(), this.getPropertydef(), null,
				"propertydef", null, 0, -1, NedFile.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_Property(), this.getProperty(), null,
				"property", null, 0, -1, NedFile.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_Channel(), this.getChannel(), null,
				"channel", null, 0, -1, NedFile.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_ChannelInterface(), this
				.getChannelInterface(), null, "channelInterface", null, 0, -1,
				NedFile.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE,
				IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE,
				IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_SimpleModule(), this.getSimpleModule(), null,
				"simpleModule", null, 0, -1, NedFile.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getNedFile_CompoundModule(), this.getCompoundModule(),
				null, "compoundModule", null, 0, -1, NedFile.class,
				IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED,
				IS_ORDERED);
		initEReference(getNedFile_ModuleInterface(), this.getModuleInterface(),
				null, "moduleInterface", null, 0, -1, NedFile.class,
				IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getNedFile_Filename(), theXMLTypePackage.getString(),
				"filename", null, 1, 1, NedFile.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getNedFile_Package(), theXMLTypePackage.getString(),
				"package", null, 0, 1, NedFile.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(operatorEClass, Operator.class, "Operator", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getOperator_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Operator.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getOperator_Group(), ecorePackage.getEFeatureMapEntry(),
				"group", null, 0, -1, Operator.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getOperator_Operator(), this.getOperator(), null,
				"operator", null, 0, -1, Operator.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getOperator_Function(), this.getFunction(), null,
				"function", null, 0, -1, Operator.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getOperator_Ref(), this.getRef(), null, "ref", null, 0,
				-1, Operator.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE,
				IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE,
				IS_DERIVED, IS_ORDERED);
		initEReference(getOperator_Const(), this.getConst(), null, "const",
				null, 0, -1, Operator.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEAttribute(getOperator_Name(), theXMLTypePackage.getString(),
				"name", null, 1, 1, Operator.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(paramEClass, Param.class, "Param", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getParam_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Param.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getParam_Expression(), this.getExpression(), null,
				"expression", null, 0, 1, Param.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getParam_Property(), this.getProperty(), null,
				"property", null, 0, -1, Param.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getParam_IsFunction(), theXMLTypePackage.getBoolean(),
				"isFunction", "false", 0, 1, Param.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE,
				!IS_DERIVED, IS_ORDERED);
		initEAttribute(getParam_Name(), theXMLTypePackage.getNMTOKEN(), "name",
				null, 1, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getParam_Qualifier(), theXMLTypePackage.getString(),
				"qualifier", null, 0, 1, Param.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getParam_Type(), theXMLTypePackage.getString(), "type",
				null, 1, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getParam_Value(), theXMLTypePackage.getString(),
				"value", null, 0, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(parametersEClass, Parameters.class, "Parameters",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getParameters_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Parameters.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getParameters_Group(), ecorePackage
				.getEFeatureMapEntry(), "group", null, 0, -1, Parameters.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getParameters_Property(), this.getProperty(), null,
				"property", null, 0, -1, Parameters.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getParameters_Param(), this.getParam(), null, "param",
				null, 0, -1, Parameters.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getParameters_ParamGroup(), this.getParamGroup(), null,
				"paramGroup", null, 0, -1, Parameters.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEAttribute(getParameters_IsImplicit(), theXMLTypePackage
				.getBoolean(), "isImplicit", "true", 0, 1, Parameters.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(paramGroupEClass, ParamGroup.class, "ParamGroup",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getParamGroup_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, ParamGroup.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getParamGroup_Condition(), this.getCondition(), null,
				"condition", null, 1, 1, ParamGroup.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getParamGroup_Group(), ecorePackage
				.getEFeatureMapEntry(), "group", null, 0, -1, ParamGroup.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE,
				!IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getParamGroup_Property(), this.getProperty(), null,
				"property", null, 0, -1, ParamGroup.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getParamGroup_Param(), this.getParam(), null, "param",
				null, 0, -1, ParamGroup.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);

		initEClass(propertyEClass, Property.class, "Property", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getProperty_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Property.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getProperty_KeyValue(), this.getKeyValue(), null,
				"keyValue", null, 0, -1, Property.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getProperty_Property(), this.getProperty(), null,
				"property", null, 0, -1, Property.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getProperty_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, Property.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getProperty_Qualifier(), theXMLTypePackage.getString(),
				"qualifier", null, 0, 1, Property.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(propertydefEClass, Propertydef.class, "Propertydef",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getPropertydef_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Propertydef.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getPropertydef_KeyValue(), this.getKeyValue(), null,
				"keyValue", null, 0, -1, Propertydef.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getPropertydef_Property(), this.getProperty(), null,
				"property", null, 0, -1, Propertydef.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getPropertydef_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, Propertydef.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(refEClass, Ref.class, "Ref", !IS_ABSTRACT, !IS_INTERFACE,
				IS_GENERATED_INSTANCE_CLASS);
		initEReference(getRef_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Ref.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getRef_Expression(), this.getExpression(), null,
				"expression", null, 0, -1, Ref.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getRef_Module(), theXMLTypePackage.getString(),
				"module", null, 0, 1, Ref.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEAttribute(getRef_ModuleIndex(), theXMLTypePackage.getString(),
				"moduleIndex", null, 0, 1, Ref.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getRef_Name(), theXMLTypePackage.getNMTOKEN(), "name",
				null, 1, 1, Ref.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);

		initEClass(simpleModuleEClass, SimpleModule.class, "SimpleModule",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getSimpleModule_Whitespace(), this.getWhitespace(),
				null, "whitespace", null, 0, -1, SimpleModule.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getSimpleModule_Extends(), this.getExtends(), null,
				"extends", null, 0, 1, SimpleModule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getSimpleModule_InterfaceName(),
				this.getInterfaceName(), null, "interfaceName", null, 0, -1,
				SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE,
				!IS_DERIVED, IS_ORDERED);
		initEReference(getSimpleModule_Parameters(), this.getParameters(),
				null, "parameters", null, 0, 1, SimpleModule.class,
				!IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getSimpleModule_Gates(), this.getGates(), null, "gates",
				null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getSimpleModule_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, SimpleModule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(submoduleEClass, Submodule.class, "Submodule", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getSubmodule_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getSubmodule_Expression(), this.getExpression(), null,
				"expression", null, 0, -1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getSubmodule_Parameters(), this.getParameters(), null,
				"parameters", null, 0, 1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getSubmodule_Gates(), this.getGates(), null, "gates",
				null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getSubmodule_LikeParam(), theXMLTypePackage.getString(),
				"likeParam", null, 0, 1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getSubmodule_LikeType(), theXMLTypePackage.getNMTOKEN(),
				"likeType", null, 0, 1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getSubmodule_Name(), theXMLTypePackage.getNMTOKEN(),
				"name", null, 1, 1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getSubmodule_Type(), theXMLTypePackage.getNMTOKEN(),
				"type", null, 0, 1, Submodule.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getSubmodule_VectorSize(),
				theXMLTypePackage.getString(), "vectorSize", null, 0, 1,
				Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE,
				!IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(submodulesEClass, Submodules.class, "Submodules",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getSubmodules_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Submodules.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEReference(getSubmodules_Submodule(), this.getSubmodule(), null,
				"submodule", null, 0, -1, Submodules.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		initEClass(typesEClass, Types.class, "Types", !IS_ABSTRACT,
				!IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEReference(getTypes_Whitespace(), this.getWhitespace(), null,
				"whitespace", null, 0, -1, Types.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getTypes_Group(), ecorePackage.getEFeatureMapEntry(),
				"group", null, 0, -1, Types.class, !IS_TRANSIENT, !IS_VOLATILE,
				IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED,
				IS_ORDERED);
		initEReference(getTypes_Channel(), this.getChannel(), null, "channel",
				null, 0, -1, Types.class, IS_TRANSIENT, IS_VOLATILE,
				IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getTypes_ChannelInterface(), this.getChannelInterface(),
				null, "channelInterface", null, 0, -1, Types.class,
				IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED,
				IS_ORDERED);
		initEReference(getTypes_SimpleModule(), this.getSimpleModule(), null,
				"simpleModule", null, 0, -1, Types.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getTypes_CompoundModule(), this.getCompoundModule(),
				null, "compoundModule", null, 0, -1, Types.class, IS_TRANSIENT,
				IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES,
				!IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
		initEReference(getTypes_ModuleInterface(), this.getModuleInterface(),
				null, "moduleInterface", null, 0, -1, Types.class,
				IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE,
				!IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED,
				IS_ORDERED);

		initEClass(whitespaceEClass, Whitespace.class, "Whitespace",
				!IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
		initEAttribute(getWhitespace_Content(), theXMLTypePackage.getString(),
				"content", null, 1, 1, Whitespace.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
		initEAttribute(getWhitespace_Locid(), theXMLTypePackage.getNMTOKEN(),
				"locid", null, 1, 1, Whitespace.class, !IS_TRANSIENT,
				!IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID,
				!IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

		// Create resource
		createResource(eNS_URI);

		// Create annotations
		// http:///org/eclipse/emf/ecore/util/ExtendedMetaData
		createExtendedMetaDataAnnotations();
	}

	/**
	 * Initializes the annotations for <b>http:///org/eclipse/emf/ecore/util/ExtendedMetaData</b>.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void createExtendedMetaDataAnnotations() {
		String source = "http:///org/eclipse/emf/ecore/util/ExtendedMetaData";
		addAnnotation(channelEClass, source, new String[] { "name", "channel",
				"kind", "elementOnly" });
		addAnnotation(getChannel_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getChannel_Extends(), source, new String[] { "kind",
				"element", "name", "extends" });
		addAnnotation(getChannel_InterfaceName(), source, new String[] {
				"kind", "element", "name", "interface-name" });
		addAnnotation(getChannel_Parameters(), source, new String[] { "kind",
				"element", "name", "parameters" });
		addAnnotation(getChannel_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(channelInterfaceEClass, source, new String[] { "name",
				"channel-interface", "kind", "elementOnly" });
		addAnnotation(getChannelInterface_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getChannelInterface_Extends(), source, new String[] {
				"kind", "element", "name", "extends" });
		addAnnotation(getChannelInterface_Parameters(), source, new String[] {
				"kind", "element", "name", "parameters" });
		addAnnotation(getChannelInterface_Name(), source, new String[] {
				"kind", "attribute", "name", "name" });
		addAnnotation(compoundModuleEClass, source, new String[] { "name",
				"compound-module", "kind", "elementOnly" });
		addAnnotation(getCompoundModule_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getCompoundModule_Extends(), source, new String[] {
				"kind", "element", "name", "extends" });
		addAnnotation(getCompoundModule_InterfaceName(), source, new String[] {
				"kind", "element", "name", "interface-name" });
		addAnnotation(getCompoundModule_Parameters(), source, new String[] {
				"kind", "element", "name", "parameters" });
		addAnnotation(getCompoundModule_Gates(), source, new String[] { "kind",
				"element", "name", "gates" });
		addAnnotation(getCompoundModule_Types(), source, new String[] { "kind",
				"element", "name", "types" });
		addAnnotation(getCompoundModule_Submodules(), source, new String[] {
				"kind", "element", "name", "submodules" });
		addAnnotation(getCompoundModule_Connections(), source, new String[] {
				"kind", "element", "name", "connections" });
		addAnnotation(getCompoundModule_IsNetwork(), source, new String[] {
				"kind", "attribute", "name", "is-network" });
		addAnnotation(getCompoundModule_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(conditionEClass, source, new String[] { "name",
				"condition", "kind", "elementOnly" });
		addAnnotation(getCondition_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getCondition_Expression(), source, new String[] { "kind",
				"element", "name", "expression" });
		addAnnotation(getCondition_Condition(), source, new String[] { "kind",
				"attribute", "name", "condition" });
		addAnnotation(connectionEClass, source, new String[] { "name",
				"connection", "kind", "elementOnly" });
		addAnnotation(getConnection_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getConnection_Expression(), source, new String[] {
				"kind", "element", "name", "expression" });
		addAnnotation(getConnection_Parameters(), source, new String[] {
				"kind", "element", "name", "parameters" });
		addAnnotation(getConnection_ArrowDirection(), source, new String[] {
				"kind", "attribute", "name", "arrow-direction" });
		addAnnotation(getConnection_ChannelName(), source, new String[] {
				"kind", "attribute", "name", "channel-name" });
		addAnnotation(getConnection_DestGate(), source, new String[] { "kind",
				"attribute", "name", "dest-gate" });
		addAnnotation(getConnection_DestGateIndex(), source, new String[] {
				"kind", "attribute", "name", "dest-gate-index" });
		addAnnotation(getConnection_DestGatePlusplus(), source, new String[] {
				"kind", "attribute", "name", "dest-gate-plusplus" });
		addAnnotation(getConnection_DestGateSubg(), source, new String[] {
				"kind", "attribute", "name", "dest-gate-subg" });
		addAnnotation(getConnection_DestModule(), source, new String[] {
				"kind", "attribute", "name", "dest-module" });
		addAnnotation(getConnection_DestModuleIndex(), source, new String[] {
				"kind", "attribute", "name", "dest-module-index" });
		addAnnotation(getConnection_SrcGate(), source, new String[] { "kind",
				"attribute", "name", "src-gate" });
		addAnnotation(getConnection_SrcGateIndex(), source, new String[] {
				"kind", "attribute", "name", "src-gate-index" });
		addAnnotation(getConnection_SrcGatePlusplus(), source, new String[] {
				"kind", "attribute", "name", "src-gate-plusplus" });
		addAnnotation(getConnection_SrcGateSubg(), source, new String[] {
				"kind", "attribute", "name", "src-gate-subg" });
		addAnnotation(getConnection_SrcModule(), source, new String[] { "kind",
				"attribute", "name", "src-module" });
		addAnnotation(getConnection_SrcModuleIndex(), source, new String[] {
				"kind", "attribute", "name", "src-module-index" });
		addAnnotation(connectionGroupEClass, source, new String[] { "name",
				"connection-group", "kind", "elementOnly" });
		addAnnotation(getConnectionGroup_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getConnectionGroup_Group(), source, new String[] {
				"kind", "group", "name", "group:1" });
		addAnnotation(getConnectionGroup_Loop(), source, new String[] { "kind",
				"element", "name", "loop", "group", "#group:1" });
		addAnnotation(getConnectionGroup_Condition(), source, new String[] {
				"kind", "element", "name", "condition", "group", "#group:1" });
		addAnnotation(getConnectionGroup_Connection(), source, new String[] {
				"kind", "element", "name", "connection" });
		addAnnotation(connectionsEClass, source, new String[] { "name",
				"connections", "kind", "elementOnly" });
		addAnnotation(getConnections_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getConnections_Group(), source, new String[] { "kind",
				"group", "name", "group:1" });
		addAnnotation(getConnections_Connection(), source, new String[] {
				"kind", "element", "name", "connection", "group", "#group:1" });
		addAnnotation(getConnections_ConnectionGroup(), source, new String[] {
				"kind", "element", "name", "connection-group", "group",
				"#group:1" });
		addAnnotation(getConnections_CheckUnconnected(), source, new String[] {
				"kind", "attribute", "name", "check-unconnected" });
		addAnnotation(constEClass, source, new String[] { "name", "const",
				"kind", "elementOnly" });
		addAnnotation(getConst_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getConst_Text(), source, new String[] { "kind",
				"attribute", "name", "text" });
		addAnnotation(getConst_Type(), source, new String[] { "kind",
				"attribute", "name", "type" });
		addAnnotation(getConst_UnitType(), source, new String[] { "kind",
				"attribute", "name", "unit-type" });
		addAnnotation(getConst_Value(), source, new String[] { "kind",
				"attribute", "name", "value" });
		addAnnotation(expressionEClass, source, new String[] { "name",
				"expression", "kind", "elementOnly" });
		addAnnotation(getExpression_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getExpression_Operator(), source, new String[] { "kind",
				"element", "name", "operator" });
		addAnnotation(getExpression_Function(), source, new String[] { "kind",
				"element", "name", "function" });
		addAnnotation(getExpression_Ref(), source, new String[] { "kind",
				"element", "name", "ref" });
		addAnnotation(getExpression_Const(), source, new String[] { "kind",
				"element", "name", "const" });
		addAnnotation(getExpression_Target(), source, new String[] { "kind",
				"attribute", "name", "target" });
		addAnnotation(extendsEClass, source, new String[] { "name", "extends",
				"kind", "elementOnly" });
		addAnnotation(getExtends_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getExtends_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(filesEClass, source, new String[] { "name", "files",
				"kind", "elementOnly" });
		addAnnotation(getFiles_NedFile(), source, new String[] { "kind",
				"element", "name", "ned-file" });
		addAnnotation(functionEClass, source, new String[] { "name",
				"function", "kind", "elementOnly" });
		addAnnotation(getFunction_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getFunction_Group(), source, new String[] { "kind",
				"group", "name", "group:1" });
		addAnnotation(getFunction_Operator(), source, new String[] { "kind",
				"element", "name", "operator", "group", "#group:1" });
		addAnnotation(getFunction_Function(), source, new String[] { "kind",
				"element", "name", "function", "group", "#group:1" });
		addAnnotation(getFunction_Ref(), source, new String[] { "kind",
				"element", "name", "ref", "group", "#group:1" });
		addAnnotation(getFunction_Const(), source, new String[] { "kind",
				"element", "name", "const", "group", "#group:1" });
		addAnnotation(getFunction_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(gateEClass, source, new String[] { "name", "gate",
				"kind", "elementOnly" });
		addAnnotation(getGate_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getGate_Expression(), source, new String[] { "kind",
				"element", "name", "expression" });
		addAnnotation(getGate_Property(), source, new String[] { "kind",
				"element", "name", "property" });
		addAnnotation(getGate_IsVector(), source, new String[] { "kind",
				"attribute", "name", "is-vector" });
		addAnnotation(getGate_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(getGate_Type(), source, new String[] { "kind",
				"attribute", "name", "type" });
		addAnnotation(getGate_VectorSize(), source, new String[] { "kind",
				"attribute", "name", "vector-size" });
		addAnnotation(gateGroupEClass, source, new String[] { "name",
				"gate-group", "kind", "elementOnly" });
		addAnnotation(getGateGroup_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getGateGroup_Condition(), source, new String[] { "kind",
				"element", "name", "condition" });
		addAnnotation(getGateGroup_Gate(), source, new String[] { "kind",
				"element", "name", "gate" });
		addAnnotation(gatesEClass, source, new String[] { "name", "gates",
				"kind", "elementOnly" });
		addAnnotation(getGates_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getGates_Group(), source, new String[] { "kind", "group",
				"name", "group:1" });
		addAnnotation(getGates_Gate(), source, new String[] { "kind",
				"element", "name", "gate", "group", "#group:1" });
		addAnnotation(getGates_GateGroup(), source, new String[] { "kind",
				"element", "name", "gate-group", "group", "#group:1" });
		addAnnotation(importEClass, source, new String[] { "name", "import",
				"kind", "elementOnly" });
		addAnnotation(getImport_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getImport_Filename(), source, new String[] { "kind",
				"attribute", "name", "filename" });
		addAnnotation(interfaceNameEClass, source, new String[] { "name",
				"interface-name", "kind", "elementOnly" });
		addAnnotation(getInterfaceName_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getInterfaceName_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(keyValueEClass, source, new String[] { "name",
				"key-value", "kind", "elementOnly" });
		addAnnotation(getKeyValue_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getKeyValue_Key(), source, new String[] { "kind",
				"attribute", "name", "key" });
		addAnnotation(getKeyValue_Value(), source, new String[] { "kind",
				"attribute", "name", "value" });
		addAnnotation(loopEClass, source, new String[] { "name", "loop",
				"kind", "elementOnly" });
		addAnnotation(getLoop_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getLoop_Expression(), source, new String[] { "kind",
				"element", "name", "expression" });
		addAnnotation(getLoop_FromValue(), source, new String[] { "kind",
				"attribute", "name", "from-value" });
		addAnnotation(getLoop_ParamName(), source, new String[] { "kind",
				"attribute", "name", "param-name" });
		addAnnotation(getLoop_ToValue(), source, new String[] { "kind",
				"attribute", "name", "to-value" });
		addAnnotation(moduleInterfaceEClass, source, new String[] { "name",
				"module-interface", "kind", "elementOnly" });
		addAnnotation(getModuleInterface_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getModuleInterface_Extends(), source, new String[] {
				"kind", "element", "name", "extends" });
		addAnnotation(getModuleInterface_Parameters(), source, new String[] {
				"kind", "element", "name", "parameters" });
		addAnnotation(getModuleInterface_Gates(), source, new String[] {
				"kind", "element", "name", "gates" });
		addAnnotation(getModuleInterface_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(nedFileEClass, source, new String[] { "name", "ned-file",
				"kind", "elementOnly" });
		addAnnotation(getNedFile_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getNedFile_Group(), source, new String[] { "kind",
				"group", "name", "group:1" });
		addAnnotation(getNedFile_Import(), source, new String[] { "kind",
				"element", "name", "import", "group", "#group:1" });
		addAnnotation(getNedFile_Propertydef(), source, new String[] { "kind",
				"element", "name", "propertydef", "group", "#group:1" });
		addAnnotation(getNedFile_Property(), source, new String[] { "kind",
				"element", "name", "property", "group", "#group:1" });
		addAnnotation(getNedFile_Channel(), source, new String[] { "kind",
				"element", "name", "channel", "group", "#group:1" });
		addAnnotation(getNedFile_ChannelInterface(), source, new String[] {
				"kind", "element", "name", "channel-interface", "group",
				"#group:1" });
		addAnnotation(getNedFile_SimpleModule(), source, new String[] { "kind",
				"element", "name", "simple-module", "group", "#group:1" });
		addAnnotation(getNedFile_CompoundModule(), source, new String[] {
				"kind", "element", "name", "compound-module", "group",
				"#group:1" });
		addAnnotation(getNedFile_ModuleInterface(), source, new String[] {
				"kind", "element", "name", "module-interface", "group",
				"#group:1" });
		addAnnotation(getNedFile_Filename(), source, new String[] { "kind",
				"attribute", "name", "filename" });
		addAnnotation(getNedFile_Package(), source, new String[] { "kind",
				"attribute", "name", "package" });
		addAnnotation(operatorEClass, source, new String[] { "name",
				"operator", "kind", "elementOnly" });
		addAnnotation(getOperator_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getOperator_Group(), source, new String[] { "kind",
				"group", "name", "group:1" });
		addAnnotation(getOperator_Operator(), source, new String[] { "kind",
				"element", "name", "operator", "group", "#group:1" });
		addAnnotation(getOperator_Function(), source, new String[] { "kind",
				"element", "name", "function", "group", "#group:1" });
		addAnnotation(getOperator_Ref(), source, new String[] { "kind",
				"element", "name", "ref", "group", "#group:1" });
		addAnnotation(getOperator_Const(), source, new String[] { "kind",
				"element", "name", "const", "group", "#group:1" });
		addAnnotation(getOperator_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(paramEClass, source, new String[] { "name", "param",
				"kind", "elementOnly" });
		addAnnotation(getParam_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getParam_Expression(), source, new String[] { "kind",
				"element", "name", "expression" });
		addAnnotation(getParam_Property(), source, new String[] { "kind",
				"element", "name", "property" });
		addAnnotation(getParam_IsFunction(), source, new String[] { "kind",
				"attribute", "name", "is-function" });
		addAnnotation(getParam_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(getParam_Qualifier(), source, new String[] { "kind",
				"attribute", "name", "qualifier" });
		addAnnotation(getParam_Type(), source, new String[] { "kind",
				"attribute", "name", "type" });
		addAnnotation(getParam_Value(), source, new String[] { "kind",
				"attribute", "name", "value" });
		addAnnotation(parametersEClass, source, new String[] { "name",
				"parameters", "kind", "elementOnly" });
		addAnnotation(getParameters_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getParameters_Group(), source, new String[] { "kind",
				"group", "name", "group:1" });
		addAnnotation(getParameters_Property(), source, new String[] { "kind",
				"element", "name", "property", "group", "#group:1" });
		addAnnotation(getParameters_Param(), source, new String[] { "kind",
				"element", "name", "param", "group", "#group:1" });
		addAnnotation(getParameters_ParamGroup(), source, new String[] {
				"kind", "element", "name", "param-group", "group", "#group:1" });
		addAnnotation(getParameters_IsImplicit(), source, new String[] {
				"kind", "attribute", "name", "is-implicit" });
		addAnnotation(paramGroupEClass, source, new String[] { "name",
				"param-group", "kind", "elementOnly" });
		addAnnotation(getParamGroup_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getParamGroup_Condition(), source, new String[] { "kind",
				"element", "name", "condition" });
		addAnnotation(getParamGroup_Group(), source, new String[] { "kind",
				"group", "name", "group:2" });
		addAnnotation(getParamGroup_Property(), source, new String[] { "kind",
				"element", "name", "property", "group", "#group:2" });
		addAnnotation(getParamGroup_Param(), source, new String[] { "kind",
				"element", "name", "param", "group", "#group:2" });
		addAnnotation(propertyEClass, source, new String[] { "name",
				"property", "kind", "elementOnly" });
		addAnnotation(getProperty_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getProperty_KeyValue(), source, new String[] { "kind",
				"element", "name", "key-value" });
		addAnnotation(getProperty_Property(), source, new String[] { "kind",
				"element", "name", "property" });
		addAnnotation(getProperty_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(getProperty_Qualifier(), source, new String[] { "kind",
				"attribute", "name", "qualifier" });
		addAnnotation(propertydefEClass, source, new String[] { "name",
				"propertydef", "kind", "elementOnly" });
		addAnnotation(getPropertydef_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getPropertydef_KeyValue(), source, new String[] { "kind",
				"element", "name", "key-value" });
		addAnnotation(getPropertydef_Property(), source, new String[] { "kind",
				"element", "name", "property" });
		addAnnotation(getPropertydef_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(refEClass, source, new String[] { "name", "ref", "kind",
				"elementOnly" });
		addAnnotation(getRef_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getRef_Expression(), source, new String[] { "kind",
				"element", "name", "expression" });
		addAnnotation(getRef_Module(), source, new String[] { "kind",
				"attribute", "name", "module" });
		addAnnotation(getRef_ModuleIndex(), source, new String[] { "kind",
				"attribute", "name", "module-index" });
		addAnnotation(getRef_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(simpleModuleEClass, source, new String[] { "name",
				"simple-module", "kind", "elementOnly" });
		addAnnotation(getSimpleModule_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getSimpleModule_Extends(), source, new String[] { "kind",
				"element", "name", "extends" });
		addAnnotation(getSimpleModule_InterfaceName(), source, new String[] {
				"kind", "element", "name", "interface-name" });
		addAnnotation(getSimpleModule_Parameters(), source, new String[] {
				"kind", "element", "name", "parameters" });
		addAnnotation(getSimpleModule_Gates(), source, new String[] { "kind",
				"element", "name", "gates" });
		addAnnotation(getSimpleModule_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(submoduleEClass, source, new String[] { "name",
				"submodule", "kind", "elementOnly" });
		addAnnotation(getSubmodule_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getSubmodule_Expression(), source, new String[] { "kind",
				"element", "name", "expression" });
		addAnnotation(getSubmodule_Parameters(), source, new String[] { "kind",
				"element", "name", "parameters" });
		addAnnotation(getSubmodule_Gates(), source, new String[] { "kind",
				"element", "name", "gates" });
		addAnnotation(getSubmodule_LikeParam(), source, new String[] { "kind",
				"attribute", "name", "like-param" });
		addAnnotation(getSubmodule_LikeType(), source, new String[] { "kind",
				"attribute", "name", "like-type" });
		addAnnotation(getSubmodule_Name(), source, new String[] { "kind",
				"attribute", "name", "name" });
		addAnnotation(getSubmodule_Type(), source, new String[] { "kind",
				"attribute", "name", "type" });
		addAnnotation(getSubmodule_VectorSize(), source, new String[] { "kind",
				"attribute", "name", "vector-size" });
		addAnnotation(submodulesEClass, source, new String[] { "name",
				"submodules", "kind", "elementOnly" });
		addAnnotation(getSubmodules_Whitespace(), source, new String[] {
				"kind", "element", "name", "whitespace" });
		addAnnotation(getSubmodules_Submodule(), source, new String[] { "kind",
				"element", "name", "submodule" });
		addAnnotation(typesEClass, source, new String[] { "name", "types",
				"kind", "elementOnly" });
		addAnnotation(getTypes_Whitespace(), source, new String[] { "kind",
				"element", "name", "whitespace" });
		addAnnotation(getTypes_Group(), source, new String[] { "kind", "group",
				"name", "group:1" });
		addAnnotation(getTypes_Channel(), source, new String[] { "kind",
				"element", "name", "channel", "group", "#group:1" });
		addAnnotation(getTypes_ChannelInterface(), source, new String[] {
				"kind", "element", "name", "channel-interface", "group",
				"#group:1" });
		addAnnotation(getTypes_SimpleModule(), source, new String[] { "kind",
				"element", "name", "simple-module", "group", "#group:1" });
		addAnnotation(getTypes_CompoundModule(), source, new String[] { "kind",
				"element", "name", "compound-module", "group", "#group:1" });
		addAnnotation(getTypes_ModuleInterface(), source, new String[] {
				"kind", "element", "name", "module-interface", "group",
				"#group:1" });
		addAnnotation(whitespaceEClass, source, new String[] { "name",
				"whitespace", "kind", "empty" });
		addAnnotation(getWhitespace_Content(), source, new String[] { "kind",
				"attribute", "name", "content" });
		addAnnotation(getWhitespace_Locid(), source, new String[] { "kind",
				"attribute", "name", "locid" });
	}

	/**
	 * <!-- begin-user-doc -->
	 * Defines literals for the meta objects that represent
	 * <ul>
	 *   <li>each class,</li>
	 *   <li>each feature of each class,</li>
	 *   <li>each enum,</li>
	 *   <li>and each data type</li>
	 * </ul>
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public interface Literals {
		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Channel <em>Channel</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Channel
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getChannel()
		 * @generated
		 */
		public static final EClass CHANNEL = eINSTANCE.getChannel();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL__WHITESPACE = eINSTANCE
				.getChannel_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Extends</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL__EXTENDS = eINSTANCE
				.getChannel_Extends();

		/**
		 * The meta object literal for the '<em><b>Interface Name</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL__INTERFACE_NAME = eINSTANCE
				.getChannel_InterfaceName();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL__PARAMETERS = eINSTANCE
				.getChannel_Parameters();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CHANNEL__NAME = eINSTANCE
				.getChannel_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.ChannelInterface <em>Channel Interface</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.ChannelInterface
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getChannelInterface()
		 * @generated
		 */
		public static final EClass CHANNEL_INTERFACE = eINSTANCE
				.getChannelInterface();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL_INTERFACE__WHITESPACE = eINSTANCE
				.getChannelInterface_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Extends</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL_INTERFACE__EXTENDS = eINSTANCE
				.getChannelInterface_Extends();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CHANNEL_INTERFACE__PARAMETERS = eINSTANCE
				.getChannelInterface_Parameters();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CHANNEL_INTERFACE__NAME = eINSTANCE
				.getChannelInterface_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.CompoundModule <em>Compound Module</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.CompoundModule
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getCompoundModule()
		 * @generated
		 */
		public static final EClass COMPOUND_MODULE = eINSTANCE
				.getCompoundModule();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__WHITESPACE = eINSTANCE
				.getCompoundModule_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Extends</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__EXTENDS = eINSTANCE
				.getCompoundModule_Extends();

		/**
		 * The meta object literal for the '<em><b>Interface Name</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__INTERFACE_NAME = eINSTANCE
				.getCompoundModule_InterfaceName();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__PARAMETERS = eINSTANCE
				.getCompoundModule_Parameters();

		/**
		 * The meta object literal for the '<em><b>Gates</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__GATES = eINSTANCE
				.getCompoundModule_Gates();

		/**
		 * The meta object literal for the '<em><b>Types</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__TYPES = eINSTANCE
				.getCompoundModule_Types();

		/**
		 * The meta object literal for the '<em><b>Submodules</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__SUBMODULES = eINSTANCE
				.getCompoundModule_Submodules();

		/**
		 * The meta object literal for the '<em><b>Connections</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference COMPOUND_MODULE__CONNECTIONS = eINSTANCE
				.getCompoundModule_Connections();

		/**
		 * The meta object literal for the '<em><b>Is Network</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute COMPOUND_MODULE__IS_NETWORK = eINSTANCE
				.getCompoundModule_IsNetwork();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute COMPOUND_MODULE__NAME = eINSTANCE
				.getCompoundModule_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Condition <em>Condition</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Condition
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getCondition()
		 * @generated
		 */
		public static final EClass CONDITION = eINSTANCE.getCondition();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONDITION__WHITESPACE = eINSTANCE
				.getCondition_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONDITION__EXPRESSION = eINSTANCE
				.getCondition_Expression();

		/**
		 * The meta object literal for the '<em><b>Condition</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONDITION__CONDITION = eINSTANCE
				.getCondition_Condition();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Connection <em>Connection</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Connection
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnection()
		 * @generated
		 */
		public static final EClass CONNECTION = eINSTANCE.getConnection();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION__WHITESPACE = eINSTANCE
				.getConnection_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION__EXPRESSION = eINSTANCE
				.getConnection_Expression();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION__PARAMETERS = eINSTANCE
				.getConnection_Parameters();

		/**
		 * The meta object literal for the '<em><b>Arrow Direction</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__ARROW_DIRECTION = eINSTANCE
				.getConnection_ArrowDirection();

		/**
		 * The meta object literal for the '<em><b>Channel Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__CHANNEL_NAME = eINSTANCE
				.getConnection_ChannelName();

		/**
		 * The meta object literal for the '<em><b>Dest Gate</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__DEST_GATE = eINSTANCE
				.getConnection_DestGate();

		/**
		 * The meta object literal for the '<em><b>Dest Gate Index</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__DEST_GATE_INDEX = eINSTANCE
				.getConnection_DestGateIndex();

		/**
		 * The meta object literal for the '<em><b>Dest Gate Plusplus</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__DEST_GATE_PLUSPLUS = eINSTANCE
				.getConnection_DestGatePlusplus();

		/**
		 * The meta object literal for the '<em><b>Dest Gate Subg</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__DEST_GATE_SUBG = eINSTANCE
				.getConnection_DestGateSubg();

		/**
		 * The meta object literal for the '<em><b>Dest Module</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__DEST_MODULE = eINSTANCE
				.getConnection_DestModule();

		/**
		 * The meta object literal for the '<em><b>Dest Module Index</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__DEST_MODULE_INDEX = eINSTANCE
				.getConnection_DestModuleIndex();

		/**
		 * The meta object literal for the '<em><b>Src Gate</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__SRC_GATE = eINSTANCE
				.getConnection_SrcGate();

		/**
		 * The meta object literal for the '<em><b>Src Gate Index</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__SRC_GATE_INDEX = eINSTANCE
				.getConnection_SrcGateIndex();

		/**
		 * The meta object literal for the '<em><b>Src Gate Plusplus</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__SRC_GATE_PLUSPLUS = eINSTANCE
				.getConnection_SrcGatePlusplus();

		/**
		 * The meta object literal for the '<em><b>Src Gate Subg</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__SRC_GATE_SUBG = eINSTANCE
				.getConnection_SrcGateSubg();

		/**
		 * The meta object literal for the '<em><b>Src Module</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__SRC_MODULE = eINSTANCE
				.getConnection_SrcModule();

		/**
		 * The meta object literal for the '<em><b>Src Module Index</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION__SRC_MODULE_INDEX = eINSTANCE
				.getConnection_SrcModuleIndex();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.ConnectionGroup <em>Connection Group</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.ConnectionGroup
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup()
		 * @generated
		 */
		public static final EClass CONNECTION_GROUP = eINSTANCE
				.getConnectionGroup();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION_GROUP__WHITESPACE = eINSTANCE
				.getConnectionGroup_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTION_GROUP__GROUP = eINSTANCE
				.getConnectionGroup_Group();

		/**
		 * The meta object literal for the '<em><b>Loop</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION_GROUP__LOOP = eINSTANCE
				.getConnectionGroup_Loop();

		/**
		 * The meta object literal for the '<em><b>Condition</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION_GROUP__CONDITION = eINSTANCE
				.getConnectionGroup_Condition();

		/**
		 * The meta object literal for the '<em><b>Connection</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTION_GROUP__CONNECTION = eINSTANCE
				.getConnectionGroup_Connection();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Connections <em>Connections</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Connections
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnections()
		 * @generated
		 */
		public static final EClass CONNECTIONS = eINSTANCE.getConnections();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTIONS__WHITESPACE = eINSTANCE
				.getConnections_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTIONS__GROUP = eINSTANCE
				.getConnections_Group();

		/**
		 * The meta object literal for the '<em><b>Connection</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTIONS__CONNECTION = eINSTANCE
				.getConnections_Connection();

		/**
		 * The meta object literal for the '<em><b>Connection Group</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONNECTIONS__CONNECTION_GROUP = eINSTANCE
				.getConnections_ConnectionGroup();

		/**
		 * The meta object literal for the '<em><b>Check Unconnected</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONNECTIONS__CHECK_UNCONNECTED = eINSTANCE
				.getConnections_CheckUnconnected();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Const <em>Const</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Const
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConst()
		 * @generated
		 */
		public static final EClass CONST = eINSTANCE.getConst();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference CONST__WHITESPACE = eINSTANCE
				.getConst_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Text</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONST__TEXT = eINSTANCE.getConst_Text();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONST__TYPE = eINSTANCE.getConst_Type();

		/**
		 * The meta object literal for the '<em><b>Unit Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONST__UNIT_TYPE = eINSTANCE
				.getConst_UnitType();

		/**
		 * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute CONST__VALUE = eINSTANCE
				.getConst_Value();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Expression <em>Expression</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Expression
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getExpression()
		 * @generated
		 */
		public static final EClass EXPRESSION = eINSTANCE.getExpression();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference EXPRESSION__WHITESPACE = eINSTANCE
				.getExpression_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Operator</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference EXPRESSION__OPERATOR = eINSTANCE
				.getExpression_Operator();

		/**
		 * The meta object literal for the '<em><b>Function</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference EXPRESSION__FUNCTION = eINSTANCE
				.getExpression_Function();

		/**
		 * The meta object literal for the '<em><b>Ref</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference EXPRESSION__REF = eINSTANCE
				.getExpression_Ref();

		/**
		 * The meta object literal for the '<em><b>Const</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference EXPRESSION__CONST = eINSTANCE
				.getExpression_Const();

		/**
		 * The meta object literal for the '<em><b>Target</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute EXPRESSION__TARGET = eINSTANCE
				.getExpression_Target();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Extends <em>Extends</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Extends
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getExtends()
		 * @generated
		 */
		public static final EClass EXTENDS = eINSTANCE.getExtends();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference EXTENDS__WHITESPACE = eINSTANCE
				.getExtends_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute EXTENDS__NAME = eINSTANCE
				.getExtends_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Files <em>Files</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Files
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getFiles()
		 * @generated
		 */
		public static final EClass FILES = eINSTANCE.getFiles();

		/**
		 * The meta object literal for the '<em><b>Ned File</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference FILES__NED_FILE = eINSTANCE
				.getFiles_NedFile();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Function <em>Function</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Function
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getFunction()
		 * @generated
		 */
		public static final EClass FUNCTION = eINSTANCE.getFunction();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference FUNCTION__WHITESPACE = eINSTANCE
				.getFunction_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute FUNCTION__GROUP = eINSTANCE
				.getFunction_Group();

		/**
		 * The meta object literal for the '<em><b>Operator</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference FUNCTION__OPERATOR = eINSTANCE
				.getFunction_Operator();

		/**
		 * The meta object literal for the '<em><b>Function</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference FUNCTION__FUNCTION = eINSTANCE
				.getFunction_Function();

		/**
		 * The meta object literal for the '<em><b>Ref</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference FUNCTION__REF = eINSTANCE
				.getFunction_Ref();

		/**
		 * The meta object literal for the '<em><b>Const</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference FUNCTION__CONST = eINSTANCE
				.getFunction_Const();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute FUNCTION__NAME = eINSTANCE
				.getFunction_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Gate <em>Gate</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Gate
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getGate()
		 * @generated
		 */
		public static final EClass GATE = eINSTANCE.getGate();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATE__WHITESPACE = eINSTANCE
				.getGate_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATE__EXPRESSION = eINSTANCE
				.getGate_Expression();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATE__PROPERTY = eINSTANCE
				.getGate_Property();

		/**
		 * The meta object literal for the '<em><b>Is Vector</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute GATE__IS_VECTOR = eINSTANCE
				.getGate_IsVector();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute GATE__NAME = eINSTANCE.getGate_Name();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute GATE__TYPE = eINSTANCE.getGate_Type();

		/**
		 * The meta object literal for the '<em><b>Vector Size</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute GATE__VECTOR_SIZE = eINSTANCE
				.getGate_VectorSize();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.GateGroup <em>Gate Group</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.GateGroup
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getGateGroup()
		 * @generated
		 */
		public static final EClass GATE_GROUP = eINSTANCE.getGateGroup();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATE_GROUP__WHITESPACE = eINSTANCE
				.getGateGroup_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Condition</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATE_GROUP__CONDITION = eINSTANCE
				.getGateGroup_Condition();

		/**
		 * The meta object literal for the '<em><b>Gate</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATE_GROUP__GATE = eINSTANCE
				.getGateGroup_Gate();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Gates <em>Gates</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Gates
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getGates()
		 * @generated
		 */
		public static final EClass GATES = eINSTANCE.getGates();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATES__WHITESPACE = eINSTANCE
				.getGates_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute GATES__GROUP = eINSTANCE
				.getGates_Group();

		/**
		 * The meta object literal for the '<em><b>Gate</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATES__GATE = eINSTANCE.getGates_Gate();

		/**
		 * The meta object literal for the '<em><b>Gate Group</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference GATES__GATE_GROUP = eINSTANCE
				.getGates_GateGroup();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Import <em>Import</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Import
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getImport()
		 * @generated
		 */
		public static final EClass IMPORT = eINSTANCE.getImport();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference IMPORT__WHITESPACE = eINSTANCE
				.getImport_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Filename</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute IMPORT__FILENAME = eINSTANCE
				.getImport_Filename();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.InterfaceName <em>Interface Name</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.InterfaceName
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getInterfaceName()
		 * @generated
		 */
		public static final EClass INTERFACE_NAME = eINSTANCE
				.getInterfaceName();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference INTERFACE_NAME__WHITESPACE = eINSTANCE
				.getInterfaceName_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute INTERFACE_NAME__NAME = eINSTANCE
				.getInterfaceName_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.KeyValue <em>Key Value</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.KeyValue
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getKeyValue()
		 * @generated
		 */
		public static final EClass KEY_VALUE = eINSTANCE.getKeyValue();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference KEY_VALUE__WHITESPACE = eINSTANCE
				.getKeyValue_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Key</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute KEY_VALUE__KEY = eINSTANCE
				.getKeyValue_Key();

		/**
		 * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute KEY_VALUE__VALUE = eINSTANCE
				.getKeyValue_Value();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Loop <em>Loop</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Loop
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop()
		 * @generated
		 */
		public static final EClass LOOP = eINSTANCE.getLoop();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference LOOP__WHITESPACE = eINSTANCE
				.getLoop_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference LOOP__EXPRESSION = eINSTANCE
				.getLoop_Expression();

		/**
		 * The meta object literal for the '<em><b>From Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute LOOP__FROM_VALUE = eINSTANCE
				.getLoop_FromValue();

		/**
		 * The meta object literal for the '<em><b>Param Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute LOOP__PARAM_NAME = eINSTANCE
				.getLoop_ParamName();

		/**
		 * The meta object literal for the '<em><b>To Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute LOOP__TO_VALUE = eINSTANCE
				.getLoop_ToValue();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.ModuleInterface <em>Module Interface</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.ModuleInterface
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getModuleInterface()
		 * @generated
		 */
		public static final EClass MODULE_INTERFACE = eINSTANCE
				.getModuleInterface();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference MODULE_INTERFACE__WHITESPACE = eINSTANCE
				.getModuleInterface_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Extends</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference MODULE_INTERFACE__EXTENDS = eINSTANCE
				.getModuleInterface_Extends();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference MODULE_INTERFACE__PARAMETERS = eINSTANCE
				.getModuleInterface_Parameters();

		/**
		 * The meta object literal for the '<em><b>Gates</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference MODULE_INTERFACE__GATES = eINSTANCE
				.getModuleInterface_Gates();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute MODULE_INTERFACE__NAME = eINSTANCE
				.getModuleInterface_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.NedFile <em>Ned File</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.NedFile
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getNedFile()
		 * @generated
		 */
		public static final EClass NED_FILE = eINSTANCE.getNedFile();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__WHITESPACE = eINSTANCE
				.getNedFile_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute NED_FILE__GROUP = eINSTANCE
				.getNedFile_Group();

		/**
		 * The meta object literal for the '<em><b>Import</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__IMPORT = eINSTANCE
				.getNedFile_Import();

		/**
		 * The meta object literal for the '<em><b>Propertydef</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__PROPERTYDEF = eINSTANCE
				.getNedFile_Propertydef();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__PROPERTY = eINSTANCE
				.getNedFile_Property();

		/**
		 * The meta object literal for the '<em><b>Channel</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__CHANNEL = eINSTANCE
				.getNedFile_Channel();

		/**
		 * The meta object literal for the '<em><b>Channel Interface</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__CHANNEL_INTERFACE = eINSTANCE
				.getNedFile_ChannelInterface();

		/**
		 * The meta object literal for the '<em><b>Simple Module</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__SIMPLE_MODULE = eINSTANCE
				.getNedFile_SimpleModule();

		/**
		 * The meta object literal for the '<em><b>Compound Module</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__COMPOUND_MODULE = eINSTANCE
				.getNedFile_CompoundModule();

		/**
		 * The meta object literal for the '<em><b>Module Interface</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference NED_FILE__MODULE_INTERFACE = eINSTANCE
				.getNedFile_ModuleInterface();

		/**
		 * The meta object literal for the '<em><b>Filename</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute NED_FILE__FILENAME = eINSTANCE
				.getNedFile_Filename();

		/**
		 * The meta object literal for the '<em><b>Package</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute NED_FILE__PACKAGE = eINSTANCE
				.getNedFile_Package();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Operator <em>Operator</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Operator
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getOperator()
		 * @generated
		 */
		public static final EClass OPERATOR = eINSTANCE.getOperator();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference OPERATOR__WHITESPACE = eINSTANCE
				.getOperator_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute OPERATOR__GROUP = eINSTANCE
				.getOperator_Group();

		/**
		 * The meta object literal for the '<em><b>Operator</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference OPERATOR__OPERATOR = eINSTANCE
				.getOperator_Operator();

		/**
		 * The meta object literal for the '<em><b>Function</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference OPERATOR__FUNCTION = eINSTANCE
				.getOperator_Function();

		/**
		 * The meta object literal for the '<em><b>Ref</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference OPERATOR__REF = eINSTANCE
				.getOperator_Ref();

		/**
		 * The meta object literal for the '<em><b>Const</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference OPERATOR__CONST = eINSTANCE
				.getOperator_Const();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute OPERATOR__NAME = eINSTANCE
				.getOperator_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Param <em>Param</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Param
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam()
		 * @generated
		 */
		public static final EClass PARAM = eINSTANCE.getParam();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM__WHITESPACE = eINSTANCE
				.getParam_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM__EXPRESSION = eINSTANCE
				.getParam_Expression();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM__PROPERTY = eINSTANCE
				.getParam_Property();

		/**
		 * The meta object literal for the '<em><b>Is Function</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAM__IS_FUNCTION = eINSTANCE
				.getParam_IsFunction();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAM__NAME = eINSTANCE.getParam_Name();

		/**
		 * The meta object literal for the '<em><b>Qualifier</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAM__QUALIFIER = eINSTANCE
				.getParam_Qualifier();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAM__TYPE = eINSTANCE.getParam_Type();

		/**
		 * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAM__VALUE = eINSTANCE
				.getParam_Value();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Parameters <em>Parameters</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Parameters
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters()
		 * @generated
		 */
		public static final EClass PARAMETERS = eINSTANCE.getParameters();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAMETERS__WHITESPACE = eINSTANCE
				.getParameters_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAMETERS__GROUP = eINSTANCE
				.getParameters_Group();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAMETERS__PROPERTY = eINSTANCE
				.getParameters_Property();

		/**
		 * The meta object literal for the '<em><b>Param</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAMETERS__PARAM = eINSTANCE
				.getParameters_Param();

		/**
		 * The meta object literal for the '<em><b>Param Group</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAMETERS__PARAM_GROUP = eINSTANCE
				.getParameters_ParamGroup();

		/**
		 * The meta object literal for the '<em><b>Is Implicit</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAMETERS__IS_IMPLICIT = eINSTANCE
				.getParameters_IsImplicit();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.ParamGroup <em>Param Group</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.ParamGroup
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParamGroup()
		 * @generated
		 */
		public static final EClass PARAM_GROUP = eINSTANCE.getParamGroup();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM_GROUP__WHITESPACE = eINSTANCE
				.getParamGroup_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Condition</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM_GROUP__CONDITION = eINSTANCE
				.getParamGroup_Condition();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PARAM_GROUP__GROUP = eINSTANCE
				.getParamGroup_Group();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM_GROUP__PROPERTY = eINSTANCE
				.getParamGroup_Property();

		/**
		 * The meta object literal for the '<em><b>Param</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PARAM_GROUP__PARAM = eINSTANCE
				.getParamGroup_Param();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Property <em>Property</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Property
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getProperty()
		 * @generated
		 */
		public static final EClass PROPERTY = eINSTANCE.getProperty();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PROPERTY__WHITESPACE = eINSTANCE
				.getProperty_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Key Value</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PROPERTY__KEY_VALUE = eINSTANCE
				.getProperty_KeyValue();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PROPERTY__PROPERTY = eINSTANCE
				.getProperty_Property();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PROPERTY__NAME = eINSTANCE
				.getProperty_Name();

		/**
		 * The meta object literal for the '<em><b>Qualifier</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PROPERTY__QUALIFIER = eINSTANCE
				.getProperty_Qualifier();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Propertydef <em>Propertydef</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Propertydef
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getPropertydef()
		 * @generated
		 */
		public static final EClass PROPERTYDEF = eINSTANCE.getPropertydef();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PROPERTYDEF__WHITESPACE = eINSTANCE
				.getPropertydef_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Key Value</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PROPERTYDEF__KEY_VALUE = eINSTANCE
				.getPropertydef_KeyValue();

		/**
		 * The meta object literal for the '<em><b>Property</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference PROPERTYDEF__PROPERTY = eINSTANCE
				.getPropertydef_Property();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute PROPERTYDEF__NAME = eINSTANCE
				.getPropertydef_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Ref <em>Ref</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Ref
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef()
		 * @generated
		 */
		public static final EClass REF = eINSTANCE.getRef();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference REF__WHITESPACE = eINSTANCE
				.getRef_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference REF__EXPRESSION = eINSTANCE
				.getRef_Expression();

		/**
		 * The meta object literal for the '<em><b>Module</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute REF__MODULE = eINSTANCE.getRef_Module();

		/**
		 * The meta object literal for the '<em><b>Module Index</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute REF__MODULE_INDEX = eINSTANCE
				.getRef_ModuleIndex();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute REF__NAME = eINSTANCE.getRef_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.SimpleModule <em>Simple Module</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.SimpleModule
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule()
		 * @generated
		 */
		public static final EClass SIMPLE_MODULE = eINSTANCE.getSimpleModule();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SIMPLE_MODULE__WHITESPACE = eINSTANCE
				.getSimpleModule_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Extends</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SIMPLE_MODULE__EXTENDS = eINSTANCE
				.getSimpleModule_Extends();

		/**
		 * The meta object literal for the '<em><b>Interface Name</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SIMPLE_MODULE__INTERFACE_NAME = eINSTANCE
				.getSimpleModule_InterfaceName();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SIMPLE_MODULE__PARAMETERS = eINSTANCE
				.getSimpleModule_Parameters();

		/**
		 * The meta object literal for the '<em><b>Gates</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SIMPLE_MODULE__GATES = eINSTANCE
				.getSimpleModule_Gates();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute SIMPLE_MODULE__NAME = eINSTANCE
				.getSimpleModule_Name();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Submodule <em>Submodule</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Submodule
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule()
		 * @generated
		 */
		public static final EClass SUBMODULE = eINSTANCE.getSubmodule();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SUBMODULE__WHITESPACE = eINSTANCE
				.getSubmodule_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Expression</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SUBMODULE__EXPRESSION = eINSTANCE
				.getSubmodule_Expression();

		/**
		 * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SUBMODULE__PARAMETERS = eINSTANCE
				.getSubmodule_Parameters();

		/**
		 * The meta object literal for the '<em><b>Gates</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SUBMODULE__GATES = eINSTANCE
				.getSubmodule_Gates();

		/**
		 * The meta object literal for the '<em><b>Like Param</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute SUBMODULE__LIKE_PARAM = eINSTANCE
				.getSubmodule_LikeParam();

		/**
		 * The meta object literal for the '<em><b>Like Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute SUBMODULE__LIKE_TYPE = eINSTANCE
				.getSubmodule_LikeType();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute SUBMODULE__NAME = eINSTANCE
				.getSubmodule_Name();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute SUBMODULE__TYPE = eINSTANCE
				.getSubmodule_Type();

		/**
		 * The meta object literal for the '<em><b>Vector Size</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute SUBMODULE__VECTOR_SIZE = eINSTANCE
				.getSubmodule_VectorSize();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Submodules <em>Submodules</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Submodules
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodules()
		 * @generated
		 */
		public static final EClass SUBMODULES = eINSTANCE.getSubmodules();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SUBMODULES__WHITESPACE = eINSTANCE
				.getSubmodules_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Submodule</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference SUBMODULES__SUBMODULE = eINSTANCE
				.getSubmodules_Submodule();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Types <em>Types</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Types
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getTypes()
		 * @generated
		 */
		public static final EClass TYPES = eINSTANCE.getTypes();

		/**
		 * The meta object literal for the '<em><b>Whitespace</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference TYPES__WHITESPACE = eINSTANCE
				.getTypes_Whitespace();

		/**
		 * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute TYPES__GROUP = eINSTANCE
				.getTypes_Group();

		/**
		 * The meta object literal for the '<em><b>Channel</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference TYPES__CHANNEL = eINSTANCE
				.getTypes_Channel();

		/**
		 * The meta object literal for the '<em><b>Channel Interface</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference TYPES__CHANNEL_INTERFACE = eINSTANCE
				.getTypes_ChannelInterface();

		/**
		 * The meta object literal for the '<em><b>Simple Module</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference TYPES__SIMPLE_MODULE = eINSTANCE
				.getTypes_SimpleModule();

		/**
		 * The meta object literal for the '<em><b>Compound Module</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference TYPES__COMPOUND_MODULE = eINSTANCE
				.getTypes_CompoundModule();

		/**
		 * The meta object literal for the '<em><b>Module Interface</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EReference TYPES__MODULE_INTERFACE = eINSTANCE
				.getTypes_ModuleInterface();

		/**
		 * The meta object literal for the '{@link org.omnetpp.ned2.model.Whitespace <em>Whitespace</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see org.omnetpp.ned2.model.Whitespace
		 * @see org.omnetpp.ned2.model.meta.Ned2Package#getWhitespace()
		 * @generated
		 */
		public static final EClass WHITESPACE = eINSTANCE.getWhitespace();

		/**
		 * The meta object literal for the '<em><b>Content</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute WHITESPACE__CONTENT = eINSTANCE
				.getWhitespace_Content();

		/**
		 * The meta object literal for the '<em><b>Locid</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		public static final EAttribute WHITESPACE__LOCID = eINSTANCE
				.getWhitespace_Locid();

	}

} //Ned2Package
