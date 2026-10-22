package synchronizedSortedList;

import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;

public class Sort extends Thread {
    private final Semaphore semaphore;
    private final List<Integer> list;

    public Sort(Semaphore semaphore, List<Integer> list) {
        super();
        this.semaphore = semaphore;
        this.list = list;
    }

    @Override
    public void run() {
        try {
            semaphore.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        Collections.sort(list);
    }
}
