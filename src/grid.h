#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/io.h"

typedef struct {
    int var;
} Grid;

void GridStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(_app->windowWidth, _app->windowHeight, 1.0f);
}

void GridDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4();
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform,_entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, _entity->transform.scale);

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    ShaderSetFloat(_entity->shaderId, "SCROLL", 1);
    DrawModel(*_entity->model);

    UnBindShader();
}