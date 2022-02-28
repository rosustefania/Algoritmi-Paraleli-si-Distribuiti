import java.io.FileWriter;
import java.io.IOException;
import java.util.Comparator;

public class OutputFile {
    public String name;

    public OutputFile(String name) {
        this.name = name;
    }

    /**
     * writes result data to a file
     * @throws IOException
     */
    public void writeData() throws IOException {
        FileWriter fileWriter = new FileWriter(name);

        // sorts the results by the rank
        Tema2.reduceResults.sort(Comparator.comparing(Quartet::getRank).reversed());

        // writes the results
        for (int i = 0; i < Tema2.reduceResults.size(); i++) {
            String[] path = (Tema2.reduceResults.get(i).getFilename()).split("/");
            fileWriter.write(path[path.length - 1] + "," + Tema2.reduceResults.get(i).getRank() +
                    "," + Tema2.reduceResults.get(i).getMaxLength() + "," + Tema2.reduceResults.get(i).getOccurences() +
                    "\n");
        }

        fileWriter.close();
    }
}
