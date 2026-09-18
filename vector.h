#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <stdexcept>
#include <algorithm>
// ==================== FORWARD DECLARATION ====================
template <typename K>
struct SecondaryIndex;

template <typename T>
class MyVector {
private:
    T* arr;
    int capacity;
    int currentSize;
    long long nextId;

    void resize(int newCapacity) {
        capacity = newCapacity;
        T* newArr = new T[capacity];
        for (int i = 0; i < currentSize; i++) {
            newArr[i] = std::move(arr[i]);
        }
        delete[] arr;
        arr = newArr;
    }

    // ✅ Generic sortData (cho T có field .id)
    void quickSortData(int low, int high) {
        if (low >= high) return;
        long long pivot = arr[high].id;
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j].id < pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        int pivotIndex = i + 1;
        quickSortData(low, pivotIndex - 1);
        quickSortData(pivotIndex + 1, high);
    }

    void sortData() {
        if (currentSize <= 1) return;
        quickSortData(0, currentSize - 1); // ✅ Thay thế Bubble Sort O(N^2) cũ
    }

public:
    MyVector() : arr(nullptr), capacity(0), currentSize(0), nextId(1) {}
    ~MyVector() { delete[] arr; }

    MyVector(const MyVector& other) {
        currentSize = other.currentSize;
        capacity = other.capacity;
        nextId = other.nextId;
        if (capacity > 0) {
            arr = new T[capacity];
            for (int i = 0; i < currentSize; i++) arr[i] = other.arr[i];
        } else arr = nullptr;
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] arr;
            currentSize = other.currentSize;
            capacity = other.capacity;
            nextId = other.nextId;
            if (capacity > 0) {
                arr = new T[capacity];
                for (int i = 0; i < currentSize; i++) arr[i] = other.arr[i];
            } else arr = nullptr;
        }
        return *this;
    }

    void insert_ordered(T value) {
        if (value.id <= 0) {
            value.id = nextId++;
        } else if (value.id >= nextId) {
            nextId = value.id + 1;
        }
        if (currentSize == capacity) {
            int newCapacity = (capacity == 0) ? 1 : (capacity > 100000 ? capacity + 50000 : capacity * 2);
            resize(newCapacity);
        }

        int low = 0, high = currentSize;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (arr[mid].id <= value.id) low = mid + 1;
            else high = mid;
        }

        for (int i = currentSize; i > low; i--) {
            arr[i] = arr[i - 1];
        }
        arr[low] = value;
        currentSize++;
    }
    

    void insert_bulk(T* rawArray, int count) {
        if (count <= 0) return;

        if (currentSize + count > capacity) {
            resize(currentSize + count);
        }

        for (int i = 0; i < count; i++) {
            if (rawArray[i].id <= 0) rawArray[i].id = nextId++;
            else if (rawArray[i].id >= nextId) nextId = rawArray[i].id + 1;
            arr[currentSize + i] = rawArray[i];
        }
        currentSize += count;
        sortData();
    }

    void push_back(T value) {
        if (value.id <= 0) {
            value.id = nextId++;
        } else if (value.id >= nextId) {
            nextId = value.id + 1;
        }
        if (currentSize == capacity) {
            int newCapacity = (capacity == 0) ? 1 : capacity * 2;
            resize(newCapacity);
        }
        arr[currentSize++] = value;
    }

    T* find_by_id(long long targetId) {
        int low = 0;
        int high = currentSize - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid].id == targetId) {
                return &arr[mid];
            }
            if (arr[mid].id < targetId) low = mid + 1;
            else high = mid - 1;
        }
        return nullptr;
    }

    bool delete_by_index(int index) {
        if (index < 0 || index >= currentSize) return false;
        for (int i = index; i < currentSize - 1; i++) {
            arr[i] = arr[i + 1];
        }
        currentSize--;
        return true;
    }

    bool delete_by_id(long long targetId) {
        int low = 0;
        int high = currentSize - 1;
        int indexToDel = -1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid].id == targetId) {
                indexToDel = mid;
                break;
            }
            if (arr[mid].id < targetId) low = mid + 1;
            else high = mid - 1;
        }

        if (indexToDel == -1) return false;
        return delete_by_index(indexToDel);
    }

    void pop_back() { if (currentSize > 0) currentSize--; }
    T& operator[](int index) { return arr[index]; }
    const T& operator[](int index) const { return arr[index]; }
    int size() const { return currentSize; }
    void clear() { currentSize = 0; }
    
    long long getNextId() const { return nextId; }
    long long getLastId() const { return (currentSize > 0) ? arr[currentSize - 1].id : 0; }
};


template <typename K>
class MyVector<SecondaryIndex<K>> {
private:
    SecondaryIndex<K>* arr;
    int capacity;
    int currentSize;
    long long nextId;  // ← Giữ cho consistency với generic version

    void resize(int newCapacity) {
        capacity = newCapacity;
        SecondaryIndex<K>* newArr = new SecondaryIndex<K>[capacity];
        for (int i = 0; i < currentSize; i++) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = newArr;
    }

    // ✅ KHÔNG CÓ sortData() - SecondaryIndex không có .id field

public:
    MyVector() : arr(nullptr), capacity(0), currentSize(0), nextId(1) {}
    
    ~MyVector() { 
        delete[] arr; 
    }

    MyVector(const MyVector& other) {
        currentSize = other.currentSize;
        capacity = other.capacity;
        nextId = other.nextId;
        if (capacity > 0) {
            arr = new SecondaryIndex<K>[capacity];
            for (int i = 0; i < currentSize; i++) arr[i] = other.arr[i];
        } else arr = nullptr;
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] arr;
            currentSize = other.currentSize;
            capacity = other.capacity;
            nextId = other.nextId;
            if (capacity > 0) {
                arr = new SecondaryIndex<K>[capacity];
                for (int i = 0; i < currentSize; i++) arr[i] = other.arr[i];
            } else arr = nullptr;
        }
        return *this;
    }

    void insert_ordered(SecondaryIndex<K> value) {
        // ✅ SecondaryIndex không cần auto-ID
        // ✅ Chỉ allocate + thêm vào
        if (currentSize == capacity) {
            int newCapacity = (capacity == 0) ? 1 : capacity * 2;
            resize(newCapacity);
        }
        arr[currentSize++] = value;
    }

    void insert_bulk(SecondaryIndex<K>* rawArray, int count) {
        if (count <= 0) return;

        if (currentSize + count > capacity) {
            resize(currentSize + count);
        }

        for (int i = 0; i < count; i++) {
            arr[currentSize + i] = rawArray[i];
        }
        currentSize += count;
        // ✅ KHÔNG gọi sortData() 
        // ✅ Sorting được làm bởi IndexSorter<K>::sortNumeric() từ database.h
    }

    void push_back(SecondaryIndex<K> value) {
        if (currentSize == capacity) {
            int newCapacity = (capacity == 0) ? 1 : capacity * 2;
            resize(newCapacity);
        }
        arr[currentSize++] = value;
    }

    // ✅ find_by_id không dùng cho SecondaryIndex
    SecondaryIndex<K>* find_by_id(long long targetId) {
        // ❌ SecondaryIndex không có .id field
        // ✅ Không implement - dùng manual loop hoặc IndexSorter thay thế
        return nullptr;
    }

    bool delete_by_index(int index) {
        if (index < 0 || index >= currentSize) return false;
        for (int i = index; i < currentSize - 1; i++) {
            arr[i] = arr[i + 1];
        }
        currentSize--;
        return true;
    }

    bool delete_by_id(long long targetId) {
        // ❌ SecondaryIndex không hỗ trợ xóa theo .id
        // ✅ Dùng delete_by_index() thay thế (với vị trí được tìm từ findIndexPosition)
        return false;
    }

    void pop_back() { if (currentSize > 0) currentSize--; }
    
    SecondaryIndex<K>& operator[](int index) { return arr[index]; }
    
    const SecondaryIndex<K>& operator[](int index) const { return arr[index]; }
    
    int size() const { return currentSize; }
    
    void clear() { currentSize = 0; }
    
    long long getNextId() const { return nextId; }
};

#endif
