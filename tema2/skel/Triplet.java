import java.util.List;
import java.util.Map;

public class Triplet {
    private final String filename;
    private final Map<Integer, Integer> dictionary;
    private final List<String> wordsList;

    public String getFilename() {
        return filename;
    }

    public Map<Integer, Integer> getDictionary() {
        return dictionary;
    }

    public List<String> getWordsList() {
        return wordsList;
    }

    public Triplet(String filename, Map<Integer, Integer> dictionary, List<String> wordsList) {
        this.filename = filename;
        this.dictionary = dictionary;
        this.wordsList = wordsList;
    }
}
