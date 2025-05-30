#include <core.h>

static
void OnPosition(ecs_iter_t *it) {
    test_assert(ecs_field(it, Position, 0) != NULL);
    probe_iter(it);
}

static
void Add_to_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_ctx(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_id(it->world, it->entities[i], ctx->component);

            test_assert( !!ecs_get_type(it->world, it->entities[i]));
        }

        if (ctx->component_2) {
            ecs_add_id(it->world, it->entities[i], ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

static
void Remove_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_ctx(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (ctx->component) {
            ecs_remove_id(it->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, e, ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

static Probe pv_probe;

static
void On_PV(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    probe_system_w_ctx(it, &pv_probe);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void ObserverOnSet_set_1_of_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_1_of_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);
    
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_1_of_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Mass);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);
    
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Mass, {10});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);
    
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_1_of_2_1_from_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA), Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_1_of_3_1_from_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA), Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Mass);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Mass, {10});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);
    
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);

    ecs_fini(world);
}

void ObserverOnSet_add_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA), Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Velocity);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

void ObserverOnSet_add_base_to_1_overridden(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ObserverOnSet_add_base_to_2_overridden(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA), Velocity(self|up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ObserverOnSet_add_base_to_1_of_2_overridden(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Velocity);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);

    ecs_os_zeromem(&ctx);

    e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ObserverOnSet_on_set_after_remove_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);

    ecs_fini(world);
}

void ObserverOnSet_on_set_after_remove_override_isa_before_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    ecs_add(world, e, Position);

    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], Base);

    ecs_fini(world);
}

void ObserverOnSet_on_set_w_override_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    ecs_add(world, e, Position);

    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position);

    ecs_delete(world, e);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ObserverOnSet_on_set_w_override_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    ecs_add(world, e, Position);

    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position);

    ecs_clear(world, e);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ObserverOnSet_on_set_w_override_after_delete_w_ecs_init(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    ecs_add(world, e, Position);

    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position);

    ecs_delete(world, e);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

static
void OnSetPosVel(ecs_iter_t *it) {
    probe_iter(it);

    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    test_assert(p != NULL);
    test_assert(v != NULL);

    test_int(it->count, 1);

    test_int(p->x, 10); test_int(p->y, 20);
    test_int(v->x, 1); test_int(v->y, 2);
}

void ObserverOnSet_on_set_w_2_overrides(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});
    ecs_set(world, base, Velocity, {1, 2});

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_observer(world, {
        .query.terms = {
            { ecs_id(Position) },
            { ecs_id(Velocity) }
        },
        .events = {EcsOnSet},
        .callback = OnSetPosVel
    });

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.invoked, 1);

    {
        const Position *p = ecs_get(world, e, Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *v = ecs_get(world, e, Velocity);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void ObserverOnSet_no_set_after_remove_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position(self|up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, ecs_pair(EcsIsA, Base));
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ObserverOnSet_un_set_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnRemove, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void ObserverOnSet_un_set_after_remove_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnRemove, Position(self|up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, Base);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, ecs_pair(EcsIsA, Base));
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void ObserverOnSet_add_to_current_in_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Add_to_current, EcsOnSet, Position);

    IterData ctx = {.component = ecs_id(Velocity)};
    ecs_set_ctx(world, &ctx, NULL);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {11, 21}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {12, 22}));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    const Position *p = ecs_get(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_int(p->x, 11);
    test_int(p->y, 21);

    p = ecs_get(world, e3, Position);
    test_int(p->x, 12);
    test_int(p->y, 22);

    ecs_fini(world);
}

void ObserverOnSet_remove_from_current_in_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Remove_from_current, EcsOnSet, Position);

    IterData ctx = {.component = ecs_id(Velocity)};
    ecs_set_ctx(world, &ctx, NULL);

    /* Create entities from scratch so they don't have the EcsName component */
    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {11, 21});
    ecs_set(world, e3, Position, {12, 22});

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( !ecs_has(world, e1, Velocity));
    test_assert( !ecs_has(world, e2, Velocity));
    test_assert( !ecs_has(world, e3, Velocity));

    const Position *p = ecs_get(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_int(p->x, 11);
    test_int(p->y, 21);

    p = ecs_get(world, e3, Position);
    test_int(p->x, 12);
    test_int(p->y, 22);

    ecs_fini(world);
}

void ObserverOnSet_remove_set_component_in_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Remove_from_current, EcsOnSet, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_ctx(world, &ctx, NULL);

    /* Create entities from scratch so they don't have the EcsName component */
    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {11, 21});
    ecs_set(world, e3, Position, {12, 22});

    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e2, Position));
    test_assert( !ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

void ObserverOnSet_match_table_created_w_add_in_on_set(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_OBSERVER(world, Add_to_current, EcsOnSet, Position);
    ECS_SYSTEM(world, On_PV, EcsOnUpdate, Position, Velocity);

    IterData add_ctx = {.component = ecs_id(Velocity)};
    ecs_set_ctx(world, &add_ctx, NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 1);

    test_int(pv_probe.count, 3);
    test_int(pv_probe.invoked, 1);
    test_int(pv_probe.system, On_PV);
    test_int(pv_probe.term_count, 2);
    test_null(pv_probe.param);

    test_int(pv_probe.e[0], e1);
    test_int(pv_probe.e[1], e2);
    test_int(pv_probe.e[2], e3);
    test_int(pv_probe.c[0][0], ecs_id(Position));
    test_int(pv_probe.s[0][0], 0);
    test_int(pv_probe.c[0][1], ecs_id(Velocity));
    test_int(pv_probe.s[0][1], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position, ?Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_optional_one_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, ?Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void ObserverOnSet_set_from_nothing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnSet, Position, Velocity());

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_os_zeromem(&ctx);
    ecs_set(world, e, Velocity, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

static
void Add0(ecs_iter_t *it) {
    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(it->world, it->entities[i], 0);
    }
}

void ObserverOnSet_add_0_entity_in_on_set(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Add0, EcsOnSet, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    test_expect_abort();
    ecs_insert(world, ecs_value(Position, {10, 20}));
}

static int dummy_invoked = 0;

static
void Dummy(ecs_iter_t *it) {
    dummy_invoked ++;
}

void ObserverOnSet_on_set_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Dummy, EcsOnSet, Position);

    ECS_PREFAB(world, Prefab, #0);
    test_assert(ecs_has_id(world, Prefab, EcsPrefab));
    ecs_set(world, Prefab, Position, {10, 20});

    test_int(dummy_invoked, 0);

    ecs_fini(world);
}
