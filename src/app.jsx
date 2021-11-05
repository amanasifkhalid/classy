import React from 'react';
import AddClass from './add-class';
import ClassCard from './class-card';

class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      classes = {}
    };
  }

  componentDidMount() {
    fetch('')
      .then((response) => {
        if (!response.ok) {
          throw Error(response.statusText);
        }

        return response.json();
      })
      .then((data) => {
        const classComponents = data.classes.map((classData) => [
          classData.name, (
            <div className="pure-u-1 pure-u-md-1-3">
              <ClassCard onClassDelete={} />
            </div>
          )
        ]);
    });

    this.setState({
      classes: Object.fromEntries(classComponents)
    });
  }

  render() {
    const { classes } = this.state;
    return (
      <div>
        <div className="pure-g">
          {classes}
        </div>
        <AddClass />
      </div>
    );
  }
}

export default App;
