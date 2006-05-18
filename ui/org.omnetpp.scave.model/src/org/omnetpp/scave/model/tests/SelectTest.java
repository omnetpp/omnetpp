/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.tests;

import junit.textui.TestRunner;

import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.Select;

/**
 * <!-- begin-user-doc -->
 * A test case for the model object '<em><b>Select</b></em>'.
 * <!-- end-user-doc -->
 * @generated
 */
public class SelectTest extends SelectDeselectOpTest {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static void main(String[] args) {
		TestRunner.run(SelectTest.class);
	}

	/**
	 * Constructs a new Select test case with the given name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public SelectTest(String name) {
		super(name);
	}

	/**
	 * Returns the fixture for this Select test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private Select getFixture() {
		return (Select)fixture;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#setUp()
	 * @generated
	 */
	protected void setUp() throws Exception {
		setFixture(ScaveModelFactory.eINSTANCE.createSelect());
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

} //SelectTest
