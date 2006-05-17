/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Set Operation</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getFilenamePattern <em>Filename Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getRunNamePattern <em>Run Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getExperimentNamePattern <em>Experiment Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getMeasurementNamePattern <em>Measurement Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getReplicationNamePattern <em>Replication Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getModuleNamePattern <em>Module Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getNamePattern <em>Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getSourceDataset <em>Source Dataset</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getFromRunsWhere <em>From Runs Where</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation()
 * @model abstract="true"
 * @generated
 */
public interface SetOperation extends EObject {
	/**
	 * Returns the value of the '<em><b>Filename Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Filename Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Filename Pattern</em>' attribute.
	 * @see #setFilenamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_FilenamePattern()
	 * @model
	 * @generated
	 */
	String getFilenamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getFilenamePattern <em>Filename Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Filename Pattern</em>' attribute.
	 * @see #getFilenamePattern()
	 * @generated
	 */
	void setFilenamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Run Name Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Run Name Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Run Name Pattern</em>' attribute.
	 * @see #setRunNamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_RunNamePattern()
	 * @model
	 * @generated
	 */
	String getRunNamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getRunNamePattern <em>Run Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Run Name Pattern</em>' attribute.
	 * @see #getRunNamePattern()
	 * @generated
	 */
	void setRunNamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Experiment Name Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Experiment Name Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Experiment Name Pattern</em>' attribute.
	 * @see #setExperimentNamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_ExperimentNamePattern()
	 * @model
	 * @generated
	 */
	String getExperimentNamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getExperimentNamePattern <em>Experiment Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Experiment Name Pattern</em>' attribute.
	 * @see #getExperimentNamePattern()
	 * @generated
	 */
	void setExperimentNamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Measurement Name Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Measurement Name Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Measurement Name Pattern</em>' attribute.
	 * @see #setMeasurementNamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_MeasurementNamePattern()
	 * @model
	 * @generated
	 */
	String getMeasurementNamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getMeasurementNamePattern <em>Measurement Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Measurement Name Pattern</em>' attribute.
	 * @see #getMeasurementNamePattern()
	 * @generated
	 */
	void setMeasurementNamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Replication Name Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Replication Name Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Replication Name Pattern</em>' attribute.
	 * @see #setReplicationNamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_ReplicationNamePattern()
	 * @model
	 * @generated
	 */
	String getReplicationNamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getReplicationNamePattern <em>Replication Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Replication Name Pattern</em>' attribute.
	 * @see #getReplicationNamePattern()
	 * @generated
	 */
	void setReplicationNamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Module Name Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Module Name Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Module Name Pattern</em>' attribute.
	 * @see #setModuleNamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_ModuleNamePattern()
	 * @model
	 * @generated
	 */
	String getModuleNamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getModuleNamePattern <em>Module Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Module Name Pattern</em>' attribute.
	 * @see #getModuleNamePattern()
	 * @generated
	 */
	void setModuleNamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Name Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Name Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Name Pattern</em>' attribute.
	 * @see #setNamePattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_NamePattern()
	 * @model
	 * @generated
	 */
	String getNamePattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getNamePattern <em>Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name Pattern</em>' attribute.
	 * @see #getNamePattern()
	 * @generated
	 */
	void setNamePattern(String value);

	/**
	 * Returns the value of the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Source Dataset</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Source Dataset</em>' reference.
	 * @see #setSourceDataset(Dataset)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_SourceDataset()
	 * @model
	 * @generated
	 */
	Dataset getSourceDataset();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getSourceDataset <em>Source Dataset</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Source Dataset</em>' reference.
	 * @see #getSourceDataset()
	 * @generated
	 */
	void setSourceDataset(Dataset value);

	/**
	 * Returns the value of the '<em><b>From Runs Where</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>From Runs Where</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>From Runs Where</em>' attribute.
	 * @see #setFromRunsWhere(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_FromRunsWhere()
	 * @model
	 * @generated
	 */
	String getFromRunsWhere();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getFromRunsWhere <em>From Runs Where</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>From Runs Where</em>' attribute.
	 * @see #getFromRunsWhere()
	 * @generated
	 */
	void setFromRunsWhere(String value);

} // SetOperation