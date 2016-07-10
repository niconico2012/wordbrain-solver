# wordbrain-solver
A multithreaded C++ implementation to solve Wordbrain puzzles

Compile single-threaded version using:

  ```bash
  $ g++ -Ofast -flto -march=native -std=c++11 wordbrain.cpp -o wordbrain -pthread
  ```

Compile multi-threaded version using:

  ```bash
  $ g++ -Ofast -flto -march=native -std=c++11 wordbrain_multi.cpp -o wordbrain_multi.cpp -pthread
  ```
  
Options:

  `--dict DICT_PATH` - where `DICT_PATH` is the location of a dictionary. By default `./dict_full.txt` will be used.
  
  `--noquery` - specifies to not output query strings. Would be helpful if piping input to the program.
  
  `--threads NUM_THREADS` - where `NUM_THREADS` is the number of threads to utilize (capped at the machine's capability)
  
Example usage:

  ```bash
  $ ./wordbrain_multi --threads 16 --dict dict_med.txt
  ```
  
  ```bash
  $ echo "4 3 5 6 5 abcdefghijklhmnop" | ./wordbrain_multi --threads 16 --dict dict_med.txt --noquery
  ```
  
Example program interaction:

  ```bash
  $ echo "5 4 4 7 7 7 vanmoipveotoarrtsmedmiipb" | ./wm --noquery --threads 4 --dict dict_med.txt
  Using 4 threads.
  Reading dictionary from dict_med.txt ... Done.
  Analyzing grid... Done.
  -- Possible Combinations:
  post  bedroom  vampire  vitamin  
  pots  bedroom  vampire  vitamin  
  stop  bedroom  vampire  vitamin  
  opts  bedroom  vampire  vitamin
  ```
  
The same interaction without the shortcut method:

  ```
  $ ./wordbrain_multi --threads 4 --dict dict_med.txt
  Using 4 threads.
  Reading dictionary from dict_med.txt ... Done.
  Grid dimension: 5
  Number of words: 4
  Enter the length of each word (space separated, in order): 4 7 7 7
  Enter the characters in the grid, row-major: vanmoipveotoarrtsmedmiipb
  Analyzing grid... Done.
  -- Possible Combinations:
  post  bedroom  vampire  vitamin  
  pots  bedroom  vampire  vitamin  
  stop  bedroom  vampire  vitamin  
  opts  bedroom  vampire  vitamin
  ```
  
  Both of these solve a 5x5 grid with 4 words, with lengths 4,7,7,7 (in the order presented) with the grid containing characters in the following fashion:
  
  ```
  v a n m o 
  i p v e o 
  t o a r r
  t s m e d 
  m i i p b
  ```

If you find that there are incorrect solutions, too many solutions, or too few solutions, try using a different dictionary. There are several included.
