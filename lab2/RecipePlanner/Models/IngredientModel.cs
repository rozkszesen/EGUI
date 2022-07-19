using System;
using System.Collections.Generic; //List class

namespace RecipePlanner.Models
{
    public class IngredientModel
    {
        public string product{ get; set; }
        public float quantity{ get; set; } //description of the recipe stored as lines of text
        public string unit{ get; set; }

        public IngredientModel(string product, float quantity, string unit)
        {
            this.product = product;
            this.quantity = quantity;
            this.unit = unit;
        }
        public IngredientModel()
        {
            
        }
    }
    

}