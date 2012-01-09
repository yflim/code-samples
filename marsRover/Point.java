class Point {
  private int x;
  private int y;

  public Point() {
    x = 0;
    y = 0;
  }

  public Point(Point p) {
    this.x = p.x;
    this.y = p.y;
  }

  public Point(int x1, int y1) {
    x = x1;
    y = y1;
  }

  public int getX() {
    return x;
  }

  public int getY() {
    return y;
  }

  public void set(Point p) {
    x = p.x;
    y = p.y;
  }

  public void changeX(int change) {
    x += change;
  }

  public void changeY(int change) {
    y += change;
  }

  public void print() {
    System.out.printf("%d %d", x, y);
  }
}

