#ifndef AZURELOGINDIALOG_H
#define AZURELOGINDIALOG_H

#include <QDialog>

namespace Ui {
class AzureLoginDialog;
}

class AzureLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AzureLoginDialog(QWidget *parent = nullptr);
    ~AzureLoginDialog();

    QString getAccName() const;
    QString getSafetyKey() const;
    QString getContainerID() const;

    void setAccName(const QString&);
    void setSafetyKey(const QString&);
    void setContainer(const QString&);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void setDialogButtons();

private slots:
    void letProceed();
    void changeKeyEchoMode();

private:
    Ui::AzureLoginDialog *ui;

    bool isMouseHoverFrame;
    bool isMousePressed;
    QPoint startPos;
};

#endif // AZURELOGINDIALOG_H
