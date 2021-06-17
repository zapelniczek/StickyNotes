#include "azurelogindialog.h"
#include "ui_azurelogindialog.h"
#include <QMouseEvent>
#include <QPainter>

AzureLoginDialog::AzureLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AzureLoginDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setDialogButtons();
    letProceed();

    connect(ui->accLine, &QLineEdit::textChanged,
            this, &AzureLoginDialog::letProceed);

    connect(ui->keyLine, &QLineEdit::textChanged,
            this, &AzureLoginDialog::letProceed);

    connect(ui->containerName, &QLineEdit::textChanged,
            this, &AzureLoginDialog::letProceed);

    connect(ui->echoButton, &QPushButton::clicked,
            this, &AzureLoginDialog::changeKeyEchoMode);
}

AzureLoginDialog::~AzureLoginDialog()
{
    delete ui;
}

QString AzureLoginDialog::getAccName() const
{
    return ui->accLine->text();
}

QString AzureLoginDialog::getSafetyKey() const
{
    return ui->keyLine->text();
}

QString AzureLoginDialog::getContainerID() const
{
    return ui->containerName->text();
}

void AzureLoginDialog::setAccName(const QString& name)
{
    ui->accLine->setText(name);
}

void AzureLoginDialog::setSafetyKey(const QString& key)
{
    ui->keyLine->setText(key);
}

void AzureLoginDialog::setContainer(const QString& container)
{
    ui->containerName->setText(container);
}

void AzureLoginDialog::setDialogButtons()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setStyleSheet(QStringLiteral("QPushButton{border: 1 solid gray;\n"
    "border-radius:15;\n width:90;\n height:30;\n"
    "background-color:rgb(222, 222, 222);\n}\n"
    "QPushButton:hover { background-color:#B2B2AE;}"));

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setStyleSheet(QStringLiteral("QPushButton{border: 1 solid gray;\n"
    "border-radius:15;\n width:90;\n height:30;\n"
    "background-color:rgb(222, 222, 222);\n}\n"
    "QPushButton:hover { background-color:#B2B2AE;}"));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
void AzureLoginDialog::paintEvent([[maybe_unused]] QPaintEvent* event)
{
    QPainter painter (this);
    painter.setPen(Qt::black);
    painter.setBrush(palette().base());

    QRectF rec(rect().x() + 1 / 2, rect().y() + 1 / 2,
               rect().width() - 1, rect().height() - 1);

    painter.drawRoundedRect(rec.translated(0.5, 0.5), 20, 20);
}

void AzureLoginDialog::mousePressEvent(QMouseEvent *event)
{
    QRect frameRect = ui->mainLabel->contentsRect();
    isMouseHoverFrame = frameRect.contains(event->pos());

    isMousePressed = true;

    startPos = event->pos();

    QDialog::mousePressEvent(event);
}

void AzureLoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    QDialog::mouseReleaseEvent(event);
}

void AzureLoginDialog::mouseMoveEvent(QMouseEvent* event)
{
    if(isMousePressed  && isMouseHoverFrame )
    {
        QPoint deltaPos = event->pos() - startPos;
        move(pos() + deltaPos);
    }
    QDialog::mouseMoveEvent(event);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~EVENTS
//**************************************************************SLOTS
void AzureLoginDialog::letProceed()
{
    bool line_1 = ui->accLine->text().isEmpty();
    bool line_2 = ui->keyLine->text().isEmpty();
    bool line_3 = ui->containerName->text().isEmpty();

    if (line_1 || line_2 || line_3)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
}

void AzureLoginDialog::changeKeyEchoMode()
{
    if(ui->keyLine->echoMode() == QLineEdit::Normal)
    {
        ui->keyLine->setEchoMode(QLineEdit::Password);
        ui->echoButton->setIcon(QIcon(QStringLiteral(":/AzureLogin/Icons/images/view.png")));
        ui->echoButton->setToolTip(tr("Click to show account key"));
    }
    else
    {
        ui->keyLine->setEchoMode(QLineEdit::Normal);
        ui->echoButton->setIcon(QIcon(QStringLiteral(":/AzureLogin/Icons/images/hide.png")));
        ui->echoButton->setToolTip(tr("Click to hide account key"));
    }
}
//**************************************************************SLOTS
