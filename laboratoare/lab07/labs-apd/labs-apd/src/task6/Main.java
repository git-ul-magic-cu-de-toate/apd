package task6;

import java.util.concurrent.ForkJoinPool;

public class Main {
    public static int N = 4;

    public static void main(String[] args) {
        int[] graph = new int[N];
        ForkJoinPool tpe = new ForkJoinPool(8);
        Task task = new Task(graph, 0);
        tpe.invoke(task);
        tpe.shutdown();
    }
}
