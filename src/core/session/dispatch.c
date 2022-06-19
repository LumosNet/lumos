#include "dispatch.h"

void session_run(Session *sess, int epoch, int batch, int subdivision)
{
    fprintf(stderr, "\nSession Start To Running\n");
    fprintf(stderr, "Epoch   Batch   Subdivision\n");
    fprintf(stderr, "%3d     %3d     %3d\n", epoch, batch, subdivision);
    sess->epoch = epoch;
    sess->batch = batch;
    sess->subdivision = subdivision;
    for (int i = 0; i < sess->epoch; ++i){
        int sub_epoch = (int)(sess->batch / sess->subdivision);
        for (int j = 0; j < sub_epoch; ++j){
            load_data(sess, i*sess->batch+j*sess->subdivision, sess->subdivision);
            forward_session(sess);
            backward_session(sess);
        }
    }
}

void forward_session(Session *sess)
{
    Graph *graph = sess->graph;
    Layer **layers = graph->layers;
    Layer *l;
    float *input = sess->input;
    for (int i = 0; i < graph->layer_num; ++i){
        l = layers[i];
        l->input = input;
        l->forward(*l);
        input = l->output;
    }
}

void backward_session(Session *sess)
{
    Graph *graph = sess->graph;
    Layer **layers = graph->layers;
    Layer *l;
    float *delta = NULL;
    for (int i = graph->layer_num-1; i >= 0; --i){
        l = layers[i];
        l->backward(*l, delta);
        delta = l->delta;
    }
}

void update_session(Session *sess)
{
    Graph *graph = sess->graph;
    Layer **layers = graph->layers;
    Layer *l;
    float rate = sess->learning_rate / sess->batch;
    float *delta = NULL;
    for (int i = graph->layer_num-1; i >= 0; --i){
        l = layers[i];
        l->update(*l, rate, delta);
        delta = l->delta;
    }
}

void create_run_scene(Session *sess, int h, int w, int c, char *dataset_list_file)
{
    set_input_dimension(sess, h, w, c);
    bind_train_data(sess, dataset_list_file);
}


void init_run_scene(Session *sess, char *weights_file)
{
    init_graph(sess->graph, sess->width, sess->height, sess->channel);
    create_run_memory(sess);
    set_graph_memory(sess);
    init_weights(sess, weights_file);
    set_graph_weight(sess);
}