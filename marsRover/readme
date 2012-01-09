Usage: java Driver inputfile
Example input is in input.txt.

Point.java defines the data and functionality describing and supporting an integer-valued point on a plane (i.e. 2-dimensional space).
Position.java defines the data and functionality describing and supporting a Point combined with an orientation (limited to N, S, E, or W),
i.e. the location information used by a Rover.
Rover.java defines the data and functionality describing and supporting a Mars rover, which encompasses a Position, and a sequence of characters
(Characters in this case) which make up the instructions to that rover on how to move.
Drive.java is the driver code for the program; it reads input from a named text file, then prints the ending Position of each rover to standard output.


I assumed, among other things, that:

1. The rovers would only be used on an integer-valued two-dimensional coordinate system, so Point is defined as having int x and y;
that would eliminate the need to check for integer-valued input, which could be clunky in this context, if, say, doubles were used. 
For more generality, the code could be templated.

2. The provided directions will not end up driving any rovers off the plane: I would have incorporated error checking for this in Rover.java, but
as far as I've been able to verify (and I may well be wrong), there is no way to initialize a static final field in Java based on input
(as opposed to hard-coded, e.g. private static final int Pi = 3.142), so I would have had to give each rover minimum and maximum coordinate values,
which seems possibly unnecessary/ugly.

