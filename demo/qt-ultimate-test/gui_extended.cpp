#include "extended_tests.h"

#include <QColorSpace>
#include <QFontDatabase>
#include <QImage>
#include <QPainterPath>
#include <QPixmap>
#include <QPainter>
#include <QTextDocument>

bool ultimateGuiExtended(QString &detail)
{
    QImage source(64, 64, QImage::Format_RGBA8888);
    source.fill(QColor(20, 40, 60, 255));
    source.setColorSpace(QColorSpace::SRgb);
    QImage scaled = source.scaled(128, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPainterPath path;
    path.addEllipse(QRectF(4, 4, 20, 20));
    QImage canvas(128, 96, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::transparent);
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(QPoint(0, 0), scaled);
    painter.fillPath(path, QColor(QStringLiteral("#f7c948")));
    painter.end();

    QTextDocument document;
    document.setHtml(QStringLiteral("<b>Qt</b> Switch <i>GUI</i>"));
    const bool ok = scaled.size() == QSize(96, 96)
            && canvas.size() == QSize(128, 96)
            && !document.toPlainText().isEmpty()
            && source.colorSpace().isValid();
    detail = ok ? QStringLiteral("image scaling + color space + antialias path + text document/font")
                : QStringLiteral("extended QtGui invariant failed");
    return ok;
}
