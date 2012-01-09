import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;

public class Driver {
  public static void main(String args[]) {
    if (args.length != 1) {
      System.err.println("usage: java Driver <inputfile>");
      System.exit(1);
    }

    Point maxCoords;
    String s;
    ArrayList<Rover> rovers = new ArrayList<Rover>(50);
    try {
      BufferedReader br = new BufferedReader(new FileReader(args[0]));
      if ((s = br.readLine()) == null)
        System.err.println("Input file is empty");
      else {
        int i = 0;
	int x, y;
        maxCoords = readMaxcoords(s, new Point(0, 0));
        rovers = readRoverInfo(br, maxCoords, rovers);
	if (!rovers.isEmpty()) {
	  for (Rover r : rovers) {
	    System.out.printf("Rover %d:\n", i);
	    r.printPosition();
	    r.printDirs();
	    System.out.println("Driving...");
	    r.drive();
	    x = r.getCoords().getX();
	    y = r.getCoords().getY();
            if (x < 0 || x > maxCoords.getX() || y < 0 || y > maxCoords.getY()) {
	      System.err.println("Bad directions! Rover got driven off the plateau!");
	      System.exit(1);
	    }
	    r.printPosition();
	    ++i;
	  }
	}
	else {
          System.err.println("No rover (information) provided");
	}
      }
    }
    catch (IOException e) {
      System.err.println("Error reading input file");
      System.exit(1);
    }

    System.exit(0);
  }

  public static Point readCoords(String sx, String sy, Point mincoords, Point maxcoords) {
    int x = Integer.parseInt(sx);
    int y = Integer.parseInt(sy);
    if (x < mincoords.getX() || x > maxcoords.getX()) {
      System.err.printf("x must be between %d and %d, inclusive\n", mincoords.getX(), maxcoords.getX());
      System.exit(1);
    }
    if (y < mincoords.getY() || y > maxcoords.getY()) {
      System.err.printf("y must be between %d and %d, inclusive\n", mincoords.getY(), maxcoords.getY());
      System.exit(1);
    }
    return (new Point(x, y));
  }
    
  /* read maximum coordinates, specifying Integer.MAX_VALUE as a sensible initial limit on their value */
  public static Point readMaxcoords(String line, Point mincoords) {
    String[] result = line.split("\\s");
    return readCoords(result[0], result[1], mincoords, new Point(Integer.MAX_VALUE, Integer.MAX_VALUE));
  }

  /* relies on error-checking for invalid orient value in Position */
  public static Position readPosition(String line, Point mincoords, Point maxcoords) {
    String[] result = line.split("\\s");
    if (result.length != 3) {
      System.out.println("Inappropriate position information length: should be 3 for x, y, orient");
      System.exit(1);
    }
    Point p = readCoords(result[0], result[1], mincoords, maxcoords);
    return new Position(p, result[2].charAt(0));
  }

  public static ArrayList<Rover> readRoverInfo(BufferedReader br, Point maxcoords, ArrayList<Rover> rovers) {
    String pline;
    String dline;
    Position p;
    try {
      while ((pline = br.readLine()) != null) {
        p = readPosition(pline, new Point(0, 0), maxcoords);
        if ((dline = br.readLine()) != null) 
          rovers.add(new Rover(readPosition(pline, new Point(0, 0), maxcoords), dline.toCharArray()));
        else 
          System.err.println("EOF reached unexpectedly (rover position read but not directions): last rover will not be included");
      }
    }
    catch (IOException e) {
      System.err.println("Error reading input file");
      System.exit(1);
    }
    rovers.trimToSize();
    return rovers;
  }
}

