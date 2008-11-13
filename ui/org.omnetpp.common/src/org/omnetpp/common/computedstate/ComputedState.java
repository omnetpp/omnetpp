package org.omnetpp.common.computedstate;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.Debug;


@SuppressWarnings("unchecked")
public class ComputedState<T> {
	private static long currentPulse = 0;
	
	private long pulse = -1;
	
	private List<ComputedState> dependsOn = new ArrayList<ComputedState>();
	
	private IComputation computation;
	
	private T value;
	
	public ComputedState() {
		this(null);
	}

	public ComputedState(IComputation computation) {
		this.computation = computation;
	}
	
	public boolean isValid() {
		if (pulse == -1)
			return false;

		if (computation == null || pulse == currentPulse)
			return true;

		return isValidRecursive(this);
	}
	
	private boolean isValidRecursive(ComputedState computedState) {
		for (Object o : computedState.dependsOn) {
			ComputedState c = (ComputedState)o;
			if (!c.isValid())
				return false;
			
			if (c.pulse >= pulse)
				return false;
		}

		return true;
	}

	public T recomputeIfInvalidAs(IComputation computation) {
		this.computation = computation;
		
		return recomputeIfInvalid();
	}

	public T recomputeIfInvalid() {
		if (!isValid())
		   recompute();

		return getValue0();
	}

	@SuppressWarnings("unchecked")
	public void recompute() {
		if (ComputedContext.hasOwnerOnStack(this))
			throw new RuntimeException("Circularity detected!");

		ComputedContext.pushComputedContextStack(this);
		value = (T)computation.compute();
		pulse = currentPulse;
		dependsOn = ComputedContext.popComputedContextStack().getDependsOn();
	}

	public T getValue() {
		if (computation != null)
			throw new RuntimeException("getValue may be called on constants only");

		return getValue0();
	}

	public void setValue(T value) {
		if (computation != null)
			throw new RuntimeException("setValue may be called on constants only");

		this.value = value;
		this.pulse = ++currentPulse; 
		Debug.println("P: " + currentPulse);
	}

	private T getValue0() {
		assert(isValid());
		// TODO: add circularity check, go up on stack and check for owner == this
	   
		if (!ComputedContext.isComputedContextStackEmpty())
			ComputedContext.peekComputedContextStack().addComputedState(this);
	   
		return value;
	}
}
