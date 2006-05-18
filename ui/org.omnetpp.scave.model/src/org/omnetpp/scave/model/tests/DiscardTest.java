/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.tests;

import junit.textui.TestRunner;

import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * <!-- begin-user-doc -->
 * A test case for the model object '<em><b>Discard</b></em>'.
 * <!-- end-user-doc -->
 * @generated
 */
public class DiscardTest extends AddDiscardOpTest {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static void main(String[] args) {
		TestRunner.run(DiscardTest.class);
	}

	/**
	 * Constructs a new Discard test case with the given name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public DiscardTest(String name) {
		super(name);
	}

	/**
	 * Returns the fixture for this Discard test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private Discard getFixture() {
		return (Discard)fixture;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#setUp()
	 * @generated
	 */
	protected void setUp() throws Exception {
		setFixture(ScaveModelFactory.eINSTANCE.createDiscard());
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

} //DiscardTest
