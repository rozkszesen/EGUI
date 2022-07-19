const express = require('express');
const app = express();
const fs = require('fs');

//read list of recipes
app.get('/index', (req, res) => {
    let data = fs.readFileSync('recipes.json');
    res.json(JSON.parse(data));
})

app.listen(5000);