#include "motor_gui_ui_theme.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTextCodec>
#include <QWidget>

namespace MotorGuiUiTheme {

void apply(QWidget* root)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	if (QTextCodec* codec = QTextCodec::codecForName("UTF-8")) {
		QTextCodec::setCodecForLocale(codec);
	}
#endif

	QFont uiFont(QStringLiteral("Microsoft YaHei UI"), 9);
	uiFont.setStyleStrategy(QFont::PreferAntialias);
	qApp->setFont(uiFont);

	QFile styleFile(QStringLiteral(":/styles/motor_gui_theme.qss"));
	if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
		root->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
	}

	root->setWindowTitle(QString::fromUtf8(u8"测风电机控制"));
	root->setMinimumSize(780, 500);

	if (QLabel* status = root->findChild<QLabel*>(QStringLiteral("label_5"))) {
		status->setWordWrap(true);
		status->setAlignment(Qt::AlignCenter);
		QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(status);
		status->setGraphicsEffect(effect);
		QPropertyAnimation* pulse = new QPropertyAnimation(effect, "opacity", status);
		pulse->setDuration(1400);
		pulse->setStartValue(0.82);
		pulse->setEndValue(1.0);
		pulse->setEasingCurve(QEasingCurve::InOutSine);
		pulse->setLoopCount(-1);
		pulse->start();
	}

	if (QProgressBar* bar = root->findChild<QProgressBar*>(QStringLiteral("progressBar"))) {
		bar->setTextVisible(true);
		bar->setFormat(QString::fromUtf8(u8"%p%"));
	}
}

}
