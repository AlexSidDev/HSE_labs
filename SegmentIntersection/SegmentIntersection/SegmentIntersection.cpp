// SegmentIntersection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>

struct Point
{
    float x, y;

    Point(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Point()
    {
        this->x = 0.0;
        this->y = 0.0;
    }
};

struct Segment
{
    Point first, last;

    Segment(const Point first, const Point last)
    {
        this->first = first;
        this->last = last;
    }

    Segment()
    {
        this->first = Point();
        this->last = Point();
    }
};

std::pair<float, float> get_equation(const Segment& seg)
{
    float k = (seg.first.y - seg.last.y) / (seg.first.x - seg.last.x);
    float b = seg.first.y - k * seg.first.x;
    return std::make_pair(k, b);
}

bool is_intersect(const Segment& first, const Segment& second)
{
    std::pair<float, float> first_eq = get_equation(first);
    std::pair<float, float> second_eq = get_equation(second);

    float common_x = (second_eq.second - first_eq.second) / (first_eq.first - second_eq.first);
    bool is_on_both = ((common_x - first.first.x) * (common_x - first.last.x)) <= 0;
    is_on_both &= ((common_x - second.first.x) * (common_x - second.last.x)) <= 0;

    return is_on_both;
}

std::pair<int, int> naive_algorithm(const std::vector<Segment>& seqments)
{
    size_t seqments_count = seqments.size();
    Segment current_segment, candidate;
    bool is_intersection = false;
    int first_ind = -1, second_ind = -1;
    for (int i = 0; i < seqments_count; i++)
    {
        current_segment = seqments[i];
        for (int j = 0; j < seqments_count; j++)
        {
            if (i == j)
                continue;
            candidate = seqments[j];
            is_intersection = is_intersect(current_segment, candidate);
            if (is_intersection)
            {
                second_ind = j;
                break;
            }
        }
        if (is_intersection)
        {
            first_ind = i;
            break;
        }
    }
    return std::make_pair(first_ind, second_ind);
}

int main()
{
    Segment first(Point(1, 5), Point(4, 3));
    Segment second(Point(1, 7), Point(3, 5));

    std::vector<Segment> segments = { first , second };

    std::pair<int, int> answer = naive_algorithm(segments);
    std::cout << answer.first << "\t" << answer.second << '\n';
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
