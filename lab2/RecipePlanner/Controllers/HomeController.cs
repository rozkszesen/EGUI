using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Globalization; //for converting the string to float
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using RecipePlanner.Models;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace RecipePlanner.Controllers
{
    public class HomeController : Controller
    {
        private readonly ILogger<HomeController> _logger;

        public List<RecipeModel> listOfRecipes = new List<RecipeModel>();
        public static List<IngredientModel> shoppingList = new List<IngredientModel>();

        //recipe object needed for editing
        public static string nameToEdit = String.Empty;
        
        public static bool editMode;

        public static string filename = "../recipes.json";
        public HomeController(ILogger<HomeController> logger)
        {
            _logger = logger;
        }

        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
        public IActionResult Error()
        {
            return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
        }
        
        public IActionResult Index()
        {
            readJsonFile(filename);

            //remove the last recipe if its name is temporary
            if(listOfRecipes.Last().name == "temporary_recipe")
                listOfRecipes.RemoveAt(listOfRecipes.Count() - 1);
            
            saveJsonFile(filename);

            Console.WriteLine("recipes: " + listOfRecipes.Count());
           
            ViewBag.Recipes = listOfRecipes;
            return View();
        }

        public IActionResult ShoppingList()
        {
            readJsonFile(filename);

            ViewBag.Recipes = listOfRecipes;
            return View();
        }

        [HttpPost]
        public IActionResult addToShopping(string recipeToAdd)
        {
            //readJsonFile(filename);
            /*
            int recipeIndex = 0;
            int nrOfRecipes = listOfRecipes.Count();
            int nrOfIngredients = 0;

            //traverse the list of recipes
            for(int i = 0; i < nrOfRecipes; i++)
            {
                if(listOfRecipes.ElementAt(i).name == recipeToAdd)
                {
                    recipeIndex = i;
                    nrOfIngredients = listOfRecipes.ElementAt(i).ingredients.Count();
                    break;
                }
            }
            IngredientModel ingredientToAdd = new IngredientModel("test", 2, "unit");
            shoppingList.Add(ingredientToAdd);
            Console.WriteLine("ingredient count: " + nrOfIngredients);
            /*
            for(int i = 0; i < nrOfIngredients; i++)
            {
                bool ingredientPresent = false;

                for(int j = 0; j < shoppingList.Count(); j++)
                {
                    if()
                
                }
            }
            */

            //ViewBag.Recipes = listOfRecipes;
            //ViewBag.ShoppingList = shoppingList;
            Console.WriteLine("in addtoshopping action");
            return RedirectToAction("ShoppingList");
        }

        public IActionResult AddRecipe()
        {
            readJsonFile(filename);
            
            editMode = false;
            RecipeModel tempRecipe = new RecipeModel();
            tempRecipe.name = "temporary_recipe";
            listOfRecipes.Add(tempRecipe);

            saveJsonFile(filename);
            ViewBag.recipeToAdd = tempRecipe;
            return View();
        }

        public IActionResult AddIngredient(string productInput, string quantityInput, string unitInput)
        {
            readJsonFile(filename);
            
            Console.WriteLine("in AddIngredient action");
            
            NumberFormatInfo dot = new NumberFormatInfo();
            dot.NumberDecimalSeparator = ".";
            float quantityFloat = float.Parse(quantityInput, NumberStyles.Any, dot);
            
            IngredientModel ingredientToAdd = new IngredientModel(productInput, quantityFloat, unitInput);
            listOfRecipes.Last().ingredients.Add(ingredientToAdd);
            
            saveJsonFile(filename);
            if(editMode == false)
            {
                ViewBag.recipeToAdd = listOfRecipes.Last();
                return View("~/Views/Home/AddRecipe.cshtml", listOfRecipes.Last());
            }
            else
            {
                ViewBag.nameToEdit = nameToEdit;
                ViewBag.recipeToEdit = listOfRecipes.Last();
                return View("~/Views/Home/EditRecipe.cshtml", listOfRecipes.Last());

            }
        }

         public IActionResult EditRecipe(string recipeToEdit)
        {
            readJsonFile(filename);
            editMode = true;
            int recipeIndex = 0;
            int nrOfRecipes = listOfRecipes.Count();
            //save for later
            nameToEdit = recipeToEdit;

            //traverse the list of recipes
            for(int i = 0; i<nrOfRecipes; i++)
            {
                //remove if the name is equal to the recipeToDelete
                if(listOfRecipes.ElementAt(i).name == recipeToEdit)
                {
                    recipeIndex = i;
                    break;
                }
            }
            //add the recipe to edit to the viewbag
            ViewBag.recipeToEdit = listOfRecipes.ElementAt(recipeIndex);
            ViewBag.nameToEdit = nameToEdit;

            RecipeModel tempRecipe = new RecipeModel((listOfRecipes.ElementAt(recipeIndex)));
            tempRecipe.name = "temporary_recipe";
            listOfRecipes.Add(tempRecipe);
            saveJsonFile(filename);
            
            //if the user clicks cancel: the last one will be deleted
            //if the user clicks save: the previous version of the recipe will be deleted

            return View(tempRecipe);
        }
        
        public IActionResult SaveAddedRecipe(string nameInput, string descriptionInput)
        {
            readJsonFile(filename);

            listOfRecipes.Last().name = nameInput;
            listOfRecipes.Last().description = descriptionInput;

            saveJsonFile(filename);
            ViewBag.Recipes = listOfRecipes;
            return View("~/Views/Home/Index.cshtml");
        }

        [HttpPost]
        public IActionResult SaveEditedRecipe(string nameInput, string descriptionInput)
        {
            readJsonFile(filename);
            int nrOfRecipes = listOfRecipes.Count();

            for(int i = 0; i<nrOfRecipes; i++)
            {
                //remove the previous version of recipe
                if(listOfRecipes.ElementAt(i).name == nameToEdit)
                {
                    listOfRecipes.RemoveAt(i);
                    break;
                }
            }

            listOfRecipes.Last().name = nameInput;
            listOfRecipes.Last().description = descriptionInput;

            Console.WriteLine("Save edited recipe action: " + nameInput + descriptionInput);

            saveJsonFile(filename);
            ViewBag.Recipes = listOfRecipes;
            return View("~/Views/Home/Index.cshtml");
        }
        
        public IActionResult Cancel()
        {
            readJsonFile(filename);

            //remove the temporary Recipe object used for editing 
            listOfRecipes.RemoveAt(listOfRecipes.Count() - 1);

            saveJsonFile(filename);
            ViewBag.Recipes = listOfRecipes;
            return View("~/Views/Home/Index.cshtml");
        }
        
        public IActionResult DeleteIngredient(string ingredientToDelete, string descriptionInput)
        {
            readJsonFile(filename);
            int ingredientIndex = 0;
            int nrOfIngredients = listOfRecipes.Last().ingredients.Count();
            //find the ingredient to delete
            for(int i = 0; i<nrOfIngredients; i++)
            {
                if(listOfRecipes.Last().ingredients.ElementAt(i).product == ingredientToDelete)
                {
                    ingredientIndex = i;
                    break;
                } 
            }
            listOfRecipes.Last().ingredients.RemoveAt(ingredientIndex);

            saveJsonFile(filename);
        
            ViewBag.recipeToEdit = listOfRecipes.Last();
            ViewBag.nameToEdit = nameToEdit;

            return View("~/Views/Home/EditRecipe.cshtml");
        }

        public IActionResult DeleteRecipe(string recipeToDelete)
        {
            //import files from json 
            readJsonFile(filename);

            int nrOfRecipes = listOfRecipes.Count();
            //traverse the list of recipes
            for(int i = 0; i<nrOfRecipes; i++)
            {
                //remove if the name is equal to the recipeToDelete
                if(listOfRecipes.ElementAt(i).name == recipeToDelete)
                {
                    listOfRecipes.RemoveAt(i);
                    break;
                }
            }
        
            //save json file
            saveJsonFile(filename);
            
            //refresh home page
            return RedirectToAction("Index");
        }

        public bool readJsonFile(string filePath)
        {
            /*
            if(listOfRecipes.Any())
            {
                Console.WriteLine("The listOfRecipes is not empty");
                return false;
            }
            */

            if(!System.IO.File.Exists(filePath))
            {
                Console.WriteLine("The Json file: " + filePath + " doesn't exist\n");
                return false;
            }

            //read the json file as a string
            string jsonString = System.IO.File.ReadAllText(filePath);
            //get json object from the string
            JObject allRecipes = JObject.Parse(jsonString);

            //IList<JToken> recipes = allRecipes.Children().ToList();            

            //parse through the recipes
            foreach(JProperty recipe in allRecipes.Properties())
            {
                //recipe object used for adding to the list of recipes
                RecipeModel recipeToAdd = new RecipeModel();
                //read the name of the recipe
                recipeToAdd.name = recipe.Name;
                //read the "body" (value) of each recipe as JObject
                JObject recipeBody = JObject.Parse(recipe.Value.ToString());

                //extract recipe description and consecutive ingredients (properties of recipe value (body))
                foreach(JProperty property in recipeBody.Properties())
                {
                    //adding recipe description
                    if(property.Name == "recipe")
                    {
                        //array of lines of recipe description
                        JArray recipeDecription = (JArray)property.Value;

                        //add lines one by one to the recipeToAdd description
                        foreach(JToken line in recipeDecription)
                        {
                            recipeToAdd.description += line.ToString() + "\n";
                        }
                    }
                    //adding ingredients
                    else
                    {
                        //ingredient object to be used for adding to the list of ingredients 
                        IngredientModel ingredientToAdd = new IngredientModel();
                        ingredientToAdd.product = property.Name.ToString();
                        string quantityString = property.Value.ToString().Split(' ')[0];
                        ingredientToAdd.quantity = float.Parse(quantityString, CultureInfo.InvariantCulture.NumberFormat);
                        ingredientToAdd.unit = property.Value.ToString().Split(' ')[1];
                        
                        recipeToAdd.ingredients.Add(ingredientToAdd);
                    }
                }
                //add the recipe to the list
                listOfRecipes.Add(recipeToAdd);
            }
           
           return true;
        }

        public bool saveJsonFile(string filePath)
        {
            
            //Json object that will hold all the recipes
            JObject allRecipes= new JObject();
            
            foreach(RecipeModel tempRecipe in listOfRecipes)
            {
                //recipe to be added to the 
                JObject recipeToAdd = new JObject();

                //JObject for storing the body of the recipe
                JObject recipeBody = new JObject();

                //JArray for storing the lines of the description
                JArray descriptionArray = new JArray();

                foreach(string line in tempRecipe.description.Split('\n'))
                {
                    descriptionArray.Add(line);
                }
                //removing the last line which was unnecessary "\n" 
                descriptionArray.Last.Remove();
                
                //create JProperty with recipe description
                JProperty descriptionToAdd = new JProperty("recipe", descriptionArray);

                //add the recipe description as a property of recipeBody
                recipeBody.Add(descriptionToAdd);
                
                //adding ingredients as consecutive properties
                foreach(IngredientModel tempIngredient in tempRecipe.ingredients)
                {   
                    //specifying new decimal separator: dot (to replace the comma)
                    NumberFormatInfo dot = new NumberFormatInfo();
                    dot.NumberDecimalSeparator = ".";

                    //converting the quantity (float) + unit of an ingredient to a string    
                    string valueString = (string)tempIngredient.quantity.ToString(dot) + " " + tempIngredient.unit;
                    
                    //converting the string to a JToken (value)
                    JValue ingredientValue = new JValue(valueString);
                    
                    //adding the ingredient as a property of the recipeBody object
                    recipeBody.Add(tempIngredient.product, ingredientValue);
                }
                
                //add the processed recipe to the json object
                allRecipes.Add(new JProperty(tempRecipe.name, recipeBody));
            } 
            
            //export the object into json file
            System.IO.File.WriteAllText(filename, JsonConvert.SerializeObject(allRecipes, Formatting.Indented));
           
            return true;
        }

        public void printRecipesToConsole()
        {            
            foreach(RecipeModel recipe in listOfRecipes)
            {
                Console.WriteLine(recipe.name + "\n" + recipe.description);
                Console.WriteLine("ingredients: " + recipe.ingredients.Count());
                foreach(IngredientModel ingredient in recipe.ingredients)
                {
                    Console.WriteLine(ingredient.product + " " + ingredient.quantity.ToString() + " " + ingredient.unit);
                }
                Console.WriteLine("---------------------------------------");
            }
            Console.WriteLine("recipes: " + listOfRecipes.Count());
        }
    }
}
