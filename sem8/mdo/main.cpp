#include <iostream>
#include <utility>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cstdint>
#include <sys/time.h>

double timer() {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec+(double)tp.tv_usec * 1.e-6);
}


#define MAX_THINGS 100


struct Elem {
    std::bitset<MAX_THINGS> solution;
    int weight;
    int price;

    Elem(const Elem& base, size_t position, int _weight, int _price) {
        solution = base.solution;
        solution[position] = true;
        weight = base.weight + _weight;
        price = base.price + _price;
    }

    Elem() {
        solution = 0;
        weight = 0;
        price = 0;
    }
};

bool operator==(const Elem& left, const Elem& right) {
    return (left.weight == right.weight && left.price == right.price);
}

inline bool is_dominated(const Elem& sub, const Elem& dom) {
    return (sub.weight >= dom.weight && sub.price <= dom.price);
}

bool weight_comparator(const Elem& e1, const Elem& e2) {
    return e1.weight < e2.weight;
}

bool price_comparator(const Elem& e1, const Elem& e2) {
    return e1.price < e2.price;
}

void print(const std::vector<Elem>& list) {
    std::cout << "list" << std::endl;
    for (const auto& elem : list) {
        std::cout << elem.solution << ' ' << elem.weight << ' ' << elem.price << std::endl;
    }
    std::cout << "\n" << std::endl;
}

std::vector<Elem> merge(std::vector<Elem>& base, std::vector<Elem>& addition) {
    std::vector<Elem> result;
    bool add;

    for (auto it_base = base.begin(), it_addition = addition.begin(); it_base != base.end(); it_base++) {
        add = true;

        for ( ; it_addition != addition.end(); ) {
            if (is_dominated(*it_addition, *it_base)) {
                it_addition = addition.erase(it_addition);
            } else if (is_dominated(*it_base, *it_addition)) {
                add = false;
                break;
            } else if (it_base->weight > it_addition->weight && it_base->price > it_addition->price) {
                it_addition++;
            } else {
                break;
            }
        }

        if (add) {
            result.push_back(std::move(*it_base));
        }
    }

    for (const auto& it : addition) {
        result.push_back(std::move(it));
    }

    return result;
}


std::vector<Elem> merge_sort(std::vector<Elem>& base, std::vector<Elem>& addition) {
    std::vector<Elem> result;
//    bool base_end = false, add_end = false;
    auto it_base = base.begin();
    auto it_addition = addition.begin();

    for ( ; it_base != base.end() && it_addition != addition.end(); ) {

        auto weight1 = it_base->weight, weight2 = it_addition->weight;
        auto price1 = it_base->price, price2 = it_addition->price;

        if (weight1 < weight2 && price1 < price2) {
            //add element from the first list
            result.push_back(std::move(*it_base));
            it_base++;
        } else if ((weight1 < weight2 && price1 >= price2) || (weight1 == weight2 && price1 >= price2)) {
            //first list dominating second list
//            it_addition = addition.erase(it_addition);
            it_addition++;
        } else if (weight1 > weight2 && price1 > price2) {
            //add element from the second list
            result.push_back(std::move(*it_addition));
            it_addition++;
        } else if ((weight1 > weight2 && price1 <= price2) || (weight1 == weight2 && price1 < price2)) {
            // second list dominating first list
//            it_base = base.erase(it_base);
            it_base++;
        }
    }

    for (; it_base != base.end(); it_base++)
        result.push_back(std::move(*it_base));

    for (; it_addition != addition.end(); it_addition++)
        result.push_back(std::move(*it_addition));

    return result;
}

Elem get_max(const std::vector<Elem>& list) {
    int max_price = 0, position = list.size() - 1;
    for (auto it = list.end() - 1 ; it >= list.begin(); it--) {
        if (max_price < it->price) {
            max_price = it->price;
            position = it - list.begin();
        }
    }
    return list[position];
}

int main(int argc, char** argv) {
//    auto t1 = timer();

    int C, n;
    std::cin >> n >> C;

    int weight, price;

    std::vector<Elem> list;
    std::vector<Elem> result;

    list.emplace_back();

    for (int i = 0; i < n; i++) {
        //update list
        std::cin >> weight >> price;

        std::vector<Elem> new_list;
        for (const auto& elem : list) {
            Elem tmp(elem, i, weight, price);
            if (tmp.weight <= C)
                new_list.push_back(std::move(tmp));
        }

        //merge list
        list = merge_sort(list, new_list);

        //sort list
//        std::sort(list.begin(), list.end(), weight_comparator);
//        print(list);
    }

//    std::sort(list.begin(), list.end(), price_comparator);
    auto final = list[list.size()-1];
//    auto final = get_max(list);

    std::cout << final.price << ' ' << final.solution.count() << std::endl;
    for (int i = 0; i < MAX_THINGS; i++) {
        if (final.solution[i]) {
            std::cout << i << std::endl;
        }
    }

//    auto t2 = timer();
//    std::cout << "Time: " << t2-t1 << std::endl;

    return 0;
}
