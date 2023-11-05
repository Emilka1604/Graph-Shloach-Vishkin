#include <shiloach_vishkin.h>
#include <igraph/igraph_components.h>
#include <iostream>
#include <unordered_set>
#include <random>
#include <algorithm>

// #define NO_USED

#ifdef NO_USED
igraph_error_t shiloach_vishkin_test() {
    igraph_t graph;
    igraph_vector_int_t edges;
    igraph_vector_int_t membership;
    igraph_vector_int_init(&membership, 0);
    /* Create a directed graph with no vertices or edges. */
    igraph_empty(&graph, 10, IGRAPH_UNDIRECTED);
    igraph_vector_int_init_int(&edges, 6,
                               0,3, 0,3, 1,5);
    igraph_add_edges(&graph, &edges, NULL);

    shiloach_vishkin(&graph, &membership);

    igraph_integer_t count = igraph_vector_int_size(&membership);
    
    for (igraph_integer_t i = 0; i < count; ++i) {
        std::cout << VECTOR(membership)[i] << " ";
    }

    igraph_vector_int_destroy(&membership);
    igraph_vector_int_destroy(&edges);
    igraph_destroy(&graph);
    return IGRAPH_SUCCESS;
}

igraph_error_t cc_test() {
    igraph_t graph;
    igraph_vector_int_t edges;
    igraph_vector_int_t membership;
    igraph_vector_int_init(&membership, 0);
    /* Create a directed graph with no vertices or edges. */
    igraph_empty(&graph, 5, IGRAPH_UNDIRECTED);
    igraph_vector_int_init_int(&edges, 4,
                               3, 4, 4, 1);
    igraph_add_edges(&graph, &edges, NULL);

    igraph_connected_components(&graph, &membership, NULL, NULL, IGRAPH_WEAK);

    igraph_integer_t count = igraph_vector_int_size(&membership);
    
    for (igraph_integer_t i = 0; i < count; ++i) {
        std::cout << VECTOR(membership)[i] << " ";
    }

    igraph_vector_int_destroy(&membership);
    igraph_vector_int_destroy(&edges);
    igraph_destroy(&graph);
    return IGRAPH_SUCCESS;
}
#endif

igraph_error_t check(const igraph_t *graph, igraph_vector_int_t* membership) {
    std::unordered_set<igraph_integer_t> set;
    igraph_integer_t count = igraph_vector_int_size(membership);
    // for (igraph_integer_t i = 0; i < count; ++i) {
    //     std::cout << VECTOR(*membership)[i] << " ";
    // }
    if(count != igraph_vcount(graph)) return IGRAPH_FAILURE;
    for (igraph_integer_t i = 0; i < count; ++i) {
        auto curr_component = VECTOR(*membership)[i];
        if(curr_component == -1) continue;
        if(set.find(curr_component) == set.end()) {
            set.insert(curr_component);
            igraph_vector_int_t verts_from_same_component;
            igraph_vector_int_init(&verts_from_same_component, 0);
            IGRAPH_CHECK(igraph_subcomponent(graph, &verts_from_same_component, i, IGRAPH_ALL));
            igraph_integer_t verts_from_same_component_size = igraph_vector_int_size(&verts_from_same_component);
            for (igraph_integer_t j = 0; j < verts_from_same_component_size; ++j) {
                if(VECTOR(*membership)[VECTOR(verts_from_same_component)[j]] != curr_component) {
                    return IGRAPH_FAILURE;
                }
                VECTOR(*membership)[VECTOR(verts_from_same_component)[j]] = -1;
            }
            igraph_vector_int_destroy(&verts_from_same_component);
        } else {
            return IGRAPH_FAILURE;
        }
    }
    return IGRAPH_SUCCESS;
}

igraph_integer_t graph_size_generate(igraph_integer_t minSize, igraph_integer_t maxSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<igraph_integer_t> distrib(minSize, maxSize);
    return distrib(gen);
}

void fill_adj_matrix(igraph_matrix_t* m, igraph_integer_t graph_size, igraph_integer_t fill_percent) {
    igraph_integer_t possible_edges_number = (graph_size-1)*graph_size/2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<igraph_integer_t> distrib(0, possible_edges_number);
    igraph_integer_t edges_num = distrib(gen) / 100 * fill_percent;
    // std::cout << possible_edges_number << " " << edges_num << "\n";
    std::uniform_int_distribution<igraph_integer_t> distrib1(0, graph_size-1);
    for(igraph_integer_t i = 0; i < edges_num; ++i) {
        igraph_integer_t x = distrib1(gen);
        igraph_integer_t y = distrib1(gen);
        igraph_matrix_set(m, std::min(x,y), std::max(x,y), 1);
    }
}

igraph_error_t create_graph(igraph_t *graph, igraph_integer_t minSize, igraph_integer_t maxSize, igraph_integer_t fill_percent) {
    igraph_matrix_t m;
    igraph_integer_t graph_size = graph_size_generate(minSize, maxSize);
    IGRAPH_CHECK(igraph_matrix_init(&m, graph_size, graph_size));
    fill_adj_matrix(&m, graph_size, fill_percent);
    IGRAPH_CHECK(igraph_adjacency(graph, &m, IGRAPH_ADJ_UPPER, IGRAPH_NO_LOOPS));
    igraph_matrix_destroy(&m);
    return IGRAPH_SUCCESS;
}

igraph_error_t test(igraph_integer_t minSize, igraph_integer_t maxSize, igraph_integer_t fill_percent) {
    igraph_t graph;
    igraph_vector_int_t membership;
    IGRAPH_CHECK(igraph_vector_int_init(&membership, 0));
    IGRAPH_CHECK(create_graph(&graph, minSize, maxSize, fill_percent));
    IGRAPH_CHECK(shiloach_vishkin(&graph, &membership));
    // igraph_integer_t no;
    // IGRAPH_CHECK(igraph_connected_components(&graph, &membership, NULL, &no, IGRAPH_WEAK));
    // std::cout << no << " " << igraph_vcount(&graph);
    IGRAPH_CHECK(check(&graph, &membership));
    igraph_vector_int_destroy(&membership);
    igraph_destroy(&graph);
    return IGRAPH_SUCCESS;
}


int main() {

    std::size_t i = 0;
    while(test(1000, 2000, 5) == IGRAPH_SUCCESS) {
        std::cout << "ok";
    }

    return 0;
}