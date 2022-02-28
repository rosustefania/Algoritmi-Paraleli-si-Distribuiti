public class Quartet {
    private final String filename;
    private final Double rank;
    private final Integer maxLength;
    private final Integer occurences;

    public Quartet(String filename, Double rank, Integer maxLength, Integer occurences) {
        this.filename = filename;
        this.rank = rank;
        this.maxLength = maxLength;
        this.occurences = occurences;
    }

    public String getFilename() {
        return filename;
    }

    public Double getRank() {
        return rank;
    }

    public Integer getMaxLength() {
        return maxLength;
    }

    public Integer getOccurences() {
        return occurences;
    }
}
