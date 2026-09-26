#pragma once
#include "renderer.h"
#include <btBulletDynamicsCommon.h>

class PhysDebugger : public btIDebugDraw {
public:
    PhysDebugger(Renderer* renderer) : m_renderer() {}

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}
	virtual void draw3dText(const btVector3& location, const char* textString) override {}
	virtual void reportErrorWarning(const char* warningString) override;
	virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

private:
    Renderer* m_renderer;
    int m_debugMode = btIDebugDraw::DBG_NoDebug;
};