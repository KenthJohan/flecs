#include <meta.h>

static
void meta_test_bitmask(
    ecs_world_t *world, 
    ecs_entity_t t,
    int32_t count) 
{
    const EcsComponent *ct = ecs_get(world, t, EcsComponent);
    test_assert(ct != NULL);
    test_int(ct->size, ECS_SIZEOF(ecs_i32_t));
    test_int(ct->alignment, ECS_ALIGNOF(ecs_i32_t));

    const EcsType *mt = ecs_get(world, t, EcsType);
    test_assert(mt != NULL);
    test_assert(mt->kind == EcsBitmaskType);

    const EcsBitmask *bt = ecs_get(world, t, EcsBitmask);
    test_assert(bt != NULL);
    test_int(ecs_map_count(&bt->constants), count);
}

static
void meta_test_constant(
    ecs_world_t *world, 
    ecs_entity_t t, 
    const char *name,
    int32_t value,
    int32_t order)
{
    ecs_entity_t m = ecs_lookup_child(world, t, name);
    test_assert(m != 0);
    test_assert(ecs_has_id(world, m, EcsConstant) || 
        ecs_has_pair(world, m, EcsConstant, EcsWildcard));

    const EcsBitmask *bt = ecs_get(world, t, EcsBitmask);
    test_assert(bt != NULL);

    ecs_map_iter_t it = ecs_map_iter(&bt->constants);
    bool constant_found = false;
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);

        test_int(c->value, key);
        test_assert(c->constant != 0);
        test_str(c->name, ecs_get_name(world, c->constant));

        if (!ecs_os_strcmp(c->name, name)) {
            test_int(c->value, value);

            const ecs_u32_t *vptr = ecs_get_pair_second(world, c->constant, 
                EcsConstant, ecs_u32_t);
            if (vptr) {
                test_int(*vptr, value);
            }

            constant_found = true;
        }
    }

    test_assert(constant_found == true);

    /* Ensure ordering is correct */
    ecs_bitmask_constant_t* constants = ecs_vec_first(&bt->ordered_constants);
    test_assert(constants != NULL);
    int32_t i, count = ecs_vec_count(&bt->ordered_constants);
    for (i = 0; i < count; i++) {
        if (constants[i].value == value) {
            test_int(i, order);
            break;
        }
    }
}

void BitmaskTypes_bitmask_1_constant(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}
        }
    });

    meta_test_bitmask(world, b, 1);
    meta_test_constant(world, b, "Lettuce", 1, 0);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_2_constants(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}
        }
    });

    meta_test_bitmask(world, b, 2);
    meta_test_constant(world, b, "Lettuce", 1, 0);
    meta_test_constant(world, b, "Bacon", 2, 1);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_3_constants(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    meta_test_bitmask(world, b, 3);
    meta_test_constant(world, b, "Lettuce", 1, 0);
    meta_test_constant(world, b, "Bacon", 2, 1);
    meta_test_constant(world, b, "Tomato", 4, 2);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_4_constants(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });

    meta_test_bitmask(world, b, 4);
    meta_test_constant(world, b, "Lettuce", 1, 0);
    meta_test_constant(world, b, "Bacon", 2, 1);
    meta_test_constant(world, b, "Tomato", 4, 2);
    meta_test_constant(world, b, "Cheese", 8, 3);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_4_constants_manual_values(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce", 8}, {"Bacon", 4}, {"Tomato", 2}, {"BLT", 16}
        }
    });

    meta_test_bitmask(world, b, 4);
    meta_test_constant(world, b, "Lettuce", 8, 0);
    meta_test_constant(world, b, "Bacon", 4, 1);
    meta_test_constant(world, b, "Tomato", 2, 2);
    meta_test_constant(world, b, "BLT", 16, 3);

    ecs_fini(world);
}

void BitmaskTypes_struct_w_bitmask(void) {
    typedef struct {
        ecs_bool_t before;
        ecs_u32_t v;
        ecs_bool_t after;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    test_assert(b != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"before", ecs_id(ecs_bool_t)},
            {"v", b},
            {"after", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);
    test_str(ecs_get_name(world, t), "T");
    
    meta_test_struct(world, t, T);
    meta_test_member(world, t, T, before, ecs_id(ecs_bool_t), 1);
    meta_test_member(world, t, T, v, b, 1);
    meta_test_member(world, t, T, after, ecs_id(ecs_bool_t), 1);

    ecs_fini(world);
}

void BitmaskTypes_bitmask_w_short_notation(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    test_assert(b != 0);
    test_assert(ecs_has(world, b, EcsBitmask));

    ecs_fini(world);
}
