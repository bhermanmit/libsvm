#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "svm_fortran.hpp"

void trim(std::string& s)
{
    size_t p = s.find_first_not_of(" \t");
    s.erase(0, p);
 
    p = s.find_last_not_of(" \t");
    if (std::string::npos != p)
      s.erase(p+1);
}

double svmpredict(svm_model *model, int *xidx, double *xval, int n)
{
    // Define a svm_node to copy data to
    svm_node *node = new svm_node[n+1];

    // Loop around each input and copy over
    for (int i = 0; i < n; i++)
    {
      node[i].index = xidx[i];
      node[i].value = xval[i];
    }

     // Set -1 index for termination
     node[n].index = -1;
     node[n].value =  0;

     // Predict y value
     double y_predict = svm_predict(model, node);

     // Free memory from node
     delete[] node;

     return y_predict;
}

svm_model *svmtrain(svm_problem *prob, svm_parameter *param)
{
    svm_model *model;
    model = svm_train(prob, param);
    return model;
}

void svmmodeldestroy(svm_model *model)
{
    svm_free_and_destroy_model(&model);
}

void svmdatafinalize(svm_problem *prob, svm_parameter *param)
{
    const char *error_msg;

    error_msg = svm_check_parameter(prob, param);

    if (error_msg)
    {
      fprintf(stderr, "ERROR: %s\n", error_msg);
      exit(1);
    }
}

svm_problem *svmproblemcreate(int n)
{
    // Allocate a pointer of svm_problem type
    svm_problem *prob = new svm_problem;

    // Set and allocate data in problem
    prob -> l = n; // copies n over to problem
    prob -> y = new double [n]; // allocate 1-D array of y
    prob -> x = new svm_node *[n]; // allocate 1-D array of pointers to svm_nodes

    return prob;
}

void svmproblemdestroy(svm_problem * prob)
{
    delete[] prob -> y;
    for (int i = 0; i < prob -> l; i++)
       delete[] prob -> x[i];
    delete[] prob -> x;    
    delete prob;
}

svm_problem *svmproblemadddata(svm_problem *prob, double y, int yidx, int *xidx, double *xval, int n)
{

    // Copy over y value
    prob -> y[yidx - 1] = y;

    // Create a new pointer to array of svm nodes
    svm_node *xtemp = new svm_node [n+1];
    for (int i = 0; i < n; i++)
    {
      xtemp[i].index = xidx[i];
      xtemp[i].value = xval[i];
    }

    // Terminate array with -1 for index
    xtemp[n].index = -1;
    xtemp[n].value = 0;

    // Point first value in x to first value of xtemp
    prob -> x[yidx - 1] = &xtemp[0];

    return prob;
}

void svmproblemprintdata(svm_problem *prob, int i)
{
    // Create temporary pointer to a svm_mode
    svm_node * a_node;

    // Print out y value
    printf("Y VALUE: %f\n", prob -> y[i-1]);

    // Point a_node to the first value in x, loop around and print
    a_node = prob -> x[i-1];
    while(a_node -> index != -1)
    {
       printf("X INDEX: %d  X VALUE: %f\n", a_node -> index, a_node -> value);
       a_node ++;
    }
}

svm_parameter *svmparametercreate()
{
    // Allocate a svm_parameter
    svm_parameter *param = new svm_parameter;

    // Setup default values
    param -> svm_type = EPSILON_SVR;
    param -> kernel_type = RBF;
    param -> degree = 3;
    param -> gamma = 0;    // 1/num_features
    param -> coef0 = 0;
    param -> nu = 0.5;
    param -> cache_size = 100;
    param -> C = 1;
    param -> eps = 1e-3;
    param -> p = 0.1;
    param -> shrinking = 1;
    param -> probability = 0;
    param -> nr_weight = 0;
    param -> weight_label = NULL;
    param -> weight = NULL;

    return param;
}

void svmparameterdestroy(svm_parameter *param)
{
    svm_destroy_param(param);
    delete param;
}

svm_parameter *svmparameterset(svm_parameter *param, const char *optstr, void *val)
{

    // Copy fortran string to C++ string and trim whitespace off
    std :: string optionstr = optstr;
    trim(optionstr);

    if (optionstr == "svm_type")
    {
      const char *valchar = static_cast<const char*>(val);
      std :: string tempstr = valchar; 
      trim(tempstr);
      if (tempstr == "c_svc")
        param -> svm_type = C_SVC;
      else if (tempstr == "nu_svc")
        param -> svm_type = NU_SVC;
      else if (tempstr == "one_class")
        param -> svm_type = ONE_CLASS;
      else if (tempstr == "epsilon_svr")
        param -> svm_type = EPSILON_SVR;
      else if (tempstr == "nu_svr")
        param -> svm_type = NU_SVR;
      else 
        printf("SVM TYPE not recognized.\n");
    }
    else if (optionstr == "kernel_type")
    {
      const char *valchar = static_cast<const char*>(val);
      std :: string tempstr = valchar;
      trim(tempstr);
      if (tempstr == "linear")
        param -> kernel_type = LINEAR;
      else if (tempstr == "poly")
        param -> kernel_type = POLY;
      else if (tempstr == "rbf")
        param -> kernel_type = RBF;
      else if (tempstr == "sigmoid")
        param -> kernel_type = SIGMOID;
      else if (tempstr == "precomputed")
        param -> kernel_type = PRECOMPUTED;
      else
        printf("KERNEL TYPE not recognized.\n");
    }
    else if (optionstr == "degree")
    {
      int *valint = static_cast<int *>(val);
      param -> degree = *valint;
    }
    else if (optionstr == "gamma")
    {
      double *valdoub = static_cast<double *>(val);
      param -> gamma = *valdoub;
    }
    else if (optionstr == "coef0")
    {
      double *valdoub = static_cast<double *>(val);
      param -> coef0 = *valdoub;
    }
    else if (optionstr == "cache_size") 
    {
      double *valdoub = static_cast<double *>(val);
      param -> cache_size = *valdoub;
    }
    else if (optionstr == "eps")
    { 
      double *valdoub = static_cast<double *>(val);
      param -> eps = *valdoub;
    }
    else if (optionstr == "C")
    {
      double *valdoub = static_cast<double *>(val);
      param -> C = *valdoub;
    }
    else if (optionstr == "nr_weight")    
    {
      int *valint = static_cast<int *>(val);
      param -> nr_weight = *valint;
    }
    else if (optionstr == "nu")
    {
      double *valdoub = static_cast<double *>(val);
      param -> nu = *valdoub;
    }
    else if (optionstr == "p")
    {
      double *valdoub = static_cast<double *>(val);
      param -> p = *valdoub;
    }
    else if (optionstr == "probability")
    {
      int *valint = static_cast<int *>(val);
      param -> probability = *valint;
    }
    else if (optionstr == "shrinking")
    {
      int *valint = static_cast<int *>(val);
      param -> shrinking = *valint;
    }
    else 
      printf("Parameter option not recognized.\n");

    return param;
} 

void svmparameterprint(svm_parameter *param)
{
    // Print out all parameter options
    printf(" SVM TYPE: %d\n", param -> svm_type);
    printf(" KERNEL TYPE: %d\n", param -> kernel_type);
    printf(" DEGREE: %d\n", param -> degree);
    printf(" GAMMA: %f\n", param -> gamma);
    printf(" COEF0: %f\n", param -> coef0);
    printf(" Nu: %f\n", param -> nu);
    printf(" Cache Size: %f\n", param -> cache_size);
    printf(" C: %f\n", param -> C);
    printf(" EPS: %f\n", param -> eps);
    printf(" P: %f\n", param -> p);
    printf(" SHRINKING: %d\n", param -> shrinking);
    printf(" PROBABILITY: %d\n", param -> probability);
    printf(" NR WEIGHT: %d\n", param -> nr_weight);
}
