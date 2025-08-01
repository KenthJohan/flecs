/**
 * @file addons/script/visit_free.c
 * @brief Script free visitor (frees AST resources).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static
void flecs_script_scope_free(
    ecs_script_visit_t *v,
    ecs_script_scope_t *node)
{
    ecs_script_visit_scope(v, node);
    ecs_vec_fini_t(&v->script->allocator, &node->stmts, ecs_script_node_t*);
    ecs_vec_fini_t(&v->script->allocator, &node->components, ecs_id_t);
    flecs_free_t(&v->script->allocator, ecs_script_scope_t, node);
}

static
void flecs_script_with_free(
    ecs_script_visit_t *v,
    ecs_script_with_t *node)
{
    flecs_script_scope_free(v, node->expressions);
    flecs_script_scope_free(v, node->scope);
}

static
void flecs_script_template_free(
    ecs_script_visit_t *v,
    ecs_script_template_node_t *node)
{
    flecs_script_scope_free(v, node->scope);
}

static
void flecs_script_entity_free(
    ecs_script_visit_t *v,
    ecs_script_entity_t *node)
{
    flecs_script_scope_free(v, node->scope);
    if (node->name_expr) {
        flecs_expr_visit_free(&v->script->pub, node->name_expr);
    }
}

static
void flecs_script_pair_scope_free(
    ecs_script_visit_t *v,
    ecs_script_pair_scope_t *node)
{
    flecs_script_scope_free(v, node->scope);
}

static
void flecs_script_if_free(
    ecs_script_visit_t *v,
    ecs_script_if_t *node)
{
    flecs_script_scope_free(v, node->if_true);
    flecs_script_scope_free(v, node->if_false);
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

static
void flecs_script_for_range_free(
    ecs_script_visit_t *v,
    ecs_script_for_range_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->from);
    flecs_expr_visit_free(&v->script->pub, node->to);
    flecs_script_scope_free(v, node->scope);
}

static
void flecs_script_component_free(
    ecs_script_visit_t *v,
    ecs_script_component_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

static
void flecs_script_default_component_free(
    ecs_script_visit_t *v,
    ecs_script_default_component_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

static
void flecs_script_var_node_free(
    ecs_script_visit_t *v,
    ecs_script_var_node_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

static
int flecs_script_stmt_free(
    ecs_script_visit_t *v,
    ecs_script_node_t *node)
{
    ecs_allocator_t *a = &v->script->allocator;
    switch(node->kind) {
    case EcsAstScope:
        flecs_script_scope_free(v, (ecs_script_scope_t*)node);
        break;
    case EcsAstWith:
        flecs_script_with_free(v, (ecs_script_with_t*)node);
        flecs_free_t(a, ecs_script_with_t, node);
        break;
    case EcsAstTemplate:
        flecs_script_template_free(v, (ecs_script_template_node_t*)node);
        flecs_free_t(a, ecs_script_template_node_t, node);
        break;
    case EcsAstEntity:
        flecs_script_entity_free(v, (ecs_script_entity_t*)node);
        flecs_free_t(a, ecs_script_entity_t, node);
        break;
    case EcsAstPairScope:
        flecs_script_pair_scope_free(v, (ecs_script_pair_scope_t*)node);
        flecs_free_t(a, ecs_script_pair_scope_t, node);
        break;
    case EcsAstIf:
        flecs_script_if_free(v, (ecs_script_if_t*)node);
        flecs_free_t(a, ecs_script_if_t, node);
        break;
    case EcsAstFor:
        flecs_script_for_range_free(v, (ecs_script_for_range_t*)node);
        flecs_free_t(a, ecs_script_for_range_t, node);
        break;
    case EcsAstTag:
        flecs_free_t(a, ecs_script_tag_t, node);
        break;
    case EcsAstComponent:
    case EcsAstWithComponent:
        flecs_script_component_free(v, (ecs_script_component_t*)node);
        flecs_free_t(a, ecs_script_component_t, node);
        break;
    case EcsAstDefaultComponent:
        flecs_script_default_component_free(v, 
            (ecs_script_default_component_t*)node);
        flecs_free_t(a, ecs_script_default_component_t, node);
        break;
    case EcsAstVarComponent:
        flecs_free_t(a, ecs_script_var_component_t, node);
        break;
    case EcsAstWithVar:
        flecs_free_t(a, ecs_script_var_component_t, node);
        break;
    case EcsAstWithTag:
        flecs_free_t(a, ecs_script_tag_t, node);
        break;
    case EcsAstUsing:
        flecs_free_t(a, ecs_script_using_t, node);
        break;
    case EcsAstModule:
        flecs_free_t(a, ecs_script_module_t, node);
        break;
    case EcsAstAnnotation:
        flecs_free_t(a, ecs_script_annot_t, node);
        break;
    case EcsAstProp:
    case EcsAstConst:
    case EcsAstExportConst:
        flecs_script_var_node_free(v, (ecs_script_var_node_t*)node);
        flecs_free_t(a, ecs_script_var_node_t, node);
        break;
    }

    return 0;
}

int flecs_script_visit_free_node(
    ecs_script_t *script,
    ecs_script_node_t *node)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(script);

    ecs_script_visit_t v = {
        .script = impl
    };

    if (ecs_script_visit_from(
        flecs_script_impl(script), &v, flecs_script_stmt_free, node, 0))
    {
        goto error;
    }

    return 0;
error:
    return -1;
}

int flecs_script_visit_free(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(script);
    if (!impl->expr && !impl->root) {
        return 0;
    }

    ecs_script_visit_t v = {
        .script = impl
    };

    if (ecs_script_visit(
        flecs_script_impl(script), &v, flecs_script_stmt_free)) 
    {
        goto error;
    }

    return 0;
error:
    return - 1;
}

#endif
