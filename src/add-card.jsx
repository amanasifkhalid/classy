import React from 'react';

class AddCard extends React.PureComponent {
  constructor(props) {
    super(props);
    this.handleAddCard = this.handleAddCard.bind(this);
  }

  handleAddCard() {
    event.preventDefault(); // Prevent form submission from reloading page
    const { trigger } = this.props;
    const name = event.target.elements.text.value;
    trigger(name);
    event.target.reset(); // Clear card name input
  }

  render() {
    return (
      <>
        <form onSubmit={this.handleAddCard}>
          <input
            name="text"
            type="text"
            placeholder="Sticky Name"
            maxLength="20"
            required
          />
          <input type="submit" value="Create" />
        </form>
      </>
    );
  }
}

export default AddCard;
