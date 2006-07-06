package org.omnetpp.experimental.animation.controller;

import java.util.ArrayList;

import org.eclipse.swt.widgets.Display;

public class TimerQueue implements Runnable {
	private boolean running;
	
	private ArrayList<Timer> timers = new ArrayList<Timer>();
	
	private Timer lastScheduledTimer;
	
	public void addTimer(Timer timer) {
		//System.out.println("Adding timer: " + timer);

		timers.add(timer);
		
		if (running)
			scheduleTimer();
	}
	
	public void removeTimer(Timer timer) {
		//System.out.println("Removing timer: " + timer);

		if (running && timer == getFirstTimer())
			scheduleTimer();

		timers.remove(timer);
	}
	
	public void start() {
		running = true;
		scheduleTimer();
	}
	
	public void stop() {
		running = false;
		Display.getDefault().timerExec(-1, this);
	}
	
	public boolean isRunning() {
		return running;
	}
	
	public void run() {
		runFirstTimer();
		scheduleTimer();
	}

	private void runFirstTimer() {
		Timer firstTimer = getFirstTimer();
		firstTimer.execute();
		
		if (!firstTimer.isRecurring())
			timers.remove(0);
	}

	private void scheduleTimer() {
		sortTimers();
		Timer firstTimer = getFirstTimer();
		long currentTime = System.currentTimeMillis();
		long nextOccurance = firstTimer.getNextExecutionTime() - currentTime;
		
		if (true || firstTimer != lastScheduledTimer || nextOccurance < 0) {
			//System.out.println("Scheduling timer: " + firstTimer + " at: " + currentTime + " to: " + firstTimer.getNextExecutionTime());
			
			if (nextOccurance < 0) {
				if (firstTimer.isAllowSkipping())
					firstTimer.calculateNextOccurance();
				else
					runFirstTimer();

				lastScheduledTimer = null;
				scheduleTimer();
			}
			else
				Display.getDefault().timerExec((int)nextOccurance, this);

			lastScheduledTimer = firstTimer;
		}
	}

	private Timer getFirstTimer() {
		return timers.get(0);
	}

	private void sortTimers() {
		java.util.Collections.sort(timers);
	}
}
