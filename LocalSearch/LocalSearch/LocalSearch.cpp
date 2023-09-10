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

struct Pair
{
    int new_vert;
    int old_vert;

    Pair(int n_v, int o_v)
    {
        new_vert = n_v;
        old_vert = o_v;
    }

    Pair& operator=(const Pair& other)
    {
        new_vert = other.new_vert;
        old_vert = other.old_vert;
        return *this;
    }

    Pair() {}
};

int* sort_verts(int verts, const std::vector<std::unordered_set<int>>& adjacency_list, std::vector<int>& pows)
{
    int* sorted_verts = new int[verts];
    int min_pow;
    int min_ind;
    for (int i = 0; i < verts; i++)
    {
        min_ind = 0;
        min_pow = INT_MAX;
        for (int j = 0; j < verts; j++)
        {
            if (pows[j] < min_pow)
            {
                min_ind = j;
                min_pow = pows[j];
            }
        }
        for (int neighbour : adjacency_list[min_ind])
        {
            pows[neighbour]--;
        }
        pows[min_ind] = INT_MAX;
        sorted_verts[verts - i - 1] = min_ind;
    }
    return sorted_verts;
}

void read_graph(std::ifstream& input_file, int& verts, int& edges, std::vector<std::unordered_set<int>>& adjacency_list, std::vector<int>& pows)
{
    std::string line;
    char e;
    int src_vert, tgt_vert;
    int color;
    std::string p, edge;

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
        adjacency_list[src_vert - 1].insert(tgt_vert - 1);
        adjacency_list[tgt_vert - 1].insert(src_vert - 1);
        pows[src_vert - 1]++;
        pows[tgt_vert - 1]++;
    }
}

std::vector<Pair>* get_neighbourhoods(const std::unordered_set<int>& current_solution, int verts, int* sorted_verts, const std::vector<std::unordered_set<int>>& adjacency_list)
{
    int cur_ver;
    bool is_dense;
    int not_neighbour_count;
    int not_a_neighbour;
    std::vector<Pair>* possible_verts = new std::vector<Pair>();
    possible_verts->reserve(verts);

    for (int i = 0; i < verts; i++)
    {
        is_dense = true;
        not_neighbour_count = 0;
        cur_ver = sorted_verts[i];
        if (current_solution.find(cur_ver) != current_solution.end())
            continue;
        for (int vert_in_clique : current_solution)
        {
            if (adjacency_list[vert_in_clique].find(cur_ver) == adjacency_list[vert_in_clique].end())
            {
                not_neighbour_count++;
                not_a_neighbour = vert_in_clique;
            }
        }
        if (not_neighbour_count == 1)
        {
            possible_verts->push_back(Pair(cur_ver, not_a_neighbour));
        }
    }
    return possible_verts;
}

std::unordered_set<int>* fill_clique(int start_vert, int& global_last_vert, std::unordered_set<int>& clique, int verts, int* sorted_verts, const std::vector<std::unordered_set<int>>& adjacency_list)
{
    int cur_ver;
    bool is_dense;
    for (int i = global_last_vert; i < verts; i++)
    {
        cur_ver = sorted_verts[i];
        is_dense = true;
        if (adjacency_list[start_vert].find(cur_ver) != adjacency_list[start_vert].end())
        {
            for (int vert_in_clique : clique)
            {
                is_dense &= adjacency_list[vert_in_clique].find(cur_ver) != adjacency_list[vert_in_clique].end();
                if (!is_dense)
                    break;
            }
            if (is_dense)
            {
                clique.insert(cur_ver);
                start_vert = cur_ver;
                global_last_vert = i + 1;
            }
        }
    }
    return &clique;
}

std::unordered_set<int> local_search(const std::unordered_set<int>& current_solution, int verts, int* sorted_verts, const std::vector<std::unordered_set<int>>& adjacency_list)
{
    int best_value = current_solution.size();
    std::unordered_set<int> best_solution = current_solution;
    std::unordered_set<int>* solution = new std::unordered_set<int>();
    solution->reserve(verts);
    solution->insert(current_solution.begin(), current_solution.end());

    const int max_steps = 100;
    int global_last_vert = 0;
    int iter_count = 0;
    int permute_index;
    //Pair pair;
    std::vector<Pair>* neighbourhoods = get_neighbourhoods(current_solution, verts, sorted_verts, adjacency_list);

    for (Pair neighbourhood: *neighbourhoods)
    {
        //permute_index = std::rand() % neighbourhoods->size();
        //pair = (*neighbourhoods)[permute_index];
        solution->erase(solution->find(neighbourhood.old_vert));
        solution->insert(neighbourhood.new_vert);
        solution = fill_clique(0, global_last_vert, *solution, verts, sorted_verts, adjacency_list);
        global_last_vert = 0;
        if (solution->size() > best_solution.size())
        {
            best_solution = *solution;
        }
        solution->clear();
        solution->insert(current_solution.begin(), current_solution.end());
    }
    //while (!neighbourhoods->empty() && iter_count < max_steps)
    //{
    //    permute_index = std::rand() % neighbourhoods->size();
    //    pair = (*neighbourhoods)[permute_index];
    //    solution->erase(solution->find(pair.old_vert));
    //    solution->insert(pair.new_vert);
    //    solution = fill_clique(0, global_last_vert, *solution, verts, sorted_verts, adjacency_list);
    //    global_last_vert = 0;
    //    if (solution->size() > best_solution.size())
    //    {
    //        best_solution = *solution;
    //    }
    //    iter_count++;
    //}
    return best_solution;
}

std::unordered_set<int> get_greedy_solution(int start_vert, int verts, int* sorted_verts, const std::vector<std::unordered_set<int>>& adjacency_list)
{
    int cur_ver;
    std::unordered_set<int>* clique = new std::unordered_set<int>();
    std::unordered_set<int> best_clique;
    int global_last_vert = 0;
    bool is_dense;
    clique->reserve(verts);

    clique->insert(start_vert);

    while (!clique->empty())
    {
        clique = fill_clique(start_vert, global_last_vert, *clique, verts, sorted_verts, adjacency_list);
        if (clique->size() > best_clique.size())
        {
            best_clique = *clique;
        }
        if (clique->empty())
            break;
        clique->erase(clique->begin());
    }
    return best_clique;
}

void greedy_search_random_start(std::ifstream& input_file)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::unordered_set<int>> adjacency_list;
    int verts = 0, edges = 0;
    int color;
    std::vector<int> pows;

    read_graph(input_file, verts, edges, adjacency_list, pows);

    int* sorted_verts = sort_verts(verts, adjacency_list, pows);

    int cur_ver, start_vert, global_last_vert = 0;
    bool is_dense;
    std::unordered_set<int> clique;
    std::unordered_set<int> best_clique;
    clique.reserve(verts);

    start_vert = sorted_verts[0];

    int starts = 0;
    const int max_iters = 10000;

    while (starts < max_iters)
    {
        clique = get_greedy_solution(start_vert, verts, sorted_verts, adjacency_list);
        clique = local_search(clique, verts, sorted_verts, adjacency_list);

        if (clique.size() > best_clique.size())
        {
            best_clique = clique;
        }
        start_vert = sorted_verts[std::rand() % verts];
        starts++;
    }

    std::cout << "Largest clique: " << best_clique.size() << '\n';

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by function: " << duration.count() / 1000000.0 << " seconds" << std::endl;

}

void greedy_search_random(std::ifstream& input_file)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::srand(0);

    std::vector<std::unordered_set<int>> adjacency_list;
    int verts = 0, edges = 0;
    std::vector<int> pows;

    read_graph(input_file, verts, edges, adjacency_list, pows);

    int* sorted_verts = sort_verts(verts, adjacency_list, pows);

    int cur_ver, last_vert, last_ind = 0;
    bool is_dense;
    std::unordered_set<int> clique;
    std::unordered_set<int> best_clique;

    const int candidates_num = 2;
    int* candidates = new int[candidates_num];
    clique.reserve(verts);

    last_vert = sorted_verts[0];
    clique.insert(last_vert);

    int starts = 0;
    int candidate_iterations = 0;
    const int max_iters = 10000;

    while (starts < max_iters)
    {
        for (int i = 0; i < verts; i++)
        {
            cur_ver = sorted_verts[i];
            is_dense = true;

            for (int verts_in_clique : clique)
            {
                is_dense &= adjacency_list[verts_in_clique].find(cur_ver) != adjacency_list[verts_in_clique].end();
            }
            if (is_dense)
            {
                //clique.insert(cur_ver);
                if (candidate_iterations == 0)
                {
                    last_ind = i;
                }
                candidates[candidate_iterations] = cur_ver;
                candidate_iterations++;
            }
            if (candidate_iterations == candidates_num)
            {
                last_vert = candidates[(std::rand() % candidates_num)];
                clique.insert(last_vert);
                candidate_iterations = 0;
                i = last_ind;
            }

        }
        if (clique.size() > best_clique.size())
        {
            best_clique = clique;
        }

        //std::cout << starts << "\n";
        starts++;
        candidate_iterations = 0;
        clique.clear();
        clique.reserve(verts);
        last_vert = sorted_verts[std::rand() % verts];
        clique.insert(last_vert);
    }

    std::cout << "Largest clique: " << best_clique.size() << '\n';

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by function: " << duration.count() / 1000000.0 << " seconds" << std::endl;
}

int main()
{
    std::string file = "d:/Учёба/Мага/HSE_labs/MaximumClique/MaximumClique/max_clique_txt/DIMACS_all_ascii/brock200_1.clq";
    std::srand(std::time(0));

    std::ifstream input_file(file);
    if (!input_file.is_open())
    {
        std::cout << "File wasn't open\n";
        return 1;
    }

    //greedy_search_random(input_file);
    //input_file = std::ifstream(file);

    greedy_search_random_start(input_file);
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
