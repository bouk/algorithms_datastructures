#include <vector>
#include <iostream>
#include <cmath>

using namespace std;
typedef vector<int> vi;

void st_build(vi &segtree, vi &arr, int pos, int from, int to)
{
    if(from == to)
    {
        segtree[pos] = from;
        return;
    }
    int node_left = pos * 2;
    int node_right = node_left + 1;
    st_build(segtree, arr, node_left, from, (from + to) / 2);
    st_build(segtree, arr, node_right, (from + to) / 2 + 1, to);

    int left_pos = segtree[node_left];
    int right_pos = segtree[node_right];
    int left_value = arr[left_pos];
    int right_value = arr[right_pos];
    segtree[pos] = left_value <= right_value ? left_pos : right_pos;
}

void st_create(vi &segtree, vi &arr)
{
    int size = (int)2*pow(2.0, floor(log((double)arr.size()) / log(2.0)) + 1);
    segtree.resize(size);
    st_build(segtree, arr, 1, 0, (int)arr.size() - 1);
}

int st_rmq(vi &segtree, vi &arr, int pos, int from, int to, int left, int right)
{
    if(left > to || right < from)
    {
        return -1;
    }
    if(left >= from && right <= to)
    {
        return segtree[pos];
    }

    int left_minimum = st_rmq(segtree, arr, pos * 2, from, to, left, (left + right) / 2);
    int right_minimum = st_rmq(segtree, arr, pos * 2 + 1, from, to, (left + right) / 2 + 1, right);
    if(left_minimum == -1)
    {
        return right_minimum;
    }
    if(right_minimum == -1)
    {
        return left_minimum;
    }
    return arr[left_minimum] <= arr[right_minimum] ? left_minimum : right_minimum;
}

int st_rmq(vi &segtree, vi &arr, int from, int to)
{
    return st_rmq(segtree, arr, 1, from, to, 0, (int)arr.size() - 1);
}

int main()
{
    vi arr = {1, 3, 4, 5, 1, 100, 10, 0};
    vi segtree;
    st_create(segtree, arr);
    int best_pos = st_rmq(segtree, arr, 0, arr.size() - 1);
    cout << best_pos << ":" << arr[best_pos] << endl;
    return 0;
}
