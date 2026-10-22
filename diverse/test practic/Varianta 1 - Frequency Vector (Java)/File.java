import java.util.concurrent.*;

class SearchTask implements Callable<int[]> {
    private int[] array;
    private int start, end, target;

    public SearchTask(int[] array, int start, int end, int target) {
        this.array = array;
        this.start = start;
        this.end = end;
        this.target = target;
    }

    @Override
    public int[] call() {
        int count = 0, sum = 0;
        for (int i = start; i < end; i++) {
            if (array[i] == target) {
                count++;
                sum += i;
            }
        }
        return new int[]{count, sum};
    }
}

public class File {
    public static void main(String[] args) throws InterruptedException, ExecutionException {
        int N = Integer.parseInt(args[0]);
        int P = Integer.parseInt(args[1]);
        int X = Integer.parseInt(args[2]);
        
        int[] array = new int[N];
        for (int i = 0; i < N; i++) {
            array[i] = (i + 3) % 5;
        }
        
        ExecutorService executor = Executors.newFixedThreadPool(P);
        Future<int[]>[] results = new Future[P];
        int chunkSize = (N + P - 1) / P;
        
        for (int i = 0; i < P; i++) {
            int start = i * chunkSize;
            int end = Math.min(start + chunkSize, N);
            results[i] = executor.submit(new SearchTask(array, start, end, X));
        }
        
        executor.shutdown();
        executor.awaitTermination(Long.MAX_VALUE, TimeUnit.MILLISECONDS);
        
        int totalCount = 0, totalSum = 0;
        for (int i = 0; i < P; i++) {
            int[] result = results[i].get();
            totalCount += result[0];
            totalSum += result[1];
        }
        
        System.out.println(totalCount + " " + totalSum);
    }
}
