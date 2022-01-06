import React from 'react';
import CardTitle from './card-title';
import Post from './post';

class ClassCard extends React.Component {
  constructor(props) {
    super(props);
    const { name } = this.props;
    this.state = {
      name,
      posts: {},
      showForm: false
    };

    this.addPost = this.addPost.bind(this);
    this.deletePost = this.deletePost.bind(this);
  }

  addPost(event) {
    event.preventDefault(); // Prevent form submission from reloading page
    const content = event.target.elements.text.value;
    const sanitizedValue = content.trim().substring(0, 256);
    event.target.reset(); // Clear card name input

    if (sanitizedValue.length === 0) {
      return;
    }

    const { id } = this.props;
    const requestBody = {
      text: sanitizedValue,
      catid: id
    };
    fetch (`/post`, {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(requestBody)
    })
    .then(response => {
      if (response.ok) {
        return response.json();
      }

      alert('Failed to create note.')
      throw Error(response.statusText);
    })
    .then(data => {
      this.setState(state => {
        const { id } = data;
        state.posts[id] = <Post key={id} id={id} content={sanitizedValue} />;
        state.showForm = false;
        return state;
      });
    });
  }

  deletePost(postId) {
    const { id } = this.props;
    fetch(`/post/${id}/${postId}`, { method: 'DELETE' }).then(response => {
      if (response.ok) {
        this.setState(state => {
          delete state.posts[postId];
          return state;
        });
      } else {
        alert('Failed to delete note.');
        throw Error(response.statusText);
      }
    })
  }

  componentDidMount() {
    const { name, posts } = this.props;
    const postComponents = Object.fromEntries(posts.map(post => {
      const { id, content } = post;
      return [
        id,
        <Post
          key={id}
          id={id}
          catId={this.props.id}
          content={content}
          onDelete={this.deletePost}
        />
      ];
    }));
    this.setState({posts: postComponents});
  }

  render() {
    const { name, posts, showForm } = this.state;
    const { id, onDelete } = this.props;
    return (
      <div className="pure-u-1 pure-u-md-1-2 pure-u-lg-1-4">
        <div className="card">
          <CardTitle deleteFunc={onDelete} id={id} name={name} />
          {Object.values(posts)}
          {!showForm &&
            <button
              className="pure-button pure-button-primary"
              onClick={() => this.setState({ showForm: true })}
            >New Note</button>
          }
          {showForm &&
            <form onSubmit={this.addPost}>
              <input
                name="text"
                type="text"
                defaultValue=""
                placeholder="Remember me fondly..."
              />
              <input type="submit" value="Create" />
            </form>
          }
        </div>
      </div>
    );
  }
}

export default ClassCard;
