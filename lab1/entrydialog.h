#ifndef ENTRYDIALOG_H
#define ENTRYDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QVector>
#include <QModelIndex>
#include <QModelIndexList>
#include <algorithm>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


namespace Ui {
class EntryDialog;
}

class EntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EntryDialog(QWidget *parent = nullptr);
    ~EntryDialog();

    QJsonObject newRecipe;
    QString newRecipeName;

    QStringList JsonObjectNames; //list of names of the recipes
    QVector<QJsonObject> JsonObjectList; //vector of objects containing information abt. recipes

    bool addingMode; //bool 1-adding, 0-editing
    int indexEdited; //index nr of recipe to be edited

    void fillNameAndBody();
    void fillIngredients();
    void loadNameAndBody();
    void updateIngredients();
    void deleteSelectedIngredients();
    bool saveJsonFile(QString filename);

signals:
    void recipeSaved();

private slots:
    void on_btnCancel_clicked();

    void on_btnAddIngredient_clicked();

    void on_btnDeleteIngredient_clicked();

    void on_btnSaveRecipe_clicked();

private:
    Ui::EntryDialog *ui;
    QStandardItemModel *ingredientsModel;
};

#endif // ENTRYDIALOG_H
