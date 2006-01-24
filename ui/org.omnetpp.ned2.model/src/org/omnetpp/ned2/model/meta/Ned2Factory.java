/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model.meta;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.eclipse.emf.ecore.plugin.EcorePlugin;

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
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned2.model.meta.Ned2Package
 * @generated
 */
public class Ned2Factory extends EFactoryImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The singleton instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final Ned2Factory eINSTANCE = init();

	/**
	 * Creates the default factory implementation.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static Ned2Factory init() {
		try {
			Ned2Factory theNed2Factory = (Ned2Factory) EPackage.Registry.INSTANCE
					.getEFactory("http://omnetpp.org/schema/ned2");
			if (theNed2Factory != null) {
				return theNed2Factory;
			}
		} catch (Exception exception) {
			EcorePlugin.INSTANCE.log(exception);
		}
		return new Ned2Factory();
	}

	/**
	 * Creates an instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Ned2Factory() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EObject create(EClass eClass) {
		switch (eClass.getClassifierID()) {
		case Ned2Package.CHANNEL:
			return createChannel();
		case Ned2Package.CHANNEL_INTERFACE:
			return createChannelInterface();
		case Ned2Package.COMPOUND_MODULE:
			return createCompoundModule();
		case Ned2Package.CONDITION:
			return createCondition();
		case Ned2Package.CONNECTION:
			return createConnection();
		case Ned2Package.CONNECTION_GROUP:
			return createConnectionGroup();
		case Ned2Package.CONNECTIONS:
			return createConnections();
		case Ned2Package.CONST:
			return createConst();
		case Ned2Package.EXPRESSION:
			return createExpression();
		case Ned2Package.EXTENDS:
			return createExtends();
		case Ned2Package.FILES:
			return createFiles();
		case Ned2Package.FUNCTION:
			return createFunction();
		case Ned2Package.GATE:
			return createGate();
		case Ned2Package.GATE_GROUP:
			return createGateGroup();
		case Ned2Package.GATES:
			return createGates();
		case Ned2Package.IMPORT:
			return createImport();
		case Ned2Package.INTERFACE_NAME:
			return createInterfaceName();
		case Ned2Package.KEY_VALUE:
			return createKeyValue();
		case Ned2Package.LOOP:
			return createLoop();
		case Ned2Package.MODULE_INTERFACE:
			return createModuleInterface();
		case Ned2Package.NED_FILE:
			return createNedFile();
		case Ned2Package.OPERATOR:
			return createOperator();
		case Ned2Package.PARAM:
			return createParam();
		case Ned2Package.PARAMETERS:
			return createParameters();
		case Ned2Package.PARAM_GROUP:
			return createParamGroup();
		case Ned2Package.PROPERTY:
			return createProperty();
		case Ned2Package.PROPERTYDEF:
			return createPropertydef();
		case Ned2Package.REF:
			return createRef();
		case Ned2Package.SIMPLE_MODULE:
			return createSimpleModule();
		case Ned2Package.SUBMODULE:
			return createSubmodule();
		case Ned2Package.SUBMODULES:
			return createSubmodules();
		case Ned2Package.TYPES:
			return createTypes();
		case Ned2Package.WHITESPACE:
			return createWhitespace();
		default:
			throw new IllegalArgumentException("The class '" + eClass.getName()
					+ "' is not a valid classifier");
		}
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Channel createChannel() {
		Channel channel = new Channel() {
		};
		return channel;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ChannelInterface createChannelInterface() {
		ChannelInterface channelInterface = new ChannelInterface() {
		};
		return channelInterface;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public CompoundModule createCompoundModule() {
		CompoundModule compoundModule = new CompoundModule() {
		};
		return compoundModule;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Condition createCondition() {
		Condition condition = new Condition() {
		};
		return condition;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Connection createConnection() {
		Connection connection = new Connection() {
		};
		return connection;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ConnectionGroup createConnectionGroup() {
		ConnectionGroup connectionGroup = new ConnectionGroup() {
		};
		return connectionGroup;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Connections createConnections() {
		Connections connections = new Connections() {
		};
		return connections;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Const createConst() {
		Const const_ = new Const() {
		};
		return const_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Expression createExpression() {
		Expression expression = new Expression() {
		};
		return expression;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Extends createExtends() {
		Extends extends_ = new Extends() {
		};
		return extends_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Files createFiles() {
		Files files = new Files() {
		};
		return files;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Function createFunction() {
		Function function = new Function() {
		};
		return function;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Gate createGate() {
		Gate gate = new Gate() {
		};
		return gate;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public GateGroup createGateGroup() {
		GateGroup gateGroup = new GateGroup() {
		};
		return gateGroup;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Gates createGates() {
		Gates gates = new Gates() {
		};
		return gates;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Import createImport() {
		Import import_ = new Import() {
		};
		return import_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public InterfaceName createInterfaceName() {
		InterfaceName interfaceName = new InterfaceName() {
		};
		return interfaceName;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public KeyValue createKeyValue() {
		KeyValue keyValue = new KeyValue() {
		};
		return keyValue;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Loop createLoop() {
		Loop loop = new Loop() {
		};
		return loop;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ModuleInterface createModuleInterface() {
		ModuleInterface moduleInterface = new ModuleInterface() {
		};
		return moduleInterface;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NedFile createNedFile() {
		NedFile nedFile = new NedFile() {
		};
		return nedFile;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Operator createOperator() {
		Operator operator = new Operator() {
		};
		return operator;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Param createParam() {
		Param param = new Param() {
		};
		return param;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Parameters createParameters() {
		Parameters parameters = new Parameters() {
		};
		return parameters;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ParamGroup createParamGroup() {
		ParamGroup paramGroup = new ParamGroup() {
		};
		return paramGroup;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Property createProperty() {
		Property property = new Property() {
		};
		return property;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Propertydef createPropertydef() {
		Propertydef propertydef = new Propertydef() {
		};
		return propertydef;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Ref createRef() {
		Ref ref = new Ref() {
		};
		return ref;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public SimpleModule createSimpleModule() {
		SimpleModule simpleModule = new SimpleModule() {
		};
		return simpleModule;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Submodule createSubmodule() {
		Submodule submodule = new Submodule() {
		};
		return submodule;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Submodules createSubmodules() {
		Submodules submodules = new Submodules() {
		};
		return submodules;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Types createTypes() {
		Types types = new Types() {
		};
		return types;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Whitespace createWhitespace() {
		Whitespace whitespace = new Whitespace() {
		};
		return whitespace;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Ned2Package getNed2Package() {
		return (Ned2Package) getEPackage();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @deprecated
	 * @generated
	 */
	public static Ned2Package getPackage() {
		return Ned2Package.eINSTANCE;
	}

} //Ned2Factory
