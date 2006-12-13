#include <stdlib.h>
#include "forcedirectedparameters.h"
#include "forcedirectedembedding.h"

void main()
{
    ForceDirectedEmbedding embedding;

    srand(0);

    int count = 10;
    IBody *previous = NULL;

	for (int i = 0; i < count; i++) {
        IBody *body = new Body(new Variable(Pt(500.0 * (double)rand() / RAND_MAX, 500.0 * (double)rand() / RAND_MAX)));
		embedding.addBody(body);
		
		if (previous)
			embedding.addForceProvider(new Spring(previous, body));

		previous = body;
    }

    std::vector<IBody *> bodies = embedding.getBodies();
    for (int i = 0; i < bodies.size(); i++) {
		for (int j = i + 1; j < bodies.size(); j++) {
			IBody *body1 = bodies[i];
			IBody *body2 = bodies[j];

			if (body1->getVariable() != body2->getVariable())
				embedding.addForceProvider(new ElectricRepeal(body1, body2));
		}
    }
    embedding.addForceProvider(new Friction());

    embedding.maxCycle = 100;

    for (int i = 0; i < 100; i++) {
        embedding.embed();
    }

    for (int i = 0; i < bodies.size(); i++) {
        printf("%g %g\n", bodies[i]->getPosition().x, bodies[i]->getPosition().y);
    }
}
