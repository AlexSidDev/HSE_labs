// MaximumClique.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <chrono>

void read_graph(std::ifstream& input_file, int& verts, int& edges, std::vector<std::unordered_set<int>>& adjacency_list, std::vector<int>& pows)
{
    std::string line;
    char e;
    int src_vert, tgt_vert;
    int color;

    while (std::getline(input_file, line))
    {
        if (line[0] == 'p')
        {
            std::istringstream iss(line.substr(7));
            iss >> verts >> edges;
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

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream input_file("max_clique_txt/DIMACS_all_ascii/brock200_2.clq");
    if (!input_file.is_open())
    {
        std::cout << "File wasn't open\n";
        return 1;
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
            if (pows[j] < min_pow && pows[j]!= -1)
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

    int cur_ver, last_vert = sorted_verts[0];
    bool is_dense;
    std::unordered_set<int> clique;
    clique.insert(last_vert);

    for (int i = 1; i < verts; i++)
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
    std::cout << "Largest clique: " << clique.size() << '\n';

    return 0;
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
