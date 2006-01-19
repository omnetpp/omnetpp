/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.util;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

import org.omnetpp.ned.model.emf.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned.model.emf.NedPackage
 * @generated
 */
public class NedAdapterFactory extends AdapterFactoryImpl {
    /**
     * The cached model package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected static NedPackage modelPackage;

    /**
     * Creates an instance of the adapter factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedAdapterFactory() {
        if (modelPackage == null) {
            modelPackage = NedPackage.eINSTANCE;
        }
    }

    /**
     * Returns whether this factory is applicable for the type of the object.
     * <!-- begin-user-doc -->
     * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
     * <!-- end-user-doc -->
     * @return whether this factory is applicable for the type of the object.
     * @generated
     */
    public boolean isFactoryForType(Object object) {
        if (object == modelPackage) {
            return true;
        }
        if (object instanceof EObject) {
            return ((EObject)object).eClass().getEPackage() == modelPackage;
        }
        return false;
    }

    /**
     * The switch the delegates to the <code>createXXX</code> methods.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected NedSwitch modelSwitch =
        new NedSwitch() {
            public Object caseChannel(Channel object) {
                return createChannelAdapter();
            }
            public Object caseChannelAttr(ChannelAttr object) {
                return createChannelAttrAdapter();
            }
            public Object caseCompoundModule(CompoundModule object) {
                return createCompoundModuleAdapter();
            }
            public Object caseConnAttr(ConnAttr object) {
                return createConnAttrAdapter();
            }
            public Object caseConnection(Connection object) {
                return createConnectionAdapter();
            }
            public Object caseConnections(Connections object) {
                return createConnectionsAdapter();
            }
            public Object caseDisplayString(DisplayString object) {
                return createDisplayStringAdapter();
            }
            public Object caseForLoop(ForLoop object) {
                return createForLoopAdapter();
            }
            public Object caseGate(Gate object) {
                return createGateAdapter();
            }
            public Object caseGates(Gates object) {
                return createGatesAdapter();
            }
            public Object caseGatesize(Gatesize object) {
                return createGatesizeAdapter();
            }
            public Object caseGatesizes(Gatesizes object) {
                return createGatesizesAdapter();
            }
            public Object caseImport(Import object) {
                return createImportAdapter();
            }
            public Object caseNedFile(NedFile object) {
                return createNedFileAdapter();
            }
            public Object caseNetwork(Network object) {
                return createNetworkAdapter();
            }
            public Object caseParam(Param object) {
                return createParamAdapter();
            }
            public Object caseParams(Params object) {
                return createParamsAdapter();
            }
            public Object caseSimpleModule(SimpleModule object) {
                return createSimpleModuleAdapter();
            }
            public Object caseSubmodule(Submodule object) {
                return createSubmoduleAdapter();
            }
            public Object caseSubmodules(Submodules object) {
                return createSubmodulesAdapter();
            }
            public Object caseSubstparam(Substparam object) {
                return createSubstparamAdapter();
            }
            public Object caseSubstparams(Substparams object) {
                return createSubstparamsAdapter();
            }
            public Object defaultCase(EObject object) {
                return createEObjectAdapter();
            }
        };

    /**
     * Creates an adapter for the <code>target</code>.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param target the object to adapt.
     * @return the adapter for the <code>target</code>.
     * @generated
     */
    public Adapter createAdapter(Notifier target) {
        return (Adapter)modelSwitch.doSwitch((EObject)target);
    }


    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Channel <em>Channel</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Channel
     * @generated
     */
    public Adapter createChannelAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.ChannelAttr <em>Channel Attr</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.ChannelAttr
     * @generated
     */
    public Adapter createChannelAttrAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.CompoundModule <em>Compound Module</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.CompoundModule
     * @generated
     */
    public Adapter createCompoundModuleAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.ConnAttr <em>Conn Attr</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.ConnAttr
     * @generated
     */
    public Adapter createConnAttrAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Connection <em>Connection</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Connection
     * @generated
     */
    public Adapter createConnectionAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Connections <em>Connections</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Connections
     * @generated
     */
    public Adapter createConnectionsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.DisplayString <em>Display String</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.DisplayString
     * @generated
     */
    public Adapter createDisplayStringAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.ForLoop <em>For Loop</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.ForLoop
     * @generated
     */
    public Adapter createForLoopAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Gate <em>Gate</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Gate
     * @generated
     */
    public Adapter createGateAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Gates <em>Gates</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Gates
     * @generated
     */
    public Adapter createGatesAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Gatesize <em>Gatesize</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Gatesize
     * @generated
     */
    public Adapter createGatesizeAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Gatesizes <em>Gatesizes</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Gatesizes
     * @generated
     */
    public Adapter createGatesizesAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Import <em>Import</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Import
     * @generated
     */
    public Adapter createImportAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.NedFile <em>File</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.NedFile
     * @generated
     */
    public Adapter createNedFileAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Network <em>Network</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Network
     * @generated
     */
    public Adapter createNetworkAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Param <em>Param</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Param
     * @generated
     */
    public Adapter createParamAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Params <em>Params</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Params
     * @generated
     */
    public Adapter createParamsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.SimpleModule <em>Simple Module</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.SimpleModule
     * @generated
     */
    public Adapter createSimpleModuleAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Submodule <em>Submodule</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Submodule
     * @generated
     */
    public Adapter createSubmoduleAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Submodules <em>Submodules</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Submodules
     * @generated
     */
    public Adapter createSubmodulesAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Substparam <em>Substparam</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Substparam
     * @generated
     */
    public Adapter createSubstparamAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for an object of class '{@link org.omnetpp.ned.model.emf.Substparams <em>Substparams</em>}'.
     * <!-- begin-user-doc -->
     * This default implementation returns null so that we can easily ignore cases;
     * it's useful to ignore a case when inheritance will catch all the cases anyway.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @see org.omnetpp.ned.model.emf.Substparams
     * @generated
     */
    public Adapter createSubstparamsAdapter() {
        return null;
    }

    /**
     * Creates a new adapter for the default case.
     * <!-- begin-user-doc -->
     * This default implementation returns null.
     * <!-- end-user-doc -->
     * @return the new adapter.
     * @generated
     */
    public Adapter createEObjectAdapter() {
        return null;
    }

} //NedAdapterFactory
