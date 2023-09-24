// Coloring.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <chrono>

int* sort_verts(int verts, const std::vector<std::unordered_set<int>>& adjacency_list, std::vector<int>& pows)
{
    int* sorted_verts = new int[verts];
    int min_pow;
    int min_ind;
    bool is_need_random_change;
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
        if (tgt_vert < src_vert)
            continue;
        adjacency_list[src_vert - 1].insert(tgt_vert - 1);
        adjacency_list[tgt_vert - 1].insert(src_vert - 1);
        pows[src_vert - 1]++;
        pows[tgt_vert - 1]++;
    }
}

int smaller_degree_last_with_remove(std::ifstream& input_file)
{
    
    int verts, edges;
    int src_vert, tgt_vert;
    int need_paint, color;
    std::vector<std::unordered_set<int>> adjacency_list;
    std::vector<int> pows;

    read_graph(input_file, verts, edges, adjacency_list, pows);

    int* sorted_verts = sort_verts(verts, adjacency_list, pows);

    std::vector<int> colors_of_verts(verts, -1);
    std::vector<bool> available_colors;
    available_colors.reserve(verts);

    std::vector<std::unordered_set<int>> color_classes;
    color_classes.resize(verts);
    int num_colors = 0;
    bool is_find;
    int cur_ver;
    for (int cur_ind = 0; cur_ind < verts; cur_ind++)
    {
        cur_ver = sorted_verts[cur_ind];
        
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
    //std::cout << "Num colors: " << num_colors << "\n";

    for (std::unordered_set<int> color_class : color_classes)
    {
        if (color_class.empty())
            continue;
        std::cout << '{';
        for (int vert : color_class)
        {
            std::cout << vert + 1 << ',';
        }
        std::cout << "}\t";
    }
    std::cout << "\n";

    return num_colors;
}

int main()
{
    std::vector<std::string> files = { "graphs/myciel3.col.txt", "graphs/myciel7.col.txt",
        "graphs/school1.col.txt", "graphs/school1_nsh.col.txt",
        "graphs/anna.col.txt",
        "graphs/miles1000.col.txt", "graphs/miles1500.col.txt",
        "graphs/le450_5a.col.txt", "graphs/le450_15b.col.txt", 
        "graphs/queen11_11.col.txt" };

    std::ofstream fout("color.csv");

    int num_colors;
    for (std::string file : files)
    {
        std::ifstream input_file(file);
        if (!input_file.is_open())
        {
            std::cout << "File wasn't open\n";
            continue;
        }
        auto start = std::chrono::high_resolution_clock::now();
        num_colors = smaller_degree_last_with_remove(input_file);
        auto stop = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        //std::cout << "Time taken by function: " << duration.count() / 1000000.0 << " seconds" << std::endl;
        fout << file << "; " << num_colors << "; " << duration.count() / 1000000.0 << '\n';
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
