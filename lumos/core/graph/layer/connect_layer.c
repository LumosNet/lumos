#include "connect_layer.h"

Layer *make_connect_layer(int output, int bias, char *active, char *weights_init)
{
    Layer *l = malloc(sizeof(Layer));
    l->type = CONNECT;
    l->weights_init_type = "guass";
    if (weights_init){
        l->weights_init_type = weights_init;
    }
    l->filters = 1;
    l->weights = 1;

    l->ksize = output;

    l->active_str = active;
    Activation type = load_activate_type(active);
    l->active = type;
    l->gradient = type;

    l->bias = bias;
#ifdef GPU
    l->forward = forward_connect_layer_gpu;
    l->backward = backward_connect_layer_gpu;
    l->update = update_connect_layer_gpu;
#else
    l->forward = forward_connect_layer;
    l->backward = backward_connect_layer;
    l->update = update_connect_layer;
#endif
    l->init_layer_weights = init_connect_weights;

    fprintf(stderr, "Connect         Layer    :    [output=%4d, bias=%d, active=%s]\n", l->ksize, l->bias, l->active_str);
    return l;
}

Layer *make_connect_layer_by_cfg(CFGParams *p)
{
    int output = 0;
    int bias = 0;
    char *active = NULL;

    CFGParam *param = p->head;
    while (param)
    {
        if (0 == strcmp(param->key, "output"))
        {
            output = atoi(param->val);
        }
        else if (0 == strcmp(param->key, "active"))
        {
            active = param->val;
        }
        else if (0 == strcmp(param->key, "bias"))
        {
            bias = atoi(param->val);
        }
        param = param->next;
    }

    Layer *l = make_connect_layer(output, bias, active, NULL);
    return l;
}

void init_connect_layer(Layer *l, int w, int h, int c)
{
    l->input_h = h;
    l->input_w = w;
    l->input_c = c;
    l->inputs = l->input_h * l->input_w * l->input_c;

    l->output_h = l->ksize;
    l->output_w = 1;
    l->output_c = 1;
    l->outputs = l->output_h * l->output_w * l->output_c;

    l->workspace_size = l->input_c * l->input_h * l->input_w * l->output_c * l->output_h * l->output_w;

    l->kernel_weights_size = l->inputs * l->outputs;
    l->bias_weights_size = 0;
    if (l->bias)
    {
        l->bias_weights_size = l->outputs;
    }
    l->deltas = l->inputs;

    fprintf(stderr, "Connect         Layer    %3d*%3d*%3d ==> %3d*%3d*%3d\n",
            l->input_w, l->input_h, l->input_c, l->output_w, l->output_h, l->output_c);
}

void init_connect_weights(Layer *l)
{
#ifdef GPU
    float kernel_weights[l->kernel_weights_size];
    float bias_weights[l->bias_weights_size];
#else
    float *kernel_weights = l->kernel_weights;
    float *bias_weights = l->bias_weights;
#endif
    if (0 == strcmp(l->weights_init_type, "guass")){
        guass_list(0, 1, l->index*2, l->kernel_weights_size, kernel_weights);
    }
    if (l->bias){
        for (int i = 0; i < l->output_h; ++i){
            bias_weights[i] = 0.01;
        }
    }
#ifdef GPU
    cudaMemcpy(l->kernel_weights, kernel_weights, l->kernel_weights_size*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(l->bias_weights, bias_weights, l->bias_weights_size*sizeof(float), cudaMemcpyHostToDevice);
#endif
}

void forward_connect_layer(Layer l, int num)
{
    for (int i = 0; i < num; ++i)
    {
        int offset_i = i * l.inputs;
        int offset_o = i * l.outputs;
        float *input = l.input + offset_i;
        float *output = l.output + offset_o;
        gemm(0, 0, l.outputs, l.inputs, l.inputs, 1,
             1, l.kernel_weights, input, output);
        // if (l.index == 2){
        //     printf("\n\n\n");
        //     for (int j = 0; j < l.bias_weights_size; ++j){
        //         printf("%f ", l.bias_weights[j]);
        //     }
        //     printf("\n\n\n");
        // }
        if (l.bias)
        {
            add_bias(output, l.bias_weights, l.ksize, 1);
        }
        activate_list(output, l.outputs, l.active);
    }
}

void backward_connect_layer(Layer l, float rate, int num, float *n_delta)
{
    for (int i = 0; i < num; ++i)
    {
        int offset_i = i * l.inputs;
        int offset_o = i * l.outputs;
        float *output = l.output + offset_o;
        float *delta_l = l.delta + offset_i;
        float *delta_n = n_delta + offset_o;
        gradient_list(output, l.outputs, l.gradient);
        matrix_multiply_cpu(delta_n, output, l.outputs, delta_n);
        gemm(1, 0, l.output_h, l.input_h, l.output_h, l.input_w, 1,
             l.kernel_weights, delta_n, delta_l);
    }
    l.update(l, rate, num, n_delta);
}

void update_connect_layer(Layer l, float rate, int num, float *n_delta)
{
    for (int i = 0; i < num; ++i)
    {
        int offset_i = i * l.inputs;
        int offset_o = i * l.outputs;
        float *input = l.input + offset_i;
        float *delta_n = n_delta + offset_o;
        gemm(0, 1, l.output_h, l.output_w,
             l.input_h, l.input_w, 1,
             delta_n, input, l.workspace);
        saxpy_cpu(l.update_kernel_weights, l.workspace, l.output_h * l.input_h, rate, l.update_kernel_weights);
        if (l.bias)
        {
            saxpy_cpu(l.update_bias_weights, delta_n, l.outputs, rate, l.update_bias_weights);
        }
    }
}
