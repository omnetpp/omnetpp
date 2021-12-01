//=========================================================================
//  FORCEDIRECTEDEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cfloat>
#include "common/lcgrandom.h"
#include "forcedirectedembedding.h"
#include "forcedirectedparameters.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace layout {

ForceDirectedEmbedding::ForceDirectedEmbedding()
{
    parameters = getParameters();
}

ForceDirectedEmbedding::~ForceDirectedEmbedding()
{
    for (auto & variable : variables)
        delete variable;

    for (auto & forceProvider : forceProviders)
        delete forceProvider;

    for (auto & body : bodies)
        delete body;
}

ForceDirectedParameters ForceDirectedEmbedding::getParameters(int32_t seed)
{
    LCGRandom lcgRandom(seed);

    ForceDirectedParameters parameters;

    parameters.defaultBodySize = Rs(10, 10);
    parameters.defaultBodyMass = 10;
    parameters.defaultBodyCharge = 1;

    parameters.defaultSpringCoefficient = 0.1 + 0.9 * lcgRandom.next01();
    parameters.defaultSpringReposeLength = 50;

    parameters.electricRepulsionCoefficient = 10000 + 90000 * lcgRandom.next01();
    parameters.defaultElectricRepulsionLinearityDistance = -1;
    parameters.defaultElectricRepulsionMaxDistance = -1;

    parameters.frictionCoefficient = 1 + 4 * lcgRandom.next01();

    parameters.defaultSlippery = false;
    parameters.defaultPointLikeDistance = false;

    parameters.timeStep = 1;
    parameters.minTimeStep = 0;
    parameters.maxTimeStep = DBL_MAX;
    parameters.timeStepMultiplier = 2;

    parameters.minAccelerationError = 0.2;
    parameters.maxAccelerationError = 0.5;

    parameters.defaultMaxForce = 1000;
    parameters.maxVelocity = 100;

    parameters.velocityRelaxLimit = 0.2;
    parameters.accelerationRelaxLimit = 1;

    parameters.maxCycle = 1000;
    parameters.maxCalculationTime = 1000 + 9000 * lcgRandom.next01();

    return parameters;
}

void ForceDirectedEmbedding::reinitialize()
{
    initialized = true;
    finished = false;

    relaxFactor = 0;
    cycle = 0;
    probCycle = 0;
    elapsedTime = 0;
    elapsedTicks = 0;
    elapsedCalculationTime = 0;
    kineticEnergySum = 0;
    totalMass = 0;

    // internal state arrays
    pn = createPtArray();
    vn = createPtArray();
    an = createPtArray();
    a1 = createPtArray();
    a2 = createPtArray();
    a3 = createPtArray();
    a4 = createPtArray();
    dpn = createPtArray();
    tpn = createPtArray();
    dvn = createPtArray();
    tvn = createPtArray();

    // reinitialize parts
    for (auto & variable : variables)
        variable->reinitialize();
    for (auto & forceProvider : forceProviders)
        forceProvider->reinitialize();
    for (auto & body : bodies)
        body->reinitialize();

    // reinitialize positions and velocities
    for (int i = 0; i < (int)variables.size(); i++) {
        Variable *variable = variables[i];
        pn[i].assign(variable->getPosition());
        vn[i].assign(variable->getVelocity());
        double variableMass = 0;
        for (auto body : bodies)
            if (body->getVariable() == variable)
                variableMass += body->getMass();
        variable->setMass(variableMass);
        totalMass += variable->getMass();
    }

    // some values needs to be reinitialized
    updatedTimeStep = parameters.timeStep;

    if (parameters.frictionCoefficient == -1)
        parameters.frictionCoefficient = getEnergyBasedFrictionCoefficient(parameters.maxCalculationTime);
}

/**
 * Find the solution for the differential equation using a
 * modified Runge-Kutta 4th order algorithm.
 *
 * a1 = a[pn, vn]
 * a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
 * a3 = a[pn + h / 2 * vn + h * h / 8 * a2, vn + h / 2 * a2]
 * a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
 *
 * pn+1 = pn + h * vn + h * h / 6 * [a1 + a2 + a3]
 * vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
 *
 * This algorithm adaptively modifies timeStep and friction.
 */
void ForceDirectedEmbedding::embed()
{
    if (finished)
        return;

    if (!initialized)
        reinitialize();

    long begin = clock();

    // modified Runge Kutta 4th order
    do {
        double hMultiplier = 0;
        cycle++;
        double nextUpdatedTimeStep = POSITIVE_INFINITY;

        while (true) {
            probCycle++;

            // update acceleration errors
            elapsedCalculationTime = ticksToMilliseconds(elapsedTicks + clock() - begin);
            if (elapsedCalculationTime > parameters.maxCalculationTime)
                break;

            // a1 = a[pn, vn]
            a(a1, pn, vn);

            // a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
            addMultiplied(tpn, pn, updatedTimeStep / 2, vn);
            incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 8, a1);
            addMultiplied(tvn, vn, updatedTimeStep / 2, a1);
            a(a2, tpn, tvn);

            // a3 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
            addMultiplied(tpn, pn, updatedTimeStep / 2, vn);
            incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 8, a2);
            addMultiplied(tvn, vn, updatedTimeStep / 2, a2);
            a(a3, tpn, tvn);

            // a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
            addMultiplied(tpn, pn, updatedTimeStep, vn);
            incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 2, a3);
            addMultiplied(tvn, vn, updatedTimeStep, a3);
            a(a4, tpn, tvn);

            // Adjust time step (h)
            // lastAccelerationError = maximumDifference(a1, a2, a3, a4);
            lastAccelerationError = averageRelativeError(a1, a2, a3, a4);

            if (debugLevel >= 3) {
                std::cout << "Prob cycle ";
                writeDebugInformation(std::cout);
            }

            if (lastAccelerationError == 0)
                break;

            // stop if acceleration error and time step are within range
            if (parameters.minAccelerationError < lastAccelerationError && lastAccelerationError < parameters.maxAccelerationError &&
                parameters.minTimeStep < updatedTimeStep && updatedTimeStep < parameters.maxTimeStep)
                break;

            // find now the update time step
            if (lastAccelerationError < parameters.maxAccelerationError) {
                if (hMultiplier == 0)
                    hMultiplier = parameters.timeStepMultiplier;
                else if (hMultiplier == 1.0 / parameters.timeStepMultiplier)
                    break;
            }
            else {
                if (hMultiplier == 0 || hMultiplier == parameters.timeStepMultiplier)
                    hMultiplier = 1.0 / parameters.timeStepMultiplier;
            }

            // stop if time step would be out of valid time step range
            nextUpdatedTimeStep = updatedTimeStep * hMultiplier;
            if (nextUpdatedTimeStep < parameters.minTimeStep || parameters.maxTimeStep < nextUpdatedTimeStep)
                break;

            updatedTimeStep = nextUpdatedTimeStep;
        }

        Assert(elapsedCalculationTime > parameters.maxCalculationTime ||
                lastAccelerationError <= parameters.maxAccelerationError ||
                nextUpdatedTimeStep <= parameters.minTimeStep);

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
        for (auto & vi : vn) {
            if (vi.getLength() > parameters.maxVelocity) {
                vi.normalize();
                vi.multiply(parameters.maxVelocity);
            }
        }

        elapsedTime += updatedTimeStep;
        elapsedCalculationTime = ticksToMilliseconds(elapsedTicks + clock() - begin);
        if (debugLevel >= 2) {
            std::cout << "Cycle ";
            writeDebugInformation(std::cout);
        }

        // check if relaxed
        lastMaxVelocity = 0;
        lastMaxAcceleration = 0;
        for (int i = 0; i < (int)vn.size(); i++) {
            double velocity = vn[i].getLength();
            if (velocity > lastMaxVelocity)
                lastMaxVelocity = velocity;

            an[i].assign(a1[i]).add(a2[i]).add(a3[i]).add(a4[i]).divide(4);
            double acceleration = an[i].getLength();
            if (acceleration > lastMaxAcceleration)
                lastMaxAcceleration = acceleration;
        }
        double velocityRelaxFactor = std::min(1.0, parameters.velocityRelaxLimit / lastMaxVelocity);
        double accelerationRelaxFactor = std::min(1.0, parameters.accelerationRelaxLimit / lastMaxAcceleration);
        relaxFactor = std::max(relaxFactor, std::min(velocityRelaxFactor, accelerationRelaxFactor));

        // stop condition
        if (cycle > parameters.maxCycle ||
            elapsedCalculationTime > parameters.maxCalculationTime ||
            (lastMaxVelocity <= parameters.velocityRelaxLimit && lastMaxAcceleration <= parameters.accelerationRelaxLimit))
            finished = true;
    } while (!inspected && !finished);

    elapsedTicks += clock() - begin;
    elapsedCalculationTime = ticksToMilliseconds(elapsedTicks);

    if (debugLevel >= 1) {
        if (finished) {
            std::cout << "Runge-Kutta-4 ended ";
            writeDebugInformation(std::cout);
        }

        std::cout.flush();
    }
}

/**
 * an = b[pn, vn]
 */
void ForceDirectedEmbedding::a(std::vector<Pt>& an, const std::vector<Pt>& pn, const std::vector<Pt>& vn)
{
    if (debugLevel >= 4) {
        std::cout << "\nCalling a() with:\n";
        for (int i = 0; i < (int)pn.size(); i++) {
            std::cout << "p[" << i << "] = " << pn[i].x << ", " << pn[i].y << ", " << pn[i].z << " ";
            std::cout << "v[" << i << "] = " << vn[i].x << ", " << vn[i].y << ", " << vn[i].z << "\n";
        }
    }

    // set positions and velocities and reset forces
    for (int i = 0; i < (int)variables.size(); i++) {
        Variable *variable = variables[i];
        variable->assignPosition(pn[i]);
        variable->assignVelocity(vn[i]);
        variable->resetForce();

        if (inspected)
            variable->resetForces();
    }

    // calculate forces
    for (auto & forceProvider : forceProviders)
        forceProvider->applyForces();

    // return accelerations
    for (int i = 0; i < (int)variables.size(); i++) {
        Variable *variable = variables[i];
        an[i].assign(variable->getAcceleration());
    }

    if (debugLevel >= 4) {
        std::cout << "Returning from a() with:\n";
        for (int i = 0; i < (int)pn.size(); i++)
            std::cout << "a[" << i << "] = " << an[i].x << ", " << an[i].y << ", " << an[i].z << "\n";
    }
}

Rc ForceDirectedEmbedding::getBoundingRectangle()
{
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    const std::vector<IBody *>& bodies = getBodies();
    for (auto body : bodies) {
        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
            bottom = std::max(bottom, body->getBottom());
            left = std::min(left, body->getLeft());
            right = std::max(right, body->getRight());
        }
    }

    return Rc(left, top, 0, right - left, bottom - top);
}

void ForceDirectedEmbedding::writeDebugInformation(std::ostream& ostream)
{
    if (initialized)
        ostream << "at real time: " << elapsedCalculationTime << " time: " << elapsedTime << " relaxFactor: " << relaxFactor << " h: " << updatedTimeStep << " friction: " << parameters.frictionCoefficient << " min acceleration error: " << parameters.minAccelerationError << " max acceleration error: " << parameters.maxAccelerationError << " last acceleration error: " << lastAccelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << " last max velocity: " << lastMaxVelocity << " last max acceleration: " << lastMaxAcceleration << "\n";
}

}  // namespace layout
}  // namespace omnetpp

