package shortestPathsFloyd_Warshall;

import java.util.concurrent.*;

public class Main {
    public static int NUMBER_OF_THREADS = 2;
    public static CyclicBarrier barrier;
    public static int M = 9;
    public static int[][] graph = {{0, 1, M, M, M},
            {1, 0, 1, M, M},
            {M, 1, 0, 1, 1},
            {M, M, 1, 0, M},
            {M, M, 1, M, 0}};

    public static int[][] graph2 = {{0, 1, M, M, M},
            {1, 0, 1, M, M},
            {M, 1, 0, 1, 1},
            {M, M, 1, 0, M},
            {M, M, 1, M, 0}};

    public static void main(String[] args) {
        // Parallelize me (You might want to keep the original code in order to compare)
     // int cores = Runtime.getRuntime().availableProcessors();
        for (int k = 0; k < 5; k++) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    graph2[i][j] = Math.min(graph2[i][k] + graph2[k][j], graph2[i][j]);
                }
            }
        }

        barrier = new CyclicBarrier(NUMBER_OF_THREADS);
        Thread[] t = new Thread[NUMBER_OF_THREADS];

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            t[i] = new Thread(new Task(i));
            t[i].start();
        }

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        System.out.println("Serial:");
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph2[i][j] + " ");
            }
            System.out.println();
        }

        System.out.println("Parallel:");
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }
    }
}
