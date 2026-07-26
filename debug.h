#ifndef DEBUG_H
#define DEBUG_H

#include <bits/stdc++.h>
using namespace std;

// ===== base types =====

void __print(int x){cerr<<x;}
void __print(long x){cerr<<x;}
void __print(long long x){cerr<<x;}
void __print(unsigned x){cerr<<x;}
void __print(unsigned long x){cerr<<x;}
void __print(unsigned long long x){cerr<<x;}
void __print(float x){cerr<<x;}
void __print(double x){cerr<<x;}
void __print(long double x){cerr<<x;}
void __print(char x){cerr<<'\''<<x<<'\'';}
void __print(const char *x){cerr<<'\"'<<x<<'\"';}
void __print(const string &x){cerr<<'\"'<<x<<'\"';}
void __print(bool x){cerr<<(x?"true":"false");}

// ===== pair =====

template<typename T,typename V>
void __print(const pair<T,V>&x){
    cerr<<"{";
    __print(x.first);
    cerr<<",";
    __print(x.second);
    cerr<<"}";
}

// ===== 1D C array =====

template<typename T,size_t N>
void __print(const T (&a)[N]){
    cerr<<"{";
    for(size_t i=0;i<N;i++){
        if(i) cerr<<", ";
        __print(a[i]);
    }
    cerr<<"}";
}

// ===== 2D C array =====

template<typename T,size_t N,size_t M>
void __print(const T (&a)[N][M]){
    cerr<<"\n[\n";
    for(size_t i=0;i<N;i++){
        cerr<<"  ";
        for(size_t j=0;j<M;j++){
            cerr<<setw(4);
            __print(a[i][j]);
        }
        cerr<<"\n";
    }
    cerr<<"]";
}

// ===== vector =====

template<typename T>
void __print(const vector<T>&v){
    cerr<<"{";
    for(size_t i=0;i<v.size();i++){
        if(i) cerr<<", ";
        __print(v[i]);
    }
    cerr<<"}";
}

// ===== set =====

template<typename T>
void __print(const set<T>&v){
    cerr<<"{";
    bool f=0;
    for(auto &x:v){
        if(f) cerr<<", ";
        __print(x);
        f=1;
    }
    cerr<<"}";
}

// ===== multiset =====

template<typename T>
void __print(const multiset<T>&v){
    cerr<<"{";
    bool f=0;
    for(auto &x:v){
        if(f) cerr<<", ";
        __print(x);
        f=1;
    }
    cerr<<"}";
}

// ===== map =====

template<typename T,typename V>
void __print(const map<T,V>&m){
    cerr<<"{";
    bool f=0;
    for(auto &x:m){
        if(f) cerr<<", ";
        __print(x);
        f=1;
    }
    cerr<<"}";
}

// ===== unordered_map =====

template<typename T,typename V>
void __print(const unordered_map<T,V>&m){
    cerr<<"{";
    bool f=0;
    for(auto &x:m){
        if(f) cerr<<", ";
        __print(x);
        f=1;
    }
    cerr<<"}";
}

// ===== deque =====

template<typename T>
void __print(const deque<T>&dq){
    cerr<<"{";
    bool f=0;
    for(auto &x:dq){
        if(f) cerr<<", ";
        __print(x);
        f=1;
    }
    cerr<<"}";
}

// ===== stack =====

template<typename T>
void __print(stack<T> st){
    cerr<<"stack{";
    bool f=0;
    while(!st.empty()){
        if(f) cerr<<", ";
        __print(st.top());
        st.pop();
        f=1;
    }
    cerr<<"}";
}

// ===== queue =====

template<typename T>
void __print(queue<T> q){
    cerr<<"queue{";
    bool f=0;
    while(!q.empty()){
        if(f) cerr<<", ";
        __print(q.front());
        q.pop();
        f=1;
    }
    cerr<<"}";
}

// ===== priority_queue =====

template<typename T>
void __print(priority_queue<T> pq){
    cerr<<"pq{";
    bool f=0;
    while(!pq.empty()){
        if(f) cerr<<", ";
        __print(pq.top());
        pq.pop();
        f=1;
    }
    cerr<<"}";
}

// ===== print wrapper =====

void _print(){cerr<<"]\n";}

template<typename T,typename... V>
void _print(T t,V... v){
    __print(t);
    if(sizeof...(v)) cerr<<", ";
    _print(v...);
}

// ===== debug macro =====

#ifndef ONLINE_JUDGE
#define debug(x...) cerr<<"["<<#x<<"] = ["; _print(x)
#else
#define debug(x...)
#endif

#endif