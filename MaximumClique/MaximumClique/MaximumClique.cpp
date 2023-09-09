﻿// MaximumClique.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
            
            //std::istringstream iss(line.substr(7));
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

void greedy_search_random_start(std::string file)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::srand(std::time(0));

    std::ifstream input_file(file);
    if (!input_file.is_open())
    {
        std::cout << "File wasn't open\n";
        return;
    }

    std::vector<std::unordered_set<int>> adjacency_list;
    int verts = 0, edges = 0;
    int color;
    std::vector<int> pows;

    read_graph(input_file, verts, edges, adjacency_list, pows);

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

    int cur_ver, last_vert;
    bool is_dense;
    std::unordered_set<int> clique;
    std::unordered_set<int> best_clique;
    clique.reserve(verts);

    last_vert = sorted_verts[0];
    clique.insert(last_vert);

    int j = 0;
    const int max_iters = 10000;

    while (j < max_iters)
    {
        for (int i = 0; i < verts; i++)
        {
            cur_ver = sorted_verts[i];
            is_dense = true;
            if (adjacency_list[last_vert].find(cur_ver) != adjacency_list[last_vert].end())
            {
                for (int verts_in_clique : clique)
                {
                    is_dense &= adjacency_list[verts_in_clique].find(cur_ver) != adjacency_list[verts_in_clique].end();
                }
                if (is_dense)
                {
                    clique.insert(cur_ver);
                    last_vert = cur_ver;
                }
            }
        }
        if (clique.size() > best_clique.size())
        {
            best_clique = clique;
        }

        clique.clear();
        clique.reserve(verts);
        last_vert = sorted_verts[std::rand() % verts];
        clique.insert(last_vert);
        j++;
    }

    std::cout << "Largest clique: " << best_clique.size() << '\n';

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by function: " << duration.count() / 1000000.0 << " seconds" << std::endl;

}

void greedy_search_random(std::string file)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::srand(0);

    std::ifstream input_file(file);
    if (!input_file.is_open())
    {
        std::cout << "File wasn't open\n";
        return;
    }

    std::vector<std::unordered_set<int>> adjacency_list;
    int verts = 0, edges = 0;
    std::vector<int> pows;

    read_graph(input_file, verts, edges, adjacency_list, pows);

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

        clique.clear();
        clique.reserve(verts);

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
    std::string file = "max_clique_txt/DIMACS_all_ascii/C125.9.clq";
    greedy_search_random(file);
    greedy_search_random_start(file);
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
