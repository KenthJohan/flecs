#ifndef API_H
#define API_H

/* This generated file contains includes for project dependencies */
#include <api/bake_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SYS_COLUMNS (20)
#define MAX_ENTITIES (64)
#define MAX_INVOCATIONS (1024)

typedef struct Probe {
    ecs_entity_t system;
    uint32_t offset;
    uint32_t count;
    uint32_t invoked;
    uint32_t column_count;
    ecs_entity_t e[MAX_ENTITIES];
    ecs_entity_t c[MAX_INVOCATIONS][MAX_SYS_COLUMNS];
    ecs_entity_t s[MAX_INVOCATIONS][MAX_SYS_COLUMNS];
    void *param;
} Probe;

typedef struct IterData {
    ecs_entity_t component;
    ecs_entity_t component_2;
    ecs_entity_t component_3;
    ecs_entity_t new_entities[MAX_ENTITIES];
    uint32_t entity_count;
} IterData;

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

typedef float Mass;

typedef float Rotation;

typedef struct Color {
    float r;
    float g;
    float b;
    float a;
} Color;

void probe_system(ecs_rows_t *rows);

void probe_trigger(
    ecs_rows_t *rows);

void probe_has_entity(Probe *probe, ecs_entity_t e);

#ifdef __cplusplus
}
#endif

#endif

