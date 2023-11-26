// SegmentIntersection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>

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

    bool operator<(const Point& other) const
    {
        if (this->x == other.x)
            return this->y < other.y;
        return this->x < other.x;
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

    float get_left_y() const
    {
        if (this->first < this->last)
            return this->first.y;
        else
            return this->last.y;
    }

    bool operator<(const Segment& other) const
    {
        return this->get_left_y() < other.get_left_y();
    }

    bool operator>(const Segment& other) const
    {
        return (this->get_left_y() > other.get_left_y());
    }

    bool operator==(const Segment& other) const
    {
        return (this->get_left_y() == other.get_left_y());
    }
};

template<typename T>
struct node
{
    T key;
    int height;
    node* left;
    node* right;

    static int get_height(const node* cur_node)
    {
        return cur_node ? cur_node->height : 0;
    }

    node()
    {
        key = 0;
        left = right = nullptr;
        height = 1;
    }

    node(T k)
    {
        key = k;
        left = right = nullptr;
        height = 1;
    }

    ~node()
    {

    }

    int balance_factor()
    {
        return abs(get_height(left) - get_height(right));
    }

    void update_height()
    {
        this->height = 1 + std::max(get_height(this->left), get_height(this->right));
    }

    void rotate_left()
    {
        node* new_parent = this->right;
        this->right = new_parent->right;
        this->left = new node(*this);

        this->left->right = new_parent->left;
        this->left->update_height();
        this->update_height();
        this->key = new_parent->key;
    }

    void rotate_right()
    {
        node* new_parent = this->left;
        this->left = new_parent->left;
        this->right = new node(*this);

        this->right->left = new_parent->right;
        this->right->update_height();
        this->update_height();
        this->key = new_parent->key;
    }

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

    node* insert(T key)
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
                child = this->right;
            }
            else
            {
                this->left = new node(key);
                child = this->left;
            }
        }
        balance();
        return child;
    }

    void delete_key(T key)
    {
        if (this->key > key)
        {
            this->left->delete_key(key);
            if (this->left->key == key)
                this->left = nullptr;
        }
        else if (this->key < key)
        {
            this->right->delete_key(key);
            if (this->right->key == key)
                this->right = nullptr;
        }
        else
        {
            node* child = nullptr;
            if (this->left)
                child = this->left;
            if (this->right)
                child = this->right;

            if (this->left && this->right)
            {
                node* sub_tree_root = this->right;
                while (sub_tree_root->left)
                {
                    sub_tree_root = sub_tree_root->left;
                }
                this->key = sub_tree_root->key;
                this->right->delete_key(sub_tree_root->key);
            }
            else if (child != nullptr)
            {
                this->key = child->key;
                this->left = child->left;
                this->right = child->right;
            }
        }
        update_height();
    }

    T get_previous()
    {
        node* sub_tree_root = this->left;
        while (sub_tree_root->right)
        {
            sub_tree_root = sub_tree_root->right;
        }
        return sub_tree_root;
    }

    T get_next()
    {
        node* sub_tree_root = this->right;
        while (sub_tree_root->left)
        {
            sub_tree_root = sub_tree_root->left;
        }
        return sub_tree_root;
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

std::vector<std::pair<const Segment*, const Segment*>>* avl_tree_algorithm(const std::vector<Segment>& seqments)
{
    std::vector<std::pair<const Segment*, const Segment*>>* answer = new std::vector<std::pair<const Segment*, const Segment*>>();

    std::vector<std::pair<const Point*, int>> all_points_and_segments;
    all_points_and_segments.reserve(seqments.size() * 2);
    for (const Segment& segment : seqments)
    {
        int cur_segment_ind = all_points_and_segments.size();
        all_points_and_segments.push_back(std::make_pair(&segment.first, cur_segment_ind));
        all_points_and_segments.push_back(std::make_pair(&segment.last, cur_segment_ind));
    }

    auto comporator = [](std::pair< Point*, int>& first, std::pair< Point*, int>& last) {
        return first.first < last.first;
    };
    std::sort(all_points_and_segments.begin(), all_points_and_segments.end(), comporator);

    node<const Segment*>* tree_root = new node<const Segment*>(&seqments[all_points_and_segments[0].second]);
    
    const Point* cur_point;
    const Segment* cur_segment, *previous, *next;
    node<const Segment*>* cur_node;

    for (int ind = 1; ind < all_points_and_segments.size(); ind++)
    {
        cur_point = all_points_and_segments[0].first;
        cur_segment = &seqments[all_points_and_segments[0].second];

        float x_diff = cur_point->x - cur_segment->first.x + cur_point->x - cur_segment->last.x;
        if (x_diff < 0 || (x_diff == 0 && cur_point->y - cur_segment->first.y + cur_point->y - cur_segment->last.y < 0))
        {
            cur_node = tree_root->insert(cur_segment);
            previous = cur_node->get_previous();
            next = cur_node->get_next();

            if (is_intersect(*cur_segment, *previous))
            {
                answer->push_back(std::make_pair(cur_segment, previous));
            }
            if (is_intersect(*cur_segment, *next))
            {
                answer->push_back(std::make_pair(cur_segment, previous));
            }
        }
        else
        {
            previous = cur_node->get_previous();
            next = cur_node->get_next();

            tree_root->delete_key(cur_segment);
            if (is_intersect(*next, *previous))
            {
                answer->push_back(std::make_pair(next, previous));
            }
        }
    }
    return answer;
}

template <typename T>
void print_tree(node<T>* root)
{
    std::vector<std::vector<float>> nodes_on_each_level;
    int tree_height = node::get_height(root);
    nodes_on_each_level.resize(tree_height);

    std::function<void(node*, int)> dfs_step;
    dfs_step = [&tree_height, &nodes_on_each_level, &dfs_step](node<T>* root, int height)
    {
        nodes_on_each_level[height].push_back(root->key);
        height++;
        if (root->left)
            dfs_step(root->left, height);
        if (root->right)
            dfs_step(root->right, height);
    };
    dfs_step(root, 0);

    for (const auto& level : nodes_on_each_level)
    {
        for (const auto& node_key : level)
        {
            std::cout << node_key << ' ';
        }
        std::cout << '\n';
    }
}

int main()
{
    
    /*node<int>* tree_root = new node<int>(7);
    tree_root->insert(6);
    tree_root->insert(5);
    tree_root->insert(4);
    tree_root->insert(3);
    tree_root->insert(2);
    tree_root->insert(1);
    
    print_tree(tree_root);*/

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
