#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "entrydialog.h"

#include <iostream>

#include <QMainWindow>
#include <QListView>
#include <QMenuBar>
#include <QMessageBox>
#include <QFile>
#include <QVector>
#include <QModelIndex>
#include <algorithm>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QStringList JsonObjectNames; //list of names of the recipes
    QVector<QJsonObject> JsonObjectList; //vector of objects containing information abt. recipes

signals:
    void recipeSelected();

private slots:
    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_btnEdit_clicked();

    void slotSelectionChange(const QItemSelection &, const QItemSelection &);

    void updateRecipes();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *recipesModel;
    QJsonObject recipesJson;

    //reading recipes from Json file
    bool readJsonFile(QString filename);
    //saving changes
    bool saveJsonFile(QString filename);
    //loading recipes into the table:
    void loadRecipeList(QStringList nameList, QVector<QJsonObject> valueList);

    void deleteRecipes();
};
#endif // MAINWINDOW_H
