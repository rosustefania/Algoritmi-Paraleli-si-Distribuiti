import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class InputFile {
    public String name;
    public static int fragmentDimension;
    public int numberOfFiles;
    public BufferedReader reader;
    public String[] files;

    public InputFile(String name) throws FileNotFoundException {
        this.name = name;
        this.reader = new BufferedReader(new FileReader(name));
    }

    /**
     * reads data from the input file
     * @throws IOException
     */
    public void readDataFromFile() throws IOException {
        fragmentDimension = Integer.parseInt(reader.readLine());
        numberOfFiles = Integer.parseInt(reader.readLine());

        files = new String[numberOfFiles];
        for (int i = 0; i < numberOfFiles; i++) {
            files[i] = reader.readLine();
        }
        reader.close();
    }

    /**
     * does the map stage using Executor Service and Future
     * @throws IOException
     */
    public void map() throws IOException {
        ExecutorService tpe = Executors.newFixedThreadPool(Tema2.numberOfWorkers);
        List<Future<Triplet>> futuresList = new ArrayList<>();

        char[] buffer = new char[fragmentDimension];

        for (int i = 0; i < numberOfFiles; i++) {
            // reads files by chuncks of given bytes
            int offset = 0;
            BufferedReader fileReader = new BufferedReader(new FileReader(files[i]));
            int r = fileReader.read(buffer);

            while (r != -1) {
                // creates a task for every chunck of data that is submitted to the executor
                Future<Triplet> future = tpe.submit(new MapTask(files[i], tpe, offset, r));
                // saves the future results
                futuresList.add(future);

                // clears buffer, increase offset and reads the next chunck
                buffer = new char[fragmentDimension];
                r = fileReader.read(buffer);
                offset += fragmentDimension;
            }

            // closes files
            fileReader.close();
        }

        for (Future<Triplet> future : futuresList) {
            Triplet result = null;

            try {
                // gets the result
                result = future.get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }

            // adds result to the coordinator's list
            Tema2.mapResults.add(result);
        }

        // waits for all running threads to finish
        tpe.shutdown();
    }

    /**
     * does the reduce stage using Executor Service and Future
     */
    public void reduce() {
        ExecutorService tpe = Executors.newFixedThreadPool(Tema2.numberOfWorkers);
        List<Future<Quartet>> futuresList = new ArrayList<>();

        // creates new tasks for the reduce stage by the map stage results
        for (int i = 0; i < numberOfFiles; i++) {
            List<Map<Integer, Integer>> fileDictionary = new ArrayList<>();
            List<List<String>> fileMaxWords = new ArrayList<>();

            // gets the dictonary and the maximum length words for every give file
            for (int j = 0; j < Tema2.mapResults.size(); j++) {
                if (Objects.equals(Tema2.mapResults.get(j).getFilename(), files[i])) {
                    fileDictionary.add(Tema2.mapResults.get(j).getDictionary());
                    fileMaxWords.add(Tema2.mapResults.get(j).getWordsList());
                }
            }

            // creates task and submitts it to the executor
            Future<Quartet> future =
                    tpe.submit(new ReduceTask(files[i], tpe, fileDictionary, fileMaxWords));
            // saves the future result
            futuresList.add(future);
        }

        for (Future<Quartet> future : futuresList) {
            Quartet result = null;

            try {
                // gets the result
                result = future.get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }

            // adds result to the coordinator's list
            Tema2.reduceResults.add(result);
        }

        // waits for all running threads to finish
        tpe.shutdown();
    }

}
