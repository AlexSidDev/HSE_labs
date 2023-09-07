// Coloring.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <chrono>


void greedy_search()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream input_file("myciel3.col.txt");
    if (!input_file.is_open())
    {
        std::cout << "File wasn't open\n";
        return;
    }

    std::string line;
    char e;
    int verts, edges;
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
    std::vector<std::unordered_set<int>> adjacency_list;
    adjacency_list.resize(verts);

    while (std::getline(input_file, line))
    {
        std::istringstream iss(line);
        iss >> e >> src_vert >> tgt_vert;
        adjacency_list[src_vert - 1].insert(tgt_vert - 1);
        adjacency_list[tgt_vert - 1].insert(src_vert - 1);
    }


    std::vector<std::unordered_set<int>> color_classes;
    color_classes.resize(verts);
    int num_colors = 1;
    bool is_find;
    for (int cur_ver = 0; cur_ver < verts; cur_ver++)
    {
        color = 0;
        for (std::unordered_set<int> color_class : color_classes)
        {
            is_find = false;
            if (color_class.empty())
                continue;
            for (int neighbour : adjacency_list[cur_ver])
            {
                if (color_class.find(neighbour) != color_class.end())
                {
                    is_find = true;
                    break;
                }
            }

            if (is_find)
                color++;
            else
                break;

        }
        color_classes[color].insert(cur_ver);
        if (color + 1 > num_colors)
            num_colors = color + 1;

    }

    std::cout << "Num colors: " << num_colors << "\n";
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by function: " << duration.count() / 1000000.0 << " seconds" << std::endl;
}


void smaller_degree_last_with_remove()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream input_file("graphs/myciel3.col.txt");
    if (!input_file.is_open())
    {
        std::cout << "File wasn't open\n";
        return;
    }

    std::string line;
    char e;
    int verts, edges;
    int src_vert, tgt_vert;
    int need_paint, color;

    while (std::getline(input_file, line))
    {
        if (line[0] == 'p')
        {
            std::istringstream iss(line.substr(7));
            iss >> verts >> edges;
            break;
        }
    }

    std::vector<std::unordered_set<int>> adjacency_list;
    adjacency_list.resize(verts);

    int* pows = new int[verts];
    for (int i = 0; i < verts; i++)
    {
        pows[i] = 0;
    }

    while (std::getline(input_file, line))
    {
        std::istringstream iss(line);
        iss >> e >> src_vert >> tgt_vert;
        adjacency_list[src_vert - 1].insert(tgt_vert - 1);
        adjacency_list[tgt_vert - 1].insert(src_vert - 1);
        pows[src_vert - 1]++;
        pows[tgt_vert - 1]++;
    }

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

    std::vector<std::unordered_set<int>> color_classes;
    color_classes.resize(verts);
    int num_colors = 1;
    bool is_find;
    int cur_ver;
    for (int cur_ind = 0; cur_ind < verts; cur_ind++)
    {
        cur_ver = sorted_verts[cur_ind];
        color = 0;
        for (std::unordered_set<int> color_class : color_classes)
        {
            is_find = false;
            if (color_class.empty())
                continue;
            for (int neighbour : adjacency_list[cur_ver])
            {
                if (color_class.find(neighbour) != color_class.end())
                {
                    is_find = true;
                    break;
                }
            }

            if (is_find)
                color++;
            else
                break;

        }
        color_classes[color].insert(cur_ver);
        if (color + 1 > num_colors)
            num_colors = color + 1;

    }

    std::cout << "Num colors: " << num_colors << "\n";
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

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
    std::cout << "Time taken by function: " << duration.count() / 1000000.0 << " seconds" << std::endl;
}

int main()
{
    smaller_degree_last_with_remove();
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
