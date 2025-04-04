#include <each_w_entity_callback.h>
#include <iostream>

// This example is the same as the each example, but in addition also shows how 
// to get access to the matched entity.

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    flecs::system s = ecs.system<Position, const Velocity>() 
        // Arguments passed to each match components passed to system
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
        });

    // Create a few test entities for a Position, Velocity query
    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity("e2")
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    // This entity will not match as it does not have Position, Velocity
    ecs.entity("e3")
        .set<Position>({10, 20});

    // Run the system
    s.run();

    // Output
    //  e1: {11, 22}
    //  e2: {13, 24}
}
