#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "nn.h"

// for use in initializing connection weights
double generateRand(double high, double low)
{
  return (high-low) * ((double) rand() / ((double) RAND_MAX + 1.0)) + low;
}

NetworkData makenetworkdat(int inputs, int hiddens, unsigned short skip, double ho_low, double ho_high, 
                           double ih_low, double ih_high, double skip_low, double skip_high)
{
  NetworkData dat;
  int i, j;

  // numbers of hidden, input, and output nodes; whether to use skip layer or not
  dat.hiddenNodes = hiddens; 
  dat.inputNodes = inputs; 
  dat.outputNodes = 1;
  dat.useSkipLayer = skip;

  // values in the input layer
  dat.inputLayer = calloc(inputs, sizeof(double));

  // values in the hidden layer
  dat.hiddenLayer = calloc(hiddens, sizeof(double));
  for (i=0; i<hiddens; ++i)
  {
    dat.hiddenLayer[i] = 0.0;
  }

  // values in the output layer - only one node in output layer here
  dat.outputLayer = 0.0;
  
  //input->hidden layer weights
  dat.weights_IH = calloc(hiddens, sizeof(double*));
  //hidden->output layer weights
  dat.weights_HO = calloc(hiddens, sizeof(double));
  //input->output layer weights (the skip layer)
  dat.weights_skip = calloc(inputs, sizeof(double));

  // each hidden node has a weighted connection to an (the) output node, and to each input node
  for (i=0; i<hiddens; ++i)
  {
    dat.weights_HO[i] = generateRand(ho_low, ho_high);
    dat.weights_IH[i] = calloc(inputs, sizeof(double));
    for(j=0; j < dat.inputNodes; j++)
      dat.weights_IH[i][j] = generateRand(ih_low, ih_high);
  }

  // each input node has a connection to an (the) output node
  if (skip)
    for(j=0; j < dat.inputNodes; j++)
      dat.weights_skip[j] = generateRand(skip_low, skip_high);
  else
    for(j=0; j < dat.inputNodes; j++)
      dat.weights_skip[j] = 0; 
  
  dat.patternerr = 0.0;

  return dat;
}

// creates neural network; also calls function to initialize network data
NeuralNetwork makeneuralnetwork(int inputs, int hiddens, unsigned short skip, int fn, double *fnpars, double learnr, int bias,
                           double ho_low, double ho_high, double ih_low, double ih_high, double skip_low, double skip_high)
{
  NeuralNetwork netwk;

  if (bias)
  {
    netwk.hbias = generateRand(BIAS_LOW, BIAS_HIGH);   // hidden node bias - imposed in this system that all hidden nodes have the same bias
    netwk.obias = generateRand(BIAS_LOW, BIAS_HIGH);   // output node bias 
  }
  else
  {
    netwk.hbias = 0;
    netwk.obias = 0;
  }

  netwk.activationFunction = fn;

  // array of activation function parameters 
  netwk.fnpars = fnpars;

  // lr_ih, lr_ho, lr_skip: learning rate for the input->hidden, hidden->output, skip layer weights
  netwk.lr_ih = netwk.lr_ho = netwk.lr_skip = learnr;
  
  netwk.data = makenetworkdat(inputs, hiddens, skip, ho_low, ho_high, ih_low, ih_high, skip_low, skip_high);

  return netwk;
} 

// computes value of given function type from given argument ("x") value
// currently only supports logistic activation function; others can be added if wanted
double activate(double num, unsigned short fn, double *fnpars, unsigned short numpars)
{
  double ret;
  if (fn == 0)
  {
    if (numpars > 1)
    {
      printf("Only 1 parameter is taken for a logistic activation function (0).\n");
      exit(-1);
    }
    double pow = (-1.0) * fnpars[0] * num;
    ret = (1.0 / (1 + exp(pow)));
  }
  else
  {
    printf("This system currently only supports a logistic activation function (0).\n");
    exit(-1);
  }
  return ret;
}

// computes derivative of given function type at "x-value" of num
// currently only supports logistic activation function; others can be added if wanted
double deriv(double num, unsigned short fn, double *fnpars, unsigned short numpars)
{
  double ret;
  if (fn == 0)
  {
    if (numpars > 1)
    {
      printf("Only 1 parameter is taken for a logistic activation function (0).\n");
      exit(-1);
    }
    ret = fnpars[0] * num * (1.0 - num);
  }
  else
  {
    printf("This system currently only supports a logistic activation function (0).\n");
    exit(-1);
  }
  return ret;
}

NeuralNetwork predict(NeuralNetwork netwk, Pattern pat)
{
  int i, j;

  // fill input nodes with input pattern
  for (j=0; j<netwk.data.inputNodes; ++j)
    netwk.data.inputLayer[j] = pat.inputs[j];

  // initialize output layer value to bias (0 if none used)
  netwk.data.outputLayer = netwk.obias;
  for (i=0; i<netwk.data.hiddenNodes; ++i)
  {
    netwk.data.hiddenLayer[i] = netwk.hbias;	// initialize hidden layer values to bias (0 if none used)
    for (j=0; j<netwk.data.inputNodes; ++j)	// compute hidden layer values
      netwk.data.hiddenLayer[i] += netwk.data.weights_IH[i][j] * netwk.data.inputLayer[j];
    // transform hidden layer values with activation function
    netwk.data.hiddenLayer[i] = activate(netwk.data.hiddenLayer[i], netwk.activationFunction, netwk.fnpars, 1);
    // update output layer value
    netwk.data.outputLayer += netwk.data.weights_HO[i] * netwk.data.hiddenLayer[i];
  }

  // update output layer value using skip layer weights if used
  if (netwk.data.useSkipLayer)
    for (j=0; j<netwk.data.inputNodes; ++j)
      netwk.data.outputLayer += netwk.data.weights_skip[j] * netwk.data.inputLayer[j];

  netwk.data.outputLayer = activate(netwk.data.outputLayer, netwk.activationFunction, netwk.fnpars, 1);
    
  return netwk;
}
      
// output hidden->output layer weights
NeuralNetwork updateweights_HO(NeuralNetwork netwk)
{
  double wtchg = 0.0;	// weight change
  int i;

  for (i=0; i<netwk.data.hiddenNodes; ++i)
  {
    wtchg = netwk.lr_ho * netwk.data.patternerr * netwk.data.hiddenLayer[i];
    netwk.data.weights_HO[i] += wtchg;
  }

  return netwk;
}
  
// output input->hidden layer weights
NeuralNetwork updateweights_IH(NeuralNetwork netwk)
{
  double wtchg = 0.0;
  int i, j;

  for (i=0; i<netwk.data.hiddenNodes; ++i)
    for (j=0; j<netwk.data.inputNodes; ++j)
    {
      wtchg = netwk.lr_ih * netwk.data.patternerr * deriv(netwk.data.hiddenLayer[i], netwk.activationFunction, netwk.fnpars, 1);
      wtchg *= netwk.data.weights_HO[i] * netwk.data.inputLayer[j];
      netwk.data.weights_IH[i][j] += wtchg;
    }

  return netwk;
}

// output skip layer weights
NeuralNetwork updateweights_skip(NeuralNetwork netwk)
{
  double wtchg = 0.0;
  int i;

  for (i=0; i<netwk.data.inputNodes; ++i)
  {
    wtchg = netwk.lr_skip * netwk.data.patternerr * netwk.data.inputLayer[i];
    netwk.data.weights_skip[i] += wtchg;
  }

  return netwk;
}

// train network
NeuralNetwork train(NeuralNetwork netwk, int epochs, Pattern patterns[], int numpatterns, double rmsearr[], int *converge, double tol)
{
  int i, j;
  int rmse_it = 0;
  double rmse; 
  unsigned short converged = 0;		// updated to 1 once rmse < tol

  /*
  "epoch" == iteration in which all training patterns are iterated through once, 
  output is computed, and connection weights updated according to output error
  An epoch can be thought of as a single training session.
  */
  for (i=1; i<=epochs; ++i)
  {
    rmse = 0.0;
    for (j=0; j<numpatterns; ++j)
    {
      netwk = predict(netwk, patterns[j]);
      netwk.data.patternerr = patterns[j].output - netwk.data.outputLayer;
      
      netwk = updateweights_HO(netwk);
      netwk = updateweights_IH(netwk);

      if (netwk.data.useSkipLayer)
        netwk = updateweights_skip(netwk);

      rmse += pow(netwk.data.patternerr, 2);
    }

    rmse = sqrt(rmse);
    if (rmse < tol && !converged)
    {
      *converge = i;
      printf("Converged at epoch %d. Tolerance: %f\n", i, tol);
      converged = 1;
    }

    // array to record RMSE every 10 epochs
    if (i%10 == 0)
    {
      rmsearr[rmse_it] = rmse;
      ++rmse_it;
    }
  }

  if (!*converge)
  { 
    printf("%d\n", *converge);
    printf("Network did not converge on correct output. Tolerance: %f \n", tol);
  }

  return netwk;
}




      
    
  
