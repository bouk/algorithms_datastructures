#include <cstdio>
#include <algorithm>
#include <vector>

using namespace std;

struct point
{
    int x;
    int y;

    bool operator<(const point &a) const
    {
        if(x == a.x)
        {
            return y < a.y;
        }
        return x < a.x;
    }
};

typedef vector<point> vp;
typedef vector<int> vi;

bool is_left(point &a, point &b, point &c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > 0;
}

vi convex_hull(vp &points)
{
    int current = distance(points.begin(), min_element(points.begin(), points.end()));
    vi hull;
    do
    {
        hull.push_back(current);
        int endpoint = 0;
        for(int i = 1; i < (int)points.size(); i++)
        {
            if(endpoint == current || is_left(points[current], points[i], points[endpoint]))
            {
                endpoint = i;
            }
        }
        current = endpoint;
    }
    while(current != hull[0]);
    return hull;
}

int main()
{
    int n;
    scanf("%d", &n);
    vp points(n);
    for(int i = 0; i < n; i++)
    {
        scanf("%d %d", &points[i].x, &points[i].y);
    }
    vi hull = convex_hull(points);
    for(int i = 0; i < (int)hull.size(); i++)
    {
        printf("%d %d\n", points[hull[i]].x, points[hull[i]].y);
    }
    return 0;
}
