#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "azurelogindialog.h"
#include "Header.h"
#include <QSettings>
#include <tuple>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    QString getAccName() const;
    QString getSafetyKey() const;
    QString getContainer() const;

    void setAccName(const QString&);
    void setSafetyKey(const QString&);
    void setContainer(const QString&);

    void setDeleteWarning(bool);
    bool getDeleteWarning()const;

    void setTheme(const Theme&);
    Theme getTheme()const;

    void setLocationSave(const SaveLocation&);
    SaveLocation getLocationSave() const;

protected:
    void paintEvent(QPaintEvent* event) override;

    void closeEvent(QCloseEvent * event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void themeChanged(const Theme);

    void saveLocationChanged(const SaveLocation);

    void safeDeleteChanged(const bool);

    void creditentalsChanged(const std::tuple<QString, QString, QString>);

    void requestTerminate();

    void moved(const QPoint);

private slots:
    void backToSticky();

    void showLogInDialog();

    void setCreditentals();

    void setAskLabel();

    void darkButton_toggled(bool);

    void lightButton_toggled(bool);

    void changeLocation(int);

private:
    void writeSettings();

private:
    Ui::SettingsDialog *ui;

    AzureLoginDialog* dialog = nullptr;

    QString accName;
    QString safetyKey;
    QString container;

    Theme theme;

    SaveLocation location;

    bool askDelete;

    QPoint startPos;
    bool isMousePressed = false;
    bool isMouseHoverFrame = false;
};

#endif // SETTINGSDIALOG_H
