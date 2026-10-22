package task1;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    private final ExecutorService tpe;
    private final AtomicInteger counter;
    private final ArrayList<Integer> partialPath;
    private final int destination;

    public MyRunnable(ExecutorService tpe, AtomicInteger counter, ArrayList<Integer> partialPath, int destination) {
        this.tpe = tpe;
        this.counter = counter;
        this.partialPath = partialPath;
        this.destination = destination;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
        }

        // se verifica nodurile pentru a evita ciclarea in graf
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                counter.incrementAndGet();
                tpe.submit(new MyRunnable(tpe, counter, newPartialPath, destination));
            }
        }

        int left = counter.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
