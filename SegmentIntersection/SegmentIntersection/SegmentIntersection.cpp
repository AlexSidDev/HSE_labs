// SegmentIntersection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>
#include <set>
#include <iomanip>
#include <ios>

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
        if (this->balance_factor() > 1)
        {
            throw std::runtime_error("Balance function didn't balance");
        }
    }

    node* insert(T key)
    {
        node* child;
        if (*key > *this->key)
            child = this->right;
        else
            child = this->left;

        if (child)
        {
            int old_height = child->height;
            child->insert(key);
        }
        else
        {
            if (*key > *this->key)
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
        update_height();
        balance();
        return child;
    }

    void delete_key(T key)
    {
        if (*this->key > *key)
        {
            this->left->delete_key(key);
            if (*this->left->key == *key)
                this->left = nullptr;
        }
        else if (*this->key < *key)
        {
            this->right->delete_key(key);
            if (*this->right->key == *key)
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
                if (*this->right->key == *this->key)
                    this->right = nullptr;
            }
            else if (child != nullptr)
            {
                this->key = child->key;
                this->left = child->left;
                this->right = child->right;
            }
        }
        update_height();
        balance();  
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
        node* cur_node = this;
        node* parent = nullptr;
        T previous = nullptr, next = nullptr;

        while (*cur_node->key != *key)
        {
            parent = cur_node;
            if (*cur_node->key > *key)
            {
                if (!next || *cur_node->key < *next)
                    next = cur_node->key;
                cur_node = cur_node->left;
            }
            else
            {
                if (!previous || *cur_node->key > *previous)
                    previous = cur_node->key;
                cur_node = cur_node->right;
            }  
        }

        if (cur_node->right)
        {
            next = cur_node->get_next();
        }
        if (cur_node->left)
        {
            previous = cur_node->get_previous();
        }
        return std::make_pair(previous, next);
    }

    void check_tree()
    {
        if (this->left)
        {
            if (*this->key == *this->left->key)
                std::cout << "GOVNO";
            this->left->check_tree();
        }
        if (this->right)
        {
            if (*this->key == *this->right->key)
                std::cout << "GOVNO";
            this->right->check_tree();
        }
    }
};

template<typename T>
struct Tree
{
    node<T>* tree_root;

    Tree()
    {
        this->tree_root = nullptr;
    }

    Tree(node<T>* tree_root)
    {
        this->tree_root = tree_root;
    }

    void insert(T key)
    {
        if (this->tree_root)
            this->tree_root->insert(key);
        else
            this->tree_root = new node<T>(key);
    }

    void delete_key(T key)
    {
        this->tree_root->delete_key(key);
        if (*key == *this->tree_root->key)
        {
            this->tree_root = nullptr;
        }
    }

    void check_tree()
    {
        if (this->tree_root)
            this->tree_root->check_tree();
    }
};

std::vector<Segment>* generate_random_segments(int num_segments, int num_ranges,
    int start_x = 0, int end_x = 100,
    int start_y = 0, int end_y = 50,
    int max_len = 10)
{
    float x_ratio = end_x - start_x;
    std::vector<Segment>* segments = new std::vector<Segment>();
    segments->reserve(num_ranges * num_segments * 2);

    auto generate_segment = [&start_y, &end_y, &max_len](float start_x, float end_x) {
        static int dirs[] = { -1, 1 };

        float first_x = start_x + (end_x - start_x) * ((std::rand() % 100) / 100.0);
        float first_y = start_y + (end_y - start_y) * ((std::rand() % 100) / 100.0);

        float diff_x = dirs[std::rand() % 2] * (max_len * ((std::rand() % 100) / 100.0));
        float last_x = first_x + diff_x;
        float last_y = first_y + std::sqrt(max_len * max_len - diff_x * diff_x);

        return Segment(Point(first_x, first_y), Point(last_x, last_y));
    };

    float half_range_len = (x_ratio / num_ranges) / 2.0;
    float cur_range_start = start_x, cur_range_end = start_x + half_range_len;
    while (cur_range_end < end_x)
    {
        for (int i = 0; i < num_segments; i++)
        {
            segments->push_back(generate_segment(cur_range_start, cur_range_end));
        }
        cur_range_start += half_range_len;
        cur_range_end += half_range_len;
    }
    return segments;
}

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

std::pair<const Segment*, const Segment*>* naive_algorithm(const std::vector<Segment>& seqments)
{
    std::pair<const Segment*, const Segment*>* answer{};

    size_t seqments_count = seqments.size();
    const Segment *current_segment, *candidate;
    bool is_intersection = false;
    for (int i = 0; i < seqments_count; i++)
    {
        current_segment = &seqments[i];
        for (int j = i + 1; j < seqments_count; j++)
        {
            candidate = &seqments[j];
            is_intersection = is_intersect(*current_segment, *candidate);
            if (is_intersection)
            {
                answer = new std::pair<const Segment*, const Segment*>(current_segment, candidate);
                break;
            }
        }
        if (is_intersection)
        {
            break;
        }
    }
    return answer;
}

std::pair<const Segment*, const Segment*>* avl_tree_algorithm(const std::vector<Segment>& seqments)
{
    bool is_intersection = false;
    std::pair<const Segment*, const Segment*>* answer{};

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

    Tree<const Segment*>* tree = new Tree<const Segment*>();
    
    const Point* cur_point;
    const Segment* cur_segment, *previous, *next;

    for (int ind = 0; ind < all_points_and_segments.size(); ind++)
    {
        cur_point = all_points_and_segments[ind].first;
        cur_segment = &seqments[all_points_and_segments[ind].second];

        if (*cur_point < cur_segment->first || *cur_point < cur_segment->last)
        {
            tree->insert(cur_segment);
            auto previous_and_next = tree->tree_root->get_previous_and_next(cur_segment);
            previous = previous_and_next.first;
            next = previous_and_next.second;

            if (previous && is_intersect(*cur_segment, *previous))
            {
                answer = new std::pair<const Segment*, const Segment*>(cur_segment, previous);
                is_intersection = true;
            }
            if (next && is_intersect(*cur_segment, *next))
            {
                answer = new std::pair<const Segment*, const Segment*>(cur_segment, next);
                is_intersection = true;
            }
        }
        else
        {
            auto previous_and_next = tree->tree_root->get_previous_and_next(cur_segment);
            previous = previous_and_next.first;
            next = previous_and_next.second;

            tree->delete_key(cur_segment);
            if (next && previous && is_intersect(*next, *previous))
            {
                answer = new std::pair<const Segment*, const Segment*>(next, previous);
                is_intersection = true;
            }
        }
        if (is_intersection)
            break;
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

void print_answer(std::pair<const Segment*, const Segment*>* answer)
{
    std::cout << "First segment: \n" << std::fixed;
    std::cout << std::setprecision(5) << answer->first->first.x << " " << std::setprecision(5) << answer->first->first.y << '\t';
    std::cout << std::setprecision(5) << answer->first->last.x << " " << std::setprecision(5) << answer->first->last.y << '\n';
    std::cout << "Second segment: \n";
    std::cout << std::setprecision(5) << answer->second->first.x << " " << std::setprecision(5) << answer->second->first.y << '\t';
    std::cout << std::setprecision(5) << answer->second->last.x << " " << std::setprecision(5) << answer->second->last.y << '\n';
}

int main()
{
    /*std::vector<Segment> segments = { Segment(Point(11.84133401630779, 6.362186524422792), Point(15.18651604557676, 10.078334690340823)),
                                 Segment(Point(29.70129460535119, 21.99951116440324), Point(30.952928519651138, 26.840318178174208)),
                                 Segment(Point(23.67718594521382, 15.934772863637846), Point(28.562210940131507, 14.868690878502807)),
                                 Segment(Point(27.593236552753847, 12.10216409569465), Point(29.32832769721218, 7.41287217468757)),
                                 Segment(Point(32.93071394593878, 38.612025419923654), Point(33.63336324230165, 43.562407623988385)),
                                 Segment(Point(2.984211728296482, 39.97591206152052), Point(4.3642657886644916, 35.17013940871413)),
                                 Segment(Point(3.5787592311388083, 20.25290695660063), Point(4.152398082590253, 25.21989180337694)),
                                 Segment(Point(27.16326297145007, 33.31809600861279), Point(31.150398494129604, 30.30101955552331)),
                                 Segment(Point(7.108233600809654, 18.28480990134105), Point(10.624825176794538, 21.83918518930828)),
                                 Segment(Point(24.64902872823438, 28.344136757710796), Point(25.395696226250486, 23.400202327784932)),
                                 Segment(Point(8.489553745598851, 24.107601829592106), Point(12.393543187141223, 20.983683386302385)),
                                 Segment(Point(11.284842703891591, 45.46670315330415), Point(15.47287164290983, 42.73532738165614)),
                                 Segment(Point(56.02970123759886, 12.978254125547839), Point(60.57312132254104, 10.890831328952967)),
                                 Segment(Point(47.06279606152346, 18.45229368217028), Point(51.99236759958119, 17.616037470730816)),
                                 Segment(Point(36.48279884273812, 0.22788827601673), Point(38.44431484824879, 4.8270680898667475)),
                                 Segment(Point(55.580926487083765, 20.51920829082196), Point(60.5786373553729, 20.670489740394217)),
                                 Segment(Point(64.19297069522472, 37.69930661268347), Point(65.57226575277463, 42.50529716034567)),
                                 Segment(Point(53.23870858904888, 46.453344376505456), Point(57.855343838778865, 48.373417023816124)),
                                 Segment(Point(40.36019203974892, 23.60064941677206), Point(42.090859570227906, 28.2915757537492)),
                                 Segment(Point(56.66124465290936, 5.30904032093088), Point(59.31681951670403, 1.072541513804655)),
                                 Segment(Point(38.644640717903414, 37.709651226299215), Point(42.50973962947336, 40.8815608094653)),
                                 Segment(Point(60.98578032625896, 21.165223568109724), Point(64.0916965955526, 17.24689686808367)),
                                 Segment(Point(46.09648661345054, 36.54819041007453), Point(47.13472125121097, 41.43920961234255)),
                                 Segment(Point(55.10750029093448, 19.68904863810268), Point(55.60186849429654, 14.713548655236178)),
                                 Segment(Point(96.6336336454596, 32.975569178463545), Point(97.70838122485996, 37.858694984806554)),
                                 Segment(Point(98.92923026498698, 38.93295863128602), Point(103.23202152139977, 41.4797188091861)),
                                 Segment(Point(92.79082648992605, 12.554921889016585), Point(96.03179326870698, 16.36230082193597)),
                                 Segment(Point(82.1835578570851, 21.820279335028342), Point(84.79048584506194, 17.55367336945066)),
                                 Segment(Point(75.44836249463593, 15.529643186342101), Point(80.04137504589679, 17.50555707716838)),
                                 Segment(Point(82.90537046020401, 13.208926528573583), Point(84.35427977504236, 17.994390121306754)),
                                 Segment(Point(99.12114063700508, 12.944902251386752), Point(103.52892399902684, 15.305293285407206)),
                                 Segment(Point(97.98503592870807, 41.64861511009813), Point(100.45945639873835, 37.30382013938949)),
                                 Segment(Point(96.42806750630123, 19.178985074001474), Point(96.8918079811377, 14.200537045549389)),
                                 Segment(Point(99.03194543451951, 15.84863068576804), Point(101.64646196640504, 20.110590716610336)),
                                 Segment(Point(67.36524911320046, 39.75939866483581), Point(72.14427199375453, 41.22941237468368)),
                                 Segment(Point(70.39366469776603, 0.5204254181630341), Point(71.60083088615944, 5.372512576654699)),
                                 Segment(Point(33.13104838386328, 46.8778378189979), Point(34.33523258523492, 42.02500973537831)),
                                 Segment(Point(17.078266658279205, 0.9513339596586345), Point(19.296146885921978, -3.529849733946354)),
                                 Segment(Point(45.45439406984099, 44.040422791977974), Point(46.85953053602206, 39.24192391093109)),
                                 Segment(Point(44.119122916009786, 21.442570272570478), Point(47.26677361849212, 17.55768995843434)),
                                 Segment(Point(17.283491964698953, 19.518977355430962), Point(22.058401451374273, 18.035656960486556)),
                                 Segment(Point(28.53991976007554, 21.807634825896073), Point(33.08799507281923, 19.730374382520864)),
                                 Segment(Point(27.009405909749567, 29.39366980485824), Point(29.444029248029565, 25.026449956865427)),
                                 Segment(Point(35.123385458252265, 33.71269370948164), Point(36.806267871377365, 38.420974370571656)),
                                 Segment(Point(44.578995083214366, 8.967052318259745), Point(49.299644811223764, 10.614919468683222)),
                                 Segment(Point(44.60881690221589, 40.122504770572576), Point(46.70399141889793, 44.662355400785664)),
                                 Segment(Point(23.1995793986049, 38.89784328697491), Point(24.60915228425877, 43.69504083137825)),
                                 Segment(Point(41.769133097044424, 8.988166458523894), Point(43.03229416605014, 13.82597837236969)),
                                 Segment(Point(71.9120113502837, 15.273759551475209), Point(73.39851840056828, 10.49984116199113)),
                                 Segment(Point(64.7482491865228, 40.83008328617397), Point(67.49228629348377, 36.650339232332726)),
                                 Segment(Point(50.390782424862, 19.96248273210286), Point(50.440452859127134, 24.96223601081171)),
                                 Segment(Point(49.0901982633797, 19.08327875282028), Point(49.57424114841547, 14.106763658354451)),
                                 Segment(Point(68.72801194336212, 9.99599250876268), Point(69.60215316636166, 14.91898723984384)),
                                 Segment(Point(66.51562662946603, 36.10872254642399), Point(67.61719440901008, 40.98586806681503)),
                                 Segment(Point(71.50868600301132, 33.58576893963993), Point(74.14730958393262, 29.338691638929937)),
                                 Segment(Point(50.76159321070935, 45.869047112637325), Point(52.74059018989901, 50.460732092724506)),
                                 Segment(Point(55.299232820070706, 11.103975434443264), Point(58.356618817989485, 15.060285692718036)),
                                 Segment(Point(77.12182105315073, 33.230116684042194), Point(78.0808626749474, 38.13727871190123)),
                                 Segment(Point(76.08596659320503, 12.370348599224535), Point(77.54827091039141, 17.151736063542614)),
                                 Segment(Point(78.48014300376772, 35.62651058551893), Point(82.04103183182369, 39.13650641945181)),
                                Segment(Point(11.93524716859276, 36.241718111755425), Point(14.436646523263096, 31.912399312168723)),
                                 Segment(Point(25.502321730327374, 9.46363603215621), Point(28.811238605375397, 5.715160460137309)),
                                 Segment(Point(7.213446424135197, 8.436266980396335), Point(8.823791006172996, 3.702685849043297)),
                                 Segment(Point(21.567834625340726, 1.641355309073761), Point(22.663515867534144, 6.519826649104928)),
                                 Segment(Point(8.498002618590318, 31.760935955238306), Point(10.375586220727573, 27.126857857386987)),
                                 Segment(Point(28.510413446103758, 35.97950544098956), Point(33.206034706975714, 37.69739396311411)),
                                 Segment(Point(31.3854905373413, 18.327554466463475), Point(35.27932529432366, 21.46412120181538)),
                                 Segment(Point(20.37475712249952, 14.329426331367678), Point(21.776111077773347, 19.129031220526617)),
                                 Segment(Point(15.949203322727977, 7.632981160711511), Point(18.849672750901668, 3.5602330594622105)),
                                 Segment(Point(12.861647367618882, 3.6124375686843124), Point(13.238398409591063, 8.598223229176837)),
                                 Segment(Point(6.8698723463465905, 43.17367535364342), Point(7.071177432843107, 48.16962133635286)),
                                 Segment(Point(27.83747812465657, 41.48239332734258), Point(31.229736955589388, 45.15561807189039)),
                                 Segment(Point(58.88535430459174, 40.780159783231376), Point(63.42425599242865, 42.877389257597805)),
                                 Segment(Point(57.88951657786049, 46.63778901419162), Point(61.49054819739267, 43.16898932624055)),
                                 Segment(Point(57.48090147431419, 43.098735210142785), Point(58.927076337626694, 38.31244455194796)),
                                 Segment(Point(35.669656668362876, 36.34122876862987), Point(39.19963239322245, 39.88231213137384)),
                                 Segment(Point(51.75834275909225, 35.265675701949235), Point(52.373849409589155, 40.22764623422712)),
                                 Segment(Point(34.713911483536855, 30.401688889067852), Point(35.27733381320274, 25.433534777284095)),
                                 Segment(Point(61.34789578385755, 23.886086562148396), Point(62.21238965631127, 18.961388566891227)),
                                 Segment(Point(40.263398024729405, 4.792207737497234), Point(41.99774597625887, 9.48177458135665)),
                                 Segment(Point(59.72294923219259, 43.319125535606794), Point(61.614986833618815, 47.94732104949655)),
                                 Segment(Point(51.587190171322234, 9.211547314143886), Point(54.105414149842844, 4.891993171179148)),
                                 Segment(Point(58.17864635835883, 5.975006622706058), Point(63.04129423242426, 4.811109869189227)),
                                 Segment(Point(33.16184321994764, 14.273137909146111), Point(37.32280764241991, 11.500706652122771)),
                                 Segment(Point(97.72157697827721, 32.82636615575317), Point(98.46187721370627, 27.881474290259682)),
                                 Segment(Point(90.40985246984324, 29.47135955767144), Point(91.75200995910164, 24.654865679960437)),
                                 Segment(Point(68.48037286570397, 9.66686789774866), Point(72.3023702906889, 12.890582476213783)),
                                 Segment(Point(73.36416641303359, 19.62898505518975), Point(77.60566093044272, 16.98139666973626)),
                                 Segment(Point(86.50971183323148, 40.16900937948664), Point(87.72360666253117, 35.31860121692965)),
                                 Segment(Point(89.20648262765775, 12.226269729424294), Point(90.15290930275883, 7.316659098360246)),
                                 Segment(Point(84.64293943959453, 48.31125440644605), Point(89.60089211666957, 48.958328785389014)),
                                 Segment(Point(99.64732929081859, 17.77359229634629), Point(101.0411965443043, 22.57537676461017)),
                                 Segment(Point(80.74054314671254, 27.84073610502709), Point(83.61906258157697, 23.75244479164105)),
                                 Segment(Point(97.53615600086314, 27.695240672316345), Point(98.12382023978836, 32.66058564948444)),
                                 Segment(Point(91.21352208087657, 18.386231073704174), Point(93.43231782279516, 22.86696153375686)),
                                 Segment(Point(77.30712994015417, 28.361075743328414), Point(78.22414196304912, 23.44588612848484)),
                                 Segment(Point(48.67144906610628, 43.14901404792384), Point(50.988283514440845, 47.57984671673568)),
                                 Segment(Point(34.90672207859216, 17.60345963993438), Point(37.19353996456399, 22.04985862747467)),
                                 Segment(Point(33.686617250081454, 29.890216591974273), Point(34.32152054724617, 24.93069062639557)),
                                 Segment(Point(25.328173417536867, 41.61550315640323), Point(25.768533153627704, 36.6349325763562)),
                                 Segment(Point(43.93224849556202, 25.602629488035006), Point(47.287023650040524, 29.310119698796694)),
                                 Segment(Point(32.242513944634176, 3.0840044378006857), Point(33.262547737633625, -1.810842961367051)),
                                 Segment(Point(26.196477969358046, 14.969849445428013), Point(27.250264076833496, 10.082157266117644)),
                                 Segment(Point(48.97112850862501, 22.834417345220082), Point(51.06976872119043, 18.296167764357065)),
                                 Segment(Point(21.564563562555822, 2.549391142566315), Point(23.475491638580067, -2.071036747603845)),
                                 Segment(Point(46.77722375490636, 0.8623460361387114), Point(49.25707820182759, 5.204041778827851)),
                                 Segment(Point(28.17121295723838, 42.90379823241936), Point(28.315061388630774, 47.90172856694644)),
                                 Segment(Point(28.843301816068358, 11.875413178610405), Point(33.83527564116992, 11.592221848409668)),
                                 Segment(Point(80.84791215375134, 32.65731926401166), Point(81.8465717960564, 37.556572157956936)),
                                 Segment(Point(50.17999173852189, 42.353332382155415), Point(50.85435065922555, 37.39901708677227)),
                                 Segment(Point(63.9188363688437, 7.178230304877342), Point(68.8870229672629, 7.741366099591116)),
                                 Segment(Point(71.84454280186823, 25.621500657288166), Point(72.19934409519726, 30.608896374719464)),
                                 Segment(Point(76.67276582099365, 18.331237960809254), Point(76.86098292931091, 23.32769413695417)),
                                 Segment(Point(56.278187630933665, 24.05193669866402), Point(57.266566422773955, 19.150599395045717)),
                                 Segment(Point(55.50414420287367, 48.84161526991769), Point(57.19704395313767, 53.54630340363666)),
                                 Segment(Point(79.65521306362422, 41.29064561396645), Point(83.6768729583655, 38.31974479249334)),
                                 Segment(Point(74.0572498801055, 45.339343031140366), Point(75.46519340166712, 40.54166702706374)),
                                 Segment(Point(74.04729589976444, 31.15776528246136), Point(74.49220280482893, 26.177598834858834)),
                                 Segment(Point(64.05190355960124, 4.022314552128154), Point(67.65777256746718, 0.5585436737194871)),
                                 Segment(Point(75.11825669520884, 35.739442299997094), Point(75.81919350884866, 40.69006726895651))
    };*/
    std::ios_base::fmtflags f(std::cout.flags());

    for (int i = 0; i < 100; i++)
    {
        std::vector<Segment> segments = *generate_random_segments(15, 3, 0, 200);

        std::pair<const Segment*, const Segment*>* naive_answer, * answer;
        std::vector<std::chrono::microseconds> times;
        for (int i = 0; i < 100; i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            naive_answer = naive_algorithm(segments);
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
            answer = avl_tree_algorithm(segments);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            times.push_back(duration);
        }

        std::sort(times.begin(), times.end());
        std::cout << "AVL algorithm: " << times[50].count() / 1000000.0 << '\n';

        std::cout << "Num segments: " << segments.size() << '\n';

        if (naive_answer)
            print_answer(naive_answer);
        std::cout << "================================================\n";
        if (answer)
            print_answer(answer);

        std::cout.flags(f);

        std::cout << "\n\n\n";
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
