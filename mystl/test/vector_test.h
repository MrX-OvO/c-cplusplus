//
// Created by mrxovo on 11/30/22.
//

#ifndef VECTOR_TEST_H
#define VECTOR_TEST_H

#include "../Vector.h"
#include <chrono>
#include <iostream>
#include <vector>
using namespace mystl;

namespace test {
    void test1() {
        Vector<int> vint1;
        Vector<int> vint2;
        Vector<int> vint3;

        std::cout << "===obj.push_back(i[0->4]) 5 times===" << std::endl;
        for (int i = 0; i < 5; ++i) { vint1.push_back(i); }
        std::cout << "vint1\n";
        vint1.show();

        std::cout << "===obj.pop_back() 1 time===" << std::endl;
        vint1.pop_back();
        vint1.show();

        std::cout << "===obj.reserve(10)===" << std::endl;
        vint1.reserve(10);
        vint1.show();

        std::cout << "===obj.resize(17)===" << std::endl;
        vint1.resize(17);
        vint1.show();

        std::cout << "===obj.resize(4)===" << std::endl;
        vint1.resize(4);
        vint1.show();

        std::cout << "===obj.at(1) obj.operator[0]===" << std::endl;
        std::cout << "vint1.at(1) = " << vint1.at(1) << ", vint1[0] = " << vint1[0] << std::endl;

        std::cout << "===obj.front() obj.back()===" << std::endl;
        std::cout << "vint1.front() = " << vint1.front() << ", vint1.back() = " << vint1.back() << std::endl;

        std::cout << "===obj.push_back(i[0->10]) 10 times===" << std::endl;
        for (int i = 0; i < 10; ++i) { vint2.push_back(i); }
        std::cout << "vint2\n";
        vint2.show();

        std::cout << "===obj.swap(other)===" << std::endl;
        vint1.swap(vint2);
        std::cout << "vint1\n";
        vint1.show();
        std::cout << "vint2\n";
        vint2.show();

        std::cout << "===obj.operator=(other)===" << std::endl;
        std::cout << "vint3\n";
        vint3.show();
        vint3 = vint2;
        std::cout << "vint3\n";
        vint3.show();

        std::cout << "===obj.rbegin() obj.rend()===" << std::endl;
        std::cout << "vint3\n";
        for (auto it = vint3.rbegin(); it != vint3.rend();) { std::cout << *(it++) << (it != vint3.rend() ? ", " : ""); }
        std::cout << std::endl;

        std::cout << "===obj.begin() obj.end()===" << std::endl;
        std::cout << "vint3\n";
        for (auto it = vint3.begin(); it != vint3.end();) { std::cout << *(it++) << (it != vint3.end() ? ", " : ""); }
        std::cout << std::endl;

        std::cout << "===obj.find(4) obj.rfind(2)===" << std::endl;
        auto f = vint3.find(4);
        auto rf = vint3.rfind(2);
        std::cout << "vint3.find(4) = " << *f << ", vint3.rfind(2) = " << *rf << std::endl;

        std::cout << "===obj.insert(find(2),5) obj.insert(find(5),10,3)===" << std::endl;
        vint3.insert(vint3.find(2), 5);
        vint3.show();
        vint3.insert(vint3.begin(), 10, 3);
        vint3.show();

        std::cout << "===obj.erase(find(10)) obj.erase(begin()+2,begin()+5)===" << std::endl;
        vint3.erase(vint3.find(10));
        vint3.show();
        vint3.erase(vint3.begin() + 2, vint3.begin() + 5);
        vint3.show();

        std::cout << "===obj.clear()===" << std::endl;
        vint1.clear();
        vint1.show();
    }

    void test2() {
        std::vector<bool> stl_vec;
        Vector<bool> mystl_vec;
        constexpr int max = 10000;

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < max; ++i) { stl_vec.insert(stl_vec.begin(), true); }
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double, std::milli>{end - start};
        auto d1 = duration.count();
        std::cout << "STL vector<bool> took " << d1 << " ms to insert " << max << " times, about " << d1 / max << " ms each time\n";

        start = std::chrono::steady_clock::now();
        for (int i = 0; i < max; ++i) { mystl_vec.insert(mystl_vec.begin(), true); }
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration<double, std::milli>{end - start};
        auto d2 = duration.count();
        std::cout << "mystl Vector<bool> took " << d2 << " ms to insert " << max << " times, about " << d2 / max << " ms each time\n";

        std::cout << "mystl Vector<bool> is almost " << d1 / d2 << " times faster than STL vector<bool>\n";
    }

    void test3() {
        std::vector<int> stl_vec;
        Vector<int> mystl_vec;
        constexpr int max = 10000;

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < max; ++i) { stl_vec.push_back(i); }
        for (int i = 0; i < max; ++i) { stl_vec.erase(stl_vec.begin()); }
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double, std::milli>{end - start};
        auto d1 = duration.count();
        std::cout << "STL vector<int> took " << d1 << " ms to push_back and erase " << max << " times, about " << d1 / max << " ms each time\n";

        start = std::chrono::steady_clock::now();
        for (int i = 0; i < max; ++i) { mystl_vec.push_back(i); }
        for (int i = 0; i < max; ++i) { mystl_vec.erase(mystl_vec.begin()); }
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration<double, std::milli>{end - start};
        auto d2 = duration.count();
        std::cout << "mystl Vector<int> took " << d2 << " ms to push_back and erase " << max << " times, about " << d2 / max << " ms each time\n";

        std::cout << "mystl Vector<int> is almost " << d1 / d2 << " times faster than STL vector<int>\n";
    }
}// namespace test

#endif//VECTOR_TEST_H