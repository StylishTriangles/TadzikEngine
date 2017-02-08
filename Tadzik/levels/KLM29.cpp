#include <vector>
#include <iostream>
#include <queue>
using namespace std;

void setTimes(vector <vector <int> >& tree, vector <pair <int, int> >& times, int on, int& level) {
    times[on].first=level;
    level++;
    for (int i=0; i<tree[on].size(); i++) {
        setTimes(tree, times, tree[on][i], level);
    }
    times[on].second=level;
    level++;
}
int main() {
    ios_base::sync_with_stdio(false);
    int n, m;
    cin >> n >> m;
    vector <vector <int> > tree;
    tree.resize(n);
    bool tab[n];
    for (int i=0; i<n; i++) {
        tab[i]=false;
    }
    for (int i=0; i<m; i++) {
        int t1, t2;
        cin >> t1 >> t2;
        t1--; t2--;
        tree[t1].push_back(t2);
        tab[t2]=true;
    }
    vector <pair <int, int> > times;
    times.resize(n);
    int tmp=0;
    for (int i=0; i<n; i++) {
        if (!tab[i])
            setTimes(tree, times, i, tmp);
    }
    cin >> m;
    for (int i=0; i<m; i++) {
        int t1, t2;
        cin >> t1 >> t2;
        t1--; t2--;
        if (times[t1].first<times[t2].first && times[t1].second>times[t2].second)
            cout << "TAK\n";
        else
            cout << "NIE\n";
    }
}
