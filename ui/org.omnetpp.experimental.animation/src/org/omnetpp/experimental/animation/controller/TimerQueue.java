package org.omnetpp.experimental.animation.controller;

import java.util.ArrayList;

import org.eclipse.swt.widgets.Display;

public class TimerQueue implements Runnable {
	private boolean running;
	
	private ArrayList<Timer> timers = new ArrayList<Timer>();
	
	private Timer lastScheduledTimer;

	private long lastScheduledNextExecutionTime;
	
	public void addTimer(Timer timer) {
		//System.out.println("Adding timer: " + timer);
		timers.add(timer);
		
		if (running)
			scheduleTimer();
	}
	
	public void removeTimer(Timer timer) {
		//System.out.println("Removing timer: " + timer);
		Timer firstTimer = getFirstTimer();
		timers.remove(timer);

		if (running && timer == firstTimer)
			scheduleTimer();
	}
	
	public boolean hasTimer(Timer timer) {
		return timers.contains(timer);
	}
	
	public void resetTimer(Timer timer) {
		//System.out.println("Resetting timer: " + timer);
		timer.reset();
		
		if (running)
			scheduleTimer();
	}
	
	public void start() {
		//System.out.println("Starting timer queue");
		running = true;
		scheduleTimer();
	}
	
	public void stop() {
		//System.out.println("Stopping timer queue");
		running = false;
		Display.getDefault().timerExec(-1, this);
	}
	
	public boolean isRunning() {
		return running;
	}
	
	public void run() {
		if (running) {
			runFirstTimer();
			scheduleTimer();
		}
	}

	private void runFirstTimer() {
		Timer firstTimer = getFirstTimer();
		
		if (firstTimer != null) {
			//System.out.println("Running timer: " + timer);
			firstTimer.execute();
			
			if (!firstTimer.isRecurring())
				timers.remove(firstTimer);
		}
	}

	private void scheduleTimer() {
		if (timers.size() != 0) {
			sortTimers();
			Timer firstTimer = getFirstTimer();
			long currentTime = System.currentTimeMillis();
			long nextExecutionTime = firstTimer.getNextExecutionTime();
			long nextExecutionSleepTime = nextExecutionTime - currentTime;
			
			if (firstTimer != lastScheduledTimer ||
				firstTimer.getNextExecutionTime() != lastScheduledNextExecutionTime ||
				nextExecutionSleepTime < 0)
			{
				//System.out.println("Scheduling timer: " + firstTimer + " at: " + currentTime + " to: " + firstTimer.getNextExecutionTime());
				
				if (nextExecutionSleepTime < 0) {
					if (firstTimer.isAllowSkipping())
						firstTimer.calculateNextExecution();
					else
						runFirstTimer();
	
					lastScheduledTimer = null;
					scheduleTimer();
				}
				else {
					Display.getDefault().timerExec((int)nextExecutionSleepTime, this);

					lastScheduledTimer = firstTimer;
					lastScheduledNextExecutionTime = nextExecutionTime;
				}
			}
		}
	}

	private Timer getFirstTimer() {
		return timers.size() != 0 ? timers.get(0) : null;
	}

	private void sortTimers() {
		java.util.Collections.sort(timers);
	}
}
