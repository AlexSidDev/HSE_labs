// Bnb.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// MaximumClique.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef IL_STD
#define IL_STD
#endif

#include <ilcplex/ilocplex.h>

#define EPS 0.00001
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <list>
#include <chrono>
#include <memory>
#include <algorithm>


std::vector<int>* collection_difference(const std::vector<int>& first, const std::unordered_set<int>& second)
{
    std::vector<int>* diff = new std::vector<int>;
    diff->reserve(first.size());
    for (const int& elem : first)
    {
        if (second.find(elem) == second.end())
        {
            diff->push_back(elem);
        }
    }
    return diff;
}


class MaxCliqueBnBSolver;


class Graph
{
    size_t verts;
    std::vector<int> pows;
    std::vector<std::unordered_set<int>> adjacency_list;

    int* sort_verts()
    {
        int* sorted_verts = new int[verts];
        int min_pow;
        int min_ind;

        std::vector<int> tmp_pows = pows;
        for (int i = 0; i < verts; i++)
        {
            min_ind = 0;
            min_pow = INT_MAX;
            for (int j = 0; j < verts; j++)
            {   
                if (tmp_pows[j] < min_pow)
                {
                    min_ind = j;
                    min_pow = tmp_pows[j];
                }
            }
            for (int neighbour : adjacency_list[min_ind])
            {
                tmp_pows[neighbour]--;
            }
            tmp_pows[min_ind] = INT_MAX;
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

    ~Graph()
    {
        pows.clear();
        adjacency_list.clear();
    }

    int coloring(const int* vertices_order, std::vector<std::unordered_set<int>>& color_classes, int num_verts)
    {

        std::vector<int> colors_of_verts(verts, -1);
        std::vector<bool> available_colors;
        available_colors.reserve(num_verts);

        color_classes.resize(num_verts);
        int num_colors = 0;
        bool is_find;
        int cur_ver;
        for (int cur_ind = 0; cur_ind < num_verts; cur_ind++)
        {
            cur_ver = vertices_order[cur_ind];

            is_find = false;
            for (int neighbour : adjacency_list[cur_ver])
            {
                if (colors_of_verts[neighbour] != -1)
                {
                    available_colors[colors_of_verts[neighbour]] = false;
                }
            }
            for (int color = 0; color < available_colors.size(); color++)
            {
                if (available_colors[color] && !is_find)
                {
                    color_classes[color].insert(cur_ver);
                    is_find = true;
                    colors_of_verts[cur_ver] = color;
                }
                available_colors[color] = true;
            }
            if (!is_find)
            {
                available_colors.push_back(true);
                num_colors++;
                color_classes[num_colors - 1].insert(cur_ver);
                colors_of_verts[cur_ver] = num_colors - 1;
            }
        }
        
        return num_colors;
    }

    int all_independent_sets(int* vertices_order, std::vector<std::unordered_set<int>>& independent_sets)
    {
        independent_sets.reserve((verts * verts) / 2);

        std::vector<int> all_verts{ vertices_order, vertices_order + verts };

        std::vector<std::vector<int>>* non_neighbourhood_sets = new std::vector<std::vector<int>>();
        non_neighbourhood_sets->resize(verts);

        for (int cur_ind = 0; cur_ind < verts; cur_ind++)
        {
            std::vector<int>* tmp_vector;
            tmp_vector = collection_difference(all_verts, adjacency_list[cur_ind]);
            non_neighbourhood_sets->at(cur_ind) = *tmp_vector;
        }
        
        for (int cur_ind = 0; cur_ind < verts; cur_ind++)
        {
            std::vector<std::unordered_set<int>> divided_sets;

            coloring(non_neighbourhood_sets->at(cur_ind).begin()._Ptr, divided_sets, non_neighbourhood_sets->at(cur_ind).size());

            for (auto& set : divided_sets)
            {
                if (set.empty())
                    continue;

                std::unordered_set<int> tmp_set;
                tmp_set.reserve(set.size() + 1);
                tmp_set.insert(cur_ind);
                tmp_set.insert(set.begin(), set.end());

                independent_sets.push_back(tmp_set);
            }
        }

        return independent_sets.size();
    }

    bool check_clique_candidate(const std::unordered_set<int>& clique, int vertex_to_insert)
    {
        bool is_dense = true;

        for (int vert_in_clique : clique)
        {
            is_dense &= (adjacency_list[vert_in_clique].find(vertex_to_insert) != adjacency_list[vert_in_clique].end()) || (vert_in_clique == vertex_to_insert);
            if (!is_dense)
                break;
        }
        return is_dense;
    }

    bool check_clique(const std::unordered_set<int>& clique)
    {
        bool is_clique = true;
        for (int vert : clique)
        {
            is_clique &= check_clique_candidate(clique, vert);
        }
        return is_clique;
    }

    int find_max_clique(int max_iters, int random_candidates)
    {
        int* sorted_verts = sort_verts();
        int cur_ver, start_vert, last_ind = 0, global_last_index = -1;
        bool is_dense;
        std::unordered_set<int> clique;
        std::unordered_set<int> best_clique;
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
                if (adjacency_list[cur_ver].size() + 1 <= best_clique.size())
                    continue;
                is_dense = check_clique_candidate(clique, cur_ver);
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
                    clique.insert(start_vert);
                    candidate_iterations = 0;
                    global_last_index = i;
                    i = last_ind;
                }
            }
            if (clique.size() > best_clique.size())
            {
                best_clique = clique;
            }

            starts++;
            candidate_iterations = 0;
            clique.clear();
            do
            {
                rand_ind = std::rand() % (verts / 2);
                start_vert = sorted_verts[rand_ind];
                global_last_index = -1;
            } while (adjacency_list[start_vert].size() + 1 <= best_clique.size());

            clique.insert(start_vert);
        }
        return best_clique.size();
    }

    friend class MaxCliqueBnBSolver;
};


class MaxCliqueBnBSolver
{
    Graph* graph;
    std::vector<std::unordered_set<int>>* independent_sets = new std::vector<std::unordered_set<int>>();
    IloEnv env;
    IloModel model;
    IloFloatVarArray x;
    float upper_bound = -1;
    int* sorted_verts;
    std::unordered_set<int> best_clique;

    bool process_solution(IloCplex& cplex, int& best_bounding_candidate, std::unordered_set<int>& clique)
    {
        int cur_ver;
        bool is_integer = true, is_zero, is_one, is_best_found = false;
        for (int i = 0; i < graph->verts; i++)
        {
            cur_ver = sorted_verts[i];
            is_zero = abs(cplex.getValue(x[cur_ver]) - 0.0) < EPS;
            is_one = abs(cplex.getValue(x[cur_ver]) - 1.0) < EPS;

            if (!is_zero && !is_one && !is_best_found)
            {
                is_best_found = true;
                best_bounding_candidate = cur_ver;
            }

            if (is_one)
                clique.insert(i);

            is_integer &= (is_zero || is_one);
        }
        return is_integer;
    }

public:
    MaxCliqueBnBSolver(Graph* target_graph)
    {
        graph = target_graph;
        model = IloModel(env);
        x = IloFloatVarArray(env, graph->verts);

        sorted_verts = graph->sort_verts();
        int num_sets = graph->all_independent_sets(sorted_verts, *independent_sets);

        std::cout << "Total number of constraints: " << num_sets << '\n';
        create_problem();
    }

    ~MaxCliqueBnBSolver()
    {
        //delete graph;
        independent_sets->clear();
        best_clique.clear();
        delete [] sorted_verts;
    }

    void create_problem()
    {
        for (auto i = 0; i < graph->verts; ++i)
            x[i] = IloFloatVar(env, 0., 1.);

        IloRangeArray i_sets(env);

        IloExpr expr(env);

        for (const auto& set : *independent_sets) {
            for (auto& i : set)
                expr += x[i];
            i_sets.add(IloRange(env, 0., expr, 1.));
            expr.clear();
        }

        for (auto i = 0; i < graph->verts; ++i)
            expr += x[i];

        model.add(i_sets);

        IloObjective obj(env, expr, IloObjective::Maximize);

        model.add(obj);

        expr.end();
    }

    void BnB_step()
    {
        IloCplex cplex(model);
        cplex.setOut(env.getNullStream());

        double ans = -1;
        if (cplex.solve()) {
            ans = cplex.getObjValue();
        }

        if (std::floor(ans + EPS) <= upper_bound)
        {
            cplex.end();
            return;
        }

        int best_bounding_candidate;
        std::unordered_set<int> clique;
        clique.reserve(std::floor(ans + EPS));
        bool is_integer = process_solution(cplex, best_bounding_candidate, clique);
        cplex.end();
        if (is_integer)
        {
            upper_bound = ans;
            std::cout << "New answer: " << ans << '\n';
            std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << '\n';
            best_clique = clique;
            clique.clear();
        }
        else
        {
            std::cout << "Non-only-integer answer: " << ans << '\n';
            clique.clear();
            float variants[] = { 1.0, 0.0 };
            IloRangeArray i_sets(env);
            IloExpr expr(env);

            expr += x[best_bounding_candidate];
            for (int var = 0; var < 2; var++)
            {
                if (graph->pows[best_bounding_candidate] <= upper_bound && var == 0)
                    continue;
                
                i_sets.add(IloRange(env, variants[var], expr, variants[var]));

                model.add(i_sets);
                BnB_step();
                model.remove(i_sets);
                i_sets.clear();
            }
        }
    }

    double solve()
    {
        BnB_step();
        
        bool is_valid = graph->check_clique(best_clique);
        if (!is_valid)
            std::runtime_error("Found clique is invalid!!!\n");

        std::cout << "Clique: \n";
        for (int i : best_clique)
        {
            std::cout << i << ' ';
        }
        std::cout << '\n';

        return upper_bound;
    }
};


int main()
{
    std::srand(std::time(0));

    /*std::vector<std::string> files = {
       "brock200_2.clq", "brock200_1.clq", "brock200_3.clq", "brock200_4.clq",
       "C125.9.clq", "gen200_p0.9_44.clq",
       "keller4.clq",
       "MANN_a27.clq", "MANN_a45.clq",
       "p_hat300-1.clq", "p_hat300-2.clq", "p_hat300-3.clq",
       "san200_0.7_2.clq", "san200_0.9_3.clq", "sanr200_0.7.clq",
    };*/

    std::vector<std::string> files = {
       //"keller4.clq",
       //"MANN_a27.clq",
       //"p_hat300-1.clq",
       //"san200_0.7_2.clq",
    };

    files = { "keller4.clq" };

    std::ofstream fout("clique_2.csv", std::ios_base::app);
    fout << "File; Clique; Time (sec)\n";
    fout.close();

    int clique_size;

    for (std::string file : files)
    {
        std::ofstream fout("clique_2.csv", std::ios_base::app);

        std::ifstream input_file("max_clique_txt/DIMACS_all_ascii/" + file);
        if (!input_file.is_open())
        {
            std::cout << "File wasn't open\n";
            continue;
        }
        auto start = std::chrono::high_resolution_clock::now();
        Graph graph(input_file);
        MaxCliqueBnBSolver solver(&graph);
        clique_size = solver.solve();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        std::cout << "Largest clique: " << clique_size << '\n';

        fout << file << "; " << clique_size << "; " << duration.count() / 1000000.0 << '\n';

        fout.close();
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
