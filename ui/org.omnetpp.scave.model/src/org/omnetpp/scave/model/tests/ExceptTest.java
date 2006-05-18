/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.tests;

import junit.textui.TestRunner;

import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * <!-- begin-user-doc -->
 * A test case for the model object '<em><b>Except</b></em>'.
 * <!-- end-user-doc -->
 * @generated
 */
public class ExceptTest extends SetOperationTest {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static void main(String[] args) {
		TestRunner.run(ExceptTest.class);
	}

	/**
	 * Constructs a new Except test case with the given name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ExceptTest(String name) {
		super(name);
	}

	/**
	 * Returns the fixture for this Except test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private Except getFixture() {
		return (Except)fixture;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#setUp()
	 * @generated
	 */
	protected void setUp() throws Exception {
		setFixture(ScaveModelFactory.eINSTANCE.createExcept());
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#tearDown()
	 * @generated
	 */
	protected void tearDown() throws Exception {
		setFixture(null);
	}

} //ExceptTest
