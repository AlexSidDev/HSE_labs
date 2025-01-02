// MaximumClique.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <list>
#include <chrono>
#include <memory>
#include <numeric>
#include <algorithm>

std::unordered_set<int>* collection_difference(const std::vector<int>& first, const std::unordered_set<int>& second)
{
    std::unordered_set<int>* diff = new std::unordered_set<int>;
    diff->reserve(first.size());
    for (const int& elem : first)
    {
        if (second.find(elem) == second.end())
        {
            diff->insert(elem);
        }
    }
    return diff;
}

class Graph
{
    int verts;
    std::vector<int> pows;
    std::vector<float> weights;
    std::vector<float> sum_of_weights;
    std::vector<std::unordered_set<int>> adjacency_list;

    void initialize_weights()
    {
        weights.reserve(verts);
        sum_of_weights.reserve(verts);
        for (size_t i = 0; i < verts; i++)
        {
            weights.push_back(std::ceil(10 * i / verts) * 0.1);
        }

        for (size_t i = 0; i < verts; i++)
        {
            float neighbours_weights_sum = 0;
            for (auto& neighbour : adjacency_list[i])
            {
                neighbours_weights_sum += weights[neighbour];
            }
            sum_of_weights.push_back(neighbours_weights_sum + weights[i]);
        }
    }

    int* sort_verts()
    {
        int* sorted_verts = new int[verts];
        float min_pow;
        int min_ind;
        bool is_need_random_change;
        for (int i = 0; i < verts; i++)
        {
            min_ind = 0;
            min_pow = INT_MAX;
            for (int j = 0; j < verts; j++)
            {   // random changing of same-degree vertice's order
                if (sum_of_weights[j] == min_pow)
                {
                    is_need_random_change = std::rand() % 3 > 0;
                    if (is_need_random_change)
                        min_ind = j;
                }
                else if (sum_of_weights[j] < min_pow)
                {
                    min_ind = j;
                    min_pow = sum_of_weights[j];
                }
            }
            //for (int neighbour : adjacency_list[min_ind])
            //{
            //    pows[neighbour]--;
            //}
            sum_of_weights[min_ind] = INT_MAX;
            sorted_verts[verts - i - 1] = min_ind;
        }
        return sorted_verts;
    }

public:
    Graph(std::ifstream& input_file)
    {
        std::string line;
        char e;
        int src_vert, tgt_vert;
        int edges;
        std::string p, edge;

        verts = 0;
        while (std::getline(input_file, line))
        {
            if (line[0] == 'p')
            {
                std::istringstream iss(line);
                iss >> p >> edge >> verts >> edges;
                break;
            }
        }
        pows.resize(verts);
        adjacency_list.resize(verts);

        while (std::getline(input_file, line))
        {
            std::istringstream iss(line);
            iss >> e >> src_vert >> tgt_vert;
            if (tgt_vert > src_vert)
                continue;
            adjacency_list[src_vert - 1].insert(tgt_vert - 1);
            adjacency_list[tgt_vert - 1].insert(src_vert - 1);
            pows[src_vert - 1]++;
            pows[tgt_vert - 1]++;
        }
    }

    Graph(std::vector<std::unordered_set<int>>& adjacency_list)
    {
        this->adjacency_list = adjacency_list;
        verts = adjacency_list.size();
        pows.reserve(verts);
        for (size_t i = 0; i < verts; i++)
        {
            pows.push_back(adjacency_list[i].size());
        }
    }

    bool check_clique(std::vector<int> clique, int vertex_to_insert)
    {
        bool is_dense = true;

        for (int verts_in_clique : clique)
        {
            is_dense &= adjacency_list[verts_in_clique].find(vertex_to_insert) != adjacency_list[verts_in_clique].end();
            if (!is_dense)
                break;
        }
        return is_dense;
    }

    float find_max_weighted_clique(int max_iters, int random_candidates)
    {
        initialize_weights();
        int* sorted_verts = sort_verts();
        int cur_ver, start_vert, last_ind = 0, global_last_index = -1;
        bool is_dense;
        std::vector<int> clique;
        std::vector<int> best_clique;
        float weights_sum = 0;
        float best_weights_sum = 0;

        clique.reserve(verts);
        best_clique.reserve(verts);

        int starts = 0;
        int* candidates = new int[random_candidates];
        int candidate_iterations = 0;

        int rand_ind = 0;

        while (starts < max_iters)
        {
            for (int i = global_last_index + 1; i < verts; i++)
            {
                cur_ver = sorted_verts[i];
                //if (adjacency_list[cur_ver].size() * weights[cur_ver] <= best_weights_sum)
                //    continue;
                is_dense = check_clique(clique, cur_ver);
                if (is_dense)
                {
                    if (candidate_iterations == 0)
                    {
                        last_ind = i;
                    }

                    candidates[candidate_iterations] = cur_ver;
                    candidate_iterations++;
                }
                if (candidate_iterations == random_candidates)
                {
                    start_vert = candidates[(std::rand() % random_candidates)];
                    clique.push_back(start_vert);
                    weights_sum += weights[start_vert];
                    candidate_iterations = 0;
                    global_last_index = i;
                    i = last_ind;
                }
            }
            if (weights_sum > best_weights_sum)
            {
                best_clique = clique;
                best_weights_sum = weights_sum;
            }

            starts++;
            candidate_iterations = 0;
            clique.clear();

            rand_ind = std::rand() % (verts / 2);
            start_vert = sorted_verts[rand_ind];
            global_last_index = -1;

            clique.push_back(start_vert);
            weights_sum = weights[start_vert];
        }
        return best_weights_sum;
    }

    Graph* get_graph_complement()
    {
        std::vector<int> all_verts(verts);
        std::iota(all_verts.begin(), all_verts.end(), 0);
        std::vector <std::unordered_set<int>> non_neighbours_list;
        non_neighbours_list.resize(verts);
        for (size_t i = 0; i < verts; i++)
        {
            non_neighbours_list[i] = *collection_difference(all_verts, adjacency_list[i]);
        }
        Graph* complement = new Graph(non_neighbours_list);
        return complement;
    }
};


int main()
{
    std::srand(std::time(0));

    std::vector<std::string> files = {
       "brock200_1.clq", "brock200_2.clq", "brock200_3.clq", "brock200_4.clq",
       "c-fat200-1.clq", "c-fat200-2.clq", "c-fat200-5.clq", "c-fat500-1.clq", "c-fat500-10.clq", "c-fat500-2.clq", "c-fat500-5.clq",
       "C125.9.clq", "gen200_p0.9_44.clq", "gen200_p0.9_55.clq",
        "johnson8-2-4.clq", "johnson8-4-4.clq", "johnson16-2-4.clq",
        "hamming6-2.clq", "hamming6-4.clq", "hamming8-2.clq", "hamming8-4.clq",
       "keller4.clq",
       "MANN_a9.clq", "MANN_a27.clq", "MANN_a45.clq",
       "p_hat300-1.clq", "p_hat300-2.clq", "p_hat300-3.clq",
       "san200_0.7_1.clq", "san200_0.7_2.clq",
        "san200_0.9_1.clq", "san200_0.9_2.clq", "san200_0.9_3.clq", "sanr200_0.7.clq",
    };

    std::ofstream fout("ind_sets.csv");
    fout << "File; Clique; Time (sec)\n";

    float clique_size;

    for (std::string file : files)
    {
        std::ifstream input_file("max_clique_txt/DIMACS_all_ascii/" + file);
        if (!input_file.is_open())
        {
            std::cout << "File wasn't open\n";
            continue;
        }
        Graph graph(input_file);

        auto start = std::chrono::high_resolution_clock::now();
        Graph* complement = graph.get_graph_complement();
        clique_size = complement->find_max_weighted_clique(10000, 1);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        std::cout << "Largest clique: " << clique_size << '\n';

        //input_file = std::ifstream(file);
        //greedy_search_random_start(input_file);
        fout << file << "; " << clique_size << "; " << duration.count() / 1000000.0 << '\n';
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
