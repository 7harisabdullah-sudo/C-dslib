A minimalist, double-ended vector in C with power-of-two resizing and zero runtime checks. Designed for mature C programmers who value performance and clarity.

## .h documentation:

1) "enum devectorBufferState" is majorly a return value which tells whether the buffer state is valid or invalid (which can only be a consequence of resize failure(INVALID_BUFFER_STATE)).

2) "struct devector" is the data structure and comprises of 5 struct fields:
- **buffer** holds the memory address of the space where data is written.
- **elem_size** is the sizeof(element) which gets written to buffer
- **capacity** is (as suggested by the name) the capacity of buffer (the unit of capacity is elem_size and (capacity * elem_size) is the actualy size of space (in bytes) occupied by the buffer).
- **head & tail** fields are the insertion points for front and back elements respectively. But there is one important thing to notice that head is the index that is calculated from the end and tail is index calculated from the start of circular buffer.

3) size() returns the current number of elements in the buffer.

4) at() returns the address of the ith index element (we are talking about a conceptual ith index (which is determined by the order in which we performed push and pop operations), not the actual ith index of buffer)

5) push() and pop() functions for front and back are the reason I am writing this great piece of literature and the reason you reading it. I am sure I don't need to explain a thing about them.

6) deploy() is the constructor and destroy() is the destructor. They together serve as the opening and closing of this great ceremony.

## .c documentation:

1) Before diving into .c, I want you to understand the two kinds of data representation in circular buffer (defining: **[E]** to be a sub-array of elements and **[O]** to be a sub-array of 0s):
- **contiguous case:** [O] [front] [E] [back] [O]
- **wrap-around case:** [E] [back] [O] [front] [E]

2) The third case is when the front meets the back. This can also be handled as a wrap-around case.

3) **(front, back) = (capacity - 1 - head, tail)**

4) Now notice that front is not where the last front element was, front is where the next front element would be inserted. Same is the case with back. That's what I meant by "insertion points" in .h documentation (point 2), explaining head and tail fields.

5) level-order of the dependency tree of functions can be shown as: **(resize)<-(advance, retreat)<-(push front & back, pop front and back)**.

6) size() returns **(tail + head) % capacity** because:
- in **wrap-around case**, head indicates the count of elements on the right of [O] and tail is the count of elements on the left of [O], summing upto **head + tail** and this is the case when **head + tail < capacity** (because front > back, so capacity - 1 - head > tail, and so head + tail < capacity - 1, and so head + tail < capacity).
- size is **head + tail - capacity** in the **contiguous case** because size = back - (front + 1) = tail - (capacity - head) = head + tail - capacity there and this is the case when **head + tail >= capacity** (because front < back, so capacity - 1 - head < tail, and so head + tail > capacity - 1, and so head + tail >= capacity).
- we can write those cases combined as **(head + tail) % capacity**.

7) at() is actually quite easier to reason about. the ith conceptual index means the (capacity - head + i) buffer index because (capacity - head) is (front + 1) and in both contiguous and wrap around cases, we can do **(front + 1 + i) % capacity** and that would give us the address of ith conceptual index element. Notice that we are treating front + 1 as a base because front is the point of insertion and so front + 1 is the actual front element.

8) resize() is pretty simple once you understand the 3 cases of data representation.
- If the case is **contiguous**, we have to copy **size** elements from **front + 1 = cpacity - head** where **size = back - (front + 1) = tail - (capacity - head)** elements precisely. We copy this chunk and place it at the start of the tmp buffer. head becomes 0 and tail becomes the size of this chunk.
- If the case is **wrap around**, **head** is the size of the right chunk and **tail** is the size of the left chunk. So, we copy **head** elements from **front + 1** i.e. **capacity - head** index of the current buffer to the **ncap - head** index of tmp buffer, i.e. copying the right chunk, and notice that **head** remains unchanged. And we copy **tail** elements from the base index of the current buffer to tmp buffer's base index, i.e. copying the left chunk, and notice that **tail** also remains unchanged.
- The 3rd case is the **push-resize** case, precisely. And that can be treated exactly the same as **wrap-around** case.

9) All other functions are trivial, once you truly understand the details that are given above.

10) **MIN_CAPACITY** is the minimum capacity that the buffer must have initially (MIN_CAPACITY is 2 because the count of **front and back** is 2). We need it because we are working with **"insert, then expand"** philosophy, and so we must provide memory in advance if we don't check for it before inserting. In addition to this definition, we can say that **MIN_CAPACITY** is the extra space in buffer that we must provide for **front & back** to exist, because they are more importantly serving us as boundaries, and when the boundaries collide, we must expand.

15) **INITIAL_CAPACITY** is library defined rather that being used defined and that is because **INITIAL_CAPACITY** is cleaner being a power of 2 i.e. **INITIAL CAPACITY = 2^k, k >= 1** because that way it grows to be a power of 2 and it shrinks to be a power of 2 and that symmetry is really very comfortable while reasoning about the growth and shrinking system.
