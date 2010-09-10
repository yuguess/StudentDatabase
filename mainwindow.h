#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QSqlDatabase>

class QGroupBox;
class QTreeWidget;
class QTreeWidgetItem;
class QTableView;
class QPushButton;
class QLineEdit;
class QAction;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QGroupBox * createDatabase();
    QGroupBox * createStudentInfo();
    QGroupBox * createQueryInfo();
    void createMenuBar();
    void showTable(const QString &t);
    QSqlDatabase currentDatabase() const;

private:
    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
                  const QString &user, const QString &passwd, int port = -1);
    QString qDBCaption(const QSqlDatabase &db);
    void qSetBold(QTreeWidgetItem *item, bool bold);
    void setActive(QTreeWidgetItem *item);
    void DBComboBoxUpdate();
    void setDBComboBox(QString DBName);
    void setTableComboBox(QString TableName);

    QTreeWidget* tree;
    QTableView* table;
    QPushButton* queryOkButton;
    QPushButton* queryClearButton;
    QLineEdit* queryLineEdit;
    QString activeDb;
    QAction *refreshAction;
    QAction *deleteRowAction;
    QAction *insertRowAction;
    QComboBox *queryDBBox;
    QComboBox *queryTableBox;
    QComboBox *queryColBox;

public slots:
    void addConnection();
    void treeUpdate();
    void treeItemSlot(QTreeWidgetItem *item, int column);
    //void on_tree_itemActivated();
    void updateActions();
    void insertRow();
    void deleteRow();
    void adjustTable(QString);
    void adjustQueryColumn(QString);
    void queryOkButtonSlot();
    void queryClearButtonSlot();
    /*
    void on_insertRowAction_triggered()
    { insertRow(); }
    void on_deleteRowAction_triggered()
    { deleteRow(); }*/
};

#endif // MAINWINDOW_H
