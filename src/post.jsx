import React from 'react';
import { EditText } from 'react-edit-text';
import 'react-edit-text/dist/index.css';

class Post extends React.PureComponent {
  constructor(props) {
    super(props);
    this.handleDeleteButton = this.handleDeleteButton.bind(this);
    this.savePost = this.savePost.bind(this);
  }

  handleDeleteButton() {
    const { id, onDelete } = this.props;
    onDelete(id);
  }

  savePost({ name, value, previousValue }) {
    const sanitizedValue = value.trim().substring(0, 256);
    if (sanitizedValue.length === 0) {
      this.handleDeleteButton();
    }

    const { catId, id } = this.props;
    const requestBody = { content: sanitizedValue };
    fetch(`/post/${catId}/${id}`, {
      method: 'PATCH', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(requestBody)
    })
    .then(response => {
      if (!response.ok) {
        alert('Failed to save post.');
        throw Error(response.statusText);
      }
    });
  }

  render() {
    const { id, content } = this.props;
    return (
      <div className="post">
        <EditText
          name={`post-${id}`}
          defaultValue={content}
          onSave={this.savePost}
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

export default Post;
