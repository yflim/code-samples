#include <stdlib.h>
#include <stdio.h>
#define BIAS_LOW 0.0
#define BIAS_HIGH 0.25
#define WEIGHT_CAP 1.0

typedef struct neural_network_data
{
  // numbers of hidden, input, and output nodes; 
  int hiddenNodes, inputNodes, outputNodes, numpatterns;

  // binary variable to indicate whether "skip layer" should be used
  unsigned short useSkipLayer;

  // values in the input layer
  double *inputLayer;

  // values in the hidden layer
  double *hiddenLayer;

  // values in the output layer - only one node in output layer here
  double outputLayer;
  
  // input->hidden layer weights
  double **weights_IH;

  // hidden->output layer weights
  double *weights_HO;
  
  // input->output layer weights (the skip layer)
  double *weights_skip;
  
  // network error evaluation using rmse (root mean squared error) 
  double patternerr;

} NetworkData;
  
// a struct that keeps the NetworkData data structure as a member:
typedef struct neuralnetwork
{
  // hidden and output node biases (0 if not used)
  double hbias;
  double obias;

  // 0 for logistic (only one supported)
  unsigned short activationFunction;

  // activation function parameters (steepness in the case of logistic)
  double *fnpars;

  /*
  lr_ih, lr_ho, lr_skip: learning rate for the input->hidden, hidden->output, skip layer connection weights
  learning rate affects how sharply weights are adjusted in response to patternerr (rmse) for each input pattern
  in each training iteration ("epoch")
  */
  double lr_ih, lr_ho, lr_skip;
  
  NetworkData data;

} NeuralNetwork;

// training pattern (correct input-output relationships)
typedef struct pattern
{
  double *inputs;
  double output;
} Pattern;

double generateRand(double high, double low);
NetworkData makenetworkdat(int inputs, int hiddens, unsigned short skip, double ho_low, double ho_high,
                           double ih_low, double ih_high, double skip_low, double skip_high);
NeuralNetwork makeneuralnetwork(int inputs, int hiddens, unsigned short skip, int fn, double *fnpars, double learnr, int bias,
                           double ho_low, double ho_high, double ih_low, double ih_high, double skip_low, double skip_high);
double activate(double num, unsigned short fn, double *fnpars, unsigned short numpars);
double deriv(double num, unsigned short fn, double *fnpars, unsigned short numpars);
NeuralNetwork predict(NeuralNetwork netwk, Pattern pat);
NeuralNetwork updateweights_HO(NeuralNetwork netwk);
NeuralNetwork updateweights_IH(NeuralNetwork netwk);
NeuralNetwork updateweights_skip(NeuralNetwork netwk);
NeuralNetwork train(NeuralNetwork netwk, int epochs, Pattern patterns[], int numpatterns, double rmsearr[], int *converge, double tol);

