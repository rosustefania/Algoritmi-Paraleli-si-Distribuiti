import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;

public class ReduceTask implements Callable<Quartet> {
    public String fileName;
    public ExecutorService tpe;
    public List<Map<Integer, Integer>> dictionary;
    public List<List<String>> maxLengthWords;

    public ReduceTask(String filename, ExecutorService tpe, List<Map<Integer, Integer>> dictionary,
                      List<List<String>> maxLengthWords) {
        this.fileName = filename;
        this.tpe = tpe;
        this.dictionary = dictionary;
        this.maxLengthWords = maxLengthWords;
    }

    /**
     * calculates the nth number in the fibonacci serie
     * @param n given position in the serie
     * @return
     */
    public static int fib(int n) {
        if (n <= 1) {
            return n;
        }
        return fib(n - 1) + fib(n - 2);
    }

    /**
     * calculates fibonacci for every key in the dictonary, gets the
     * maximum length of a word and its occurences for every input file
     * @return task's result after applying the reduce operation
     */
    @Override
    public Quartet call() throws Exception {
        Quartet result;

        double fibbonaciSum = 0;
        int occurencesSum = 0;
        int maxLength = 0;
        int occurences = 0;

        for (Map<Integer, Integer> integerIntegerMap : dictionary) {
            for (Map.Entry<Integer, Integer> entry : integerIntegerMap.entrySet()) {
                // gets all the words that have characters
                if (entry.getKey() != 0) {
                    // calculates sum if the fibonacci values
                    fibbonaciSum += fib(entry.getKey() + 1) * entry.getValue();
                    // calculates the total number of words
                    occurencesSum += entry.getValue();

                    // gets the maximum length of a word and its number of occurences
                    if (entry.getKey() > maxLength) {
                        maxLength = entry.getKey();
                        occurences = entry.getValue();
                    } else if (entry.getKey() == maxLength) {
                        occurences += entry.getValue();
                    }
                }
            }
        }

        result = new Quartet(fileName, Double.parseDouble(String.format("%.2f", fibbonaciSum / occurencesSum)),
                maxLength, occurences);

        return result;
    }
}
