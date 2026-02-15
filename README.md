devector is a double-ended dynamic array implemented as a circular buffer with power-of-two resizing, providing amortized O(1) push and pop at both ends and O(1) indexed access.

## .h documentation:

1) **"enum devectorBufferState"** is majorly a return value which tells whether the **buffer** state is valid or invalid.

2) **"struct devector"** is the data structure and comprises of 5 struct fields:
- **buffer** holds the memory address of the space where the data is written.
- **elem_size** is the sizeof(element) which gets written to the **buffer**.
- **capacity** is (as suggested by the name) the capacity of buffer (the unit of capacity is **elem_size** and **capacity * elem_size** is the actualy size of space (in bytes) occupied by the **buffer**).
- **head & tail** fields are the insertion points for **front and back** elements respectively. But there is one important thing to note that **head** is the index that is calculated from the end of the **buffer** and **tail** is the index that is calculated from the start of the **buffer**.

3) **size()** returns the current number of elements in the **buffer**.

4) **at()** returns the address of the element at ith conceptual-index.

5) **push_back()** inserts at **back** and **push_front()** inserts at **front**. On the other hand, **pop_back()** removes from **back** and **pop_front()** removes from **front**.

6) **deploy()** is the constructor and **destroy()** is the destructor.

## .c documentation:

1) Before diving deep into .c, I want you to understand the three cases of data representation in circular buffer (defining: [E] to be a sub-array of elements and [O] to be a sub-array of 0s and **(front, back) = (capacity - 1 - head, tail)**):

- contiguous case: [O] [front] [E] [back] [O]
- wrap-around case: [E] [back] [O] [front] [E]
- collision case: [E][front, back][E].

2) collision case is actually a sort of wrap-around case, where the [0] sub-array is empty and **front** and **back** are at the same index.

3) Note that **front** is not where the front element is located, **front** is where the next front element would be inserted. Same is the case with **back**.

4) **size()** returns **(tail + head) % capacity** because:
- In wrap-around case, **head** is the count of elements on the right of [O] and **tail** is the count of elements on the left of [O], summing upto **head + tail** and this is the case when **head + tail < capacity** (because **front >= back**, so **capacity - 1 - head >= tail**, and so **head + tail <= capacity - 1**, and so **head + tail < capacity**).
- In contiguous case, size is **head + tail - capacity** because the count of elements is **back - (front + 1) = head + tail - capacity**, and this is the case when **head + tail >= capacity** (because **front < back** so **capacity - 1 - head < tail** and so **head + tail > capacity - 1** and so **head + tail >= capacity**).
- Note that **0 <= head < capacity** and **0 <= tail < capacity** and so **0 <= head + tail < 2 * capacity**.
- So, the size is **head + tail** if **head + tail < capacity**, and size is **head + tail - capacity** if **head + tail >= capacity**, and **head + tail < 2 * capacity**. We can write those cases combined as **(head + tail) % capacity**.

5) **at()** is actually quite easier to reason about. The ith conceptual-index means the **(capacity - head + i) % capacity** buffer-index because **capacity - head = front + 1** and in both contiguous and wrap around cases, **(front + 1 + i) % capacity** gives us the address of the element at ith conceptual-index. Note that **front** is the point of insertion and so **(front + 1) % capacity** is the actual buffer-index where the front element is located.

6) **resize()** is pretty simple once you understand the 3 cases of data representation.
- If the case is contiguous, we have to copy **back - (front + 1) = tail - (capacity - head) = head + tail - capacity** number of elements from **front + 1 = capacity - head** index of the current buffer to the start of the tmp buffer. **head** becomes 0 and **tail** becomes the size of this chunk.
- If the case is wrap around, then **head** is the size of the right chunk and **tail** is the size of the left chunk. So, we copy **head** elements from **front + 1 = capacity - head** index of the current buffer to the **ncap - head** index of tmp, i.e. copying the right chunk, and notice that **head** remains unchanged. And we copy **tail** elements from the base index of the current buffer to the base index of tmp buffer, i.e. copying the left chunk, and notice that **tail** also remains unchanged.
- The 3rd case is the collision case, which can be treated exactly the same as a wrap-around case.

7) **MIN_CAPACITY** is the minimum capacity that the buffer must have initially (MIN_CAPACITY is 2 because the count of **front and back** is 2). We need it because we are working with **"insert, then expand"** philosophy, and so we must provide memory in advance if we don't check for it before inserting. In addition to this definition, we can say that **MIN_CAPACITY** is the extra space in buffer that we must provide for **front & back** to exist, because they are more importantly serving us as boundaries, which are crucial to define define the 3 cases of data representation.

8) **INITIAL_CAPACITY** is library defined rather that being user defined and that is because **INITIAL_CAPACITY** is cleaner being a power of 2 i.e. **INITIAL CAPACITY = 2^k, k >= 1** because that way it grows to be a power of 2 and it shrinks to be a power of 2 and that symmetry is really very comfortable while reasoning about the growth and shrinking system.
