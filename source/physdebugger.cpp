#include "physdebugger.h"

void PhysDebugger::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    m_renderer->DrawLine(Vector(from).gl(), Vector(to).gl(), Vector(color).gl());
}

void PhysDebugger::reportErrorWarning(const char* warningString) {
    printf("Bullet: %s\n", warningString);
}

void PhysDebugger::setDebugMode(int debugMode) {
    m_debugMode = debugMode;
}

int PhysDebugger::getDebugMode() const {
    return m_debugMode;
}