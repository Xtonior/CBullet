// bullet_wrapper.cpp
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btMotionState.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include <btBulletDynamicsCommon.h>
#include <cstdlib>
#include <vector>

#include "bullet_types.h"
#include "bullet_api.h"
#include "bullet_wrapper.h"

struct _ContactResultCallback : public btCollisionWorld::ContactResultCallback 
{
    std::vector<btManifoldPoint> contacts;
    btScalar addSingleResult(btManifoldPoint& cp,
                         const btCollisionObjectWrapper* colObj0Wrap,
                         int partId0, int index0,
                         const btCollisionObjectWrapper* colObj1Wrap,
                         int partId1, int index1) override
    {
        if (cp.getDistance() < 0.0f)
            contacts.push_back(cp);
        return 0.f;
    }
};

extern "C"
{
    struct BulletWorld 
    {
        btDefaultCollisionConfiguration *collisionConfig;
        btCollisionDispatcher *dispatcher;
        btBroadphaseInterface *broadphase;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld *world;
    };

    struct BulletShape
    {
        btCollisionShape* collisionShape;
    };

    struct BulletBody 
    {
        BulletShape *shape;
        btDefaultMotionState *motionState;
        btRigidBody *body;
    };

    struct BulletContactResultCallback
    {
        _ContactResultCallback *contactResultCallback;
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

    void Bullet_ShapeSetMargin(BulletShape *shape, float margin)
    {
        shape->collisionShape->setMargin(margin);
    }

    void Bullet_ShapeGetMargin(BulletShape *shape, float *out_margin)
    {
        *out_margin = shape->collisionShape->getMargin();
    }

    void Bullet_BodySetCcdMotionThreshold(BulletBody *b, float threshold)
    {
        b->body->setCcdMotionThreshold(threshold);
    }

    void Bullet_SetCcdSweptSphereRadius(BulletBody *b, float radius)
    {
        b->body->setCcdSweptSphereRadius(radius);
    }

    void Bullet_BodyGetCcdMotionThreshold(BulletBody *b, float *out_threshold)
    {
        *out_threshold = b->body->getCcdMotionThreshold();
    }

    void Bullet_GetCcdSweptSphereRadius(BulletBody *b, float *out_redius)
    {
        *out_redius = b->body->getCcdSweptSphereRadius();
    }

    // Contacts handling
    void Bullet_WorldContactTest(BulletWorld* w, BulletBody *b, BulletContactResultCallback **outContactResultCallback)
    {
        BulletContactResultCallback *result = new BulletContactResultCallback;
        result->contactResultCallback = new _ContactResultCallback();
        w->world->contactTest(b->body, *result->contactResultCallback);
        *outContactResultCallback = result;
    }

    void Bullet_GetContactPoint(BulletContactResultCallback *contactResultCallback, BulletContactPoint_t *outBulletContactPoint, int numContact)
    {
        const btManifoldPoint& cp = contactResultCallback->contactResultCallback->contacts[numContact];
        outBulletContactPoint->pointOnA = {cp.getPositionWorldOnA().getX(), cp.getPositionWorldOnA().getY(), cp.getPositionWorldOnA().getZ()};
        outBulletContactPoint->pointOnB = {cp.getPositionWorldOnB().getX(), cp.getPositionWorldOnB().getY(), cp.getPositionWorldOnB().getZ()};
        outBulletContactPoint->normalOnB = {cp.m_normalWorldOnB.getX(), cp.m_normalWorldOnB.getY(), cp.m_normalWorldOnB.getZ()};
        outBulletContactPoint->impulse = cp.getAppliedImpulse();
    }

    int Bullet_BodyGetCountContacts(BulletContactResultCallback *contactResultCallback, int maxPoints)
    {
        int count = std::min<int>(contactResultCallback->contactResultCallback->contacts.size(), maxPoints);
        return count;
    }

    void Bullet_ClearContactResultCallback(BulletContactResultCallback* callback)
    {
        callback->contactResultCallback->contacts.clear();
    }
    
    void Bullet_DestroyContactResultCallback(BulletContactResultCallback* callback)
    {
        delete callback->contactResultCallback;
        delete callback;
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
        if (b->body) delete b->body;
        if (b->motionState) delete b->motionState;
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

    BulletShape *Bullet_CreateCapsuleShape(float radius, float height)
    {
        BulletShape *b = new BulletShape();
        b->collisionShape = new btCapsuleShape(radius, height);
        return b;
    }

    void Bullet_DestroyShape(BulletShape *shape)
    {
        if (shape->collisionShape) 
        {
            delete shape->collisionShape;
            shape->collisionShape = nullptr;
        }

        delete shape;
    }

    // Forces
    void Bullet_ApplyCentralImpulse(BulletBody *b, BulletVec3_t v)
    {
        b->body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
    }

    void Bullet_ApplyImpulse(BulletBody *b, BulletVec3_t v, BulletVec3_t p)
    {
        b->body->applyImpulse(btVector3(v.x, v.y, v.z), btVector3(p.x, p.y, p.z));
    }

    void Bullet_ApplyCentralPushImpulse(BulletBody *b, BulletVec3_t v, BulletVec3_t p)
    {
        b->body->applyCentralPushImpulse(btVector3(v.x, v.y, v.z));
    }

    void Bullet_ApplyPushImpulse(BulletBody *b, BulletVec3_t v, BulletVec3_t p)
    {
        b->body->applyPushImpulse(btVector3(v.x, v.y, v.z), btVector3(p.x, p.y, p.z));
    }

    void Bullet_ApplyCentralForce(BulletBody *b, BulletVec3_t v)
    {
        b->body->applyCentralForce(btVector3(v.x, v.y, v.z));
    }

    void Bullet_ApplyForce(BulletBody *b, BulletVec3_t v, BulletVec3_t p)
    {
        b->body->applyForce(btVector3(v.x, v.y, v.z), btVector3(p.x, p.y, p.z));
    }

    void Bullet_ApplyTorque(BulletBody *b, BulletVec3_t v)
    {
        b->body->applyTorque(btVector3(v.x, v.y, v.z));
    }

    void Bullet_ApplyTorqueImpulse(BulletBody *b, BulletVec3_t v)
    {
        b->body->applyTorqueImpulse(btVector3(v.x, v.y, v.z));
    }

    void Bullet_ApplyTorqueTurnImpulse(BulletBody *b, BulletVec3_t v)
    {
        b->body->applyTorqueTurnImpulse(btVector3(v.x, v.y, v.z));
    }
    // End Forces

    void Bullet_StepSimulation(BulletWorld *w, float timeStep) 
    {
        w->world->stepSimulation(timeStep);
    }

    void Bullet_Activate(BulletBody* b, int forceActivation)
    {
        b->body->activate(forceActivation);
    }

    // Getters
    void Bullet_GetBodyPosition(BulletBody *b, BulletVec3_t *out) 
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        btVector3 origin = trans.getOrigin();
        out->x = origin.getX();
        out->y = origin.getY();
        out->z = origin.getZ();
    }

    void Bullet_GetBodyEulerZYX(BulletBody *b, BulletVec3_t *out)
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        btQuaternion rot = trans.getRotation();
        rot.getEulerZYX(out->z, out->y, out->x);
    }

    void Bullet_GetBodyRotation(BulletBody *b, BulletQuat_t *out)
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        btQuaternion rot = trans.getRotation();
        
        out->x = rot.getW();
        out->y = rot.getX();
        out->z = rot.getY();
        out->w = rot.getZ();
    }

    void Get_TotalTorque(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getTotalTorque();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    void Get_TotalForce(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getTotalForce();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    float Get_AngularDamping(BulletBody *b)
    {
        return b->body->getAngularDamping();
    }

    void Get_AngularFactor(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getAngularFactor();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    float Get_AngularSleepingThreshold(BulletBody *b)
    {
        return b->body->getAngularSleepingThreshold();
    }

    void Get_AngularVelocity(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getAngularVelocity();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    float Get_LinearDamping(BulletBody *b)
    {
        return b->body->getLinearDamping();
    }

    void Get_LinearFactor(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getLinearFactor();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    float Get_LinearSleepingThreshold(BulletBody *b)
    {
        return b->body->getLinearSleepingThreshold();
    }

    void Get_LinearVelocity(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getLinearVelocity();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    void Get_InterpolationLinearVelocity(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getInterpolationLinearVelocity();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    void Get_InterpolationAngularVelocity(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getInterpolationAngularVelocity();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    void Get_PushVelocity(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getPushVelocity();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    void Get_PushVelocityInLocalPoint(BulletBody *b, BulletVec3_t p, BulletVec3_t *out)
    {
        btVector3 pv = btVector3(p.x, p.y, p.z);
        btVector3 v = b->body->getPushVelocityInLocalPoint(pv);
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    void Get_TurnVelocity(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getTurnVelocity();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    float Get_DeactivationTime(BulletBody *b)
    {
        return b->body->getDeactivationTime();
    }

    float Get_Restitution(BulletBody *b)
    {
        return b->body->getRestitution();
    }

    float Get_Friction(BulletBody *b)
    {
        return b->body->getFriction();
    }

    void Get_AnisotropicFriction(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getAnisotropicFriction();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }

    float Get_RollingFriction(BulletBody *b)
    {
        return b->body->getRollingFriction();
    }

    float Get_SpinningFriction(BulletBody *b)
    {
        return b->body->getSpinningFriction();
    }

    float Get_Mass(BulletBody *b)
    {
        return b->body->getMass();
    }

    void Get_LocalInertia(BulletBody *b, BulletVec3_t *out)
    {
        btVector3 v = b->body->getLocalInertia();
        BulletVec3_t o { v.x(), v.y(), v.z() };
        *out = o;
    }
    // End Getters

    // Setters
    void Bullet_SetPosition(BulletBody *b, float x, float y, float z)
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        trans.setOrigin(btVector3(x, y, z));

        b->body->setWorldTransform(trans);
        b->motionState->setWorldTransform(trans);
    }

    void Bullet_SetRotation(BulletBody *b, const BulletQuat_t *rot)
    {
        btTransform trans;
        b->body->getMotionState()->getWorldTransform(trans);
        trans.setRotation(btQuaternion(rot->x, rot->y, rot->z, rot->w));

        b->body->setWorldTransform(trans);
        b->motionState->setWorldTransform(trans);
    }

    void Bullet_SetLinearVelocity(BulletBody *b, const BulletVec3_t *v)
    {
        b->body->setLinearVelocity(btVector3(v->x, v->y, v->z));
    }

    void Bullet_SetAngularVelocity(BulletBody *b, const BulletVec3_t *v)
    {
        b->body->setAngularVelocity(btVector3(v->x, v->y, v->z));
    }

    void Set_AngularFactor(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularFactor(btVector3(v.x, v.y, v.z));
    }

    void Set_AngularVelocity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_LinearFactor(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_LinearVelocity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_PushVelocity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_TurnVelocity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_Damping(BulletBody *b, float lin_damping, float ang_damping)
    {
        b->body->setDamping(lin_damping, ang_damping);
    }

    void Set_Gravity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_SleepingThresholds(BulletBody *b, float linear, float angular)
    {
        b->body->setSleepingThresholds(linear, angular);
    }

    void Set_InterpolationAngularVelocity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_InterpolationLinearVelocity(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_Friction(BulletBody *b, float frict)
    {
        b->body->setFriction(frict);
    }

    void Set_AnisotropicFriction(BulletBody *b, BulletVec3_t v)
    {
        b->body->setAngularVelocity(btVector3(v.x, v.y, v.z));
    }

    void Set_RollingFriction(BulletBody *b, float frict)
    {
        b->body->setRollingFriction(frict);
    }

    void Set_SpinningFriction(BulletBody *b, float frict)
    {
        b->body->setSpinningFriction(frict);
    }

    void Set_DeactivationTime(BulletBody *b, float time)
    {
        b->body->setDeactivationTime(time);
    }
    // End Setters

    // Todo:
    // rewrite forces for BulletVec3_t
    // mat4, quat, vector conversation (glm -> bulet)
    // contacts
    // raycast
    // friction
}