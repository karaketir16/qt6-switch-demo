#pragma once

#include <QString>

class QQmlEngine;

// Standalone checks live in separate translation units so the diagnostic stays
// one binary while coverage can grow by Qt area.
bool ultimateCoreExtended(QString &detail);
bool ultimateGuiExtended(QString &detail);
bool ultimateWidgetsExtended(QString &detail);
bool ultimateBulkCase(int caseNumber, QString &detail);
bool ultimateUpstreamCase(QQmlEngine *engine, int caseNumber, QString &detail);
bool ultimateQmlExtended(QQmlEngine *engine, QString &detail);
bool ultimateQuickExtended(QQmlEngine *engine, QString &detail);
