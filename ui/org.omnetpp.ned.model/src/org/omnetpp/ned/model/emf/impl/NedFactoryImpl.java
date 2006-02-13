/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.impl;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.omnetpp.ned.model.emf.*;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class NedFactoryImpl extends EFactoryImpl implements NedFactory {
    /**
     * Creates an instance of the factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedFactoryImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EObject create(EClass eClass) {
        switch (eClass.getClassifierID()) {
            case NedPackage.CHANNEL: return createChannel();
            case NedPackage.CHANNEL_ATTR: return createChannelAttr();
            case NedPackage.COMPOUND_MODULE: return createCompoundModule();
            case NedPackage.CONN_ATTR: return createConnAttr();
            case NedPackage.CONNECTION: return createConnection();
            case NedPackage.CONNECTIONS: return createConnections();
            case NedPackage.DISPLAY_STRING: return createDisplayString();
            case NedPackage.FOR_LOOP: return createForLoop();
            case NedPackage.GATE: return createGate();
            case NedPackage.GATES: return createGates();
            case NedPackage.GATESIZE: return createGatesize();
            case NedPackage.GATESIZES: return createGatesizes();
            case NedPackage.IMPORT: return createImport();
            case NedPackage.NED_FILE: return createNedFile();
            case NedPackage.NETWORK: return createNetwork();
            case NedPackage.PARAM: return createParam();
            case NedPackage.PARAMS: return createParams();
            case NedPackage.SIMPLE_MODULE: return createSimpleModule();
            case NedPackage.SUBMODULE: return createSubmodule();
            case NedPackage.SUBMODULES: return createSubmodules();
            case NedPackage.SUBSTPARAM: return createSubstparam();
            case NedPackage.SUBSTPARAMS: return createSubstparams();
            default:
                throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
        }
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Channel createChannel() {
        ChannelImpl channel = new ChannelImpl();
        return channel;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ChannelAttr createChannelAttr() {
        ChannelAttrImpl channelAttr = new ChannelAttrImpl();
        return channelAttr;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public CompoundModule createCompoundModule() {
        CompoundModuleImpl compoundModule = new CompoundModuleImpl();
        return compoundModule;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ConnAttr createConnAttr() {
        ConnAttrImpl connAttr = new ConnAttrImpl();
        return connAttr;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Connection createConnection() {
        ConnectionImpl connection = new ConnectionImpl();
        return connection;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Connections createConnections() {
        ConnectionsImpl connections = new ConnectionsImpl();
        return connections;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public DisplayString createDisplayString() {
        DisplayStringImpl displayString = new DisplayStringImpl();
        return displayString;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ForLoop createForLoop() {
        ForLoopImpl forLoop = new ForLoopImpl();
        return forLoop;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Gate createGate() {
        GateImpl gate = new GateImpl();
        return gate;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Gates createGates() {
        GatesImpl gates = new GatesImpl();
        return gates;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Gatesize createGatesize() {
        GatesizeImpl gatesize = new GatesizeImpl();
        return gatesize;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Gatesizes createGatesizes() {
        GatesizesImpl gatesizes = new GatesizesImpl();
        return gatesizes;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Import createImport() {
        ImportImpl import_ = new ImportImpl();
        return import_;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedFile createNedFile() {
        NedFileImpl nedFile = new NedFileImpl();
        return nedFile;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Network createNetwork() {
        NetworkImpl network = new NetworkImpl();
        return network;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Param createParam() {
        ParamImpl param = new ParamImpl();
        return param;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Params createParams() {
        ParamsImpl params = new ParamsImpl();
        return params;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public SimpleModule createSimpleModule() {
        SimpleModuleImpl simpleModule = new SimpleModuleImpl();
        return simpleModule;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Submodule createSubmodule() {
        SubmoduleImpl submodule = new SubmoduleImpl();
        return submodule;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Submodules createSubmodules() {
        SubmodulesImpl submodules = new SubmodulesImpl();
        return submodules;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Substparam createSubstparam() {
        SubstparamImpl substparam = new SubstparamImpl();
        return substparam;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Substparams createSubstparams() {
        SubstparamsImpl substparams = new SubstparamsImpl();
        return substparams;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedPackage getNedPackage() {
        return (NedPackage)getEPackage();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @deprecated
     * @generated
     */
    public static NedPackage getPackage() {
        return NedPackage.eINSTANCE;
    }

} //NedFactoryImpl
