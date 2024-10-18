struct Seed {
public:
    int n;
}

struct Apple {
public:
    Seed s;
};

int main() {
    Seed seed;
    Apple apple;
    apple.s = seed;
    apple.s.n = 5;

    return 0;
}