#include <igraph/igraph.h>

inline igraph_error_t shiloach_vishkin(const igraph_t *graph, igraph_vector_int_t *membership) {
    igraph_integer_t no_of_nodes = igraph_vcount(graph);
    igraph_vector_int_t neis;
    IGRAPH_CHECK(igraph_vector_int_init(&neis, 0));
    if (membership) {
        IGRAPH_CHECK(igraph_vector_int_resize(membership, no_of_nodes));
    }
    for (igraph_integer_t i = 0; i < no_of_nodes; ++i) {
        VECTOR(*membership)[i] = i;
    }
    bool hooking = true;
    while(hooking) {
        hooking = false;
        #pragma omp parallel for
        for (igraph_integer_t act_node = 0; act_node < no_of_nodes; ++act_node) {
            IGRAPH_CHECK(igraph_neighbors(graph, &neis, act_node, IGRAPH_ALL));
            igraph_integer_t nei_count = igraph_vector_int_size(&neis);
            #pragma omp parallel for
            for (igraph_integer_t i = 0; i < nei_count; ++i) {
                #pragma omp critical
                {
                    igraph_integer_t act_node_component = VECTOR(*membership)[act_node];
                    igraph_integer_t neighbor_component = VECTOR(*membership)[VECTOR(neis)[i]];
                    if(act_node_component < neighbor_component && neighbor_component == VECTOR(*membership)[neighbor_component]) {
                        VECTOR(*membership)[neighbor_component] = act_node_component; 
                        hooking = true;
                    }
                }
            }
        }
        #pragma omp parallel for
        for(igraph_integer_t i = 0; i < no_of_nodes; ++i) {
            #pragma omp critical
            {
                while(VECTOR(*membership)[VECTOR(*membership)[i]] != VECTOR(*membership)[i]) {
                    VECTOR(*membership)[i] = VECTOR(*membership)[VECTOR(*membership)[i]]; 
                }
            }
        }
    }
    igraph_vector_int_destroy(&neis);
    return IGRAPH_SUCCESS;
}