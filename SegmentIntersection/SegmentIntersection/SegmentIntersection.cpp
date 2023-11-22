// SegmentIntersection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>

struct node
{
    float key;
    int height;
    node* left;
    node* right;

    int get_height(const node* cur_node)
    {
        return cur_node ? cur_node->height : 0;
    }

    node(float k)
    { 
        key = k;
        left = right = nullptr;
        height = 1;
    }

    int balance_factor()
    {
        return abs(get_height(left) - get_height(right));
    }

    void update_height(node* cur_node)
    {
        cur_node->height = 1 + std::max(get_height(cur_node->left), get_height(cur_node->right));
    }

    /*node* rotate_left()
    {
        node* new_parent = this->right;
        this->right = new_parent->left;
        update_height(this);
        new_parent->left = this;
        update_height(new_parent);
        return new_parent;
    }*/

    void rotate_left()
    {
        node* new_parent = this->right;
        this->right = new_parent->right;
        this->left = new node(*this);
        this->left->right = new_parent->left;
        update_height(this->left);
        update_height(this);
        this->key = new_parent->key;
    }

   void rotate_right()
    {
        node* new_parent = this->left;
        this->left = new_parent->left;
        this->right = new node(*this);

        this->right->left = new_parent->right;
        update_height(this->right);
        update_height(this);
        this->key = new_parent->key;
    }

    /*node* rotate_right()
    {
        node* new_parent = this->left;
        this->left = new_parent->right;
        update_height(this);
        new_parent->right = this;
        update_height(new_parent);
        return new_parent;
    }*/

    void balance()
    {
        if (this->balance_factor() < 2)
            return;
        node* new_node = this;
        if (get_height(left) < get_height(right))
        {
            if (get_height(right->right) < get_height(right->left))
                right->rotate_right();

            rotate_left();
        }
        else if (get_height(left) > get_height(right))
        {
            if (get_height(left->right) > get_height(left->left))
                left->rotate_left();

            rotate_right();
        }
    }

    void insert(float key)
    {
        node* child;
        if (key > this->key)
            child = this->right;
        else
            child = this->left;

        if (child)
        {
            int old_height = child->height;
            child->insert(key);
            if (child->height > old_height)
            {
                this->height += 1;
            }
        }
        else
        {
            this->height += 1;
            if (key > this->key)
            {
                this->right = new node(key);
            }
            else
            {
                this->left = new node(key);
            }
        }
        balance();
    }
};

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
    
    node* tree_root = new node(7);
    tree_root->insert(6);
    tree_root->insert(5);
    tree_root->insert(4);
    tree_root->insert(3);
    tree_root->insert(2);
    tree_root->insert(1);
    std::cout << tree_root->height << '\t' << tree_root->key << '\n';
    
    //std::pair<int, int> answer = naive_algorithm(segments);
    //std::cout << answer.first << "\t" << answer.second << '\n';
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
