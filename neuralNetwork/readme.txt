Note: Written 2009 or 2010!

The code in nn.c and nn.h implements an artificial neural network (ANN). 

A brief description of the (kind of) ANN implemented here: 

The network has 3 layers of nodes by default: input, hidden (intermediate), and output;
it "learns" (via an implementation-dependent algorithm) to produce some desired output
given some input values via training on some set of correct input-output examples.
Training input values are passed into the input layer, and the network's output produced in the 
output layer. The input is transformed via weights and an "activation function"
(here, only the logistic function is supported) into output.

Training occurs in "epochs", or iterations, each of which can be considered a single 
"training session" during which all possible correct input-output combinations are presented once. 
The network is supposed to eventually, after some number of epochs,  converge on the correct output 
for each given input. 

By default, the input layer is connected to the hidden (with one connection for each input-hidden pair),
and the hidden to the output (with one connection from each hidden node to the only output node 
implemented here). The user can also decide whether or not to use a "skip" layer, which adds direct
connections between the input and output layers (one for each input-output pair, though again there's 
only one output here). Each connection is weighted, initially with (pseudo-)randomly generated numbers 
between 0 and 1. The weights are updated, according to the error (the metric used here is root mean 
squared error, or RMSE) between the network's and the correct output, as the network is trained.

It also has the following design features: bias, which simply sets ("biases") the initial hidden and output node 
values to some non-zero value, and learning rate, which affects how sharply weights are adjusted in response 
to output error for each input pattern.

Again, the code currently supports only a logistic activation function (with steepness parameter as
a command-line argument), but has been written so that other options can easily be coded in if wanted.


Details for the user:

map_xor.c is the driver function - it trains the network with the XOR function (combinations of 
one to four 0s and 1s possible).

User-specified parameters:
- number of epochs
- number of nodes in the input layer
- number of hidden nodes
- whether skip layer is used
- bias: initializes ("biases") hidden and output values to non-zero number
- learning rate
- activation function (only logistic supported)
- activation function parameters (only steepness for logistic)
- desired error tolerance

Outputs:
- RMSE (root mean squared error) of the output residuals for each input pattern from the last epoch
- Iteration/epoch at which network converged on correct output (if it did)
The RMSE from every 10th epoch is written to a csv file.

A Matlab file, plotrmse, with a simple plotting command is included in case it would be of use.

Commands to run (assuming Unix environment; otherwise, see Makefile for test-case details):
$ make
$ make test
In Matlab:
plotrmse 

Further details can be found in source file comments - there's some (a lot?)
of overlap; I apologize if that's any inconvenience.

