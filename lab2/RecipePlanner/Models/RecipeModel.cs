using System;
using System.Collections.Generic; //List class
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace RecipePlanner.Models
{
    public class RecipeModel
    {
        //[JsonProperty("name")]
        public string name{ get; set; }
        //[JsonProperty("recipe")]
        public string description{ get; set; } = string.Empty;
        public List<IngredientModel> ingredients{ get; set; } = new List<IngredientModel>();

        public RecipeModel(RecipeModel recipe)
        {
            name = recipe.name;
            description = recipe.description;
            ingredients = recipe.ingredients;
        }
        public RecipeModel()
        {

        }

    }
}