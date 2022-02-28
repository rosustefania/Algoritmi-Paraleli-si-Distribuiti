import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Tema2 {
    public static int numberOfWorkers;
    public static List<Triplet> mapResults = new ArrayList<>();
    public static List<Quartet> reduceResults = new ArrayList<>();

    public static void main(String[] args) throws IOException {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
        } else {
            numberOfWorkers = Integer.parseInt(args[0]);
            InputFile inputFile = new InputFile(args[1]);
            OutputFile outputFile = new OutputFile(args[2]);

            // reads data from given file
            inputFile.readDataFromFile();

            // apply the map operation
            inputFile.map();

            // apply the reduce operation
            inputFile.reduce();

            // writes the results to the given file
            outputFile.writeData();

        }
    }
}
