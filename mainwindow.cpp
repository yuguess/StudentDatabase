#include "mainwindow.h"
#include "qsqldialog.h"
#include <QtGui>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlDriver>
#include <QSqlRecord>
#include <iostream>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QGroupBox *database = createDatabase();
    QGroupBox *studentInfo = createStudentInfo();
    QGroupBox *queryInfo = createQueryInfo();

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(database, 0, 0, 3, 1);
    mainLayout->addWidget(studentInfo, 0, 1, 2, 2);
    mainLayout->addWidget(queryInfo, 2, 1, 1, 2);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnMinimumWidth(1, 500);
    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
    createMenuBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenuBar()
{

    QMenu *fileMenu = this->menuBar()->addMenu(QObject::tr("&File"));
    fileMenu->addAction(QObject::tr("Add &Connection..."), this, SLOT(addConnection()));
    fileMenu->addSeparator();
    fileMenu->addAction(QObject::tr("&Quit"), this, SLOT(close()));
}

void MainWindow::addConnection()
{
    QSqlDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (dialog.useInMemoryDatabase())
    {
        QSqlDatabase::database(":memory:", false).close();
        QSqlDatabase::removeDatabase(":memory:");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", ":memory:");
        db.setDatabaseName(":memory:");
        if (!db.open())
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                                                         "opening the connection: ") + db.lastError().text());
        QSqlQuery q("", db);
        q.exec("drop table Movies");
        q.exec("drop table Names");
        q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
        q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
        q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
        q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
        q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
        q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
        q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
        q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
        q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
        q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
        q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
        q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
        treeUpdate();
        DBComboBoxUpdate();
    } else
    {
        QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                           dialog.userName(), dialog.password(), dialog.port());
        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
    }
}

QSqlError MainWindow::addConnection(const QString &driver, const QString &DBName, const QString &host,
                                    const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;
    QSqlError err;
   // QSqlDatabase::database
   // QSqlDatabase db = QSqlDatabase::addDatabase(driver, DBName);
    QSqlDatabase DBTest = QSqlDatabase::database(DBName);
    if (DBTest.isOpen())
    {
        QMessageBox::warning(this, "Warning", tr("The %1 DataBase already open").arg(DBName));
        return err;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(driver, DBName);
    db.setDatabaseName(DBName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
    }
    treeUpdate();
    DBComboBoxUpdate();
    /*
    if (!db.open())
    {
        db.setDatabaseName(DBName);
        db.setHostName(host);
        db.setPort(port);
        if (!db.open(user, passwd)) {
            err = db.lastError();
            db = QSqlDatabase();
            QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
        }
        treeUpdate();
        DBComboBoxUpdate();
    }*/
    return err;
}

void MainWindow::DBComboBoxUpdate()
{
    queryDBBox->clear();
    QStringList connectionNames = QSqlDatabase::connectionNames();
    queryDBBox->addItems(connectionNames);
}

void MainWindow::treeUpdate()
{
    tree->clear();
    QStringList connectionNames = QSqlDatabase::connectionNames();

    bool gotActiveDb = false;
    for (int i = 0; i < connectionNames.count(); ++i) {
        QTreeWidgetItem *root = new QTreeWidgetItem(tree);
        QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
        root->setText(0, qDBCaption(db));
        if (connectionNames.at(i) == activeDb) {
            gotActiveDb = true;
            setActive(root);
        }
        if (db.isOpen()) {
            QStringList tables = db.tables();
            for (int t = 0; t < tables.count(); ++t) {
                QTreeWidgetItem *tableItem = new QTreeWidgetItem(root);
                QString str = tables.at(t);
                std::string stdStr = str.toStdString();
                tableItem->setText(0, tr(stdStr.c_str()));
                //QTextCodec* gbkCodec = QTextCodec::codecForName("GB2312");
                /*std::string stdStr = str.toStdString();
                std::ofstream out;
                out.open("D://tt.txt");
                out << stdStr;
                out << "哈哈";
                out.close(); */
                //QString gbkString = gbkCodec->toUnicode(stdStr.c_str());
                //tableItem->setText(0, str);
            }
        }
    }

    if (!gotActiveDb) {
        activeDb = connectionNames.value(0);
        setActive(tree->topLevelItem(0));
    }

    tree->doItemsLayout(); // HACK
}

QString MainWindow::qDBCaption(const QSqlDatabase &db)
{
    QString nm = db.driverName();
    nm.append(QLatin1Char(':'));
    if (!db.userName().isEmpty())
        nm.append(db.userName()).append(QLatin1Char('@'));
    nm.append(db.databaseName());
    return nm;
}

void MainWindow::qSetBold(QTreeWidgetItem *item, bool bold)
{
    QFont font = item->font(0);
    font.setBold(bold);
    item->setFont(0, font);
}

void MainWindow::setActive(QTreeWidgetItem *item)
{
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        if (tree->topLevelItem(i)->font(0).bold())
            qSetBold(tree->topLevelItem(i), false);
    }

    if (!item)
        return;

    qSetBold(item, true);
    activeDb = QSqlDatabase::connectionNames().value(tree->indexOfTopLevelItem(item));
}


QGroupBox * MainWindow::createDatabase()
{
    QGroupBox * database = new QGroupBox(tr("Database"));

    tree = new QTreeWidget();
    connect(tree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this,
            SLOT(treeItemSlot(QTreeWidgetItem*,int)));
//    tree->setObjectName(QLatin1String("tree"));
    tree->setHeaderLabels(QStringList(tr("database")));
//   tree->header()->setResizeMode(QHeaderView::Stretch);
    refreshAction = new QAction(tr("Refresh"), tree);
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(treeUpdate()));
    tree->addAction(refreshAction);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);
    QMetaObject::connectSlotsByName(this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tree);
    database->setLayout(layout);

    return database;
}

QGroupBox * MainWindow::createStudentInfo()
{
    QGroupBox *studentInfo = new QGroupBox(tr("Student Information"));

    table = new QTableView();
    insertRowAction = new QAction(tr("Insert Row"), table);
    deleteRowAction = new QAction(tr("Delete Row"), table);
    connect(insertRowAction, SIGNAL(triggered()), this, SLOT(insertRow()));
    connect(deleteRowAction, SIGNAL(triggered()), this, SLOT(deleteRow()));
    table->addAction(insertRowAction);
    table->addAction(deleteRowAction);
    table->setContextMenuPolicy(Qt::ActionsContextMenu);
    updateActions();
    if (QSqlDatabase::drivers().isEmpty())
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This demo requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(table);
    studentInfo->setLayout(layout);
    return studentInfo;
}

QGroupBox * MainWindow::createQueryInfo()
{
    QGroupBox *queryInfo = new QGroupBox(tr("Query"));

    queryDBBox = new QComboBox();
    queryTableBox = new QComboBox();
    queryColBox = new QComboBox();

    connect(queryDBBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(adjustTable(QString)));
    connect(queryTableBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(adjustQueryColumn(QString)));

    queryLineEdit = new QLineEdit();
    queryOkButton = new QPushButton(tr("Ok"));
    queryClearButton = new QPushButton(tr("Clear"));
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(new QLabel((tr("数据库"))), 0, 0);
    queryDBBox->setFixedWidth(110);
    layout->addWidget(queryDBBox, 0, 1);   
    layout->addWidget(new QLabel((tr("数据表"))), 1, 0);
    queryTableBox->setFixedWidth(110);
    layout->addWidget(queryTableBox, 1, 1);
    layout->addWidget(new QLabel((tr("数据列"))), 2, 0);
    queryColBox->setFixedWidth(110);
    layout->addWidget(queryColBox, 2, 1);
    layout->addWidget(new QLabel((tr("查询"))), 3, 0);
    queryLineEdit->setFixedWidth(110);
    layout->addWidget(queryLineEdit, 3, 1);
    layout->addWidget(queryOkButton, 4, 0);
    connect(queryOkButton, SIGNAL(clicked()), this, SLOT(queryOkButtonSlot()));
    queryClearButton->setFixedWidth(80);
    layout->addWidget(queryClearButton, 4, 1);
    connect(queryClearButton, SIGNAL(clicked()), this, SLOT(queryClearButtonSlot()));
    queryInfo->setLayout(layout);
    return queryInfo;
}

void MainWindow::queryOkButtonSlot()
{

    QString DBName = queryDBBox->itemText(queryDBBox->currentIndex());
    QSqlDatabase db = QSqlDatabase::database(DBName);
    QString tableName = queryTableBox->itemText(queryTableBox->currentIndex());
    QString colName = queryColBox->itemText(queryColBox->currentIndex());
    QString queryPara = queryLineEdit->text();
//    queryPara.is
    QString queryText = "SELECT * FROM " + tableName + " WHERE " + colName + "= '" + queryPara + "'";
    QMessageBox::about(this, "Test", queryText);
    //QSqlQuery query(queryText, db);

    QSqlQueryModel *model = new QSqlQueryModel(table);
    model->setQuery(QSqlQuery(queryText, db));
    table->setModel(model);

    if (model->lastError().type() != QSqlError::NoError)
    {
        QString queryText = "SELECT * FROM " + tableName + " WHERE " + colName + "= " + queryPara;
        QMessageBox::about(this, "Test", queryText);
        //QSqlQuery query(queryText, db);

        QSqlQueryModel *model = new QSqlQueryModel(table);
        model->setQuery(QSqlQuery(queryText, db));
        table->setModel(model);
    }

    updateActions();
}

void MainWindow::queryClearButtonSlot()
{
    queryLineEdit->clear();
}

void MainWindow::setDBComboBox(QString DBName)
{
    //int curIndex = queryDBBox->findText(DBName);
    queryDBBox->setItemText(queryDBBox->currentIndex(), DBName);
    //queryDBBox->setCurrentIndex(curIndex);
}

void MainWindow::setTableComboBox(QString tableName)
{
    queryTableBox->clear();
    QStringList currentDBTables = currentDatabase().tables();
    queryTableBox->addItems(currentDBTables);
    int curIndex = queryDBBox->findText(tableName);
    queryTableBox->setCurrentIndex(curIndex);
}

void MainWindow::adjustTable(QString DBName)
{
    queryTableBox->clear();
    QSqlDatabase db = QSqlDatabase::database(DBName);
    QStringList currentDBTables = db.tables();
    queryTableBox->addItems(currentDBTables);
}

void MainWindow::adjustQueryColumn(QString tableName)
{
    queryColBox->clear();
    QString DBName = queryDBBox->itemText(queryDBBox->currentIndex());
    QSqlDatabase db = QSqlDatabase::database(DBName);
    QString queryText = "SELECT * FROM " + tableName;
    QSqlQuery query(queryText, db);

    // QSqlDatabase db = QSqlDatabase::database(":memory:");
    // QSqlDatabase db = currentDatabase();
    // QSqlQueryModel *model = new QSqlQueryModel(table);
    // model->setQuery(QSqlQuery(queryText, currentDatabase()));
    // model->setQuery(QSqlQuery(sqlEdit->toPlainText(), connectionWidget->currentDatabase()));
    // table->setModel(model);

    QSqlRecord queryRecord = query.record();

    //QMessageBox::about(this,"Test", tr("haha%1 %2").arg(t).arg(tableName));
    QStringList fieldNameList;

    for (int i = 0; i < queryRecord.count(); i++)
    {
        fieldNameList << queryRecord.fieldName(i);
    }
    queryColBox->addItems(fieldNameList);
}

QSqlDatabase MainWindow::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

void MainWindow::showTable(const QString &t)
{
    QSqlTableModel *model = new QSqlTableModel(table, currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->select();
//    if (model->lastError().type() != QSqlError::NoError)
//        emit statusMessage(model->lastError().text());
    table->setModel(model);
    table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateActions()));
    updateActions();
}

void MainWindow::updateActions()
{
    bool enableIns = qobject_cast<QSqlTableModel *>(table->model());
    bool enableDel = enableIns && table->currentIndex().isValid();

    insertRowAction->setEnabled(enableIns);
    deleteRowAction->setEnabled(enableDel);
}

void MainWindow::treeItemSlot(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

    if (!item->parent()) {
        setActive(item);
 //       setDBComboBox(item->text(0));
    } else {
        setActive(item->parent());
//        setDBComboBox(item->parent()->text(0));
//        setTableComboBox(item->text(0));
        showTable(item->text(0));
    }
}

void MainWindow::insertRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    table->edit(insertIndex);
}

void MainWindow::deleteRow()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < currentSelection.count(); ++i) {
        if (currentSelection.at(i).column() != 0)
            continue;
        model->removeRow(currentSelection.at(i).row());
    }

    model->submitAll();
    model->setEditStrategy(QSqlTableModel::OnRowChange);

    updateActions();
}
