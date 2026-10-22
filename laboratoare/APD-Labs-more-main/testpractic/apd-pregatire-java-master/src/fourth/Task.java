package fourth;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class Task extends Thread {
    private final int id;
    private final int[] array;
    private final int size;
    private final int threads;
    private final int[] results;
    private final CyclicBarrier barrier;

    public Task(int id, int[] array, int size, int threads, int[] results, CyclicBarrier barrier) {
        this.id = id;
        this.array = array;
        this.size = size;
        this.threads = threads;
        this.results = results;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        int start = id * (int) Math.ceil((double) size / (double) threads);
        int end = Math.min((id + 1) * (int) Math.ceil((double) size / (double) threads), size);

        for (int i = start; i < end; i++) {
            results[id] += array[i];
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (id == 0) {
            int totalSum = 0;
            for (int result : results) {
                totalSum += result;
            }

            System.out.println("Total sum = " + totalSum);
        }

    }
}
