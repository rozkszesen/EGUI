#include "entrydialog.h"
#include "ui_entrydialog.h"

EntryDialog::EntryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryDialog)
{
    ui->setupUi(this);
    setMinimumSize(500, 500);
    resize(500, 500);
}

EntryDialog::~EntryDialog()
{
    delete ui;
}

void EntryDialog::on_btnAddIngredient_clicked()
{
    QString ingredient = this->ui->ingredientBox->text();
    QString amount = this->ui->amountBox->text();
    QString unit = this->ui->unitBox->text();

    //do nothing if any of the boxes is empty
    if(ingredient.isEmpty() || amount.isEmpty() || amount == "0,00" || unit.isEmpty() )
        return;

    //update the parameter holding new recipe information (key, value)
    this->newRecipe.insert(ingredient, amount + " " + unit);

    //update the ingredient table
    this->updateIngredients();

    //clear boxes
    this->ui->ingredientBox->clear();
    this->ui->amountBox->clear();
    this->ui->unitBox->clear();
}

void EntryDialog::deleteSelectedIngredients()
{
    //read all keys (ingredients) from newRecipe
    QStringList ingredients = this->newRecipe.keys();

    //create Model Index List for selected elements
    QModelIndexList selection = ui->tableOfIngredients->selectionModel()->selectedRows();
    //list of numbers of rows to delete
    QList<int> rowsToDelete;

    //read the row number of each row to delete and add to the list;
    foreach(QModelIndex index, selection)
        rowsToDelete += index.row();

    //reverse the order so that indices do not change for the items that are to be deleted
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    foreach(int row, rowsToDelete)
    {
        qDebug("removed recipe row %d", row);
        //remove the selected ingredients
        this->newRecipe.remove(this->ingredientsModel->takeItem(row)->text());
    }

    updateIngredients();
}

void EntryDialog::on_btnDeleteIngredient_clicked()
{
    //display message asking for confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete","Do you want to delete the ingredient(s)?", QMessageBox::Yes | QMessageBox::No);

    //delete only if Yes was answered
    if (reply == QMessageBox::Yes)
        deleteSelectedIngredients();
    else
        return;
}

void EntryDialog::on_btnSaveRecipe_clicked()
{
    //save the name
    this->newRecipeName = ui->recipeName->text();

    //read the text from recipeBody and split into substrings when a <new line> occurs
    QStringList newRecipeText = ui->recipeBody->toPlainText().split('\n');

    //Json array to hold all the strings read from the box
    QJsonArray newRecipeTextJson;

    //read the string list and transfer to json array
    foreach(QString string, newRecipeText)
        newRecipeTextJson.push_back(string);

    //update the newRecipe parameter
    this->newRecipe.insert("recipe", newRecipeTextJson);

    if(this->addingMode == true)
    {
        //add the created object to the existing list
        this->JsonObjectNames.push_back(newRecipeName);
        this->JsonObjectList.push_back(newRecipe);
    }
    else
    {
        //remove the previous version of the recipe
        this->JsonObjectNames.removeAt(indexEdited);
        this->JsonObjectList.removeAt(indexEdited);

        this->JsonObjectNames.insert(indexEdited, newRecipeName);
        this->JsonObjectList.insert(indexEdited, newRecipe);
    }

    //save to Json
    this->saveJsonFile("../laboratories/recipesNew.json");

    //emit signal to invoke updateRecipes
    emit recipeSaved();

    //hide the window
    this->hide();

}

void EntryDialog::on_btnCancel_clicked()
{
    close();
}


void EntryDialog::loadNameAndBody()
{
    //load the title from the class' parameter
    ui->recipeName->setText(this->newRecipeName);

    // read newRecipe's value of key "recipe" to a Json array
    QJsonArray recipe = newRecipe.value("recipe").toArray();

    // load each line to a string and separate with 'new line'
    QString recipeText = "";
    foreach(auto temp, recipe)
        recipeText += temp.toString() + "\n";

    //set the ui->recipeBody to the read text
    ui->recipeBody->setText(recipeText);
}

bool EntryDialog::saveJsonFile(QString filename)
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

void EntryDialog::updateIngredients()
{
    //read all keys (ingredients) from newRecipe stored in the class
    QStringList ingredients = this->newRecipe.keys();

    //remove "recipe" key from the list (otherwise it will appear among ingredients)
    ingredients.removeAll("recipe");

    //create model for ingredientsTable (with two columns: 1-ingredient, 2-amount)
    ingredientsModel = new QStandardItemModel(ingredients.length(), 2, this);

    //index to iterate through ingredients in the list
    QModelIndex index;

    for (int i = 0; i < ingredients.length(); i++)
    {
        if(ingredients[i] == "recipe")
        ingredients.removeAt(i);
    }

    for(int i = 0; i < ingredients.length(); ++i)
    {
        for(int j = 0; j < 2; ++j)
        {
            index = ingredientsModel->index(i, j, QModelIndex());

            if(j==0)
            {
                //assign the name of the ingredient to the model
                ingredientsModel->setData(index, ingredients[i]);
            }
            else
            {
                //assign the string containing the amount + unit
                QString amountAndUnit = this->newRecipe.value(ingredients[i]).toString();
                ingredientsModel->setData(index, amountAndUnit);
            }
        }
    }

    //set labels for columns
    QStringList tableLabels = {"Ingredient", "Amount"};
    ingredientsModel->setHorizontalHeaderLabels(tableLabels);

    //set the table of ingredients model
    this->ui->tableOfIngredients->setModel(ingredientsModel);

    //adjust the column width
    //this->ui->tableOfIngredients->setColumnWidth(0,200);
    //this->ui->tableOfIngredients->setColumnWidth(1,200);
    this->ui->tableOfIngredients->setMaximumWidth(600);
    this->ui->tableOfIngredients->resizeColumnsToContents();
    this->ui->tableOfIngredients->resizeRowsToContents();
}
