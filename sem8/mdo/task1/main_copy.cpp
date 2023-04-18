#include <cstdint>
#include <bitset>
#include <utility>
#include <iostream>
#include <vector>

struct Part {
    std::bitset<100> positions;
    int weight;
    int price;

    Part(const Part& base, size_t position, int _weight, int _price) {
        positions = base.positions;
        positions[position] = true;
        weight = base.weight + _weight;
        price = base.price + _price;
    }

    Part() {
        positions = 0;
        weight = 0;
        price = 0;
    }
};


std::vector<Part> create_list(std::vector<Part>& first, std::vector<Part>& second) {
    std::vector<Part> result;

    int first_it = 0, second_it = 0;

    while (first_it < first.size() && second_it < second.size()) {

        auto weight1 = first[first_it].weight, weight2 = second[second_it].weight;
        auto price1 = first[first_it].price, price2 = second[second_it].price;

        if (weight1 < weight2 && price1 < price2) {
            result.emplace_back(first[first_it]);
            first_it++;
        } else if ((weight1 < weight2 && price1 >= price2) || (weight1 == weight2 && price1 >= price2)) {
            second_it++;
        } else if (weight1 > weight2 && price1 > price2) {
            //add Partent from the second list
            result.emplace_back(second[second_it]);
            second_it++;
        } else if ((weight1 > weight2 && price1 <= price2) || (weight1 == weight2 && price1 < price2)) {
            first_it++;
        }
    }

    for (int i = first_it; i < first.size(); i++)
        result.emplace_back(first[i]);

    for (int i = second_it; i < second.size(); i++)
        result.emplace_back(second[i]);

    return result;
}


int main(int argc, char** argv) {

    int C, n;
    std::cin >> n >> C;

    std::vector<Part> list;

    std::vector<std::pair<int, int>> things(n);

    for (auto& thing : things) {
        std::cin >> thing.first >> thing.second;
    }

    list.emplace_back();

    for (int i = 0; i < n; i++) {

        std::vector<Part> new_list;

        for (const auto& elem : list) {
            Part tmp(elem, i, things[i].first, things[i].second);
            if (tmp.weight <= C)
                new_list.push_back(tmp);
        }

        list = create_list(list, new_list);
    }

    auto final = list[list.size()-1];

    std::cout << final.price << ' ' << final.positions.count() << std::endl;
    for (int i = 0; i < 100; i++) {
        if (final.positions[i]) {
            std::cout << i << std::endl;
        }
    }
}
