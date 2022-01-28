#ifndef DVECTOR_H
#define DVECTOR_H

/*
 TODO:
 _reserve()
 _insert()
*/

namespace std {
    // Minimal class to replace std::vector
    template<typename Data>
    class vector {
        size_t d_size;      // Stores no. of actually stored objects
        size_t d_capacity;  // Stores allocated capacity
        Data *d_data;       // Stores data

        public:
            //! Default constructor
            vector() { //: d_size(0), d_capacity(0), d_data(0) {};
                d_size=0;
                d_capacity=0;
                d_data=nullptr;
            }

            //! Copy constuctor
            vector(vector const &other) : d_size(other.d_size), d_capacity(other.d_capacity), d_data(0) {
                d_data=(Data *) malloc(d_capacity * sizeof(Data));
                memcpy(d_data,other.d_data, d_size * sizeof(Data));
            };

            //! Destructor
            ~vector() {
                free(d_data);
            };

            //! Assignment operator
            vector &operator=(vector const &other) {
            free(d_data);
                d_size=other.d_size;
                d_capacity=other.d_capacity;
                d_data=(Data *)malloc(d_capacity * sizeof(Data));
                memcpy(d_data,other.d_data,d_size * sizeof(Data));
                return (*this);
            };

            //! Adds new value. If needed, allocates more space
            void push_back(Data const &x) {
                if (d_capacity <= d_size) {
                    resize(d_size+1);
                }
                d_data[d_size-1]=x;
            };

            //! same as push_back() (need for stl compatibility)
            void emplace_back(Data const &x) {
                push_back(x);
            }

            //! Replaces the content of the container
            void assign(size_t count, const Data& value) {
                if (count > d_size) {
                    resize(count);
                }
                for (size_t ix=0; ix < d_size; ix++) {
                    d_data[ix]=value;
                }
            }

            //! @return size of vector
            size_t size() const {
                return d_size;
            };

            //! Const getter
            Data const &operator[](size_t idx) const {
                return d_data[idx];
            };

            //! std::vector getter
            Data at(size_t idx) {
                return d_data[idx];
            };

            //! Changeable getter
            Data &operator[](size_t idx) {
                return d_data[idx];
            };

            //! Resize vector to new_size (d_size is also updated)
            void resize(size_t new_size) {
                if (new_size == d_size) {
                    return;
                }
                Data *t_data;
                t_data=(Data *) realloc(d_data,new_size * sizeof(Data));
                d_capacity=new_size;
                d_size=new_size;
                d_data=t_data;
            };

            //! Resize vector to new_size and fill new spaces with val (d_size is also updated)
            void resize(size_t new_size, const Data& value) {
                resize(new_size);
                // fill the rest
                for (size_t ix=d_size; ix < d_capacity; ix++) {
                    d_data[ix]=value;
                }
            };

            /** Increase the capacity of the vector to a value that's greater or equal to new_cap. If new_cap is greater than the current capacity(), new storage is allocated, otherwise the function does nothing.
            * reserve() does not change the size of the vector.
            **/
            void reserve(size_t new_cap) {
                if (new_cap <= d_capacity) {
                    return;
                }
                Data *t_data;
                t_data=(Data *) realloc(d_data,new_cap * sizeof(Data));
                d_capacity=new_cap;
                d_data=t_data;
            }

            //! Requests the removal of unused capacity (for compatibility)
            void shrink_to_fit(void) {
                if (d_capacity > d_size) {
                    resize(d_size);
                }
            }

            //! Returns pointer to the underlying array serving as element storage.
            Data* data(void) {
                return(d_data);
            }
    };
}

#endif
