package second;

public class Task extends Thread {
    private final int id;
    private final int[] array;
    private final int size;
    private final int threads;
    private int sum = 0;

    public Task(int id, int[] array, int size, int threads) {
        this.id = id;
        this.array = array;
        this.size = size;
        this.threads = threads;
    }

    @Override
    public void run() {
        int start = id * (int) Math.ceil((double) size / (double) threads);
        int end = Math.min((id + 1) * (int) Math.ceil((double) size / (double) threads), size);

        for (int i = start; i < end; i++) {
            sum += array[i];
        }
    }

    public int getSum() {
        return sum;
    }
}
