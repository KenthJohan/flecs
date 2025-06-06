#include <change_tracking.h>
#include <stdio.h>

// Queries have a builtin mechanism for tracking changes per matched table. This
// is a cheap way of eliminating redundant work, as many entities can be skipped
// with a single check. 
// 
// This example shows how to use change tracking in combination with using 
// prefabs to store a single dirty state for multiple entities.

typedef struct {
    bool value;
} Dirty;

typedef struct {
    double x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Dirty);
    ECS_COMPONENT(world, Position);

    // Make Dirty inheritable so that queries can match it on prefabs
    ecs_add_pair(world, ecs_id(Dirty), EcsOnInstantiate, EcsInherit);

    // Create a query that just reads a component. We'll use this query for
    // change tracking.
    // Each query has its own private dirty state which is reset only when the
    // query is iterated.
    ecs_query_t *q_read = ecs_query(world, {
        .terms = {{ .id = ecs_id(Position), .inout = EcsIn }},
        .flags = EcsQueryDetectChanges // Enable change detection
    });

    // Create a query that writes the component based on a Dirty state.
    ecs_query_t *q_write = ecs_query(world, {
        .terms = {
            // Only match if Dirty is shared from prefab
            { 
                .id = ecs_id(Dirty), 
                .inout = EcsIn,
                .src.id = EcsUp,
                .trav = EcsIsA
            },
            { .id = ecs_id(Position) }
        }
    });

    // Create two prefabs with a Dirty component. We can use this to share a
    // single Dirty value for all entities in a table.
    ecs_entity_t p1 = ecs_entity(world, { .name = "p1", .add = ecs_ids( EcsPrefab ) });
    ecs_set(world, p1, Dirty, {false});

    ecs_entity_t p2 = ecs_entity(world, { .name = "p2", .add = ecs_ids( EcsPrefab ) });
    ecs_set(world, p2, Dirty, {true});

    // Create instances of p1 and p2. Because the entities have different
    // prefabs, they end up in different tables.
    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, EcsIsA, p1);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add_pair(world, e2, EcsIsA, p1);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });
    ecs_add_pair(world, e3, EcsIsA, p2);
    ecs_set(world, e3, Position, {50, 60});

    ecs_entity_t e4 = ecs_entity(world, { .name = "e4" });
    ecs_add_pair(world, e4, EcsIsA, p2);
    ecs_set(world, e4, Position, {70, 80});

    // We can use the changed() function on the query to check if any of the
    // tables it is matched with has changed. Since this is the first time that
    // we check this and the query is matched with the tables we just created,
    // the function will return true.
    printf("q_read changed: %d\n", ecs_query_changed(q_read));

    // The changed state will remain true until we have iterated each table.
    ecs_iter_t it = ecs_query_iter(world, q_read);
    while (ecs_query_next(&it)) {
        // With the it.changed() function we can check if the table we're
        // currently iterating has changed since last iteration.
        // Because this is the first time the query is iterated, all tables
        // will show up as changed.
        char *table_str = ecs_table_str(world, it.table);
        printf("it.changed for table [%s]: %d\n", table_str,
            ecs_iter_changed(&it));
        ecs_os_free(table_str);
    }

    // Now that we have iterated all tables, the dirty state is reset.
    printf("q_read changed: %d\n\n", ecs_query_changed(q_read));

    // Iterate the write query. Because the Position term is InOut (default)
    // iterating the query will write to the dirty state of iterated tables.
    it = ecs_query_iter(world, q_write);
    while (ecs_query_next(&it)) {
        Dirty *dirty = ecs_field(&it, Dirty, 0);

        char *table_str = ecs_table_str(world, it.table);
        printf("iterate table [%s]\n", table_str);
        ecs_os_free(table_str);

        // Because we enforced that Dirty is a shared component, we can check
        // a single value for the entire table.
        if (!dirty->value) {
            ecs_iter_skip(&it);
            table_str = ecs_table_str(world, it.table);
            printf("it.skip for table [%s]\n", table_str);
            ecs_os_free(table_str);
            continue;
        }

        Position *p = ecs_field(&it, Position, 1);

        // For all other tables the dirty state will be set.
        for (int i = 0; i < it.count; i ++) {
            p[i].x ++;
            p[i].y ++;
        }
    }

    // One of the tables has changed, so q_read.changed() will return true
    printf("\nq_read changed: %d\n", ecs_query_changed(q_read));

    // When we iterate the read query, we'll see that one table has changed.
    it = ecs_query_iter(world, q_read);
    while (ecs_query_next(&it)) {
        char *table_str = ecs_table_str(world, it.table);
        printf("it.changed for table [%s]: %d\n", table_str,
            ecs_iter_changed(&it));
        ecs_os_free(table_str);
    }

    // Output:
    //  q_read.changed(): 1
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p1)]: 1
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p2)]: 1
    //  q_read.changed(): 0
    //  
    //  iterate table [Position, (Identifier,Name), (IsA,p1)]
    //  it.skip() for table [Position, (Identifier,Name), (IsA,p1)]
    //  iterate table [Position, (Identifier,Name), (IsA,p2)]
    //  
    //  q_read.changed(): 1
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p1)]: 0
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p2)]: 1

    return ecs_fini(world);
}
