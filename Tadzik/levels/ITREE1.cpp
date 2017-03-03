#include <iostream>
#include <vector>

const int tabStart = 131072;

using namespace std;

int getSum(int left, int right, int tab[]) {
    int sum = tab[left];
    if (left != right)
        sum+=tab[right];
    while (left/2!=right/2) {
        if (left%2==0)
            sum+=tab[left+1];
        if (right%2==1)
            sum+=tab[right-1];
        left/=2; right/=2;
    }
    return sum;
}

void insertToTable(int place, int var, int tab[]) {
    place+=tabStart;
    tab[place]=var;
    while (place!=1) {
        place/=2;
        tab[place]=tab[place*2]+tab[place*2+1];
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    int tab[262144];
    for (int i=0; i<262144; i++) {
        tab[i]=0;
    }
    int n;
    cin >> n;
    for (int i=0; i<n; i++) {
        bool ask;
        int a, b;
        cin >> ask >> a >> b;
        if (ask) {
            insertToTable(b, a, tab);
        }
        else {
            cout << getSum(a+tabStart, b+tabStart, tab) << '\n';
        }
    }

}
