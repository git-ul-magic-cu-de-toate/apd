package shortestPathsFloyd_Warshall;

import java.util.concurrent.BrokenBarrierException;

import static shortestPathsFloyd_Warshall.Main.graph;

public class Task implements Runnable {

    private final int start;
    private final int end;

    public Task(int id) {
        this.start = (int)(id * (double) graph.length / Main.NUMBER_OF_THREADS);
        this.end = Math.min((int)((id + 1) * (double) graph.length / Main.NUMBER_OF_THREADS), graph.length);
    }

    public void run() {
        for (int k = 0; k < graph.length; ++k) {
            for (int i = start; i < end; ++i) {
                for (int j = 0; j < graph.length; ++j) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
            try {
                Main.barrier.await();
            } catch (BrokenBarrierException | InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
