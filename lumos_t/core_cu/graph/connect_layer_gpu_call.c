#include "connect_layer_gpu_call.h"

void call_forward_connect_layer_gpu(void **params, void **ret)
{
    int *ksize = (int*)params[0];
    int *bias = (int*)params[1];
    char *active = (char*)params[2];
    int *h = (int*)params[4];
    int *w = (int*)params[5];
    int *c = (int*)params[6];
    int *num = (int*)params[7];
    float *input = (float*)params[8];
    float *output = (float*)params[9];
    float *kernel_weights = (float*)params[10];
    float *bias_weights = (float*)params[11];
    float *input_g = NULL;
    float *output_g = NULL;
    float *kernel_weights_g = NULL;
    float *bias_weights_g = NULL;
    Layer *l = make_connect_layer(ksize[0], bias[0], 0, active);
    init_connect_layer(l, w[0], h[0], c[0]);
    cudaMalloc((void**)&input_g, num[0]*l->inputs*sizeof(float));
    cudaMalloc((void**)&output_g, num[0]*l->outputs*sizeof(float));
    cudaMalloc((void**)&kernel_weights_g, l->kernel_weights_size*sizeof(float));
    cudaMalloc((void**)&bias_weights_g, l->bias_weights_size*sizeof(float));
    cudaMemcpy(input_g, input, num[0]*l->inputs*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(output_g, output, num[0]*l->outputs*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(kernel_weights_g, kernel_weights, l->kernel_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(bias_weights_g, bias_weights, l->bias_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    l->input = input_g;
    l->output = output_g;
    l->kernel_weights = kernel_weights_g;
    l->bias_weights = bias_weights_g;
    l->forward(*l, num[0]);
    cudaMemcpy(output, output_g, num[0]*l->outputs*sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(input_g);
    cudaFree(output_g);
    cudaFree(kernel_weights_g);
    cudaFree(bias_weights_g);
    ret[0] = output;
}

void call_backward_connect_layer_gpu(void **params, void **ret)
{
    int *ksize = (int*)params[0];
    int *bias = (int*)params[1];
    char *active = (char*)params[2];
    int *h = (int*)params[4];
    int *w = (int*)params[5];
    int *c = (int*)params[6];
    float *rate = (float*)params[7];
    int *num = (int*)params[8];
    float *l_delta = (float*)params[9];
    float *n_delta = (float*)params[10];
    float *input = (float*)params[11];
    float *output = (float*)params[12];
    float *kernel_weights = (float*)params[13];
    float *update_kernel_weights = (float*)params[14];
    float *bias_weights = (float*)params[15];
    float *update_bias_weights = (float*)params[16];
    float *workspace = (float*)params[17];
    float *l_delta_g = NULL;
    float *n_delta_g = NULL;
    float *input_g = NULL;
    float *output_g = NULL;
    float *kernel_weights_g = NULL;
    float *update_kernel_weights_g = NULL;
    float *bias_weights_g = NULL;
    float *update_bias_weights_g = NULL;
    float *workspace_g = NULL;
    Layer *l = make_connect_layer(ksize[0], bias[0], 0, active);
    init_connect_layer(l, w[0], h[0], c[0]);
    cudaMalloc((void**)&l_delta_g, num[0]*l->inputs*sizeof(float));
    cudaMalloc((void**)&n_delta_g, num[0]*l->outputs*sizeof(float));
    cudaMalloc((void**)&input_g, num[0]*l->inputs*sizeof(float));
    cudaMalloc((void**)&output_g, num[0]*l->outputs*sizeof(float));
    cudaMalloc((void**)&kernel_weights_g, l->kernel_weights_size*sizeof(float));
    cudaMalloc((void**)&update_kernel_weights_g, l->kernel_weights_size*sizeof(float));
    cudaMalloc((void**)&bias_weights_g, l->bias_weights_size*sizeof(float));
    cudaMalloc((void**)&update_bias_weights_g, l->bias_weights_size*sizeof(float));
    cudaMalloc((void**)&workspace_g, l->workspace_size*sizeof(float));
    cudaMemcpy(l_delta_g, l_delta, num[0]*l->inputs*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(n_delta_g, n_delta, num[0]*l->outputs*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(input_g, input, num[0]*l->inputs*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(output_g, output, num[0]*l->outputs*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(kernel_weights_g, kernel_weights, l->kernel_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(update_kernel_weights_g, update_kernel_weights, l->kernel_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(bias_weights_g, bias_weights, l->bias_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(update_bias_weights_g, update_bias_weights, l->bias_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(workspace_g, workspace, l->workspace_size*sizeof(float), cudaMemcpyHostToDevice);
    l->input = input_g;
    l->output = output_g;
    l->delta = l_delta_g;
    l->kernel_weights = kernel_weights_g;
    l->update_kernel_weights = update_kernel_weights_g;
    l->bias_weights = bias_weights_g;
    l->update_bias_weights = update_bias_weights_g;
    l->workspace = workspace_g;
    l->backward(*l, rate[0], num[0], n_delta_g);
    cudaMemcpy(l_delta, l_delta_g, num[0]*l->inputs*sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(update_kernel_weights, update_kernel_weights_g, l->kernel_weights_size*sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(update_bias_weights, update_bias_weights_g, l->bias_weights_size*sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(l_delta_g);
    cudaFree(n_delta_g);
    cudaFree(input_g);
    cudaFree(output_g);
    cudaFree(kernel_weights_g);
    cudaFree(update_kernel_weights_g);
    cudaFree(bias_weights_g);
    cudaFree(update_bias_weights_g);
    cudaFree(workspace_g);
    ret[0] = l_delta;
    ret[1] = update_kernel_weights;
    ret[2] = update_bias_weights;
}
