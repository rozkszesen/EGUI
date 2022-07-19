#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "entrydialog.h"
#include "ui_entrydialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //set window size
    setMinimumSize(700, 450);

    readJsonFile("../laboratories/recipes.json");
    loadRecipeList(this->JsonObjectNames, this->JsonObjectList);

    saveJsonFile("../laboratories/recipesNew.json");

    ui->btnDelete->setEnabled(false);
    ui->btnEdit->setEnabled(false);

    connect(ui->listOfRecipes->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                SLOT(slotSelectionChange(const QItemSelection &, const QItemSelection &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadRecipeList(QStringList nameList, QVector<QJsonObject> objectList)
{
    //create a model for the table
    recipesModel = new QStandardItemModel(nameList.length(), 3, this);

    QJsonObject object;
    //for each element in the list of recipe names
    for(int row = 0; row < nameList.length(); ++row)
    {
        for(int col = 0; col < recipesModel->columnCount() ; ++col)
        {
           //index required for setData method
           QModelIndex index = recipesModel->index(row, col, QModelIndex());

           //assign the "value" of the current object
           object = objectList[row];

           if(col==0)
           {
               //read recipe names
               recipesModel->setData(index, nameList[row]);
           }
           else if(col==1)
           {
               //load the recipe verses to an array
               QJsonArray recipe = object.value("recipe").toArray();
               QString recipeText = "";

               //read each line of the text
               foreach(const auto temp, recipe)
               {
                   //add <new line> after each line of the text
                   recipeText += temp.toString() + "\n";
               }
               recipesModel->setData(index, recipeText);
           }
           else
           {
                //read ingredients
                QStringList ingredients = object.keys();
                QString ingredientString = "";

                foreach(auto elem, ingredients)
                {
                    //ommit the first element: "recipe"
                    if(elem != "recipe")
                    {
                       //string with all ingredients separated with <new lines>
                       ingredientString += elem + " " + object.value(elem).toString() + "\n";
                    }
               }
               recipesModel->setData(index, ingredientString);
            }
        }
    }

    //set created model as model for TableView in MainWindow
    ui->listOfRecipes->setModel(recipesModel);

    //setting the labels for columns
    QStringList labels = {"Title","Recipe","Ingredients"};
    recipesModel->setHorizontalHeaderLabels(labels);

    //visual adjustments
    ui->listOfRecipes->setColumnWidth(0,100);
    ui->listOfRecipes->setColumnWidth(1,300);
    ui->listOfRecipes->resizeColumnToContents(2);
    ui->listOfRecipes->resizeRowsToContents();
}

void MainWindow::updateRecipes()
{
    //open the previously updated and saved document
    readJsonFile("../laboratories/recipesNew.json");

    //load the recipes to the table
    loadRecipeList(JsonObjectNames, JsonObjectList);
}

bool MainWindow::readJsonFile(QString filename)
{
    QFile file(filename);

    //check if the file was properly opened
    if (!file.open(QIODevice::ReadWrite))
    {
        //show message in MessageBox
        QMessageBox::critical(this, "Error", "Json file did not open");
        return false;
    }

    //read the Json document
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
    file.close();

    //check if the document contains an object
    if(!jsonDocument.isNull())
    {
        if(jsonDocument.isObject())
        {
            QJsonObject jsonData = jsonDocument.object();
            //temporary place for storing jsonObject keys and values:
            QStringList objectNames;
            QVector<QJsonObject> objectValues;

            //iterator to parse through the jsonRecipes elements
            QJsonObject::iterator it;
            //temporary Json Object to hold converted item values
            QJsonObject temp;


            //loop for parsing the elements:
            for(it = jsonData.begin(); it != jsonData.end(); ++it)
            {
                //add the item's key to the list of recipe names
                objectNames.push_back(it.key());
                //converting the item's value to an objectt
                temp = it.value().toObject();
                //add the item's "value" to the list
                objectValues.push_back(temp);
            }

            this->JsonObjectNames = objectNames;
            this->JsonObjectList = objectValues;
        }
        else
        {
            QMessageBox::critical(this, "Error", "Json file did not contain an object");
            return false;
        }
    }

return false;
}

bool MainWindow::saveJsonFile(QString filename)
{
    //object to update and insert in json document
    QJsonObject recipesToSave;

    for(int i=0; i < this->JsonObjectNames.size(); i++)
    {
        recipesToSave.insert(JsonObjectNames[i], JsonObjectList[i]);
    }
    //creating json document
    QJsonDocument newDocument(recipesToSave);

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Error while saving Json file", "Json file did not open");
        return false;
    }

    file.write(newDocument.toJson());
    file.close();
    return true;
}

void MainWindow::slotSelectionChange(const QItemSelection &, const QItemSelection &)
{
    //getting indexes of selected rows
    QModelIndexList selectedRows = ui->listOfRecipes->selectionModel()->selectedRows();
    if(selectedRows.isEmpty())
    {
        //if no rows are selected: buttons disactivated
        ui->btnEdit->setEnabled(false);
        ui->btnDelete->setEnabled(false);
    }
    else if (selectedRows.size() == 1)
    {
        ui->btnDelete->setEnabled(true);
        ui->btnEdit->setEnabled(true);
    }
    else
    {
        ui->btnDelete->setEnabled(true);
        ui->btnEdit->setEnabled(false);
    }
}

void MainWindow::deleteRecipes()
{
    //rows selected to delete
    QModelIndexList selection = ui->listOfRecipes->selectionModel()->selectedRows();
    QList<int> rowsToDelete;

    foreach(QModelIndex index, selection)
        rowsToDelete += index.row();

    //reverse the order so that the indices do not change for the items that are to be deleted
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    qDebug("selection size %d", selection.size());
    foreach(int row, rowsToDelete)
    {
        qDebug("removed recipe row %d", row);
        //remove from variables storing Names and Objects from JsonFile
        JsonObjectList.removeAt(row);
        JsonObjectNames.removeAt(row);
    }

    saveJsonFile("../laboratories/recipesNew.json");
    loadRecipeList(JsonObjectNames, JsonObjectList);

    //bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
}

void MainWindow::on_btnDelete_clicked()
{
    //display message asking for confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete","Do you want to delete the recipe(s)?", QMessageBox::Yes | QMessageBox::No);

    //delete only if Yes was answered
    if (reply == QMessageBox::Yes)
            deleteRecipes();
    else
        return;
}

void MainWindow::on_btnAdd_clicked()
{
    //new Dialog for adding
    EntryDialog *addDialog = new EntryDialog(this);
    //set the mode (adding/editing) - used in EntryDialog::on_btnSaveRecipe_clicked()
    addDialog->addingMode = true;

    //load data to dialog's parameters
    addDialog->JsonObjectList = this->JsonObjectList;
    addDialog->JsonObjectNames = this->JsonObjectNames;

    addDialog->show();

    //saving the recipe invokes updateRecipes
    connect(addDialog, SIGNAL(recipeSaved()), SLOT(updateRecipes()));
}

void MainWindow::on_btnEdit_clicked()
{
    //take the selected recipe
    QModelIndexList selection = ui->listOfRecipes->selectionModel()->selectedRows();
    QModelIndex selectedRecipe = selection.takeLast();

    //create new dialog (for editing)
    EntryDialog *editDialog = new EntryDialog(this);

    //set the mode (adding/editing) - used in EntryDialog::on_btnSaveRecipe_clicked()
    editDialog->addingMode = false;

    //set the recipe to edit index
    editDialog->indexEdited = selectedRecipe.row();

    //load data into dialog's parameters so they can be loaded in the boxes
    editDialog->JsonObjectList = this->JsonObjectList;
    editDialog->JsonObjectNames = this->JsonObjectNames;

    //load the selected recipe's information
    editDialog->newRecipe = this->JsonObjectList[selectedRecipe.row()];
    editDialog->newRecipeName = this->JsonObjectNames[selectedRecipe.row()];

    //call EntryDialog's methods for filling the boxes with selected recipe's name, description and ingredients
    editDialog->loadNameAndBody();
    editDialog->updateIngredients();
    editDialog->show();

    //saving the recipe invokes updateRecipes
    connect(editDialog, SIGNAL(recipeSaved()), SLOT(updateRecipes()));
}
