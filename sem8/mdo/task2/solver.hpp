#include <iostream>
#include <vector>
#include <bitset>
#include <cstdint>

#define MAX_THINGS 100

class ListSolver {
private:
    int n;
    int C;


struct Elem {
    std::bitset<MAX_THINGS> solution;
    double weight;
    double price;

    Elem(const Elem& base, size_t position, double _weight, double _price) {
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

std::vector<Elem> merge_sort(std::vector<Elem>& base, std::vector<Elem>& addition) {
    std::vector<Elem> result;
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
            it_addition++;
        } else if (weight1 > weight2 && price1 > price2) {
            //add element from the second list
            result.push_back(std::move(*it_addition));
            it_addition++;
        } else if ((weight1 > weight2 && price1 <= price2) || (weight1 == weight2 && price1 < price2)) {
            // second list dominating first list
            it_base++;
        }
    }

    for (; it_base != base.end(); it_base++)
        result.push_back(std::move(*it_base));

    for (; it_addition != addition.end(); it_addition++)
        result.push_back(std::move(*it_addition));

    return result;
}

public:
    ListSolver(int _n, int _C) : n(_n), C(_C) {}

std::pair<double, std::vector<int>> solve(const std::vector<double>& price, const std::vector<double>& weight) {

    std::vector<Elem> list;
    list.emplace_back();

    for (int i = 0; i < n; i++) {

        std::vector<Elem> new_list;
        for (const auto& elem : list) {
            Elem tmp(elem, i, weight[i], price[i]);
            if (tmp.weight <= C)
                new_list.push_back(std::move(tmp));
        }

        //merge and sort list
        list = merge_sort(list, new_list);
    }

    auto final = list[list.size()-1];

    std::pair<double, std::vector<int>> result;
    result.first = final.price;

    for (int i = 0; i < MAX_THINGS; i++) {
        if (final.solution[i]) {
            result.second.push_back(i);
        }
    }

    return result;
}

};
