#include "switchbutton.h"
#include <QPainter>
#include <QPropertyAnimation>

SwitchButton::SwitchButton(QWidget* parent) : QAbstractButton(parent)
{
        setStyleSheet(QStringLiteral("QAbstractButton{background-color:rgb(97, 98, 99);}"));
        circleRect = QRect(0, 0, height(), height());
}

void SwitchButton::setChecked(bool state)
{
    QPalette pal = palette();

    checked = state;
    if (checked)
    {
        pal.setBrush(QPalette::Base, QBrush(QColor(26, 137, 198)));
        circleRect = QRect(width() - height(), 0,
                           height(), height());
    }
    else
    {
        pal.setBrush(QPalette::Base, QBrush(QColor(97, 98, 99)));
        circleRect =  QRect(0, 0, height(), height());
    }
    setPalette(pal);
}

bool SwitchButton::isChecked() const
{
    return checked;
}

void SwitchButton::setCircleRect(const QRect& rect)
{
    circleRect =  rect;

}

QRect SwitchButton::getCircleRect() const
{
    return circleRect;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void SwitchButton::paintEvent ([[maybe_unused]] QPaintEvent* event)
{
    QPainter painter (this);

    painter.setPen(Qt::gray);
    painter.setBrush(palette().base());

    QRectF rec(rect().x() + 1 / 2,
                    rect().y() + 1 / 2,
                    rect().width() - 1,
                    rect().height() - 1);

    painter.drawRoundedRect(rec.translated(0.5, 0.5), 14, 14);

    QPen pen;
    pen.setBrush(Qt::transparent);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QRectF recCircle(circleRect.x() + 1 / 2,
                    circleRect.y() + 1 / 2,
                    circleRect.width() - 1,
                    circleRect.height() - 1);

    painter.drawEllipse(recCircle);
}

void SwitchButton::mousePressEvent (QMouseEvent* event)
{
    checked = !checked;
    QPalette pal = palette();
    QPropertyAnimation * animation = new QPropertyAnimation(this, "cRect");
    animation->setDuration(25);

    animation->setStartValue(circleRect);

    if(checked)
    {
        pal.setBrush(QPalette::Base, QBrush(QColor(26, 137, 198)));
        animation->setEndValue(QRect(width()- height(), 0,
                                     height(), height()));
    }
    else
    {
        pal.setBrush(QPalette::Base, QBrush(QColor(97, 98, 99)));
        animation->setEndValue(QRect(0, 0, height(), height()));
    }
    setPalette(pal);
    animation->start();

    connect(animation, &QPropertyAnimation::finished,
            animation, &QPropertyAnimation::deleteLater);

    QAbstractButton::mousePressEvent(event);

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
