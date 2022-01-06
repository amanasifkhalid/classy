import React from 'react';
import { EditText } from 'react-edit-text';
import 'react-edit-text/dist/index.css';

class CardTitle extends React.PureComponent {
  constructor(props) {
    super(props);
    this.handleDeleteButton = this.handleDeleteButton.bind(this);
    this.saveTitle = this.saveTitle.bind(this);
  }

  handleDeleteButton() {
    const { deleteFunc, id } = this.props;
    deleteFunc(id);
  }

  saveTitle({ name, value, previousValue }) {
    const sanitizedValue = value.trim().substring(0, 20);
    if (sanitizedValue.length === 0) {
      alert('Title cannot be empty.');
      value = previousValue;
      return;
    }

    const { id } = this.props;
    const requestBody = { name: sanitizedValue };
    fetch(`/category/${id}`, {
      method: 'PATCH', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(requestBody)
    })
    .then(response => {
      if (!response.ok) {
        alert('Failed to save new title.');
        value = previousValue;
        throw Error(response.statusText);
      }
    });
  }

  render() {
    const { name } = this.props;
    return (
      <div className="stickyTitle">
        <EditText
          className="editText"
          name="title"
          defaultValue={name}
          onSave={this.saveTitle}
          style={{fontWeight: "bold", width: "90%"}}
          inline
        />
        <a
          className="delete-btn"
          type="button"
          href="#"
          onClick={this.handleDeleteButton}
        >X</a>
      </div>
    );
  }
}

export default CardTitle;
