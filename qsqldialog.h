#ifndef QSQLDIALOG_H
#define QSQLDIALOG_H

#include <QtGui/QDialog>
#include <QObject>

class QLineEdit;
class QCheckBox;
class QComboBox;
class QSpinBox;

class QSqlDialog : public QDialog
{
    Q_OBJECT

public:
    QSqlDialog(QWidget* parent = 0);
    ~QSqlDialog();
    bool useInMemoryDatabase();
    QString driverName();
    QString databaseName();
    QString userName();
    QString password();
    QString hostName();
    int port();

private:
    QComboBox *driverBox;
    QLineEdit *DBEdit;
    QLineEdit *userNameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *hostnameEdit;
    QSpinBox  *portBox;
    QCheckBox *predefineBox;
    QPushButton *okButton;
    QPushButton *cancelButton;

private slots:
    void changeStatus();
    void okButtonSlot();
};

#endif // QSQLDIALOG_H
