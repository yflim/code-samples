class Position {
  private Point coords;
  private char orient;
  
  /* assumes pos has passed error checks directly or indirectly */
  public Position(Position pos) {
    this.coords = pos.coords;
    this.orient = pos.orient;
  }

  public Position(Point p, char c) {
    coords = p;
    if (c != 'N' && c != 'S' && c != 'E' && c != 'W') {
      System.err.println("orient(ation) must be N, S, E, or W");
      System.exit(1);
    }
    orient = c;
  }

  public Position(int x1, int y1, char c) {
    coords = new Point(x1, y1);
    if (c != 'N' && c != 'S' && c != 'E' && c != 'W') {
      System.err.println("orient(ation) must be N, S, E, or W");
      System.exit(1);
    }
    orient = c;
  }

  public Position get() {
    return this;
  }

  public Point getCoords() {
    return coords;
  }

  public char getOrient() {
    return orient;
  }

  /* assumes pos has passed error checks directly or indirectly */
  public void set(Position pos) {
    coords = pos.coords;
    orient = pos.orient;
  }

  public void setCoords(Point p) {
    coords.set(p);
  }

  public void setOrient(char c) {
    if (c != 'N' && c != 'S' && c != 'E' && c != 'W') {
      System.err.println("orient(ation) must be N, S, E, or W");
      System.exit(1);
    }
    orient = c;
  }

  public void changeX(int change) {
    coords.changeX(change);
  }

  public void changeY(int change) {
    coords.changeY(change);
  }

  public void print() {
    coords.print();
    System.out.printf(" %c\n", orient);
  }
}


