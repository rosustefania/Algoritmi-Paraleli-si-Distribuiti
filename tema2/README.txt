# Rosu Adriana-Stefania 335CC
# Homework No.2 Parallel and Distributed Algorithms
# Document processing using Map-Reduce

## Implementation
    ### Tema2 class:
        - main class that reads the given arguments, reads data from
        the given files, applies the map operation and stores its results
        in a list, then applies the reduce operation and stores its
        results in a list in order to write them in the output file;

    ### InputFile class:
        - handles the operations applied to the given documents: map
        and reduce;
        - readDataFromFile() method reads the fragments' dimension and
        the name of the given documents;
        - map() method does the map stage using Executor Service
        and Future:
            - reads the documents by chunks of given size;
            - creates a future task for every chunk and adds it to the
            tasks' pool using Executor Service and save the future result
            in a list;
            - then goes through this list of future results and gets the
            results and adds them to the coordinator's list;
            - closes the Executor Service after all running threads finish
            their job;
        - reduce() method does the reduce stage using Executor Service
        and Future similar to the map stage:
            - concatenates all the dictionaries resulted after the map stage
            and gets the maximum length words for each file and creates tasks
            with them;
            - submits the tasks to the executor and saves the future results
            in a list;
            - then goes through that list, gets the results and adds them to
            the coordinator's list;
            - closes the Executor Service after all running threads finish
            their job;

    ### Triplet class and Quartet class
        - implement tuples;
        - Triplet represents an object that has three fields in order to
        store the map stage results;
        - Quartet represents an object that has four fields in order to
        store the reduce stage results;

    ### MapTask class
        - represents a task for the map stage;
        - implements Callable<Triplet> interface that stands for a task that
         returns a result after some computations; every time a task starts,
         the call() method is executed; Triplet holds the result type of
         computation returned by call();
        - isLetter(char c) method verifies if a given character is a letter
        or not;
        - call() method:
            - adjusts the fragments: if the fragment starts in the middle
            of a word ignores that word, else if the fragment ends in the
            middle of a word gets that entire word;
            - splits fragments into words and gets every word;s length and
            its number of occurrences;
            - gets the words with the maximum length;
            - stores the result in a Triplet object;

    ### ReduceTask class
        - represents a task for the reduce stage;
        - implements Callable<Quartet> interface that stands for a task that
         returns a result after some computations; every time a task starts,
         the call() method is executed; Quartet holds the result type of
         computation returned by call();
        - fib(int n) method calculates the nth number in the fibonacci series;
        - call() method:
            - calculates fibonacci for every key in the dictionary;
            -  gets the maximum length of a word and its number of occurrences
            for every input file;
            - stores the result in a Quartet object;

    ### OutputFile class
        - writeData() writes the results to a given file after sorting them
        by rank;



