
// a basic container -- DO NOT USE FOR OBJECTS WITH CONSTRUCTORS AND DESTRUCTORS!
template<typename T> class tinyvec {
  private:
    T *v;
    short siz;
    short capacity;
  private:
    void grow() {
        int newcapacity = capacity+1+(capacity>>2);
        if (newcapacity<capacity) throw std::overflow_error("tinyvec size overflow");
        capacity = newcapacity;
        T *newv = new T[capacity];
        for (int i=0; i<siz; i++) newv[i]=v[i];
        delete [] v;
        v = newv;
    }
  public:
    tinyvec() {v=NULL; siz=capacity=0;}
    ~tinyvec() {delete [] v;}
    void operator=(const tinyvec<T>& x) {if (&x!=this) {delete [] v; siz=x.siz; capacity=x.capacity; v=new T[siz]; for (int i=0; i<siz; i++) v[i]=x.v[i];}}
    T *data() {return v;}
    const T *data() const {return v;}
    short size() const {return siz;}
    T& operator[](short k) {return v[k];}
    const T& operator[](short k) const {ASSERT(k>=0 && k<siz); return v[k];}
    void push_back(T x)  {if (siz==capacity) grow(); v[siz++]=x;}
    void remove(short k) {ASSERT(k>=0 && k<siz); v[k]=v[--siz];}
    bool empty() {return siz==0;}
    void clear() {siz=0;}
};


