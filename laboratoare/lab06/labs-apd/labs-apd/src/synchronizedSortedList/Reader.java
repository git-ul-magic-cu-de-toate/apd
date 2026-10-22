package synchronizedSortedList;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

public class Reader extends Thread {
    private final String filename;
    private final Semaphore semaphore;
    private final List<Integer> list;

    public Reader(String filename, Semaphore semaphore, List<Integer> list) {
        super();
        this.filename = filename;
        this.semaphore = semaphore;
        this.list = list;
    }

    @Override
    public void run() {
        try {
            Scanner scanner = new Scanner(new File(filename));
            while (scanner.hasNextInt()) {
                list.add(scanner.nextInt());
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        semaphore.release();
    }
}
