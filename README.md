# Designing_A_Virtual_Memory_Manager

**Overview of Project**
In this project we implemented a virtual memory manager. This problem actually comes from pages 458-461 in the popular Operating Systems Concepts book by Silberschatz 9th edition. Additionally, chapter 9 of that book is provided.

**Part 1** In part 1 we accomplished the project assuming infinite memory space. That is, there is no need for page replacement in Part 1.

**Part 2** In Part 2, we assume that physical memory can now only accommodate 128 frames. Now we will need a page replacement algorithm. We need to implement both FIFO and LRU page replacement algorithms.

**Program Specification**

- The program should accept the following command line parameters"
  - 1st parameter: a file representing the “backing store” - a generic term for storage not in memory.
  - 2nd parameter: a file containing addresses you will access.

- And Part 1 will be run as:
  ```sh
    ./part1 BACKING_STORE.bin addresses.txt
  ```

- For Part 2, the program need to accept an additional parameter:
  - 3rd parameter: page replacement algorithm indication, either `fifo` or `lru`.
- Part 2 will be run as:
  ```sh
  ./part2 BACKING_STORE.bin <address file> <strategy>
  ```
  where `<strategy>` can be `fifo` or `lru`.

  **Output** The program for both parts create a file `correct.txt` consisting of virtual and corresponding physical addresses, and the value at that address. Additionally, at the end of the run the program also output the following:
    - Number of translated addresses
    - Page faults
    - Page fault rate
    - TLB hits
    - TLB hit rate
