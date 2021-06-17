#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QPainter>
#include <QMouseEvent>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->comboLocation->setDisabled(true);

    connect (ui->exitButton, &QPushButton::clicked,
             this, &SettingsDialog::requestTerminate);

    connect(ui->backButton, &QPushButton::clicked,
            this, &SettingsDialog::backToSticky);

    connect(ui->logInButton, &QPushButton::clicked,
            this, &SettingsDialog::showLogInDialog);

    connect(ui->switchButton, &QAbstractButton::clicked,
            this, &SettingsDialog::setAskLabel);

    connect(ui->darkButton, &QRadioButton::toggled,
            this, &SettingsDialog::darkButton_toggled);

    connect(ui->lightButton, &QRadioButton::toggled,
            this, &SettingsDialog::lightButton_toggled);

    connect(ui->comboLocation, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::changeLocation);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QString SettingsDialog::getAccName() const
{
    return accName;
}

QString SettingsDialog::getSafetyKey() const
{
    return safetyKey;
}

QString SettingsDialog::getContainer() const
{
    return container;
}

void SettingsDialog::setAccName(const QString& name)
{
    accName = name;
    if(!accName.isEmpty() && safetyKey.isEmpty() && container.isEmpty())
        ui->comboLocation->setEnabled(true);
}

void SettingsDialog::setSafetyKey(const QString& key)
{
    safetyKey = key;
    if(!accName.isEmpty() && safetyKey.isEmpty() && container.isEmpty())
        ui->comboLocation->setEnabled(true);
}

void SettingsDialog::setContainer(const QString& container_)
{
    container = container_;
    if(!accName.isEmpty() && safetyKey.isEmpty() && container.isEmpty())
        ui->comboLocation->setEnabled(true);
}

void SettingsDialog::setDeleteWarning(bool ask)
{
    ui->switchButton->setChecked(ask);
    askDelete = ask;

    if(askDelete)
        ui->askLabel->setText(QObject::tr("Ask"));
    else
        ui->askLabel->setText(QObject::tr("Don't ask"));

    emit safeDeleteChanged(ui->switchButton->isChecked());
}

bool SettingsDialog::getDeleteWarning() const
{
    return askDelete;
}

void SettingsDialog::setTheme(const Theme& theme_)
{
    if (theme_ == Theme::Dark)
        ui->darkButton->setChecked(true);
    else
        ui->lightButton->setChecked(true);
}

Theme SettingsDialog::getTheme()const
{
    return theme;
}

void SettingsDialog::setLocationSave(const SaveLocation& location_)
{
    int index = static_cast<int>(location_);
    ui->comboLocation->setCurrentIndex(index);
    location = location_;
}

SaveLocation SettingsDialog::getLocationSave() const
{
    return location;
}

void SettingsDialog::writeSettings()
{
    QSettings options("XD", "StickyNotes");
    options.beginGroup("Settings");
    options.setValue("AccName", accName);
    options.setValue("AccKey", safetyKey);
    options.setValue("Container", container);
    options.setValue("AskDelete", askDelete);
    options.setValue("Theme", static_cast<int>(theme));
    options.setValue("Location", static_cast<int>(location));
    options.endGroup();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void SettingsDialog::paintEvent([[maybe_unused]]QPaintEvent* event)
{
    QPainter painter(this);

    QPen pen(Qt::transparent);
    painter.setPen(pen);
    painter.setBrush(palette().base());

    painter.drawRoundedRect(rect(), 10, 10);
}

void SettingsDialog::closeEvent(QCloseEvent * event)
{
    writeSettings();
    QDialog::closeEvent(event);
}

void SettingsDialog::mousePressEvent(QMouseEvent *event)
{
    QRect frameRect = ui->menuLayout->contentsRect();
    isMouseHoverFrame = frameRect.contains(event->pos());

    isMousePressed = true;

    startPos = event->pos();

    QDialog::mousePressEvent(event);
}

void SettingsDialog::mouseReleaseEvent(QMouseEvent *event)
{

    isMousePressed = false;
    QDialog::mouseReleaseEvent(event);
}

void SettingsDialog::mouseMoveEvent(QMouseEvent* event)
{
    if(isMousePressed  && isMouseHoverFrame )
    {
        QPoint deltaPos = event->pos() - startPos;
        move(pos() + deltaPos);

        emit moved(pos() +deltaPos);
    }
    QDialog::mouseMoveEvent(event);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
///////////////////////////////////////////////////////SLOTS
void SettingsDialog::backToSticky()
{
    close();
}

void SettingsDialog::showLogInDialog()
{
    dialog = new AzureLoginDialog;

    dialog->setAccName(accName);
    dialog->setSafetyKey(safetyKey);
    dialog->setContainer(container);
    dialog->open();

    connect(dialog, &QDialog::finished,
            this, &SettingsDialog::setCreditentals);
}

void SettingsDialog::setCreditentals()
{
    if(dialog->result() == QDialog::Accepted)
    {
        accName = dialog->getAccName();
        safetyKey = dialog->getSafetyKey();
        container = dialog->getContainerID();
        ui->comboLocation->setEnabled(true);

        emit creditentalsChanged(std::make_tuple(accName, safetyKey, container));
    }

    dialog->deleteLater();
}

void SettingsDialog::setAskLabel()
{
    if(ui->switchButton->isChecked())
        ui->askLabel->setText(QObject::tr("Ask"));
    else
        ui->askLabel->setText(QObject::tr("Don't ask"));

    askDelete = !askDelete;

    emit safeDeleteChanged(ui->switchButton->isChecked());
}

void SettingsDialog::darkButton_toggled(bool toggled)
{
    if(toggled)
    {
        theme = Theme::Dark;

        QPalette pal = palette();
        pal.setBrush(QPalette::Base, QBrush(QColor(61, 61, 61)));
        pal.setBrush(QPalette::WindowText, Qt::white);
        setPalette(pal);

        emit themeChanged(theme);
    }
}

void SettingsDialog::lightButton_toggled(bool toggled)
{
    if(toggled)
    {
        theme = Theme::Light;

        QPalette pal = palette();
        pal.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
        pal.setBrush(QPalette::WindowText, Qt::black);
        setPalette(pal);

        emit themeChanged(theme);
    }
}

void SettingsDialog::changeLocation(int index)
{
    location = static_cast<SaveLocation>(index);
    emit saveLocationChanged(location);
}
///////////////////////////////////////////////////////SLOTS
