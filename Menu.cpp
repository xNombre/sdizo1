#include <iostream>
#include <fstream>

#define NDEBUG

#include "Array.hpp"
#include "BinHeap.hpp"
#include "List.hpp"
#include "RBTree.hpp"
#include "AVLTree.hpp"
#include "TimeBench.cpp"

using namespace std;

typedef int datatype;

const string commonOperations = "r - read from file\na - add value\nx - remove value\nc - search if value exists\np - print container contents\nq - quit\n";

vector<datatype> readFromFile()
{
    string fileName;

    cout << "Enter file name\n";
    cin >> fileName;

    ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("nope");

    vector<datatype> values;
    size_t len;
    file >> len;

    for (; len > 0; len--) {
        datatype val;
        file >> val;
        values.push_back(val);
    }

    return values;
}

datatype getDataFromUser()
{
    datatype val;
    cout << "Podaj wartosc:\n";
    cin >> val;

    // Clear any garbage
    cin.clear();
    cin.ignore(10000, '\n');

    return val;
}

char getOptionFromUser()
{
    char input;
    std::cin >> input;

    // Clear any garbage
    cin.clear();
    cin.ignore(10000, '\n');

    return input;
}

void rbTreeMenu()
{
    char input;
    RBTree<datatype> container;
repeat:
    cout << commonOperations;
    input = getOptionFromUser();

    switch (input) {
    case 'q':
        return;
        break;
    case 'r': {
        auto data = readFromFile();
        for (auto val : data)
            container.add(val);
        break;
    }
    case 'a': {
        container.add(getDataFromUser());
        break;
    }
    case 'x':
        container.remove(getDataFromUser());
        break;
    case 'c':
        cout << container.contains(getDataFromUser()) << endl;
        break;
    case 'p':
        container.print();
        std::cin.get();
        break;
    }

    goto repeat;
}

void avlTreeMenu()
{
    char input;
    AVLTree<datatype> container;
repeat:
    cout << commonOperations;
    input = getOptionFromUser();

    switch (input) {
    case 'q':
        return;
        break;
    case 'r': {
        auto data = readFromFile();
        for (auto val : data)
            container.add(val);
        break;
    }
    case 'a': {
        container.add(getDataFromUser());
        break;
    }
    case 'x':
        container.remove(getDataFromUser());
        break;
    case 'c':
        cout << container.contains(getDataFromUser()) << endl;
        break;
    case 'p':
        container.print();
        std::cin.get();
        break;
    }

    goto repeat;
}

void listMenu()
{
    char input;
    List<datatype> container;
repeat:
    cout << commonOperations;
    cout << "i - insert\n";
    cout << "f - push front\n";
    cout << "y - pop_front\n";
    cout << "u - pop_back\n";
    input = getOptionFromUser();

    switch (input) {
    case 'q':
        return;
        break;
    case 'r': {
        auto data = readFromFile();
        for (auto val : data)
            container.push_back(val);
        break;
    }
    case 'a': {
        container.push_back(getDataFromUser());
        break;
    }
    case 'x':
        container.remove(getDataFromUser());
        break;
    case 'c':
        cout << container.contains(getDataFromUser()) << endl;
        break;
    case 'i': {
        auto look = getDataFromUser();
        auto val = getDataFromUser();
        auto pos = container.get_node(look);
        container.insert(val, pos);
        break;
    }
    case 'f':
        container.push_front(getDataFromUser());
        break;
    case 'y':
        container.pop_front();
        break;
    case 'u':
        container.pop_back();
        break;
    case 'p':
        container.print();
        std::cin.get();
        break;
    }

    goto repeat;
}

void arrayMenu()
{
    char input;
    Array<datatype> container;
repeat:
    cout << commonOperations;
    cout << "i - insert\n";
    cout << "f - push front\n";
    cout << "y - pop_front\n";
    cout << "u - pop_back\n";
    input = getOptionFromUser();

    switch (input) {
    case 'q':
        return;
        break;
    case 'r': {
        auto data = readFromFile();
        for (auto val : data)
            container.push_back(val);
        break;
    }
    case 'a': {
        container.push_back(getDataFromUser());
        break;
    }
    case 'x': {
        container.remove(getDataFromUser());
        break;
    }
    case 'c':
        cout << container.contains(getDataFromUser()) << endl;
        break;
    case 'i': {
        auto at = getDataFromUser();
        auto val = getDataFromUser();
        container.insert(val, at);
        break;
    }
    case 'f':
        container.push_front(getDataFromUser());
        break;
    case 'y':
        container.pop_front();
        break;
    case 'u':
        container.pop_back();
        break;
    case 'p':
        container.print();
        std::cin.get();
        break;
    }

    goto repeat;
}

void binHeapMenu()
{
    char input;
    BinHeap<datatype> container;
repeat:
    cout << commonOperations;
    input = getOptionFromUser();

    switch (input) {
    case 'q':
        return;
        break;
    case 'r': {
        auto data = readFromFile();
        for (auto val : data)
            container.add(val);
        break;
    }
    case 'a':
        container.add(getDataFromUser());
        break;
    case 'x':
        container.remove(getDataFromUser());
        break;
    case 'c':
        cout << container.contains(getDataFromUser()) << endl;
        break;
    case 'p':
        container.print();
        std::cin.get();
        break;
    }

    goto repeat;
}

int main()
{
    char input;
    cout << "SDiZO Projekt 1.\n"
        << "b - test automatyczny kontenerów\n"
        << "m - test manualny\n";
    input = getOptionFromUser();

    if (input == 'b') {
        TimeBenchmark bench;
        bench.run();
        return 0;
    }

    cout << "Wybierz strukture:\n"
        << "h - BinHeap\n"
        << "a - Array\n"
        << "r - RBTree\n"
        << "l - List\n"
        << "v - AVLTree\n";
    input = getOptionFromUser();

    switch (input) {
    case 'h':
        binHeapMenu();
        break;
    case 'a':
        arrayMenu();
        break;
    case 'r':
        rbTreeMenu();
        break;
    case 'l':
        listMenu();
        break;
    case 'v':
        avlTreeMenu();
        break;
    }

    return 0;
}