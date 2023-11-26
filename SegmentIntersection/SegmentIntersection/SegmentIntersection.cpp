// SegmentIntersection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>

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

    bool operator!=(const Segment& other) const
    {
        return (this->get_left_y() != other.get_left_y());
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
        if (this->left == nullptr)
            return nullptr;
        node* sub_tree_root = this->left;
        while (sub_tree_root->right)
        {
            sub_tree_root = sub_tree_root->right;
        }
        return sub_tree_root->key;
    }

    T get_next()
    {
        if (this->right == nullptr)
            return nullptr;
        node* sub_tree_root = this->right;
        while (sub_tree_root->left)
        {
            sub_tree_root = sub_tree_root->left;
        }
        return sub_tree_root->key;
    }

    std::pair<T, T> get_previous_and_next(T key)
    {
        if (key->first.x == 8 && key->first.y == 4)
        {
            std::cout << "";
        }
        node* cur_node = this;
        node* parent = nullptr;
        while (*cur_node->key != *key)
        {
            parent = cur_node;
            if (*cur_node->key > *key)
            {
                cur_node = cur_node->left;
            }
            else
            {
                cur_node = cur_node->right;
            }
        }

        T previous, next;

        if (!cur_node->right)
        {
            next = parent && parent->key > key ? parent->key : nullptr;
        }
        else
        {
            next = cur_node->get_next();
        }
        if (!cur_node->left)
        {
            previous = parent && parent->key < key ? parent->key : nullptr;
        }
        else
        {
            previous = cur_node->get_previous();
        }
        return std::make_pair(previous, next);
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

std::vector<std::pair<const Segment*, const Segment*>>* naive_algorithm(const std::vector<Segment>& seqments)
{
    std::vector<std::pair<const Segment*, const Segment*>>* answer = new std::vector<std::pair<const Segment*, const Segment*>>();

    size_t seqments_count = seqments.size();
    const Segment *current_segment, *candidate;
    bool is_intersection = false;
    int first_ind = -1, second_ind = -1;
    for (int i = 0; i < seqments_count; i++)
    {
        current_segment = &seqments[i];
        for (int j = i + 1; j < seqments_count; j++)
        {
            candidate = &seqments[j];
            is_intersection = is_intersect(*current_segment, *candidate);
            if (is_intersection)
            {
                answer->push_back(std::make_pair(current_segment, candidate));
            }
        }
    }
    return answer;
}

std::vector<std::pair<const Segment*, const Segment*>>* avl_tree_algorithm(const std::vector<Segment>& seqments)
{
    std::vector<std::pair<const Segment*, const Segment*>>* answer = new std::vector<std::pair<const Segment*, const Segment*>>();

    std::vector<std::pair<const Point*, int>> all_points_and_segments;
    all_points_and_segments.reserve(seqments.size() * 2);

    int cur_segment_ind = 0;
    for (const Segment& segment : seqments)
    {
        all_points_and_segments.push_back(std::make_pair(&segment.first, cur_segment_ind));
        all_points_and_segments.push_back(std::make_pair(&segment.last, cur_segment_ind));
        cur_segment_ind++;
    }

    auto comporator = [](std::pair<const Point*, int>& first, std::pair<const Point*, int>& last) {
        return *first.first < *last.first;
    };
    std::sort(all_points_and_segments.begin(), all_points_and_segments.end(), comporator);

    node<const Segment*>* tree_root = new node<const Segment*>(&seqments[all_points_and_segments[0].second]);
    
    const Point* cur_point;
    const Segment* cur_segment, *previous, *next;
    node<const Segment*>* cur_node;

    for (int ind = 1; ind < all_points_and_segments.size(); ind++)
    {
        cur_point = all_points_and_segments[ind].first;
        cur_segment = &seqments[all_points_and_segments[ind].second];

        if (*cur_point < cur_segment->first || *cur_point < cur_segment->last)
        {
            cur_node = tree_root->insert(cur_segment);
            auto previous_and_next = tree_root->get_previous_and_next(cur_segment);
            previous = previous_and_next.first;
            next = previous_and_next.second;

            if (previous && is_intersect(*cur_segment, *previous))
            {
                answer->push_back(std::make_pair(cur_segment, previous));
            }
            if (next && is_intersect(*cur_segment, *next))
            {
                answer->push_back(std::make_pair(cur_segment, next));
            }
        }
        else
        {
            auto previous_and_next = tree_root->get_previous_and_next(cur_segment);
            previous = previous_and_next.first;
            next = previous_and_next.second;

            tree_root->delete_key(cur_segment);
            if (next && previous && is_intersect(*next, *previous))
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
    std::vector<Segment> segments = { Segment(Point(13, 36), Point(17.394649504991484, 33.61524514293857)),
                                 Segment(Point(18, 6), Point(22.520089023096453, 3.8625259713196627)),
                                 Segment(Point(7, 36), Point(10.736326173440219, 39.322629489676146)),
                                 Segment(Point(8, 4), Point(10.501857369159433, 8.329054135071845)),
                                 Segment(Point(27, 32), Point(30.38631456184457, 35.67870543645987)),
                                 Segment(Point(0, 7), Point(2.2115716605621443, 11.484300479472623)),
                                 Segment(Point(22, 19), Point(24.696476742602986, 23.210583472228187)),
                                 Segment(Point(31, 7), Point(34.238341209210915, 10.809612344153456)),
                                 Segment(Point(24, 13), Point(26.7844428272499, 17.152936086887998)),
                                 Segment(Point(32, 12), Point(36.935090725247136, 12.803043917590873)),
                                 Segment(Point(33, 26), Point(37.687382867359766, 27.740241895479517)),
                                 Segment(Point(10, 28), Point(13.100168543638304, 24.077124141521228)),
                                 Segment(Point(23, 33), Point(23.017596210485255, 37.99996903724178)),
                                 Segment(Point(27, 25), Point(28.68005100865831, 20.29070826893192)),
                                 Segment(Point(31, 47), Point(32.23815687896145, 51.8442716215217)),
                                 Segment(Point(1, 26), Point(4.606421672897861, 22.536804551104197)),
                                 Segment(Point(25, 50), Point(26.35129350596175, 45.18606129445486)),
                                 Segment(Point(4, 49), Point(7.027311973651054, 52.979369574969)),
                                 Segment(Point(49, 45), Point(51.05569967157923, 49.55786121555594)),
                                 Segment(Point(42, 29), Point(46.16487642598714, 31.76655098566005)),
                                 Segment(Point(43, 27), Point(45.5516322644572, 33.29990381136381)),
                                 Segment(Point(65, 6), Point(68.72373832727358, 9.336730865681822)),
                                 Segment(Point(37, 22), Point(37.33499451938434, 26.988765245226766)),
                                 Segment(Point(34, 10), Point(36.535817984873205, 5.690750976377252)),
                                 Segment(Point(38, 25), Point(41.17986237588928, 28.858558703765407)),
                                 Segment(Point(52, 13), Point(54.6404639783393, 8.754066653950794)),
                                 Segment(Point(54, 17), Point(54.34277085493821, 21.988236977230024)),
                                 Segment(Point(62, 32), Point(64.78825464608988, 27.84962218243228)),
                                 Segment(Point(43, 19), Point(46.26533499118263, 22.786500679434557)),
                                 Segment(Point(55, 39), Point(58.699224525396296, 42.36388732134513)),
                                 Segment(Point(58, 11), Point(60.418120011306094, 15.376379280972003)),
                                 Segment(Point(54, 45), Point(55.111592809064966, 40.12512959897034)),
                                 Segment(Point(34, 39), Point(36.01299217777435, 43.57688349122187)),
                                 Segment(Point(48, 50), Point(49.34545732996988, 45.18442686970384)),
                                 Segment(Point(43, 37), Point(45.21788278152917, 32.51881757039546)),
                                 Segment(Point(49, 29), Point(51.63296161452442, 24.749410260159074)),
                                 Segment(Point(74, 45), Point(76.20340689126736, 49.48831795570629)),
                                 Segment(Point(91, 15), Point(91.57918476753525, 10.033658790915073)),
                                 Segment(Point(85, 4), Point(85.5319271693982, 8.971624833639)),
                                 Segment(Point(86, 31), Point(90.64303158596061, 29.14466776781837)),
                                 Segment(Point(68, 3), Point(72.04738897820985, 0.06424754806107558)),
                                 Segment(Point(79, 43), Point(79.1735671793841, 47.99698653532713)),
                                 Segment(Point(87, 42), Point(91.18934309015327, 39.27064028149732)),
                                 Segment(Point(77, 6), Point(78.11032576857902, 1.124840855148479)),
                                 Segment(Point(97, 28), Point(98.88947136516171, 23.370756221559084)),
                                 Segment(Point(80, 15), Point(84.10074998749947, 17.860742830109533)),
                                 Segment(Point(90, 13), Point(92.25848453920092, 17.460857270322645)),
                                 Segment(Point(87, 31), Point(89.9680929374302, 26.976267365396033)),
                                 Segment(Point(71, 2), Point(71.07705914964686, -2.9994061534802015)),
                                 Segment(Point(96, 20), Point(100.44414903905792, 22.291187316358176)),
                                 Segment(Point(78, 25), Point(78.80033961236589, 20.064469987440255)),
                                 Segment(Point(67, 0), Point(68.93635888938525, -4.609828006715515)),
                                 Segment(Point(90, 16), Point(90.64989429263467, 20.957583827672597)),
                                 Segment(Point(85, 0), Point(85.17734820160942, -4.996853771663316)),
                                 Segment(Point(49, 8), Point(49.21407391627806, 12.995415133737072)),
                                 Segment(Point(23, 27), Point(23.45518853478547, 22.02076276947966)),
                                 Segment(Point(47, 40), Point(47.65155866535806, 44.95736535930093)),
                                 Segment(Point(28, 42), Point(30.237571790430767, 46.471383732433225)),
                                 Segment(Point(33, 16), Point(36.20801666511626, 12.16481694356888)),
                                 Segment(Point(31, 3), Point(33.93962508198492, -1.0445771568070183)),
                                 Segment(Point(38, 27), Point(41.02676374830516, 23.020213421306902)),
                                 Segment(Point(38, 35), Point(40.06325011469775, 30.445551738772334)),
                                 Segment(Point(43, 42), Point(43.35042180543938, 46.98770533996071)),
                                 Segment(Point(20, 16), Point(20.887892357126017, 11.079466780707886)),
                                 Segment(Point(25, 3), Point(29.593236975496506, 4.975392134977689)),
                                 Segment(Point(18, 3), Point(21.1856140528328, 6.853811503744595)),
                                 Segment(Point(48, 23), Point(48.49641061517177, 27.975296624438062)),
                                 Segment(Point(18, 8), Point(20.657325269028025, 12.235401092527734)),
                                 Segment(Point(24, 25), Point(27.817010920779683, 28.229617257609426)),
                                 Segment(Point(17, 47), Point(19.622547793015446, 42.742977205446294)),
                                 Segment(Point(32, 20), Point(32.428926397183524, 15.018431758391825)),
                                 Segment(Point(40, 3), Point(43.6630308725315, 6.403263849142631)),
                                 Segment(Point(57, 16), Point(60.97286111042586, 12.964151749960845)),
                                 Segment(Point(68, 6), Point(70.12415180509487, 1.4736351109403243)),
                                 Segment(Point(81, 35), Point(82.02705662437182, 39.893378657975894)),
                                 Segment(Point(81, 50), Point(82.95546109066517, 54.60175748197301)),
                                 Segment(Point(55, 8), Point(58.262874663423055, 11.788620980091817)),
                                 Segment(Point(82, 35), Point(85.59690747573994, 31.526924041866327)),
                                 Segment(Point(67, 47), Point(67.78515947610043, 51.93796765857065)),
                                 Segment(Point(73, 33), Point(74.62110191201344, 37.72990788397262)),
                                 Segment(Point(71, 8), Point(73.8745283210599, 3.9089015006450207)),
                                 Segment(Point(68, 41), Point(70.66840891218372, 45.22842687974836)),
                                 Segment(Point(68, 20), Point(69.7630807039391, 24.678840287015323)),
                                 Segment(Point(82, 32), Point(82.04305677585782, 27.000185392031746)),
                                 Segment(Point(58, 20), Point(62.57209001208863, 22.02385595370802)),
                                 Segment(Point(69, 36), Point(69.34423069848917, 40.98813644823572)),
                                 Segment(Point(79, 29), Point(83.56442368642112, 31.04108706596211)),
                                 Segment(Point(54, 37), Point(58.00683839083093, 34.009139570330404)),
                                 Segment(Point(52, 33), Point(54.46086260585502, 37.3524883957461)),
                                 Segment(Point(64, 44), Point(66.86891940037344, 48.09503375738966)) };

    std::vector<std::chrono::microseconds> times;
    for (int i = 0; i < 100; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto naive_answer = naive_algorithm(segments);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        times.push_back(duration);
    }
    std::sort(times.begin(), times.end());
    std::cout << "Naive algorithm: " << times[50].count() / 1000000.0 << '\n';

    times.clear();
    for (int i = 0; i < 100; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto answer = avl_tree_algorithm(segments);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        times.push_back(duration);
    }

    std::sort(times.begin(), times.end());
    std::cout << "AVL algorithm: " << times[50].count() / 1000000.0 << '\n';

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
