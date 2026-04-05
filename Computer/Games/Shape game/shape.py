import random

# Define a dictionary mapping number of sides to shape names
shapes = {
    3: "Triangle",
    4: "Square",
    5: "Pentagon",
    6: "Hexagon",
    7: "Heptagon",
    8: "Octagon"
}

def generate_shape():
    """Generate a random shape (number of sides)"""
    return random.choice(list(shapes.keys()))

def play_game():
    """Main function to play the Shape Game"""
    print("Welcome to the Shape Game!")
    while True:
        num_sides = generate_shape()
        print(f"What shape has {num_sides} sides?")
        guess = input("Enter your guess (or 'quit' to exit): ").strip().capitalize()
        if guess == "Quit":
            print("Thanks for playing!")
            break
        elif guess == shapes[num_sides]:
            print("Congratulations! That's correct.")
        else:
            print("Sorry, that's incorrect. Try again.")

if __name__ == "__main__":
    play_game()
