#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "nn.h"

int main(int argc, char *argv[])
{
  int i, j;
  int epochs, inputs, hiddens, fn, bias;
  double tol, learnr, lsteepness;
  unsigned short skip;
  FILE *out;

  if (argc < 10)
  {
    printf("Usage: %s epochs numinputs numhiddens skip bias learnr tol fn [fnpars]\n", argv[0]);
    exit(-1);
  }

  if ((epochs=atoi(argv[1])) < 10)
  {
    printf("Must enter number of epochs equal to or greater than 10.\n");
    exit(-1);
  }

  if ((inputs=atoi(argv[2])) < 1 || inputs > 4)
  {
    printf("Number of inputs must be at least 1 and at most 4.\n");
    exit(-1);
  }

  if ((hiddens=atoi(argv[3])) < 1 || hiddens > 25)
  {
    printf("Number of hidden nodes must be at least 1 and at most 25.\n");
    exit(-1);
  }

  if ((skip=atoi(argv[4])) < 0 || skip > 1)
  {
    printf("Skip layer command-line argument must be 0 or 1.\n");
    exit(-1);
  }

  // bias command-line argument to indicate whether the user wants (hidden and output layer) biases to be used
  if ((bias=atoi(argv[5])) < 0 || bias > 1)
  {
    printf("Bias command-line argument (in this implementation) must be 0 or 1.\n");
    exit(-1);
  }

  if ((learnr=atof(argv[6])) <= 0 || learnr >= 1)
  {
    printf("Learning rate must be at least 0 and at most 1 (both exclusive).\n");
    exit(-1);
  }

  if ((tol=atof(argv[7])) < 0)
  {
    printf("Error tolerance must not be less than 0.\n");
    exit(-1);
  }

  // only logistic activation function is currently supported
  if ((fn=atoi(argv[8])) != 0)
  {
    printf("Enter 0 for logistic activation function: only one currently supported.\n");
    exit(-1);
  }

  if ((lsteepness=atof(argv[9])) < -50 || lsteepness > 50)
  {
    printf("Logistic steepness parameter must be at least -50 and at most 50.\n");
    exit(-1);
  }

  // initialize and assign XOR pattern(s): allow for combinations of one to four 0s or 1s
  int numpatterns = pow(2, inputs);
  Pattern xor[numpatterns];
  for (i=0; i<numpatterns; ++i)
    xor[i].inputs = calloc(inputs, sizeof(double));

  // binfreq controls frequency of switching between 0 and 1 for each position in input pattern
  // the following will produce, for example: 11, 10, 01, 00; 111, 110, 101, 100, 011, 010, 001, 000
  int binfreq = numpatterns/2;
  for (j=0; j<inputs; ++j, binfreq /= 2)
  {
    int round = 0;
    for (i=0; i<numpatterns; ++i)
    {
      if (i%binfreq == 0)
        ++round;
      xor[i].inputs[j] = round%2;
    }
  }
      
  // compute correct output
  for (i=0; i<numpatterns; ++i)
  {
    unsigned short result = xor[i].inputs[0];
    for (j=1; j<inputs; ++j)
      result = result ^ ((int) xor[i].inputs[j]);
    xor[i].output = result;
  }

  // create neural network using command-line arguments
  NeuralNetwork netwk = makeneuralnetwork(inputs, hiddens, skip, fn, &lsteepness, learnr, bias, 0, WEIGHT_CAP, 0, WEIGHT_CAP, 0, WEIGHT_CAP);

  int numrmses = epochs/10 + 1;		// write RMSE every 10 epochs to csv file
  double rmsearr[numrmses];

  int converge = 0;	// records epoch at which network converged on correct output; 0 if did not converge
  netwk = train(netwk, epochs, xor, numpatterns, rmsearr, &converge, tol);

  // write to csv file
  if ((out = fopen("rmses.csv", "w")) == NULL)
  {
    printf("q1: can't open rmses.csv\n");
    exit(1);
  }

  for (i=0; i<numrmses; ++i)
    fprintf(out, "%d,", i*10);
  fprintf(out, "\n");

  for (i=0; i<numrmses; ++i)
    fprintf(out, "%3.10f,", rmsearr[i]);
  fprintf(out, "\n");

  fclose(out);

  for (i=0; i<numpatterns; ++i)
  {
    printf("inputs: ");
    for (j=0; j<inputs; ++j)
      printf("%f, ", xor[i].inputs[j]);
    printf("\n");
    printf("Desired output: %f\n", xor[i].output);
    netwk = predict(netwk, xor[i]);
    printf("Actual output: %f\n", netwk.data.outputLayer);
  }

  return 0;
}

