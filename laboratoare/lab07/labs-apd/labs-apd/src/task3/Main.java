package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    public static int N = 4;

    public static void main(String[] args) {
        ExecutorService tpe = Executors.newFixedThreadPool(8);
        int[] graph = new int[N];
        tpe.submit(new MyRunnable(tpe, graph, 0));
    }
}
