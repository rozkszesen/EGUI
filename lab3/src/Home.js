import React, { Component } from 'react';

class Home extends Component {
    constructor(props) {
        super(props);
        this.state = { 'recipes': [] };
    }

    componentDidMount() {
        fetch("/index")
            .then(res => res.json())
            .then(data => {
                var fetchedRecipes = [];
                for(var recipe in data) {
                    fetchedRecipes.push(recipe);
                }

                this.setState({ 'recipes': fetchedRecipes })
            });
    }

    createTable() {
        var tableContent = [];

        for (var recipe of this.state.recipes.values()) {
            var row = (
                    <tr> 
                        <td>
                            <p> {recipe}</p>
                        </td>
                    </tr>
            );

            tableContent.push(row);
        }

        return tableContent;
    }
    render() {
        return (
            <div className = "container">
                <div className = "row">
                    <div className = "col-9">
                        <table className = "table">
                            <th>name</th>
                            <tbody> {this.createTable()}</tbody>
                        </table>
                    </div>
                </div>
            </div>
        )
    }

}

export default Home;