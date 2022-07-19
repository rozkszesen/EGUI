import './App.css';
import React, { Component } from 'react';
import Home from "./Home";

class App extends Component {
  render() {
    return (
      <>
        <header>
            <div className="container">
              <h3>
                Recipe planner
              </h3>
            </div>
        </header>
        <div className="container">
          <main role="main" className="pb-3">
            <Home />
          </main>
        </div>
      </>
    );
  }


}

export default App;
