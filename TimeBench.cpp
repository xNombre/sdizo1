#include <chrono> 
#include <iostream>
#include <random>
#include <algorithm>

#include "Array.hpp"
#include "BinHeap.hpp"
#include "List.hpp"
#include "RBTree.hpp"
#include "AVLTree.hpp"

using namespace std;

static std::random_device rd;
static std::default_random_engine generator(rd());

class TimeBenchmark {
    const size_t averagingLoopsCount = 10;
    const size_t datasetGenerationCount = 100;

    const std::vector<std::size_t> datasetSizesToTest =
    { 500, 1'000, 2'000, 5'000, 10'000, 25'000, 50'000, 100'000, 250'000 };

    size_t datasetSize = 500;

    template <typename T>
    T randomNumberWithinRange(const T &first, const T &second)
    {
        std::uniform_int_distribution<T> distribution(first, second);

        return distribution(generator);
    }

    typedef int datatype;
    typedef unsigned long long timedata;

    class AveragedTimeMeasure {
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> elapsed = std::chrono::duration<double>::zero();
        std::size_t times = 0;

    public:
        void benchmarkStart()
        {
            start = std::chrono::high_resolution_clock::now();
        }

        void benchmarkStop()
        {
            end = std::chrono::high_resolution_clock::now();

            elapsed += end - start;
            times++;
        }

        timedata getAvgElapsedNsec()
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() / times;
        }
    };

    template < typename T>
    std::vector<T> generateRandomData(std::size_t datasetSize)
    {
        std::vector<T> dataset;

        for (int i = 0; i < datasetSize; i++) {
            dataset.push_back(randomNumberWithinRange(INT32_MIN, INT32_MAX));
        }

        // Remove any duplicates
        std::vector<datatype>::iterator last = std::unique(dataset.begin(), dataset.end());
        while (last != dataset.end()) {
            while (last != dataset.end()) {
                (*last++) = randomNumberWithinRange(INT32_MIN, INT32_MAX);
            }

            last = std::unique(dataset.begin(), dataset.end());
        }

        return dataset;
    }

    template < typename T>
    T getRandomValueFromDatasetAndRemove(std::vector<T> &dataset)
    {
        std::size_t pos = randomNumberWithinRange((std::size_t)0, dataset.size() - 1);

        dataset.erase(dataset.begin() + pos);

        return dataset[pos];
    }

    template <template <typename> typename T, typename D>
    timedata benchmarkSuiteAdd(std::function<void(T<D> &, D)> containerFunc)
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                T<D> container;
                for (const auto &val : dataset) {
                    containerTimeAveraging.benchmarkStart();
                    containerFunc(container, val);
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

    template <typename D>
    timedata benchmarkSuiteListInsert()
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                List<D> container;

                // Push first element
                container.push_back(dataset[0]);

                for (size_t datasetAt = 1; datasetAt < datasetSize; datasetAt++) {
                    auto insertAt = randomNumberWithinRange((size_t)0, datasetAt - 1);
                    auto *node = container.get_node(dataset[insertAt]);

                    containerTimeAveraging.benchmarkStart();
                    container.insert(dataset[datasetAt], node);
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

    template <typename D>
    timedata benchmarkSuiteArrayInsert()
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                Array<D> container;

                // Push first element
                container.push_back(dataset[0]);

                for (size_t datasetAt = 1; datasetAt < datasetSize; datasetAt++) {
                    auto insertAt = randomNumberWithinRange((size_t)0, datasetAt - 1);

                    containerTimeAveraging.benchmarkStart();
                    container.insert(dataset[datasetAt], insertAt);
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

    template <template <typename> typename T, typename D>
    timedata benchmarkSuiteSearch(std::function<void(T<D> &, D)> containerFunc)
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);
            T<D> container;

            // Prepare container for testing
            for (const auto &val : dataset)
                containerFunc(container, val);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                while (!dataset.empty()) {
                    auto value = getRandomValueFromDatasetAndRemove(dataset);
                    containerTimeAveraging.benchmarkStart();
                    // Hack to force GCC to not skip this call during optimization
                    volatile auto tmp = container.contains(value);
                    if (!tmp)
                        throw std::runtime_error("nope");
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

    template <template <typename> typename T, typename D>
    timedata benchmarkSuiteRemove(std::function<void(T<D> &, D)> containerFunc)
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                T<D> container;

                // Prepare container for testing
                for (const auto &val : dataset)
                    containerFunc(container, val);

                while (!dataset.empty()) {
                    auto value = getRandomValueFromDatasetAndRemove(dataset);
                    containerTimeAveraging.benchmarkStart();
                    container.remove(value);
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

    template <template <typename> typename T, typename D>
    timedata benchmarkSuiteRemoveFunc(std::function<void(T<D> &, D)> containerFuncAdd,
                                      std::function<void(T<D> &)> containerFuncRemove)
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                T<D> container;

                // Prepare container for testing
                for (const auto &val : dataset)
                    containerFuncAdd(container, val);

                for (const auto &val : dataset) {
                    containerTimeAveraging.benchmarkStart();
                    containerFuncRemove(container);
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

    template <typename D>
    timedata benchmarkSuiteRemoveList(std::function<void(List<D> &, D)> containerFunc)
    {
        AveragedTimeMeasure containerTimeAveraging;

        for (std::size_t j = 0; j < datasetGenerationCount; j++) {
            auto dataset = generateRandomData<D>(datasetSize);

            for (std::size_t i = 0; i < averagingLoopsCount; i++) {
                List<D> container;

                // Prepare container for testing
                for (const auto &val : dataset)
                    containerFunc(container, val);

                while (!dataset.empty()) {
                    auto value = getRandomValueFromDatasetAndRemove(dataset);
                    // Node lookup time isnt taken into account
                    auto node = container.get_node(value);

                    containerTimeAveraging.benchmarkStart();
                    container.remove(node);
                    containerTimeAveraging.benchmarkStop();
                }
            }
        }
        return containerTimeAveraging.getAvgElapsedNsec();
    }

public:
    void run()
    {
        auto array_push_back_lambda =
            [](Array<datatype> &array, const datatype &val) { array.push_back(val); };
        auto array_push_front_lambda =
            [](Array<datatype> &array, const datatype &val) { array.push_front(val); };

        auto list_push_back_lambda =
            [](List<datatype> &list, const datatype &val) { list.push_back(val); };
        auto list_push_front_lambda =
            [](List<datatype> &list, const datatype &val) { list.push_front(val); };

        auto binheap_add_lambda =
            [](BinHeap<datatype> &binheap, const datatype &val) { binheap.add(val); };

        auto rbtree_add_lambda =
            [](RBTree<datatype> &rbtree, const datatype &val) { rbtree.add(val); };

        auto avltree_add_lambda =
            [](AVLTree<datatype> &avltree, const datatype &val) { avltree.add(val); };

        auto array_pop_back_lambda =
            [](Array<datatype> &array) { array.pop_back(); };
        auto array_pop_front_lambda =
            [](Array<datatype> &array) { array.pop_front(); };

        auto list_pop_back_lambda =
            [](List<datatype> &list) { list.pop_back(); };
        auto list_pop_front_lambda =
            [](List<datatype> &list) { list.pop_front(); };

        for (auto &datasetSizeToTest : datasetSizesToTest) {
            datasetSize = datasetSizeToTest;
            std::cout << "Testing dataset at size: " << datasetSizeToTest << "\n//////////////\n";

            if (datasetSize <= 25'000) {
                // Add
                cout << "Array push_back:  " << benchmarkSuiteAdd<Array, datatype>(array_push_back_lambda) << "ns\n";
                cout << "Array push_front: " << benchmarkSuiteAdd<Array, datatype>(array_push_front_lambda) << "ns\n";

                cout << "List push_back:   " << benchmarkSuiteAdd<List, datatype>(list_push_back_lambda) << "ns\n";
                cout << "List push_front:  " << benchmarkSuiteAdd<List, datatype>(list_push_front_lambda) << "ns\n";

                cout << "BinHeap add:      " << benchmarkSuiteAdd<BinHeap, datatype>(binheap_add_lambda) << "ns\n";

                cout << "Rbtree add:       " << benchmarkSuiteAdd<RBTree, datatype>(rbtree_add_lambda) << "ns\n";

                cout << "Avltree add:      " << benchmarkSuiteAdd<AVLTree, datatype>(avltree_add_lambda) << "ns\n";

                cout << endl;

                // Insert
                cout << "Array insert:     " << benchmarkSuiteArrayInsert<datatype>() << "ns\n";

                cout << "List insert ref:  " << benchmarkSuiteListInsert<datatype>() << "ns\n";

                cout << endl;

                // Contains
                cout << "Array contains:   " << benchmarkSuiteSearch<Array, datatype>(array_push_back_lambda) << "ns\n";

                cout << "List contains:    " << benchmarkSuiteSearch<List, datatype>(list_push_back_lambda) << "ns\n";

                cout << "BinHeap contains: " << benchmarkSuiteSearch<BinHeap, datatype>(binheap_add_lambda) << "ns\n";

                cout << "Rbtree contains:  " << benchmarkSuiteSearch<RBTree, datatype>(rbtree_add_lambda) << "ns\n";

                cout << "Avltree contains: " << benchmarkSuiteSearch<AVLTree, datatype>(avltree_add_lambda) << "ns\n";

                cout << endl;

                // Remove
                cout << "Array remove:     " << benchmarkSuiteRemove<Array, datatype>(array_push_back_lambda) << "ns\n";

                cout << "Array pop_back:   " <<
                    benchmarkSuiteRemoveFunc<Array, datatype>(array_push_back_lambda, array_pop_back_lambda)
                    << "ns\n";

                cout << "Array pop_front:  " <<
                    benchmarkSuiteRemoveFunc<Array, datatype>(array_push_back_lambda, array_pop_front_lambda)
                    << "ns\n";

                cout << "List pop_back:    " <<
                    benchmarkSuiteRemoveFunc<List, datatype>(list_push_back_lambda, list_pop_back_lambda)
                    << "ns\n";

                cout << "List pop_front:   " <<
                    benchmarkSuiteRemoveFunc<List, datatype>(list_push_back_lambda, list_pop_front_lambda)
                    << "ns\n";

                cout << "List remove val:  " << benchmarkSuiteRemove<List, datatype>(list_push_back_lambda) << "ns\n";

                cout << "List remove ref:  " << benchmarkSuiteRemoveList<datatype>(list_push_back_lambda) << "ns\n";

                cout << "BinHeap remove:   " << benchmarkSuiteRemove<BinHeap, datatype>(binheap_add_lambda) << "ns\n";

                cout << "Rbtree remove:    " << benchmarkSuiteRemove<RBTree, datatype>(rbtree_add_lambda) << "ns\n";

                cout << "Avltree remove:   " << benchmarkSuiteRemove<AVLTree, datatype>(avltree_add_lambda) << "ns\n";
            }
            else {
                cout << "Rbtree add:       " << benchmarkSuiteAdd<RBTree, datatype>(rbtree_add_lambda) << "ns\n";

                cout << "Avltree add:      " << benchmarkSuiteAdd<AVLTree, datatype>(avltree_add_lambda) << "ns\n";

                cout << endl;

                cout << "Rbtree contains:  " << benchmarkSuiteSearch<RBTree, datatype>(rbtree_add_lambda) << "ns\n";

                cout << "Avltree contains: " << benchmarkSuiteSearch<AVLTree, datatype>(avltree_add_lambda) << "ns\n";

                cout << endl;

                cout << "Rbtree remove:    " << benchmarkSuiteRemove<RBTree, datatype>(rbtree_add_lambda) << "ns\n";

                cout << "Avltree remove:   " << benchmarkSuiteRemove<AVLTree, datatype>(avltree_add_lambda) << "ns\n";
            }

            cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\n" << endl;
        }
    }
};