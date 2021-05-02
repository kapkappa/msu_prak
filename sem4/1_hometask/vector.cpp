#include <iostream>
#include <cerrno>

class Vector {
    // Privat
    int size;
    int *start, *end;   // end - points to last elem

    bool if_allocated;

public:
    Vector(): size(0), start(nullptr), end(nullptr), if_allocated(false) {}

    Vector(const Vector &vec) {
        size = vec.get_size();
        if(size) {
            start = (int*)malloc(sizeof(int) * size);
            if(start == nullptr) {
                std::perror("Allocation error");
                exit(1);
            }
            for (int i = 0; i < size; i++)
                *(start + i) = vec[i];
            end = start + size - 1;
            if_allocated = true;
        } else {
            start = nullptr;
            end = nullptr;
        }
    }

    Vector(const std::initializer_list<int> &list) {
        size = list.size();
        if(size) {
            start = (int*)malloc(sizeof(int) * size);
            if(start == nullptr) {
                std::perror("Allocation error");
                exit(1);
            }
            for (int i = 0; i < size; i++)
                *(start + i) = *(list.begin() + i);
            end = start + size - 1;
            if_allocated = true;
        } else {
            start = nullptr;
            end = nullptr;
        }
    }

    ~Vector() { free(start); }

    int operator[] (const int pos) const {
        if(0 <= pos && pos < size)
            return *(start + pos);
        else {
            perror("Out of bounds of vector");
            exit(2);
        }
    }

    Vector& operator= (const Vector& vec) {
        if(!vec.if_allocated) return *this;

        size = vec.size;
        start = (int*)realloc(start, sizeof(int) * size);
        if(start == nullptr) {
            perror("Reallocation error");
            exit(1);
        }
        for(int i = 0; i < size; i++)
            *(start+i) = vec[i];
        end = start + size - 1;
        if(!size) end--;
        return *this;
    }

    void push_back(const int value) {
        size++;
        start = (int*)realloc(start, sizeof(int) * size);
        if(start == nullptr) {
            std::perror("Reallocation error");
            exit(1);
        }
        if_allocated = true;
        end = start + size - 1;
        *end = value;
    }

    void pop_back() {
        if(!size) return;
        size--;
        start = (int*)realloc(start, size * sizeof(int));
        end = start + size - 1;
        if(!size) end++;
    }

    int get_size() const { return size; }

    void print() const {
        std::cout << "Size: " << size << std::endl;
        for(int i = 0; i < size; i++) {
            std::cout << *(start+i) << " ";
        }
        std::cout << std::endl;
    }
};



int main() {
//    Vector a = {1, 2, 3, 4, 5, 6};
//    Vector b {2, 3, 4};

    Vector a;
    for(int i = 0; i < 5; i++)
        a.push_back(i);
    a.print();

    a.pop_back();

    Vector b = a;
    b.print();

    a.pop_back();

    Vector c{a};
    c.print();

    a.pop_back();

    Vector d;
    d = a;
    d.print();

    Vector e = {3,2,1};
    e.print();

    Vector f{1, 4, 2, 5};
    f.print();

    std::cout << a[-1] << std::endl;

    return 0;
}
