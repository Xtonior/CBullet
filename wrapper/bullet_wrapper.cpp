// bullet_wrapper.cpp
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btMotionState.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include <btBulletDynamicsCommon.h>
#include <cstdlib>

#include "bullet_wrapper.h"

extern "C"
{
    struct BulletWorld 
    {
        btDefaultCollisionConfiguration* collisionConfig;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* broadphase;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* world;
    };

    struct BulletShape
    {
        btCollisionShape* collisionShape;
    };

    struct BulletBody 
    {
        BulletShape* shape;
        btDefaultMotionState* motionState;
        btRigidBody* body;
    };

    BulletWorld *Bullet_CreateWorld() 
    {
        BulletWorld* w = new BulletWorld;
        w->collisionConfig = new btDefaultCollisionConfiguration();
        w->dispatcher = new btCollisionDispatcher(w->collisionConfig);
        w->broadphase = new btDbvtBroadphase();
        w->solver = new btSequentialImpulseConstraintSolver();
        w->world = new btDiscreteDynamicsWorld(w->dispatcher, w->broadphase, w->solver, w->collisionConfig);
        w->world->setGravity(btVector3(0, -9.81f, 0));
        return w;
    }

    void Bullet_SetGravity(BulletWorld *world, float x, float y, float z)
    {
        world->world->setGravity(btVector3(x, y, z));
    }

    void Bullet_DestroyWorld(BulletWorld* w)
    {
        delete w->world;
        delete w->solver;
        delete w->broadphase;
        delete w->dispatcher;
        delete w->collisionConfig;
        delete w;
    }

    BulletBody *Bullet_CreateRigidBody(BulletWorld *w, BulletShape *shape, float mass, float x, float y, float z) 
    {
        BulletBody *b = new BulletBody;

        b->shape = shape;
        btVector3 inertia(0, 0, 0);
        if (mass != 0.0f)
            b->shape->collisionShape->calculateLocalInertia(mass, inertia);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(x, y, z));

        b->motionState = new btDefaultMotionState(startTransform);

        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, b->motionState, b->shape->collisionShape, inertia);
        b->body = new btRigidBody(rbInfo);

        w->world->addRigidBody(b->body);
        return b;
    }

    void Bullet_DestroyRigidBody(BulletWorld *w, BulletBody *b) 
    {
        w->world->removeRigidBody(b->body);
        delete b->body;
        delete b->motionState;
        delete b;
    }

    BulletShape *Bullet_CreateSphereShape(float radius) 
    {
        BulletShape* b = new BulletShape;
        b->collisionShape = new btSphereShape(radius);
        return b;
    }

    BulletShape *Bullet_CreateBoxShape(float halfExtentX, float halfExtentY, float halfExtentZ) 
    {
        BulletShape* b = new BulletShape;
        b->collisionShape = new btBoxShape(btVector3(halfExtentX, halfExtentY, halfExtentZ));
        return b;
    }

    void Bullet_DestroyShape(BulletShape *shape)
    {
        delete shape;
    }

    void Bullet_ApplyCentralImpulse(BulletBody *b, float impulseX, float impulseY, float impulseZ)
    {
        b->body->applyCentralImpulse(btVector3(impulseX, impulseY, impulseZ));
    }

    void Bullet_StepSimulation(BulletWorld *w, float timeStep) 
    {
        w->world->stepSimulation(timeStep);
    }

    void Bullet_GetBodyPosition(BulletBody *b, float *outX, float *outY, float *outZ) 
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        btVector3 origin = trans.getOrigin();
        *outX = origin.getX();
        *outY = origin.getY();
        *outZ = origin.getZ();
    }

    void Bullet_GetBodyEulerZYX(BulletBody *b, float *outX, float *outY, float *outZ)
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        btQuaternion rot = trans.getRotation();
        rot.getEulerZYX(*outZ, *outY, *outX);
    }

    void Bullet_GetBodyRotation(BulletBody *b, float *outX, float *outY, float *outZ, float *outW)
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        btQuaternion rot = trans.getRotation();
        
        *outW = rot.getW();
        *outX = rot.getX();
        *outY = rot.getY();
        *outZ = rot.getZ();
    }

    // Todo:
    // torque
    // velocity
    // active
    // mass
    // gravity
    // forces related functions
}