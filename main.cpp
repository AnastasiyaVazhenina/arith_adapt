#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <thread>
#include <chrono>

template <class T, class F, class ... Args>
long timePerform(F&& func, Args&&... args)
{
    using std::chrono::high_resolution_clock;
    auto startTime = std::chrono::high_resolution_clock::now();

    func(std::forward<Args>(args)...);

    auto endTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<T>(endTime - startTime).count();
}


template <class T>
struct Less
{
    bool operator()(T a, T b) const
    {
        return a < b;
    }
};

template <class It, class Out, class Compare>
Out merge(It first1, It last1, It first2, It last2, Out out, Compare cmp)
{
    int k = 0;
    while ((first1 != last1) && (first2 != last2))
    {
        if (cmp(*first1, *first2))
        {
            out[k] = *first1;
            ++k;
            ++first1;
        }
        else
        {
            out[k] = *first2;
            ++k;
            ++first2;
        }
    }
    while (first1 < last1)
    {
        out[k] = *first1;
        ++k;
        ++first1;
    }
    while (first2 < last2)
    {
        out[k] = *first2;
        ++k;
        ++first2;
    }
    return out;
}

template <class It, class Out, class Compare>
Out merge_sort(It first, It last, Out out, Compare cmp)
{
    size_t size = last - first;
    if (cmp(1, size))
    {
        size_t middle = size / 2;
        out = merge_sort<It, Out, Compare>(first, first + middle, out, cmp);
        out = merge_sort<It, Out, Compare>(first + middle, last, out, cmp);
        out = merge<It, Out, Compare>(first, first + middle, first + middle, last, out, cmp);
        It curr = first;
        size_t j = 0;
        while (curr != last)
        {
            *curr = out[j];
            ++curr;
            ++j;
        }
    }
    return out;
}

template <class It, class Compare>
void inplace_merge_sort(It first, It last, Compare cmp)
{
    size_t size = last - first;
    if (size < 2)
    {
        return;
    }
    size_t middle = size / 2;
    inplace_merge_sort(first, first + middle, cmp);
    inplace_merge_sort(first + middle, last, cmp);
    std::inplace_merge(first, first + middle, last, cmp);
}

template <class It, class Compare>
void maxHeapify(It first, It last, size_t index, Compare cmp)
{
    size_t size = last - first;
    size_t largest = index;
    size_t left = 2 * index;
    size_t right = 2 * index + 1;
    if (!cmp(size, left))
    {
        if (cmp(*(first + index - 1), *(first + left - 1)))
        {
            largest = left;
        }
    }
    if (!cmp(size, right))
    {
        if (cmp(*(first + largest - 1), *(first + right - 1)))
        {
            largest = right;
        }
    }
    if (largest != index)
    {
        std::iter_swap(first + index - 1, first + largest - 1);
        maxHeapify<It, Compare>(first, last, largest, cmp);
    }
}
template <class It, class Compare>
void heap_sort(It first, It last, Compare cmp)
{
    size_t size = last - first;
    for (size_t i = size / 2; i > 0; --i)
    {
        maxHeapify<It, Compare>(first, last, i, cmp);
    }
    int tmp = 0;
    for (size_t i = size; i > 1; --i)
    {
        std::iter_swap(first, first + i - 1);
        ++tmp;
        It iter = last - tmp;
        maxHeapify<It, Compare>(first, iter, 1, cmp);
    }
}

template <class It, class Compare>
void quick_sort(It first, It last, Compare cmp)
{
    if (last != first)
    {
        It i = first;
        for (It j = first; j != last; ++j)
        {
            if (cmp(*j, *(last - 1)))
            {
                std::swap(*i, *j);
                ++i;
            }
        }
        std::iter_swap(i, last - 1);
        quick_sort(first, i, cmp);
        quick_sort(i + 1, last, cmp);
    }
}

template <class It, class Compare>
void insertion_sort(It first, It last, Compare cmp)
{
    for (It curr = first + 1; curr != last; ++curr)
    {
        bool tmp = false;
        auto temp = *curr;
        It item = curr - 1;
        while (cmp(temp, *item))
        {
            *(item + 1) = *item;
            if (item == first)
            {
                tmp = true;
                break;
            }
            --item;
        }
        if (tmp)
        {
            *item = temp;
        }
        else
        {
            *(item + 1) = temp;
        }
    }
}

int main()
{
    std::vector<int> vec1 = {0, 4, 6, 8, 9};
    std::vector<int> vec2 = {1, 2, 5, 7};
    std::vector<int> out;
    out.resize(vec1.size() + vec2.size());
    Less<int> cmp;
    out = merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), out, cmp);
    std::vector<int> set = {7, 1, 8, 2, 9, 3, 4, 5, 6, 0};
    set = merge_sort(set.begin(), set.end(), set, cmp);
    std::vector<int> vector = {7, 1, 8, 2, 9, 3, 4, 5, 6, 0};
    inplace_merge_sort(vector.begin(), vector.end(), cmp);
    std::vector<int> vec = {7, 1, 8, 2, 9, 3, 4, 5, 6, 0};
    heap_sort(vec.begin(), vec.end(), cmp);
    std::vector<int> set1 = {7, 1, 8, 2, 9, 3, 4, 5, 6, 0};
    quick_sort(set1.begin(), set1.end(), cmp);
    std::vector<int> set2 = {7, 1, 8, 2, 9, 3, 4, 5, 6, 0};
    insertion_sort(set2.begin(), set2.end(), cmp);
    long time1 = 0;
    long time2 = 0;
    long time3 = 0;
    long time4 = 0;
    long time5 = 0;
    long time6 = 0;
    long timer1 = 0;
    long timer2 = 0;
    long timer3 = 0;
    long timer4 = 0;
    long timer5 = 0;
    long timer6 = 0;
    for(int j = 0; j < 30; ++j)
    {
        timer1 = timePerform<std::chrono::microseconds>(merge, vec1.begin(), vec1.end(),
                                                        vec2.begin(), vec2.end(), out, cmp);
        time1 += timer1;
        std::cout << timer1 << ' ';
        timer2 = timePerform<std::chrono::microseconds>(merge_sort, set.begin(), set.end(), set, cmp);
        time2 += timer2;
        std::cout << timer2 << ' ';
        timer3 = timePerform<std::chrono::microseconds>(inplace_merge_sort, vector.begin(), vector.end(), cmp);
        time3 += timer3;
        std::cout << timer3 << ' ';
        timer4 = timePerform<std::chrono::microseconds>(heap_sort, vec.begin(), vec.end(), cmp);
        time4 += timer4;
        std::cout << timer4 << ' ';
        timer5 = timePerform<std::chrono::microseconds>(quick_sort, set1.begin(), set1.end(), cmp);
        time5 += timer5;
        std::cout << timer5 << ' ';
        timer6 = timePerform<std::chrono::microseconds>(insertion_sort, set2.begin(), set2.end(), cmp);
        time6 += timer6;
        std::cout << timer6 << ' ';
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << time1 / 30 << ' ';
    std::cout << time2 / 30 << ' ';
    std::cout << time3 / 30 << ' ';
    std::cout << time4 / 30 << ' ';
    std::cout << time5 / 30 << ' ';
    std::cout << time6/ 30 << ' ';
    std::cout << std::endl << std::endl;

    return 0;
}
