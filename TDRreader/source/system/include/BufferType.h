#ifndef BUFFERTYPE_H
#define BUFFERTYPE_H

//! Class to store a general buffer.
template <typename T>
class Buffer {
protected:
	//! Initilizer
	Buffer(int sz	/*!< The size of the buffer in number of elements	*/
		  , T *buf	/*!< The buffer data.								*/)
		: size( sz ), buffer( buf ) { }


    void SetBuffer(T *buffr)
    { buffer = buffr; }

public:
	//! Virtual no-op destructor.
	virtual ~Buffer() { }

	//! Get data from the buffer.
	/*! \return the dataword from the buffer.
	 */
	T GetData(int idx) const
		{ return buffer[idx]; }

	//! Overloaded [] operator.
	/*! \return GetData(idx)
	 */
	T operator[](int idx) const
		{ return GetData(idx); }

	//! Give access to the data.
	/*! Used by classes to fetch the data.
	 */
	T* GetBuffer()
		{ return buffer; }

	//! Get the buffer size.
	/*! \return the number of elements in the buffer.
	 */
    int GetSize() const
		{ return size; }

	//! Create a new buffer of the same type.
    virtual Buffer* New() { return nullptr; }

protected:
	//! Size of the buffer in number of elements.
    int size;

	//! The buffer data.
    T *buffer;

};

#endif // BUFFERTYPE_H
