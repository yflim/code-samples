import java.util.Arrays;
import java.util.ArrayList;

class Rover {
  private Position position;
  private ArrayList<Character> directions;

  public Rover(Position pos, char[] dir) {
    position = pos;
    directions = new ArrayList<Character>(dir.length);
    for (char c : dir)
      directions.add(new Character(c));
  }

  public Rover(Point p, char or, char[] dir) {
    position = new Position(p, or);
    directions = new ArrayList<Character>(dir.length);
    for (char c : dir)
      directions.add(new Character(c));
  }

  public Rover(int x, int y, char or, char[] dir) {
    position = new Position(x, y, or);
    directions = new ArrayList<Character>(dir.length);
    for (char c : dir)
      directions.add(new Character(c));
  }

  public Rover(Position pos, ArrayList<Character> dir) {
    position = pos;
    directions = new ArrayList<Character>(dir);
  }

  public Rover(Point c, Character or, ArrayList<Character> dir) {
    position = new Position(c, or);
    directions = new ArrayList<Character>(dir);
  }

  public Rover(int x, int y, Character or, ArrayList<Character> dir) {
    position = new Position(x, y, or);
    directions = new ArrayList<Character>(dir);
  }

  public Point getCoords() {
    return position.getCoords();
  }

  public char getOrient() {
    return position.getOrient();
  }

  public ArrayList<Character> getDirs() {
    return directions;
  }
  
  public void printPosition() {
    position.print();
  }

  public void printDirs() {
    for (Character c : directions)
      System.out.printf("%c", c.charValue());
    System.out.printf("\n");
  }

  /* assumes position has passed checks for invalid orient value */
  public char turn(char dir) {
    switch (position.getOrient()) {
      case 'N':
        position.setOrient((dir == 'L') ? 'W' : 'E');
	break;
      case 'S':
        position.setOrient((dir == 'L') ? 'E' : 'W');
	break;
      case 'E':
        position.setOrient((dir == 'L') ? 'N' : 'S');
	break;
      case 'W':
        position.setOrient((dir == 'L') ? 'S' : 'N');
	break;
    }

    return position.getOrient();
  }

  public Point move() {
    switch (position.getOrient()) {
      case 'N':
        position.changeY(1);
	break;
      case 'S':
        position.changeY(-1);
	break;
      case 'E':
        position.changeX(1);
	break;
      case 'W':
        position.changeX(-1);
	break;
    }

    return position.getCoords();
  }

  public Position drive() {
    char dir;
    while (!directions.isEmpty()) {
      dir = directions.remove(0).charValue();
      if (dir == 'L' || dir == 'R') 
        turn(dir);
      else if (dir == 'M')
        move();
      else {
        System.err.println("Invalid direction: must be L, R, or M");
	System.exit(1);
      }
    }
    return position.get();
  }
}
