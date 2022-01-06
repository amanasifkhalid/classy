import React from 'react';
import AddCard from './add-card';
import ClassCard from './class-card';

class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      classes: {},
      showForm: false
    };

    this.addClass = this.addClass.bind(this);
    this.deleteClass = this.deleteClass.bind(this);
  }

  addClass(title) {
    if (title.trim().length === 0) {
      alert('Sticky name required.');
      return;
    }

    const requestBody = { name: title };
    fetch('/category', {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(requestBody)
    })
    .then(response => {
      if (response.ok) {
        return response.json();
      }

      alert('Failed to create sticky.');
      throw Error(response.statusText);
    })
    .then(data => {
      this.setState(state => {
        const newId = data['id'];
        state.showForm = false;
        state.classes[newId] = <ClassCard
                                 key={newId}
                                 id={newId}
                                 name={title}
                                 posts={[]}
                                 onDelete={this.deleteClass}
                               />;
        return state;
      });
    });
  }

  deleteClass(id) {
    fetch(`/category/${id}`, { method: 'DELETE' }).then(response => {
      if (response.ok) {
        this.setState(state => {
          delete state.classes[id];
          return state;
        });
      } else {
        alert('Failed to delete sticky.');
        throw Error(response.statusText);
      }
    });
  }

  componentDidMount() {
    fetch('/posts')
    .then(response => {
      if (response.ok) {
        return response.json();
      }

      alert('Failed to fetch notes.');
      throw Error(response.statusText);
    })
    .then(data => {
      const classComponents = Object.fromEntries(Object.entries(data).map(
        ([classId, classData]) => [classId,
          <ClassCard
            key={classId}
            id={classId}
            name={classData['name']}
            posts={classData['posts']}
            onDelete={this.deleteClass}
          />
        ]
      ));
      this.setState({ classes: classComponents });
    }).catch(error => {});
  }

  render() {
    const { classes, showForm } = this.state;
    return (
      <>
        <div className="addCard">
          {!showForm &&
            <button
              className="pure-button pure-button-primary"
              onClick={() => this.setState({ showForm: true })}
            >New Sticky</button>
          }
          {showForm && <AddCard trigger={this.addClass} />}
        </div>
        <div className="pure-g">
          {Object.values(classes)}
        </div>
      </>
    );
  }
}

export default App;
