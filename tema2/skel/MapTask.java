import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;

public class MapTask implements Callable<Triplet> {
    public String fileName;
    public ExecutorService tpe;
    public int offset;
    public int fragmentDimension;
    public Map<Integer, Integer> dictionary;
    List<String> maxLengthWords;

    public MapTask(String filename, ExecutorService tpe, int offset, int fragmentDimension) {
        this.fileName = filename;
        this.tpe = tpe;
        this.offset = offset;
        this.fragmentDimension = fragmentDimension;
    }


    /**
     * verifies if a given character is a letter
     * @param c given character
     * @return true if it's a character
     */
    boolean isLetter(char c) {
        String string = String.valueOf(c);
        return string.matches("[A-Za-z0-9]");
    }


    /**
     * adjusts the fragments, splits them by words, gets every word's length
     * and its occurrences, gets the words with the maximum length
     * @return task's result after applying the map operation
     */
    @Override
    @SuppressWarnings("ResultOfMethodCallIgnored")
    public Triplet call() throws IOException {
        Triplet result;

        char[] previousBuffer = new char[InputFile.fragmentDimension];
        int previousDimension = 0;
        char[] currentBuffer = new char[InputFile.fragmentDimension];
        char[] nextBuffer = new char[InputFile.fragmentDimension];

        BufferedReader fileReader = new BufferedReader(new FileReader(fileName));

        // reads the previous fragment if the current one it's not the first one in a file
        if (offset >= InputFile.fragmentDimension) {
            fileReader.skip(offset - InputFile.fragmentDimension);
            previousDimension = fileReader.read(previousBuffer);
        } else {
            fileReader.skip(offset);
        }

        // reads the current fragment
        fileReader.read(currentBuffer);
        // reads the next fragment
        fileReader.read(nextBuffer);

        StringBuilder fragment = new StringBuilder();

        // if the current fragment starts in the middle of a word, ignores that word
        if (previousDimension > 0 && isLetter(previousBuffer[previousDimension - 1])) {
            String[] words = String.valueOf(currentBuffer).split("\\W+", 2);
            if (words.length > 1) {
                fragment = new StringBuilder(words[1]);
            }
        } else {
            fragment = new StringBuilder(String.valueOf(currentBuffer));
        }

        // if the current fragment ends in the middle of a word, gets the entire word
        if (isLetter(currentBuffer[fragmentDimension - 1])) {
            if (isLetter(nextBuffer[0])) {
                String[] words = String.valueOf(nextBuffer).split("\\W+");
                if (words.length > 0) {
                    fragment.append(words[0]);
                }
            }
        }

        // gets all the words in the current fragment after adjusting it
        String[] fragmentWords = String.valueOf(fragment).split("\\W+");
        maxLengthWords = new ArrayList<>();
        dictionary = new HashMap<>();

        // gets the dictonary and the maximum legth words list
        if (fragmentWords.length == 1 && fragmentWords[0].length() != 0) {
            dictionary.put(fragmentWords[0].length(), 1);
            maxLengthWords.add(fragmentWords[0]);
        } else if (fragmentWords.length > 1) {
            int maxLength = 0;
            for (String fragmentWord : fragmentWords) {
                if (dictionary.containsKey(fragmentWord.length())) {
                    dictionary.put(fragmentWord.length(), dictionary.get(fragmentWord.length()) + 1);
                } else {
                    dictionary.put(fragmentWord.length(), 1);
                }

                if (fragmentWord.length() > maxLength) {
                    maxLength = fragmentWord.length();
                    maxLengthWords.clear();
                    maxLengthWords.add(fragmentWord);
                } else if (fragmentWord.length() == maxLength) {
                    maxLengthWords.add(fragmentWord);
                }
            }
        }

        result = new Triplet(fileName, dictionary, maxLengthWords);
        return result;
    }
}
