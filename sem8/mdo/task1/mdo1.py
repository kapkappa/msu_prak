class Elem:
    def __init__(self, base=None, position=None, w=0, p=0):
        if base is None:
            self.solution = [False] * 100
            self.weight = 0
            self.price = 0
        else:
            self.solution = base.solution.copy()
            self.solution[position] = True
            self.weight = base.weight + w
            self.price = base.price + p

    def __eq__(self, other):
        return self.weight == other.weight and self.price == other.price


def merge_sort(base, addition):
    res = []
    it_base = 0
    it_addition = 0

    while it_base < len(base) and it_addition < len(addition):

        weight1 = base[it_base].weight
        weight2 = addition[it_addition].weight
        price1 = base[it_base].price
        price2 = addition[it_addition].price

        if weight1 < weight2 and price1 < price2:
            res.append(base[it_base])
            it_base += 1
        elif (weight1 < weight2 and price1 >= price2) or (weight1 == weight2 and price1 >= price2):
            it_addition += 1
        elif weight1 > weight2 and price1 > price2:
            res.append(addition[it_addition])
            it_addition += 1
        elif (weight1 > weight2 and price1 <= price2) or (weight1 == weight2 and price1 < price2):
            it_base += 1

    for j in range(it_base, len(base)):
        res.append(base[j])

    for j in range(it_addition, len(addition)):
        res.append(addition[j])

    return res


if __name__ == "__main__":
    n, C = map(int, input().split())

    work_list = [Elem()]
    result = []

    for i in range(n):

        weight, price = map(int, input().split())

        new_list = []
        for elem in work_list:
            tmp = Elem(elem, i, weight, price)
            if tmp.weight <= C:
                new_list.append(tmp)

        work_list = merge_sort(work_list, new_list)

    final = work_list[-1]

    print(final.price, final.solution.count(True))
    for i in range(100):
        if final.solution[i]:
            print(i)
