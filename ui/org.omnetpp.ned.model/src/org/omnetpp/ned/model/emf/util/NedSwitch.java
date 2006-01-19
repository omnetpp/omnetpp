/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.util;

import java.util.List;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

import org.omnetpp.ned.model.emf.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned.model.emf.NedPackage
 * @generated
 */
public class NedSwitch {
    /**
     * The cached model package
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected static NedPackage modelPackage;

    /**
     * Creates an instance of the switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedSwitch() {
        if (modelPackage == null) {
            modelPackage = NedPackage.eINSTANCE;
        }
    }

    /**
     * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the first non-null result returned by a <code>caseXXX</code> call.
     * @generated
     */
    public Object doSwitch(EObject theEObject) {
        return doSwitch(theEObject.eClass(), theEObject);
    }

    /**
     * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the first non-null result returned by a <code>caseXXX</code> call.
     * @generated
     */
    protected Object doSwitch(EClass theEClass, EObject theEObject) {
        if (theEClass.eContainer() == modelPackage) {
            return doSwitch(theEClass.getClassifierID(), theEObject);
        }
        else {
            List eSuperTypes = theEClass.getESuperTypes();
            return
                eSuperTypes.isEmpty() ?
                    defaultCase(theEObject) :
                    doSwitch((EClass)eSuperTypes.get(0), theEObject);
        }
    }

    /**
     * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the first non-null result returned by a <code>caseXXX</code> call.
     * @generated
     */
    protected Object doSwitch(int classifierID, EObject theEObject) {
        switch (classifierID) {
            case NedPackage.CHANNEL: {
                Channel channel = (Channel)theEObject;
                Object result = caseChannel(channel);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.CHANNEL_ATTR: {
                ChannelAttr channelAttr = (ChannelAttr)theEObject;
                Object result = caseChannelAttr(channelAttr);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.COMPOUND_MODULE: {
                CompoundModule compoundModule = (CompoundModule)theEObject;
                Object result = caseCompoundModule(compoundModule);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.CONN_ATTR: {
                ConnAttr connAttr = (ConnAttr)theEObject;
                Object result = caseConnAttr(connAttr);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.CONNECTION: {
                Connection connection = (Connection)theEObject;
                Object result = caseConnection(connection);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.CONNECTIONS: {
                Connections connections = (Connections)theEObject;
                Object result = caseConnections(connections);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.DISPLAY_STRING: {
                DisplayString displayString = (DisplayString)theEObject;
                Object result = caseDisplayString(displayString);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.FOR_LOOP: {
                ForLoop forLoop = (ForLoop)theEObject;
                Object result = caseForLoop(forLoop);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.GATE: {
                Gate gate = (Gate)theEObject;
                Object result = caseGate(gate);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.GATES: {
                Gates gates = (Gates)theEObject;
                Object result = caseGates(gates);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.GATESIZE: {
                Gatesize gatesize = (Gatesize)theEObject;
                Object result = caseGatesize(gatesize);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.GATESIZES: {
                Gatesizes gatesizes = (Gatesizes)theEObject;
                Object result = caseGatesizes(gatesizes);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.IMPORT: {
                Import import_ = (Import)theEObject;
                Object result = caseImport(import_);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.NED_FILE: {
                NedFile nedFile = (NedFile)theEObject;
                Object result = caseNedFile(nedFile);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.NETWORK: {
                Network network = (Network)theEObject;
                Object result = caseNetwork(network);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.PARAM: {
                Param param = (Param)theEObject;
                Object result = caseParam(param);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.PARAMS: {
                Params params = (Params)theEObject;
                Object result = caseParams(params);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.SIMPLE_MODULE: {
                SimpleModule simpleModule = (SimpleModule)theEObject;
                Object result = caseSimpleModule(simpleModule);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.SUBMODULE: {
                Submodule submodule = (Submodule)theEObject;
                Object result = caseSubmodule(submodule);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.SUBMODULES: {
                Submodules submodules = (Submodules)theEObject;
                Object result = caseSubmodules(submodules);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.SUBSTPARAM: {
                Substparam substparam = (Substparam)theEObject;
                Object result = caseSubstparam(substparam);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            case NedPackage.SUBSTPARAMS: {
                Substparams substparams = (Substparams)theEObject;
                Object result = caseSubstparams(substparams);
                if (result == null) result = defaultCase(theEObject);
                return result;
            }
            default: return defaultCase(theEObject);
        }
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Channel</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Channel</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseChannel(Channel object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Channel Attr</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Channel Attr</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseChannelAttr(ChannelAttr object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Compound Module</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Compound Module</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseCompoundModule(CompoundModule object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Conn Attr</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Conn Attr</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseConnAttr(ConnAttr object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Connection</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Connection</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseConnection(Connection object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Connections</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Connections</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseConnections(Connections object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Display String</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Display String</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseDisplayString(DisplayString object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>For Loop</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>For Loop</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseForLoop(ForLoop object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Gate</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Gate</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseGate(Gate object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Gates</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Gates</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseGates(Gates object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Gatesize</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Gatesize</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseGatesize(Gatesize object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Gatesizes</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Gatesizes</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseGatesizes(Gatesizes object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Import</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Import</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseImport(Import object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>File</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>File</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseNedFile(NedFile object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Network</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Network</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseNetwork(Network object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Param</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Param</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseParam(Param object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Params</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Params</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseParams(Params object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Simple Module</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Simple Module</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseSimpleModule(SimpleModule object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Submodule</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Submodule</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseSubmodule(Submodule object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Submodules</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Submodules</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseSubmodules(Submodules object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Substparam</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Substparam</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseSubstparam(Substparam object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>Substparams</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>Substparams</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public Object caseSubstparams(Substparams object) {
        return null;
    }

    /**
     * Returns the result of interpretting the object as an instance of '<em>EObject</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch, but this is the last case anyway.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpretting the object as an instance of '<em>EObject</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject)
     * @generated
     */
    public Object defaultCase(EObject object) {
        return null;
    }

} //NedSwitch
