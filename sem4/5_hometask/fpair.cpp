#include <algorithm>
#include <cstdio>

class BadAddition {
    float A1, B1, A2, B2;
public:
    BadAddition (float s1, float e1, float s2, float e2) {
        A1 = s1;
        A2 = s2;
        B1 = e1;
        B2 = e2;
    }
    float GetA1() const { return A1; }
    float GetB1() const { return B1; }
    float GetA2() const { return A2; }
    float GetB2() const { return B2; }
};

class BadSegment {};

class FloatPair {
    float A, B;
public:
    virtual float Measure() const = 0;
    float get_A() const { return A; }
    float get_B() const { return B; }

    FloatPair(float start, float end) {
        if (start > end) {
            throw BadSegment();
        }
        A = start;
        B = end;
    }
};

class Segment : public FloatPair {
public:
    Segment (float start, float end) : FloatPair(start, end) {}

    float Measure() const { return abs(this->get_B() - this->get_A()); }

    Segment operator+ (const Segment & seg2) const {
        float s1(this->get_A()), e1(this->get_B()), s2(seg2.get_A()), e2(seg2.get_B());
        if (e1 < s2 || e2 < s1) {
            throw BadAddition(s1, e1, s2, e2);
        }
        Segment T(std::min(s1,s2), std::max(e1,e2));
        return T;
    }
};


int main() {
    try {
        Segment f(1, 2), g(0.5, 5), h(2.5, 6.5);
        printf("%3.3f, %3.3f, %3.3f\n", (f+g).Measure(), (g+h).Measure(), (f+g+h).Measure() );
        printf("%3.3f \n", (f+h).Measure() );
    }
    catch (const BadAddition &bad) {
        printf("Bad addition: [%3.3f; %3.3f] + [%3.3f; %3.3f]\n",
               bad.GetA1(), bad.GetB1(), bad.GetA2(), bad.GetB2() );
    }
    catch (BadSegment b) { printf("Bad segment\n"); }
    return 0;
}
