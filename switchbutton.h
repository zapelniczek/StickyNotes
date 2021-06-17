#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QAbstractButton>

class SwitchButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(QRect cRect READ getCircleRect WRITE setCircleRect)

private:
    bool checked = false;

    QRect circleRect;

protected:
    void paintEvent (QPaintEvent* ) override;

    void mousePressEvent (QMouseEvent* ) override;

public:
    explicit SwitchButton(QWidget* parent = nullptr);

    void setChecked(bool);

    bool isChecked() const;

    void setCircleRect(const QRect&);

    QRect getCircleRect() const;
};

#endif // SWITCHBUTTON_H
