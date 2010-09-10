#include "qsqldialog.h"

#include <QtGui>
#include <QtSql/QSqlDatabase>

QSqlDialog::QSqlDialog(QWidget *parent) : QDialog(parent)
{
    QGridLayout *gridLayout = new QGridLayout();
    QLabel *labelDriver = new QLabel(tr("Driver"));
   // gridLayout->addWidget(, 0, 0);
    gridLayout->addWidget(labelDriver, 0, 0);

    gridLayout->addWidget(new QLabel(tr("Database Name")), 1, 0);
    gridLayout->addWidget(new QLabel(tr("User Name")), 2, 0);
    gridLayout->addWidget(new QLabel(tr("Password")), 3, 0);
    gridLayout->addWidget(new QLabel(tr("Hostname")), 4, 0);
    gridLayout->addWidget(new QLabel(tr("Port")), 5, 0);

    driverBox = new QComboBox();

    QStringList drivers = QSqlDatabase::drivers();
    driverBox->addItems(drivers);
    gridLayout->addWidget(driverBox, 0, 1);
    DBEdit = new QLineEdit();
    gridLayout->addWidget(DBEdit, 1, 1);
    userNameEdit = new QLineEdit();
    gridLayout->addWidget(userNameEdit, 2, 1);
    passwordEdit = new QLineEdit();
    gridLayout->addWidget(passwordEdit, 3, 1);
    hostnameEdit = new QLineEdit();
    gridLayout->addWidget(hostnameEdit, 4, 1);
    portBox = new QSpinBox();
    gridLayout->addWidget(portBox, 5, 1);
    predefineBox = new QCheckBox(tr("User Predefine Memory Database"));
    connect(predefineBox, SIGNAL(clicked()), this, SLOT(changeStatus()));
    gridLayout->addWidget(predefineBox, 6, 1);

    okButton = new QPushButton(tr("Ok"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonSlot()));
    gridLayout->addWidget(okButton,7, 0);
    cancelButton = new QPushButton(tr("Cancel"));
    gridLayout->addWidget(cancelButton, 7, 1);

    QGroupBox *groupBox = new QGroupBox();
    groupBox->setLayout(gridLayout);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);
}

QSqlDialog::~QSqlDialog()
{

}

QString QSqlDialog::driverName()
{
    return driverBox->currentText();
}

QString QSqlDialog::databaseName()
{
    return DBEdit->text();
}

QString QSqlDialog::userName()
{
    return userNameEdit->text();
}

QString QSqlDialog::password()
{
    return passwordEdit->text();
}

QString QSqlDialog::hostName()
{
    return hostnameEdit->text();
}

int QSqlDialog::port()
{
    return portBox->value();
}

void QSqlDialog::okButtonSlot()
{
    accept();
}

void QSqlDialog::changeStatus()
{
    driverBox->setEnabled(!driverBox->isEnabled());
    DBEdit->setEnabled(!DBEdit->isEnabled());
    userNameEdit->setEnabled(!userNameEdit->isEnabled());
    passwordEdit->setEnabled(!passwordEdit->isEnabled());
    hostnameEdit->setEnabled(!hostnameEdit->isEnabled());
    portBox->setEnabled(!portBox->isEnabled());
}

bool QSqlDialog::useInMemoryDatabase()
{
    return predefineBox->isChecked();
}
