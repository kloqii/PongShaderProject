#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"

#include <SDL3/SDL.h>

typedef struct {
    int leftScore;
    int rightScore;
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);

void BallStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
}

void BallReset(AppContext* _app, Entity* _entity) {
    _entity -> transform.position = InitVector3(_app->windowHeight * 0.5f, _app->windowWidth * 0.5f, 0.0f);
    _entity -> velocity = InitVector2(0.0f, 0.0f);
    _entity -> color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void BallUpdate(AppContext* _app, Entity* _entity) {

    if (GetKeyDown(_app, SDL_SCANCODE_P))
    {
        SpawnBall(_app, _entity);
    }

    if (Vec2EqualsZero(_entity->velocity) && GetKeyDown(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;

        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 200.0f);
    }

    // check if ball is heading below the screen
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        _entity->velocity.y *= -1.0f; 
    
    // check if ball is heading above the screen
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
        _entity->velocity.y *= -1.0f; 

        // right player scores
    if (_entity->transform.position.x < 0) {
        _app->rightScore++;
        BallReset(_app, _entity);
    }

    // left player scores
    if (_entity->transform.position.x > _app->windowWidth) {
        _app->leftScore++;
        BallReset(_app, _entity);
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    // calculate ball boundaries
    float ballHalfWidth = _entity->transform.scale.x * 0.5f;
    float ballHalfHeight = _entity->transform.scale.y * 0.5f;

    float ballLeft = _entity->transform.position.x - ballHalfWidth;
    float ballRight = _entity->transform.position.x + ballHalfWidth;
    float ballTop = _entity->transform.position.y + ballHalfHeight;
    float ballBottom = _entity->transform.position.y - ballHalfHeight;

    Scene * scene = _app->scene;
    // loop entities to find paddles
    for (int i = 0; i < vec_count(&scene->entities); i++)
    {   
        Entity* other = &scene->entities[i];

        if (other == _entity)
            continue;

        if (other->name == NULL)
            continue;

        // when paddles are found, track collision
        if (strcmp(other->name, "leftPaddle") == 0 || strcmp(other->name, "rightPaddle") == 0)
        {   
            // calculate paddle boundaries
            float paddleHalfWidth  = other->transform.scale.x * 0.5f;
            float paddleHalfHeight = other->transform.scale.y * 0.5f;
            float paddleLeft   = other->transform.position.x - paddleHalfWidth;
            float paddleRight  = other->transform.position.x + paddleHalfWidth;
            float paddleTop    = other->transform.position.y + paddleHalfHeight;
            float paddleBottom = other->transform.position.y - paddleHalfHeight;
            
            // if boundaries make contact, bounce opposite direction
            if (ballRight > paddleLeft && ballLeft < paddleRight && 
                ballTop > paddleBottom && ballBottom < paddleTop)
                {
                    // check for overlap
                    float overlapHori = fminf(ballRight - paddleLeft, paddleRight - ballLeft);
                    float overlapVert = fminf(ballTop - paddleBottom, paddleTop - ballBottom);
                    
                    if (overlapHori < overlapVert)
                        _entity->velocity.x *= -1.0f; 
                    else
                    {
                        _entity->velocity.y *= -1.0f;
                        printf("Vertical collision\n");

                        if (_entity->transform.position.y < other->transform.position.y)
                        {
                            _entity->transform.position.y = paddleBottom - ballHalfHeight;
                        }
                        else
                        {
                            _entity->transform.position.y = paddleTop + ballHalfHeight;
                        }
                    }
                    
                    // change ball color
                    if (strcmp(other->name, "leftPaddle") == 0)
                            _entity->color = InitVector4(1.0f, 0.0f, 0.0f, 1.0f);

                    if (strcmp(other->name, "rightPaddle") == 0)
                            _entity->color = InitVector4(0.0f, 0.0f, 1.0f, 1.0f);
                }
            }
        }
    }

void BallDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void BallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBall(AppContext* _app, Entity* _entity) {
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;
    return ball;
}