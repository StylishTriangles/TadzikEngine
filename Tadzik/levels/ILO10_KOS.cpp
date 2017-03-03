#include <iostream>
#include <set>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    int n, t1, t2;
    cin >> n;
    if (n==1) {
        cin >> t1 >> t2;
        cout << min(t1, t2) << " " << max(t1, t2);
        return 0;
    }
    set <int> nums;
    for (int i=0; i<n; i++) {
        cin >> t1 >> t2;

        set<int>::iterator it = nums.find(t1);
        if (it==nums.end())
            nums.insert(t1);
        else
            nums.erase(it);
    }
    if (nums.size()!=2) {
        cout << "NIE";
    }
    else {
        for (set<int>::iterator a=nums.begin(); a!=nums.end(); a++)
            cout << *a << " ";
    }
}
