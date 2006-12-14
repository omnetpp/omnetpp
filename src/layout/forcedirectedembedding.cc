//=========================================================================
//  FORCEDIRECTEDEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "forcedirectedembedding.h"

ForceDirectedEmbedding::ForceDirectedEmbedding() {
    debug = 0;
    inspected = false;
    initialized = false;
    finished = false;

    // parameters
    springCoefficient = 0.1;
    electricRepealCoefficient = 100000;

    frictionCoefficient = 1;
    minFrictionCoefficient = 0.1;
    maxFrictionCoefficient = 10;
    frictionCoefficientMultiplier = 2;

    timeStep = 1;
    minTimeStep = 0.001;
    maxTimeStep = 1000;
    timeStepMultiplier = 2;

    maxCalculationTime = 1000;
    maxAccelerationError = 1;

    accelerationRelaxLimit = 1;
    velocityRelaxLimit = 0.1;

    maxVelocity = 100;

    minCycle = 0;
    maxCycle = INT_MAX;
}

ForceDirectedEmbedding::~ForceDirectedEmbedding() {
    for(std::vector<Variable *>::iterator it = variables.begin(); it != variables.end(); it++)
        delete *it;

    for(std::vector<IForceProvider *>::iterator it = forceProviders.begin(); it != forceProviders.end(); it++)
        delete *it;

    for(std::vector<IBody *>::iterator it = bodies.begin(); it != bodies.end(); it++)
        delete *it;
}

void ForceDirectedEmbedding::reinitialize() {
    initialized = true;
    finished = false;

    cycle = 0;
    probCycle = 0;
    timeStepSum = 0;
    kineticEnergySum = 0;
    ticks = 0;
    massSum = 0;

    pn = createPtArray();
    an = createPtArray();
    vn = createPtArray();
    a1 = createPtArray();
    a2 = createPtArray();
    a3 = createPtArray();
    a4 = createPtArray();
    dvn = createPtArray();
    tvn = createPtArray();
    dpn = createPtArray();
    tpn = createPtArray();

    // set positions and velocities
    for (int i = 0; i < variables.size(); i++) {
        Variable *variable = variables[i];
        pn[i].assign(variable->getPosition());
        vn[i].assign(variable->getVelocity());
        massSum += variable->getMass();
    }

    // initial values
    updatedTimeStep = timeStep;
    updatedMaxAccelerationError = maxAccelerationError;

    if (frictionCoefficient)
        updatedFrictionCoefficient = frictionCoefficient;
    else
        updatedFrictionCoefficient = getEnergyBasedFrictionCoefficient(maxCalculationTime);
}

/**
 * Find the solution for the differential equation using a
 * modified Runge-Kutta 4th order algorithm.
 *
 * a1 = a[pn, vn]
 * a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
 * a3 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
 * a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
 *
 * pn+1 = pn + h * vn + h * h / 6 * [a1 + a2 + a3]
 * vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
 * 
 * This algorithm adaptively modifies timeStep and friction.
 */
void ForceDirectedEmbedding::embed() {
    if (finished)
        return;

    if (!initialized)
        reinitialize();

    long begin = clock();

    // modified Runge Kutta 4th order
    do {
        long time;
        double hMultiplier = 0;
        double accelerationError = 0;
        cycle++;

        while (true) {
            probCycle++;

            // update acceleration error limit
            time = ticksToMilliseconds(ticks + clock() - begin);
            if (time > maxCalculationTime && cycle > minCycle)
                break;
            else
                updatedMaxAccelerationError = (maxCalculationTime - time) / maxCalculationTime * this->maxAccelerationError;

            // a1 = a[pn, vn]
            a(a1, pn, vn);

            // a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
            addMultiplied(tpn, pn, updatedTimeStep / 2, vn);
            incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 8, a1);
            addMultiplied(tvn, vn, updatedTimeStep / 2, a1);
            a(a2, tpn, tvn);

            // a3 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
            addMultiplied(tpn, pn, updatedTimeStep / 2, vn);
            incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 8, a1);
            addMultiplied(tvn, vn, updatedTimeStep / 2, a2);
            a(a3, tpn, tvn);

            // a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
            addMultiplied(tpn, pn, updatedTimeStep, vn);
            incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 2, a3);
            addMultiplied(tvn, vn, updatedTimeStep, a3);
            a(a4, tpn, tvn);

            // Adjust time step (h)
            accelerationError = maximumDifference(a1, a2, a3, a4);

            if (debug >= 3)
                std::cout << "Prob cycle at real time: " << time << " time: " << timeStepSum << " h: " << updatedTimeStep << " friction: " << updatedFrictionCoefficient << " acceleration error limit: " << updatedMaxAccelerationError << " acceleration error: " << accelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

            if (accelerationError < updatedMaxAccelerationError) {
                if (hMultiplier == 0)
                    hMultiplier = timeStepMultiplier;
                else
                    break;
            }
            else {
                if (hMultiplier == 0 || hMultiplier == timeStepMultiplier)
                    hMultiplier = 1.0 / timeStepMultiplier;
            }
            
            double hNext = updatedTimeStep * hMultiplier;

            if (hNext < minTimeStep || hNext > maxTimeStep)
                break;
            
            updatedTimeStep *= hMultiplier;
        }

        // pn+1 = pn + h * vn + h * h / 6 * [a1 + a2 + a3]
        add(dpn, a1, a2);
        increment(dpn, a3);
        multiply(dpn, updatedTimeStep * updatedTimeStep / 6);
        incrementWithMultiplied(dpn, updatedTimeStep, vn);
        increment(pn, dpn);

        // vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
        addMultiplied(dvn, a1, 2, a2);
        incrementWithMultiplied(dvn, 2, a3);
        increment(dvn, a4);
        multiply(dvn, updatedTimeStep / 6);
        increment(vn, dvn);

        // Maximize velocity
        for (int i = 0; i < vn.size(); i++) {
            if (vn[i].getLength() > maxVelocity) {
                vn[i].normalize();
                vn[i].multiply(maxVelocity);
            }
        }

        time = ticksToMilliseconds(ticks + clock() - begin);
        if (debug >= 2)
            std::cout << "Cycle at real time: " << time << " time: " << timeStepSum << " h: " << updatedTimeStep << " friction: " << updatedFrictionCoefficient << " acceleration error limit: " << updatedMaxAccelerationError << " acceleration error: " << accelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

        // update friction
        timeStepSum += updatedTimeStep;
        double kineticEnergy = getKineticEnergy();
        kineticEnergySum += kineticEnergy * updatedTimeStep;
        double kineticEnergyAvg = (kineticEnergySum / timeStepSum);
        double kineticEnergyExpected = kineticEnergyAvg * (maxCalculationTime - time) / maxCalculationTime;
        if (debug >= 3)
            std::cout << "Current kinetic energy: " << kineticEnergy << " expected: " << kineticEnergyExpected << "\n";
        if (frictionCoefficient) {
            if (kineticEnergy <  kineticEnergyExpected && updatedFrictionCoefficient / frictionCoefficientMultiplier > minFrictionCoefficient)
                updatedFrictionCoefficient /= frictionCoefficientMultiplier;
            else if (kineticEnergy > kineticEnergyExpected && updatedFrictionCoefficient * frictionCoefficientMultiplier < maxFrictionCoefficient)
                updatedFrictionCoefficient *= frictionCoefficientMultiplier;
        }
        else
            updatedFrictionCoefficient = getEnergyBasedFrictionCoefficient(std::max(0.0, maxCalculationTime - time));
        
        // Check if relaxed
        finished = true;
        for (int i = 0; i < vn.size(); i++) {
            an[i].assign(a1[i]).add(a2[i]).add(a3[i]).add(a4[i]).divide(4);

            if (vn[i].getLength() > velocityRelaxLimit) {
                if (debug >= 2)
                    std::cout << "Not relax due to velocity: " << vn[i].getLength() << "\n";
                finished = false;
                break;
            }

            if (an[i].getLength() > accelerationRelaxLimit) {
                if (debug >= 2)
                    std::cout << "Not relax due to acceleration: " << an[i].getLength() << "\n";
                finished = false;
                break;
            }
        }

        if ((cycle == maxCycle) || (time > maxCalculationTime && cycle > minCycle))
            finished = true;
    }
    while (!inspected && !finished);

    ticks += clock() - begin;

    if (debug >= 1) {
        if (finished)
            std::cout << "Runge-Kutta-4 ended, at real time: " << ticksToMilliseconds(ticks) << " time: " << timeStepSum << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

        std::cout.flush();
    }
}

/**
 * an = b[pn, vn]
 */
void ForceDirectedEmbedding::a(std::vector<Pt>& an, const std::vector<Pt>& pn, const std::vector<Pt>& vn)
{
    if (debug >= 4) {
        std::cout << "Calling a() with:\n";
        for (int i = 0; i < pn.size(); i++) {
            std::cout << "p[" << i << "] = " << pn[i].x << ", " << pn[i].y << " ";
            std::cout << "v[" << i << "] = " << vn[i].x << ", " << vn[i].y << "\n";
        }
    }

    // set positions and velocities and reset forces
    for (int i = 0; i < variables.size(); i++) {
        Variable *variable = variables[i];
        variable->assignPosition(pn[i]);
        variable->assignVelocity(vn[i]);
        variable->resetForce();

        if (inspected)
            variable->resetForces();
    }

    // calculate forces
    for (std::vector<IForceProvider *>::iterator it = forceProviders.begin(); it != forceProviders.end(); it++)
        (*it)->applyForces(*this);

    // return accelerations
    for (int i = 0; i < variables.size(); i++) {
        Variable *variable = variables[i];
        an[i].assign(variable->getAcceleration());
    }

    if (debug >= 4) {
        std::cout << "Returning from a() with:\n";
        for (int i = 0; i < pn.size(); i++)
            std::cout << "a[" << i << "] = " << an[i].x << ", " << an[i].y << "\n";
    }
}
