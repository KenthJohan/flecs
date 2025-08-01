#include <runtime_component.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create component for a type that isn't known at compile time
    ecs_entity_t Position = ecs_struct(ecs, {
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) }, // builtin float type
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    // Create entity, set value of position using reflection API
    ecs_entity_t ent = ecs_entity(ecs, { .name = "ent" });

    // Get type info because we need to pass in the size
    const ecs_type_info_t *ti = ecs_get_type_info(ecs, Position);
    void *ptr = ecs_ensure_id(ecs, ent, Position, (size_t)ti->size);

    ecs_meta_cursor_t cur = ecs_meta_cursor(ecs, Position, ptr);
    ecs_meta_push(&cur);          // {
    ecs_meta_set_float(&cur, 10); //   10
    ecs_meta_next(&cur);          //   ,
    ecs_meta_set_float(&cur, 20); //   20
    ecs_meta_pop(&cur);           // }

    // Convert component to string
    char *str = ecs_ptr_to_expr(ecs, Position, ptr);
    printf("%s\n", str); // {x: 10, y: 20}
    ecs_os_free(str);

    ecs_fini(ecs);
}
